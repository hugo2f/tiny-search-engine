/*
 * word.c    Hugo Fang    2/6/2024
 * 
 * See word.h for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

char onlyAlphaSpaces(char* string)
{
  for (char* ptr = string; *ptr != '\0'; ptr++) {
    if (!isalpha(*ptr) && !isspace(*ptr)) {
      return *ptr;
    }
  }
  return '\0';
}

char* nextWord(char* string, int* pos)
{
  // skip leading spaces
  while (isspace(string[*pos])) {
    (*pos)++;
  }

  // nothing to read
  if (string[*pos] == '\0') {
    return NULL;
  }

  // starting index of word
  int start = *pos;
  while (string[*pos] != '\0' && isalpha(string[*pos])) {
    (*pos)++;
  }

  // if haven't reached end of the word yet,
  // set current char to '\0' to cut off new word
  if (string[*pos] != '\0') {
    string[*pos] = '\0';
    (*pos)++;
  }
  return &string[start];
}