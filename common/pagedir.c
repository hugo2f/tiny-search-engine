/*
 * pagedir.c    Hugo Fang    1/18/2024
 * 
 * See pagedir.h for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// libcs50.a
#include "webpage.h"
#include "file.h"

// common.a
#include "print.h"
#include "pagedir.h"

/* Private functions */
static bool str2int(const char* string, int* num_p);

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
  if (page == NULL || pageDirectory == NULL || docID <= 0) {
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

webpage_t* pagedir_loadPageFromFile(const char* pageDirectory, const int docID)
{
  if (pageDirectory == NULL || docID <= 0) {
    return NULL;
  }
  // check validity and accessability of pageDirectory
  if (access(pageDirectory, F_OK) == -1) {
    return NULL;
  }

  // same logic as pagedir_save()
  int idLength = snprintf(NULL, 0, "%d", docID);
  size_t dirLen = strlen(pageDirectory);
  char filePath[dirLen + idLength + 2]; // directory to target file
  if (pageDirectory[dirLen - 1] == '/') {
    snprintf(filePath, sizeof(filePath), "%s%d", pageDirectory, docID);
  } else {
    snprintf(filePath, sizeof(filePath), "%s/%d", pageDirectory, docID);
  }
  
  FILE* fp = fopen(filePath, "r");
  if (fp == NULL) {
    return NULL;
  }

  // read url, depth, html
  char* url = file_readLine(fp);
  char* depthString = file_readLine(fp);
  char* html = file_readFile(fp);
  // if one of the lines couldn't be read, free what's read and return
  if (url == NULL || depthString == NULL || html == NULL) {
    if (url != NULL) {
      free(url);
    }
    if (depthString != NULL) {
      free(depthString);
    }
    if (html != NULL) {
      free(html);
    }
    fclose(fp);
    return NULL;
  }
  int depth;
  // can't convert (depth line doesn't contain exactly one integer)
  if (!(str2int(depthString, &depth))) {
    free(url);
    free(depthString);
    free(html);
    fclose(fp);
    return NULL;
  }
  free(depthString);

  // create page from file contents
  webpage_t* page = webpage_new(url, depth, html);
  fclose(fp);
  return page;
}

char* pagedir_loadUrlFromFile(const char* pageDirectory, const int docID)
{
if (pageDirectory == NULL || docID <= 0) {
    return NULL;
  }
  // check validity and accessability of pageDirectory
  if (access(pageDirectory, F_OK) == -1) {
    return NULL;
  }

  // same logic as pagedir_save()
  int idLength = snprintf(NULL, 0, "%d", docID);
  size_t dirLen = strlen(pageDirectory);
  char filePath[dirLen + idLength + 2]; // directory to target file
  if (pageDirectory[dirLen - 1] == '/') {
    snprintf(filePath, sizeof(filePath), "%s%d", pageDirectory, docID);
  } else {
    snprintf(filePath, sizeof(filePath), "%s/%d", pageDirectory, docID);
  }
  
  FILE* fp = fopen(filePath, "r");
  if (fp == NULL) {
    return NULL;
  }

  // read url
  char* url = file_readLine(fp);
  fclose(fp);
  return url;
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

bool pagedir_isCrawlerDirectory(char* pageDirectory)
{
  if (pageDirectory == NULL) {
    return false;
  }
  
  int dirLen = strlen(pageDirectory);
  // if pageDirectory doesn't end in '/', allocate extra char
  // in strlen("/.crawler")
  char crawlerPath[dirLen + strlen("/.crawler") + 1];
  if (pageDirectory[dirLen - 1] == '/') {
    sprintf(crawlerPath, "%s%s", pageDirectory, ".crawler");
  } else {
    sprintf(crawlerPath, "%s%s", pageDirectory, "/.crawler");
  }
  if (access(crawlerPath, F_OK) == -1) {
    return false;
  }
  return true;
}

bool pagedir_isFileWriteable(char* filePath)
{
  return (filePath != NULL && access(filePath, W_OK) == 0);
}

bool pagedir_isFileReadable(char* filePath)
{
  return (filePath != NULL && access(filePath, R_OK) == 0); 
}