/*
 * querier.c    Hugo Fang    2/10/2024
 * 
 * Reads crawler-produced document files and a indexer-produced index file
 * to answer search queries in stdin.
 * 
 * Each query must be a line containing words, "AND", and "OR". Adjacent
 * words are assumed to have an "AND" in between, and adjacent "AND"/"OR"
 * pairs are not allowed
 * 
 * Usage: ./querier pageDirectory indexFilename
 * 
 * Exits with:
 *   0 if normal return
 *   errno 1 if error parsing arguments: failure to read from `pageDirectory`
 *     or `indexFilename`
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

// libcs50.a
#include "hashtable.h"
#include "counters.h"
#include "file.h"

// common.a
#include "pagedir.h"
#include "print.h"
#include "index.h"
#include "word.h"
#include "pagedir.h"

/* Local types */
typedef struct queryResult {
  // score corresponds to the count items in a counter
  int docID, score;
} queryResult_t;

typedef struct queryResArr {
  // internal array of queryResult_t*
  queryResult_t** arr;
  // next pos to insert at
  int pos;
} queryResArr_t;

typedef struct counterPair {
  counters_t* from;
  counters_t* to;
} counterPair_t;

/* Private functions */
int fileno(FILE* stream);
static index_t* parseArgs(const int argc, char* argv[], char** pageDirectory_p);
static void queryResArr_delete(queryResArr_t* resArr);

// processing queries
static void prompt();
static void processQuery(const index_t* idx, char* query, const char* pageDirectory);
static bool isValidQuery(char* query);
static bool isAndOr(char* word);

// processing results
static queryResArr_t* getSortedResults(counters_t* res);
static void getCounterSize(void* arg, const int key, const int count);
static void addIntoArray(void* arg, const int key, const int count);
static int compareQueryResult(const void* a, const void* b);
static void outputQueryResults(const queryResArr_t* resArr, const char* pageDirectory);

// combining counters
static void intersectCounters(counters_t* from, counters_t** to_p);
static void mergeMinCount(void* arg, const int key, const int item);
static void copyIfNonzero(void* arg, const int key, const int item);

static void unionCounters(counters_t* from, counters_t* to);
static void mergeAddCount(void* arg, const int key, const int item);

static counters_t* copyCounter(counters_t* counter);
static void copyCounterHelper(void* arg, const int key, const int item);

int main(const int argc, char* argv[])
{
  // parse arguments
  char* pageDirectory = NULL;
  index_t* idx = parseArgs(argc, argv, &pageDirectory);
  
  // answer queries until EOF
  prompt();
  char* query;
  while ((query = file_readLine(stdin)) != NULL) {
    processQuery(idx, query, pageDirectory);
    prompt();
    free(query);
  }
  index_delete(idx);
  return 0;
}

/*
 * Checks argc == 3 (1 for "querier", 2 for inputs),
 * makes sure pageDirectory exists and contains ".crawler"
 * checks indexFilename is readable file, and tries to
 * build an index from it.
 * 
 * prints error to stderr and exit 1 on invalid argument

 * Returns:
 *   the index built from the file at indexFilename
 */
static index_t* parseArgs(const int argc, char* argv[], char** pageDirectory_p)
{
  if (argc != 3) {
    fprintf(stdout, "Usage: %s pageDirectory indexFilename\n", argv[0]);
    exit(1);
  }

  // validate pageDirectory
  *pageDirectory_p = argv[1];
  if (!pagedir_isCrawlerDirectory(*pageDirectory_p)) {
    printerrln("Querier: pageDirectory doesn't exist or doesn't contain \".crawler\"");
    exit(1);
  }

  // validate indexFilename
  char* indexFilename = argv[2];
  if (!pagedir_isFileReadable(indexFilename)) {
    fprintf(stderr, "Querier: failed to read %s\n", indexFilename);
    exit(1);
  }

  // try to build index
  index_t* idx = index_readIndexFile(indexFilename);
  if (idx == NULL) {
    fprintf(stderr, "Querier: failed to build index from %s", indexFilename);
  }
  return idx;
}

/*
 * Print a prompt to stdout if stdin is a tty
 */
void prompt()
{
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}

/*
 * Compute and output the results of a query. Prints a message
 * to stderr if the query has incorrect format.
 * 
 * Inputs:
 *   idx: index of words in `pageDirectory`
 *   query to process
 */
void processQuery(const index_t* idx, char* query, const char* pageDirectory)
{
  stripCompactNormalize(query);
  printf("Query: %s\n", query); // echo pre-processed query

  // empty or invalid query
  if (query[0] == '\0' || !isValidQuery(query)) {
    println("-----------------------------------------------");
    return;
  }

  counters_t* res = counters_new(); // final result
  // out of memory
  if (res == NULL) {
    return;
  }
  counters_t* temp = NULL; // current and-sequence
  int pos = 0;
  char* word = nextWord(query, &pos);
  while (word != NULL) {
    // by default, reading a word will "and" it with any previous sequence
    // if read "or", then union `temp` into `result`
    if (!isAndOr(word) && strlen(word) >= 3) { // read a length >=3 word
      counters_t* counter = index_getWord(idx, word);
      // if word isn't found, reset temp and skip this and-sequence
      if (counter == NULL) {
        if (temp != NULL) {
          counters_delete(temp);
        }
        temp = NULL;
        while (word != NULL && strcmp(word, "or") != 0) {
          word = nextWord(query, &pos);
        }
        continue;
      }

      if (temp == NULL) { // start new and-sequence
        temp = copyCounter(counter);
      } else { // update current and-sequence
        intersectCounters(counter, &temp);
      }
    } else if (strcmp(word, "or") == 0) { // read "or"
      // temp could be NULL if previous and-sequence contains
      // a nonexistent word in the index
      if (temp != NULL) {
        unionCounters(temp, res);
        counters_delete(temp);
        temp = NULL;
      }
    }
    word = nextWord(query, &pos);
  }

  if (temp != NULL) {
    unionCounters(temp, res);
  }

  queryResArr_t* resArr = getSortedResults(res);
  if (resArr == NULL) {
    return;
  }

  outputQueryResults(resArr, pageDirectory);
  println("-----------------------------------------------");

  // clear up memory for this query
  counters_delete(temp);
  counters_delete(res);
  queryResArr_delete(resArr);
}

/*
 * Check that a query is valid:
 *   1. only alphabet and space characters (satisfies isalpha() and isspace())
 *   2. no "and"/"or" at the beginning or and
 *   3. no consecutive "and"/"or"
 */
bool isValidQuery(char* query) {
  // copy query to preserve the original
  char* queryCopy = calloc(strlen(query) + 1, sizeof(char));
  strcpy(queryCopy, query);

  // query contains invalid characters
  char ch;
  if ((ch = onlyAlphaSpaces(queryCopy)) != '\0') {
    fprintf(stderr, "Error: bad character '%c' in query\n", ch);
    return false;
  }

  // query starts with, ends with, or contains adjacent "and"/"or"
  int pos = 0;
  char* curWord = nextWord(queryCopy, &pos);
  // starts with
  if (isAndOr(curWord)) {
    fprintf(stderr, "Error: query cannot start with 'and'/'or'\n");
    free(queryCopy);
    return false;
  }
  // adjacent
  char* prevWord = curWord;
  while ((curWord = nextWord(queryCopy, &pos)) != NULL) {
    if (isAndOr(prevWord) && isAndOr(curWord)) {
      fprintf(stderr, "Error: query cannot contain consecutive 'and'/'or'\n");
      free(queryCopy);
      return false;
    }
    prevWord = curWord;
  }
  // ends with
  if (isAndOr(prevWord)) {
    fprintf(stderr, "Error: query cannot end with 'and'/'or'\n");
    free(queryCopy);
    return false;
  }
  free(queryCopy);
  return true;
}

/*
 * Checks if a word is "and"/"or"
 */
bool isAndOr(char* word)
{
  return (strcmp(word, "and") == 0 || strcmp(word, "or") == 0);
}

/*
 * Turn a counter into a queryResArr, containing queryResults sorted
 * by decreasing score
 * 
 */
queryResArr_t* getSortedResults(counters_t* res)
{
  int size = 0;
  counters_iterate(res, &size, getCounterSize);
  // initialize new queryResArr_t
  queryResArr_t* resArr = malloc(sizeof(queryResArr_t));
  if (resArr == NULL) {
    return NULL;
  }
  resArr->arr = calloc(size, sizeof(queryResult_t));
  if (resArr->arr == NULL) {
    return NULL;
  }
  resArr->pos = 0;

  // add (docID, score) pairs from res into resArr
  counters_iterate(res, resArr, addIntoArray);

  // sort the array in resArr
  qsort(resArr->arr, size, sizeof(queryResult_t), compareQueryResult);
  return resArr;
}

void getCounterSize(void* arg, const int key, const int count)
{
  int* count_p = arg;
  (*count_p)++;
}

void addIntoArray(void* arg, const int key, const int count)
{
  // create queryResult from counter entry
  queryResult_t* queryRes = malloc(sizeof(queryResult_t));
  if (queryRes == NULL) {
    return;
  }
  queryRes->docID = key;
  queryRes->score = count;

  // insert queryResult into queryResArr
  queryResArr_t* resArr = arg;
  resArr->arr[resArr->pos++] = queryRes;
}

/*
 * Helper function to sort queryResults in decreasing score order
 */
int compareQueryResult(const void* a, const void* b)
{
  queryResult_t* ptrA = *(queryResult_t**) a;
  queryResult_t* ptrB = *(queryResult_t**) b;
  return ptrB->score - ptrA->score;
}

void outputQueryResults(const queryResArr_t* resArr, const char* pageDirectory)
{
  // resArr->pos is incremented once for each inserted item,
  // so it's equivalent to size of resArr->arr
  int size = resArr->pos;
  if (size == 0) { // empty
    println("No documents match");
  } else if (size == 1) {
    println("Matches 1 document:");
  } else {
    printf("Matches %d documents (ranked):\n", size);
  }

  for (int i = 0; i < size; i++) {
    queryResult_t* queryRes = resArr->arr[i];
    char* url = pagedir_loadUrlFromFile(pageDirectory, i + 1);
    if (url == NULL) {
      fprintf(stderr, "Error reading url from file in %s\n", pageDirectory);
      return;
    }
    printf("score\t%d doc %3d: %s\n", queryRes->score, queryRes->docID, url);
    free(url);
  }
}

void queryResArr_delete(queryResArr_t* resArr)
{
  for (int i = 0; i < resArr->pos; i++) {
    free(resArr->arr[i]);
  }
  free(resArr->arr);
  free(resArr);
}

/*
 * Merge one counter into another. The first counter is unchanged, and
 * entries are merged into the second counter. The minimum of counts is taken
 * if both counters contain the key. If the second counter doesn't contain the key,
 * the count becomes 0.
 * 
 * Notes:
 * A new counter is created to keep only nonzero counts after merging, and to_p is
 * reassigned to the new counter. The old target counter (*to_p) is deleted
 * 
 * Inputs:
 *   from: counters_t* to merge from
 *   to_p: counters_t** to merge to
 */
void intersectCounters(counters_t* from, counters_t** to_p)
{
  counters_t* to = *to_p;
  counterPair_t fromTo = {from, to};
  // for every key in `to`, get the min of counts in `from` and `to`
  counters_iterate(to, &fromTo, mergeMinCount);

  // create a new counter to keep only the nonzero counts
  counters_t* res = counters_new();
  counters_iterate(to, res, copyIfNonzero);
  counters_delete(to);
  *to_p = res;
}

/*
 * Adds a (key, item) pair into arg->to (a counterPair_t*). If the key already exists,
 * set the item to be the minimum in the two counters. Otherwise, set to 0.
 *
 * Inputs:
 *   arg: pair of counters
 *   (key, item) pair
 */
void mergeMinCount(void* arg, const int key, const int item)
{
  counterPair_t* fromTo = arg;
  counters_t* to = fromTo->to;
  int fromCount = counters_get(fromTo->from, key);
  // minimum of fromCount and toCount (item)
  counters_set(to, key, fromCount < item ? fromCount : item);
}

/*
 * Filter out item == 0 pairs by only adding nonzero pairs into `arg`
 */
void copyIfNonzero(void* arg, const int key, const int item)
{
  counters_t* res = arg;
  if (item > 0) {
    counters_set(res, key, item);
  }
}

/*
 * Merge one counter into another. The first counter is unchanged, and
 * entries are merged into the second counter. The sum of counts is taken,
 * where count defaults to 0 if a key is not present in the target counter.
 * 
 * Inputs:
 *   counters to intersect
 */
static void unionCounters(counters_t* from, counters_t* to)
{
  counters_iterate(from, to, mergeAddCount);
}

/*
 * Adds a (key, item) pair into arg (a counters_t*), adding the items.
 *
 * Inputs:
 *   arg: target counter to copy into
 *   (key, item) pair
 */
void mergeAddCount(void* arg, const int key, const int item)
{
  counters_t* to = arg;
  // new count is sum of original counts
  int count = counters_get(to, key);
  counters_set(to, key, count + item);
}

/*
 * Creates a copy of a counter.
 *
 * Caller is responsible for calling counters_delete on the returned counter
 */
counters_t* copyCounter(counters_t* counter)
{
  counters_t* copy = counters_new();
  counters_iterate(counter, copy, copyCounterHelper);
  return copy;
}

/*
 * Helper function for copyCounter
 */
void copyCounterHelper(void* arg, const int key, const int item)
{
  counters_t* copy = arg;
  counters_set(copy, key, item);
}