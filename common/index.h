/* 
 * index.h - header file for index.c
 * 
 * Data structure to represent the index, with functions to
 * read and write index files
 * 
 * Maps `word` keys to (docID, count) pairs, each pair representing
 * how many times `word` appears in the document
 *
 * Hugo Fang, 2/6/2024
 */

#ifndef __INDEX_H__
#define __INDEX_H__
#include <stdio.h>
#include <stdlib.h>

typedef struct index index_t;

/*
 * Allocate and initialize a new index_t structure
 *
 * Returns:
 *   pointer to new index_t, or NULL on any error
 * 
 * Caller is responsible for calling index_delete() on the returned pointer
 */
index_t* index_new();

/*
 * Delete an index_t structure created by index_new()
 * 
 * Input:
 *   pointer to the index_t
 */
void index_delete(index_t* idx);

/*
 * Update the index with an occurence of a word in a page.
 * Ignores words with length < 3 and normalizes (convert to
 * lower case) the word in-place.
 *
 * Input:
 *   idx: index to update
 *   word to add
 *   docID: file corresponding to the page containing `word`
 * 
 * The contents of `idx` will be undetermined if errors such as memory
 * allocation failure occur
 */
void index_addWord(index_t* idx, char* word, const int docID);

/*
 * Read a file, in the format written by index_saveToFile(), into an index_t
 *
 * Input:
 *   filePath: path to file to read from
 * 
 * Returns:
 *   index_t* containing the information read
 *   NULL if filePath is NULL or any error occurs
 * 
 * If the file doesn't follow the format written by index_saveToFile(),
 * the contents of the returned index will be undetermined
 */
index_t* index_readFile(const char* filePath);

/*
 * Write an index's contents into a file
 *
 * Input:
 *   index: index_t* of index to store the contents of
 *   filePath: path to file to write in
 * 
 * Nothing happens if any parameters are NULL, or filePath is unwriteable
 */
void index_saveToFile(index_t* idx, const char* filePath);

#endif // __INDEX_H__