/* 
 * pagedir.h - header file for pagedir.c
 * 
 * Handles initializing and validating a pageDirectory,
 * writing and reading page files
 *
 * Hugo Fang, 1/29/2024
 */

#ifndef __PAGEDIR_H__
#define __PAGEDIR_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// libcs50
#include "webpage.h"

/*
 * Creates a .crawler file in pageDirectory to mark it as
 * edited by the crawler
 * 
 * Input:
 *   pageDirectory: directory for pages to be saved in
 * 
 * Returns:
 *   true if success
 *   false if any error occurs:
 *     pageDirectory is NULL or invalid, memory allocation failure,
 *     file creation failure
 */
bool pagedir_init(const char* pageDirectory);

/*
 * Creates a file to store web page contents, in the format (on each line):
 * 1. URL, 2. depth, 3. page contents
 * 
 * Input:
 *   page: webpage_t* containing the page content and metadata
 *   pageDirectory: directory to save the page in
 *   docID: name of file (int greater than 0)
 *   
 * Returns:
 *   true if success
 *   false if any error occurs:
 *     pageDirectory is NULL or invalid, page is NULL, docID <= 0, file creation/write failure
 */
bool pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/*
 * Loads a file saved by pagedir_save(), into a webpage_t*
 * 
 * Input:
 *   pageDirectory: directory to load the page from
 *   docID: name of file
 *   
 * Returns:
 *   webpage_t* containing file contents if success
 *   NULL if any error occurs:
 *     pageDirectory is NULL or invalid, page is NULL, docID <= 0, file read failure
 * 
 * Caller needs to call webpage_delete() on the webpage_t* returned
 */
webpage_t* pagedir_loadPageFromFile(const char* pageDirectory, const int docID);

/*
 * Reads the URL (first line) from a file
 * 
 * Input:
 *   pageDirectory: directory to load the page from
 *   docID: name of file
 *   
 * Returns:
 *   the URL if success
 *   NULL if any error occurs:
 *     pageDirectory is NULL or invalid, page is NULL, docID <= 0, file read failure
 * 
 * Caller needs to free() the string returned
 */
char* pagedir_loadUrlFromFile(const char* pageDirectory, const int docID);

/*
 * Checks for the existence of "pageDirectory/.crawler", which
 * marks a crawler generated directory
 * 
 * Input:
 *   pageDirectory: directory for the indexer
 *   
 * Returns:
 *   true if exists (is a crawler generated directory)
 *   false if pageDirectory is NULL or ".crawler" not found
 */
bool pagedir_isCrawlerDirectory(char* pageDirectory);

/*
 * Checks if a path points to a writeable file, or if the path is
 * in an writeable directory that allows creation of the file.
 * 
 * Input:
 *   filePath: path to file
 *   
 * Returns:
 *   true if writeable
 *   false if failure to create file at filePath
 */
bool pagedir_isFileWriteable(char* filePath);

/*
 * Checks if a path points to a existing, readable file
 * 
 * Input:
 *   filePath: path to file
 *   
 * Returns:
 *   true if readable
 *   false otherwise
 */
bool pagedir_isFileReadable(char* filePath);

#endif // __PAGEDIR_H__
