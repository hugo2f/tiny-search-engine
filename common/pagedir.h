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
 *   pageDirectory: file to load the page from
 *   docID: name of file (int greater than 0)
 *   
 * Returns:
 *   webpage_t* containing file contents if success
 *   NULL if any error occurs:
 *     pageDirectory is NULL or invalid, page is NULL, docID <= 0, file read failure
 */
webpage_t* pagedir_loadPageFromFile(const char* pageDirectory, const int docID);

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
 * in an existent directory that allows creation of the file.
 * File contents will be cleared if it exists and writeable
 * 
 * Input:
 *   filePath: path to file
 *   
 * Returns:
 *   true if writeable
 *   false if failure to create file at filePath
 */
bool pagedir_isPathWriteable(char* filePath);

#endif // __PAGEDIR_H__
