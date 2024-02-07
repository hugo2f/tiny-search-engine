/*
 * index.c    Hugo Fang    2/6/2024
 * 
 * See index.h for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// contained in libcs50.a
#include "hashtable.h"
#include "counters.h"
#include "file.h"

#include "word.h"

typedef struct index {
  // <char* word, counters_t* counter>
  hashtable_t* ht;
} index_t;

/* Private function prototypes */
static bool str2int(const char* string, int* num_p);
static void index_setWordDocCount(index_t* idx, const char* word,
                                const int docID, const int count);
static index_t* index_newWithNumSlots(const int numSlots);
static void saveHashTableEntry(void* arg, const char* key, void* item);
static void saveCounterEntry(void* arg, const int key, const int item);

/* Public functions */
index_t* index_new()
{
  index_t* idx = malloc(sizeof(index_t));
  if (idx == NULL) {
    return NULL;
  }
  // By default, hashtable has 500 slots
  idx->ht = hashtable_new(500);
  return idx;
}

void index_delete(index_t* idx)
{
  // each item in the hashtable is a counter
  hashtable_delete(idx->ht, counters_delete);
  free(idx);
}

void index_addWord(index_t* idx, const char* word, const int docID)
{
  if (idx == NULL || word == NULL || strlen(word) < 3 || docID <= 0) {
    return;
  }

  normalizeWord(word);
  // <docID, count> pairs associated with `word`
  counters_t* counter = hashtable_find(idx->ht, word);
  // if first time seeing `word`, create a new counter and insert into ht
  if (counter == NULL) {
    counter = counters_new();
    if (counter == NULL) {
      return;
    }
    hashtable_insert(idx->ht, word, counter);
  }
  counters_add(counter, docID); // `word` appeared in docID once more
}

index_t* index_readFile(const char* filePath)
{
  if (filePath == NULL) {
    return NULL;
  }
  FILE* fp = fopen(filePath, "r");
  if (fp == NULL) {
    return NULL;
  }

  // initialize an index with its hashtable's numSlots = 1.5 * number of words
  index_t* idx = index_newWithNumSlots((int)(file_numLines(fp) * 1.5));
  if (idx == NULL) {
    fclose(fp);
    return NULL;
  }

  char* word;
  // read word, then read (docID, count) pairs until newline
  // (file_readWord returns empty string if newline)
  while ((word = file_readWord(fp)) != NULL) {
    while (true) {
      // try to read (docID, count) pair using two file_readWord()s
      // if first file_readWord() returns empty string, reached
      // end of line and go to next line
      char* buffer = file_readWord(fp);
      if (buffer == NULL || buffer[0] == '\0') {
        break;
      }

      int docID, count;
      // read docID
      if (!str2int(buffer, &docID)) {
        return NULL;
      }
      free(buffer);

      // read count
      buffer = file_readWord(fp);
      // if read docID but can't read count, file has wrong format
      if (buffer == NULL || buffer[0] == '\0') {
        return NULL;
      }
      if (!str2int(buffer, &count)) {
        return NULL;
      }
      free(buffer);

      // update index with word, docID and count
      index_setWordDocCount(idx, word, docID, count);
    }
    free(word);
  }

  fclose(fp);
  return idx;
}

/*
 * converts string to integer and stores in num_p
 * 
 * Returns false if num_p is NULL, failure to convert,
 * or input contains extra characters at the end
 * 
 * If string is NULL or empty, num_p is set to 0 and
 * treated as a success
 */
bool str2int(const char* string, int* num_p)
{
  if (num_p == NULL) {
    return false;
  }

  if (string == NULL || *string == '\0') {
    *num_p = 0;
    return true;
  } else {
    char extra;
    return (sscanf(string, "%d%c", num_p, &extra) == 1);
  }
}

/*
 * Internal function to update the index when reading a (docID, count) pair.
 * For the `word` entry in the hashtable (a counters_t*), set the `docID`
 * entry to `count`
 *
 * Inputs:
 *   idx: index to update
 *   word to add (assumed to have length >= 3, since reading index
 *     generated file)
 *   docID: file corresponding to the page containing `word`
 *   count of occurences in `docID`
 */
void index_setWordDocCount(index_t* idx, const char* word,
                                const int docID, const int count)
{
  if (idx == NULL || word == NULL || strlen(word) < 3 || docID <= 0) {
    return;
  }

  // <docID, count> pairs associated with `word`
  counters_t* counter = hashtable_find(idx->ht, word);
  // if first time seeing `word`, create a new counter and insert into ht
  if (counter == NULL) {
    counter = counters_new();
    if (counter == NULL) {
      return;
    }
    hashtable_insert(idx->ht, word, counter);
  }
  
  // `word` appeared in `docID` `count` times
  counters_set(counter, docID, count); 
}

/*
 * When reading from a file, number of words is known ->
 * create an index with a hashtable initialized with numSlots
 */
index_t* index_newWithNumSlots(const int numSlots)
{
  index_t* idx = malloc(sizeof(index_t));
  if (idx == NULL) {
    return NULL;
  }
  idx->ht = hashtable_new(numSlots);
  return idx;
}

void index_saveToFile(index_t* idx, const char* filePath)
{
  if (filePath == NULL) {
    return;
  }
  FILE* fp = fopen(filePath, "w");
  if (fp == NULL) {
    return;
  }

  // iterate through the hashtable and save the entries
  // see below for details about saving hashtable and counter entries
  hashtable_iterate(idx->ht, fp, saveHashtableEntry);
  fclose(fp);
}

/*
 * Internal function to save the counters in a hashtable entry.
 * Print the word, then iterate through the counter in that entry
 * to print the (docID, count) pairs, and finish with newline
 * 
 * TODO: is it ok to have extra newline at the bottom
 *
 * Inputs:
 *   arg: index_saveToFile() will pass in FILE* fp to write to
 *   key: the word saved as a key in the hashtable
 *   item: the counter saved as an item in the hashtable
 */
void saveHashtableEntry(void* arg, const char* key, void* item)
{
  FILE* fp = arg;
  counters_t* counter = item;
  // print the word, start a newline if not at the beginning of the file
  // TODO: test if this will have no extra newlines
  if (ftell(fp) != 0) {
    fputc('\n', fp);
  }
  fprintf(fp, "%s ", key);

  // print (docID, count) pairs
  counters_iterate(counter, fp, saveCounterEntry);
}

/*
 * Internal function to print the (docID, count) pair in a counter entry.
 *
 * Inputs:
 *   arg: counters_iterate() will pass in FILE* fp to write to
 *   key: the docID saved as a key in the counter
 *   count: number of occurences of word in docID
 */
void saveCounterEntry(void* arg, const int key, const int count)
{
  FILE* fp = arg;
  fprintf(fp, "%d %d", key, count);
}