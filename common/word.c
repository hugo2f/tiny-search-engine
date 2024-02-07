/*
 * word.c    Hugo Fang    2/6/2024
 * 
 * See word.h for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void normalizeWord(char* word)
{
  if (word == NULL) {
    return;
  }
  for (int i = 0; word[i] != '\0'; i++) {
    if (isalpha(word[i])) {
      word[i] = tolower(word[i]);
    }
  }
}