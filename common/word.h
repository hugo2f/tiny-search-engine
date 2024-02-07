/* 
 * word.h - header file for word.c
 * 
 * Processes words read from a page's contents
 *
 * Hugo Fang, 2/6/2024
 */

#ifndef __WORD_H__
#define __WORD_H__
#include <stdio.h>
#include <stdlib.h>

/*
 * Normalizes a word by turning all characters to lower case.
 * Non-alphabet characters are ignored
 * 
 * Input:
 *   word to normalize
 * 
 * word is modified in-place
 */
char* normalizeWord(char* word);

#endif // __WORD_H__