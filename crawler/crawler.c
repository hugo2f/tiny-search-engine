/*
 * crawler.c    Hugo Fang    1/29/2024
 * 
 * crawls from a starting URL to a certain depth, and stores html
 * of pages found
 * 
 * Usage: crawler seedURL pageDirectory maxDepth
 * 
 * Exits with:
 *   errno 1 if error parsing arguments: * seedURL is not internal,
 *   pageDirectory doesn't exist, maxDepth < 0
 *   errno 2 if failed to initialize data structures in crawl()
 *   errno 3 if error from pagedir_save()
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// header files in libcs50.a
#include "bag.h"
#include "hashtable.h"
#include "webpage.h"

#include "pagedir.h"
#include "print.h"


static void parseArgs(const int argc, char* argv[], char** seedURL_p,
               char** pageDirectory_p, int* maxDepth_p);
static bool str2int(const char string[], int* num_p);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void logr(const char* word, const int depth, const char* url);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

int main(const int argc, char* argv[])
{
  char* seedURL = NULL;
  char* pageDirectory = NULL;
  int maxDepth = -1;
  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
  crawl(seedURL, pageDirectory, maxDepth);
  return 0;
}

/*
 * checks argc == 4 (1 for crawler, 3 for inputs)
 * normalize seedURL and validate it is an internal URL
 * call pagedir_init() on pageDirectory
 * check maxDepth is a non-negative integer
 * prints error to stderr and exit 1 on invalid argument
 */
void parseArgs(const int argc, char* argv[], char** seedURL_p,
               char** pageDirectory_p, int* maxDepth_p)
{
  if (argc != 4) {
    fprintf(stdout, "Usage: %s seedURL pageDirectory, maxDepth\n", argv[0]);
    exit(1);
  }

  *seedURL_p = normalizeURL(argv[1]);
  if (!isInternalURL(*seedURL_p)) {
    printerrln("Crawler: seedURL is not an internal URL");
    exit(1);
  }

  *pageDirectory_p = argv[2];
  if (!pagedir_init(*pageDirectory_p)) {
    printerrln("Crawler: pagedir_init() failed");
    exit(1);
  }

  // not an integer OR negative input
  if (!str2int(argv[3], maxDepth_p) || *maxDepth_p < 0) {
    printerrln("Crawler: maxDepth must be a non-negative integer");
    exit(1);
  }
}

/*
 * converts string to integer and stores in num_p
 * 
 * Returns false if num_p is NULL, fail to convert,
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
 * Crawls from seedURL to maxDepth and save pages in pageDirectory.
 * If maxDepth == 0, only crawl seedURL, etc.
 * 
 * Inputs:
 *   seedURL: url to start at
 *   pageDirectory: directory to save pages in
 *   maxDepth: depth to explore
 */
void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
  if (seedURL == NULL || pageDirectory == NULL || maxDepth < 0) {
    printerrln("Failed to crawl, invalid arguments");
    exit(2);
  }

  // <char* url, "" (only keys matter)>
  hashtable_t* seen = hashtable_new(200);
  if (seen == NULL) {
    printerrln("Crawler: error initializing hashttable");
    exit(2);
  }
  // <webpage_t* page>
  bag_t* toVisit = bag_new();
  if (toVisit == NULL) {
    printerrln("Crawler: error initializing bag");
    exit(2);
  }
  webpage_t* seedPage = webpage_new(seedURL, 0, NULL);
  if (seedPage == NULL) {
    printerrln("Crawler: error initializing webpage for seedURL");
    exit(2);
  }

  // initializing the crawling
  hashtable_insert(seen, seedURL, "");
  bag_insert(toVisit, seedPage);

  // crawling
  int docID = 1;
  webpage_t* page;
  while ((page = bag_extract(toVisit)) != NULL) {
    if (!webpage_fetch(page)) {
      continue;
    }
    logr("Fetched", webpage_getDepth(page), webpage_getURL(page));
    if (!pagedir_save(page, pageDirectory, docID)) {
      printerrln("Crawler: pagedir_save() failed");
      exit(3);
    }
    if (webpage_getDepth(page) < maxDepth) {
      pageScan(page, toVisit, seen);
    }
    webpage_delete(page);
    docID++;
  }

  hashtable_delete(seen, NULL);
  bag_delete(toVisit, webpage_delete);
}

/*
 * Log crawler progress
 */
static void logr(const char* word, const int depth, const char* url)
{
#ifdef LOG
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
#else
  ;
#endif
}

/*
 * TODO
 */
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
  if (page == NULL || pagesToCrawl == NULL || pagesSeen == NULL) {
    return;
  }

  logr("Scanning", webpage_getDepth(page), webpage_getURL(page));
  int pos = 0, curDepth = webpage_getDepth(page);
  char* nextURL;
  while ((nextURL = webpage_getNextURL(page, &pos)) != NULL) {
    // skip external or visited URLs
    logr("Found", curDepth, nextURL);
    if (!isInternalURL(nextURL)) {
      logr("IgnExtrn", curDepth, nextURL);
      free(nextURL);
      continue;
    } 
    if (!hashtable_insert(pagesSeen, nextURL, "")) {
      logr("IgnDupl", curDepth, nextURL);
      free(nextURL);
      continue;
    }
    webpage_t* nextPage = webpage_new(nextURL, curDepth + 1, NULL);
    logr("Added", curDepth, webpage_getURL(nextPage));
    bag_insert(pagesToCrawl, nextPage);
  }
}
