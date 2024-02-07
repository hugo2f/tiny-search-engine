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
 * Usage: indexer pageDirectory indexFilename
 * 
 * Exits with:
 *   errno 1 if error parsing arguments: pageDirectory doesn't exist
 *   or doesn't contain ".crawler", failure to write to indexFilename,
 *   or failure to create a file at the path indexFilename 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// header files in libcs50.a
#include "hashtable.h"
#include "set.h"

#include "pagedir.h"
#include "print.h"


static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory_p, char** indexFilename_p);
// static bool str2int(const char string[], int* num_p);
static void indexBuild(const char* pageDirectory);
static void indexPage(const char* filePath);

int main(const int argc, char* argv[])
{
  char* pageDirectory = NULL;
  char* indexFilename = NULL;
  parseArgs(argc, argv, &pageDirectory, &indexFilename);
  indexBuild(pageDirectory);
  return 0;
}

/*
 * checks argc == 3 (1 for "indexer", 2 for inputs)
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
  if (!pagedir_isPathWriteable(indexFilename_p)) {
    fprintf(stderr, "Indexer: failed to write to %s\n", *indexFilename_p);
    exit(1);
  }
}

// /*
//  * converts string to integer and stores in num_p
//  * 
//  * Returns false if num_p is NULL, fail to convert,
//  * or input contains extra characters at the end
//  * 
//  * If string is NULL or empty, num_p is set to 0 and
//  * treated as a success
//  */
// bool str2int(const char* string, int* num_p)
// {
//   if (num_p == NULL) {
//     return false;
//   }

//   if (string == NULL || *string == '\0') {
//     *num_p = 0;
//     return true;
//   } else {
//     char extra;
//     return (sscanf(string, "%d%c", num_p, &extra) == 1);
//   }
// }


