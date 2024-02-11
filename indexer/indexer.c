/*
 * indexer.c    Hugo Fang    2/6/2024
 * 
 * Reads document files created by the crawler, builds an index
 * of words to occurences in pages, then writes index to file.
 * The indexer starts at the file named `1` (exits if doesn't exist),
 * and increments the filename until the file cannot be found
 * 
 * Each line has the format: word docID count [docID count]...
 * where each (docID, count) pair corresponds to a page that
 * contains `word`
 * 
 * Usage: ./indexer pageDirectory indexFilename
 * 
 * Exits with:
 *   0 if normal return
 *   errno 1 if error parsing arguments: pageDirectory doesn't exist
 *     or doesn't contain ".crawler", failure to write to indexFilename,
 *     or failure to create a file at the path indexFilename 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// libcs50.a
#include "hashtable.h"

// common.a
#include "pagedir.h"
#include "print.h"
#include "index.h"

/* Private functions */
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory_p, char** indexFilename_p);
index_t* indexBuild(const char* pageDirectory);
static void indexPage(index_t* idx, webpage_t* page, const int docID);

int main(const int argc, char* argv[])
{
  char* pageDirectory = NULL;
  char* indexFilename = NULL;
  parseArgs(argc, argv, &pageDirectory, &indexFilename);
  index_t* idx = indexBuild(pageDirectory);
  index_saveToFile(idx, indexFilename);
  index_delete(idx);
  return 0;
}

/*
 * Checks argc == 3 (1 for "indexer", 2 for inputs),
 * make sure pageDirectory exists and contains ".crawler"
 * check indexFilename either:
 *   1. exists and is writeable (will be erased if nonempty), or
 *   2. is in a writeable directory, such that the indexer can create
 *      the new file
 * prints error to stderr and exit 1 on invalid argument
 */
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory_p, char** indexFilename_p)
{
  if (argc != 3) {
    fprintf(stdout, "Usage: %s pageDirectory indexFilename\n", argv[0]);
    exit(1);
  }

  *pageDirectory_p = argv[1];
  if (!pagedir_isCrawlerDirectory(*pageDirectory_p)) {
    printerrln("Indexer: pageDirectory doesn't exist or doesn't contain \".crawler\"");
    exit(1);
  }

  *indexFilename_p = argv[2];
  if (!pagedir_isFileWriteable(*indexFilename_p)) {
    fprintf(stderr, "Indexer: failed to write to %s\n", *indexFilename_p);
    exit(1);
  }
}

/*
 * Creates a new index_t*, read all files in pageDirectory and build the index
 *
 * Inputs:
 *   pageDirectory: directory to read files from
 * 
 * Returns:
 *   the index
 * 
 * Caller needs to call index_delete() on the returned index
 */
index_t* indexBuild(const char* pageDirectory)
{
  index_t* idx = index_new();
  if (idx == NULL) {
    return NULL;
  }
  int docID = 1;
  webpage_t* page;
  while ((page = pagedir_loadPageFromFile(pageDirectory, docID)) != NULL) {
    indexPage(idx, page, docID);
    webpage_delete(page);
    docID++;
  }
  return idx;
}

/*
 * For a webpage, update index with the words in the html
 *
 * Inputs:
 *   idx: index to update
 *   page: webpage_t* containing the html
 *   docID: name of file that `page` was read from
 */
void indexPage(index_t* idx, webpage_t* page, const int docID)
{
  char* word;
  int pos = 0;
  while ((word = webpage_getNextWord(page, &pos)) != NULL) {
    index_addWord(idx, word, docID);
    free(word);
  }
}
