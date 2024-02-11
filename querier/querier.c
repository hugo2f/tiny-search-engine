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

/* Private functions */
static index_t* parseArgs(const int argc, char* argv[], char** pageDirectory_p);
int fileno(FILE* stream);
static void prompt();
static void processQuery(const index_t* idx, char* query);
static bool isValidQuery(char* query);
static bool isAndOr(char* word);

// counter related functions
static void intersectCounters(counters_t* from, counters_t** to_p);
static void mergeMinCount(void* arg, const int key, const int item);
static void setIfNonzero(void* arg, const int key, const int item);

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
    prompt();
    processQuery(idx, query);
  }
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
    printf("Query: ");
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
void processQuery(const index_t* idx, char* query)
{
  // empty or invalid query
  if (query[0] == '\0' || !isValidQuery(query)) {
    return;
  }


}

bool isValidQuery(char* query) {
  // query contains invalid characters
  char ch;
  if ((ch = onlyAlphaSpaces(query)) != '\0') {
    fprintf(stderr, "Error: bad character '%c' in query.\n", ch);
    return false;
  }

  // query starts with, ends with, or contains adjacent "AND"/"OR"
  char* prevWord = NULL;
  char* curWord = NULL;
  int pos = 0;
  while ((curWord = normalizeWord(nextWord(query, &pos))) != NULL) {
    if (prevWord == NULL && isAndOr(curWord)) { // starts with
      fprintf(stderr, "Error: query cannot start with 'AND'/'OR'\n");
      return false;
    } else if (isAndOr(prevWord) || isAndOr(curWord)) { // ends with
      fprintf(stderr, "Error: query cannot contain consecutive 'AND'/'OR'\n");
      return false;
    }
  }
  // ends with "AND"/"OR"
  if (isAndOr(prevWord)) {
    fprintf(stderr, "Error: query cannot end with 'AND'/'OR'\n");
    return false;
  }
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
  // merge `to` into `fromCopy` to initialize missing keys in fromCopy as 0,
  // then merge `fromCopy` into `to`
  counters_t* to = *to_p;
  counters_t* fromCopy = copyCounter(from);
  counters_iterate(to, fromCopy, mergeMinCount);
  counters_iterate(fromCopy, to, mergeMinCount);
  counters_delete(fromCopy);

  // create a new counter to keep only the nonzero counts
  counters_t* result = counters_new();
  counters_iterate(to, result, setIfNonzero);
  counters_delete(to);
  *to_p = result;
}

/*
 * Adds a (key, item) pair into arg (a counters_t*). If the key already exists,
 * set the item to be the minimum in the two counters. Otherwise, set to 0.
 *
 * Inputs:
 *   arg: target counter to copy into
 *   (key, item) pair
 */
void mergeMinCount(void* arg, const int key, const int item)
{
  counters_t* to = arg;
  int count = counters_get(to, key);
  // minimum of original counts
  counters_set(to, key, count < item ? count: item);
}

/*
 * Filter out item == 0 pairs by only adding nonzero pairs into `arg`
 */
void setIfNonzero(void* arg, const int key, const int item)
{
  counters_t* result = arg;
  if (item > 0) {
    counters_set(result, key, item);
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