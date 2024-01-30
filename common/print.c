/*
 * print.c    Hugo Fang    1/18/2024
 * 
 * Prints things to stdout and stderr
 * 
 * See print.h for documentation
 */

#include <stdio.h>
#include <stdlib.h>
#include "print.h"

void print(char* str)
{
  printf("%s", str);
}

void println(char* str)
{
  puts(str);
}

void printerr(char* str)
{
  fputs(str, stderr);
}

void printerrln(char* str)
{
  fprintf(stderr, "%s\n", str);
}