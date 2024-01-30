/* 
 * pagedir.h - header file for playground.c
 * 
 * Wrapper functions for printing strings to stdout and stderr,
 * with and without newlines 
 *
 * Hugo Fang, 1/29/2024
 */

#ifndef __PAGEDIR_H__
#define __PAGEDIR_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
 * Input: TODO
 *   pageDirectory: directory to save the page in
 *   fileName: name of file (a number)
 *   
 * Returns:
 *   true if success
 *   false if any error occurs:
 *     pageDirectory is NULL or invalid, page is NULL, fileID <= 0 (IDs should
 *     start from 1), file creation/write failure
 */
bool pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H__
