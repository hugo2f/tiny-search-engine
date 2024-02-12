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
#include <ctype.h>

/*
 * Normalizes a word by turning all characters to lower case, in-place.
 * Non-alphabet characters are ignored.
 * 
 * Input:
 *   word to normalize
 */
char* normalizeWord(char* word);

/*
 * Strips leading and trailing spaces, replaces consecutive spaces and tabs
 * with a single space.
 * 
 * Input:
 *   string to strip
 * 
 * Returns:
 *   pointer to the start of the stripped string.
 *   NULL if the string is all spaces
 * 
 * Caller needs to free() the original string
 */
char* stripAndCompactSpaces(char* string);

/*
 * Checks if a string contains only characters that satisfy
 * isalpha() and isspace()
 * 
 * Input:
 *   string to check
 * 
 * Returns:
 *   the first non-alphabet, non-space character
 */
char onlyAlphaSpaces(char* string);

/*
 * Given a string of space-separated words, get the next word (could be empty).
 * The string is modified in place to normalize the word and insert a '\0' after
 * the word, and pos is updated to be the index after the '\0'.
 * 
 * Input:
 *   string to read from
 *   pos: current index in string
 * 
 * Returns:
 *   pointer to the next word
 *   NULL if pos is at the end of the string
 */
char* nextWord(char* string, int* pos);

#endif // __WORD_H__