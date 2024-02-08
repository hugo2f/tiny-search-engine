/*
 * indextest.c    Hugo Fang    2/7/2024
 * 
 * Reads from a file created by the indexer, loads contents into an index,
 * and writes the index into a new file.
 * 
 * Usage: ./indextest oldIndexFilename newIndexFilename
 * 
 * Exits with:
 *   0 if normal return
 *   errno 1 if wrong number of arguments
 *   errno 2 if error reading from or writing to files
 *   
 */

#include <stdio.h>
#include <stdlib.h>

// common.a
#include "pagedir.h"
#include "index.h"

int main(const int argc, char* argv[])
{
  // one for "./indextest", two for arguments
  if (argc != 3) {
    exit(1);
  }
  char* oldFilename = argv[1];
  char* newFilename = argv[2];
  index_t* idx = index_readIndexFile(oldFilename);
  if (idx == NULL) {
    exit(2);
  }
  index_saveToFile(idx, newFilename);
  index_delete(idx);
  return 0;
}
