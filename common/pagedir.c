/*
 * pagedir.c    Hugo Fang    1/18/2024
 * 
 * Handles validating and initializing a pageDirectory,
 * writing and reading page files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// from libcs50.a
#include "webpage.h"

#include "print.h"
#include "pagedir.h"

bool pagedir_init(const char* pageDirectory)
{
  if (pageDirectory == NULL) {
    return false;
  }
  // check validity and accessability of pageDirectory
  if (access(pageDirectory, F_OK) == -1) {
    return false;
  }

  // if pageDirectory ends with a '/', append ".crawler"
  // otherwise, append "/.crawler"
  size_t dirLength = strlen(pageDirectory);
  size_t extLength = strlen(".crawler");
  // malloc extra char for potential '/'
  char* crawlerPath = malloc(sizeof(char) * (dirLength + extLength + 2));
  if (crawlerPath == NULL) {
      return false;
  }
  strcpy(crawlerPath, pageDirectory);
  if (pageDirectory[dirLength - 1] == '/') {
    strcat(crawlerPath, ".crawler");
  } else {
    strcat(crawlerPath, "/.crawler");
  }

  FILE* fp = fopen(crawlerPath, "w");
  if (fp == NULL) {
    return false;
  }
  fclose(fp);
  free(crawlerPath);
  return true;
}

bool pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
  if (pageDirectory == NULL || page == NULL || docID <= 0) {
    return false;
  }
  // check validity and accessability of pageDirectory
  if (access(pageDirectory, F_OK) == -1) {
    return false;
  }

  // get length of documentID as string, then allocate space for file path
  int idLength = snprintf(NULL, 0, "%d", docID);
  // if pageDirectory ends with a '/', append ".crawler"
  // otherwise, append "/.crawler"
  // allocate extra char for potential '/'
  size_t dirLen = strlen(pageDirectory);
  char filePath[dirLen + idLength + 2]; // directory to target file
  if (pageDirectory[dirLen - 1] == '/') {
    snprintf(filePath, sizeof(filePath), "%s%d", pageDirectory, docID);
  } else {
    snprintf(filePath, sizeof(filePath), "%s/%d", pageDirectory, docID);
  }
  
  FILE* fp = fopen(filePath, "w");
  if (fp == NULL) {
    return false;
  }
  fprintf(fp, "%s\n", webpage_getURL(page));
  fprintf(fp, "%d\n", webpage_getDepth(page));
  fprintf(fp, "%s", webpage_getHTML(page));
  fclose(fp);
  return true;
}
