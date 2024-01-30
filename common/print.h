/* 
 * print.h - header file for print.c
 * 
 * Wrapper functions for printing strings to stdout and stderr,
 * with and without newlines 
 *
 * Hugo Fang, 1/29/2024
 */

#ifndef __PRINT_H__
#define __PRINT_H__
#include <stdio.h>
#include <stdlib.h>

/*
 * print str without newline
 */
void print(char* str);

/*
 * print str with newline
 */
void println(char* str);

/*
 * print str to stderr without newline
 */
void printerr(char* str);

/*
 * print str to stderr with newline
 */
void printerrln(char* str);

#endif //  __PRINT_H__
