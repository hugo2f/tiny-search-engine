## Implementation Spec

In this document, the following implementation details are described:
* Data structures
* Control flow
* Detailed function prototypes and parameters
* Eror handling and recovery
* Testing plan

## Data structures

An `index` is represented by a `hashtable` with `char* word` keys for words at least 3 characters long. Each `word` corresponds to a `counters`, which stores `int docID` keys and `int count` items. Thus, each `word` will be mapped to the number of occurences in each page (stored in file `docID`). See below for implementation details of `index.c`.

## Control flow
The `indexer` is implemented in `indexer.c` with four functions:

### main
The `main` function calls `parseArgs`, builds the index with `indexBuild`, and finally saves it to a local file with `index_saveToFile`. By default, `main` returns 0.

### parseArgs
Given command line arguments, extract them into the function parameters. Returns only if successful.
* for `pageDirectory`, checks that the directory exists and contains a `.crawler` file
* for `indexFilename`, verify that the path is either a writeable directory, or is the path to a writeable file
* on any error, print a message to stderr and exit non-zero

### indexBuild
Build an index from the files in `pageDirectory`. Pseudocode:
```
initialize new index_t*
initialize docID from 1
initialize webpage_t*
while current docID points to an existing page file
    load webpage from file
    call indexPage() with the webpage and docID
    delete the webpage
    increment docID
return the index
```

### indexPage
Given a `webpage`, update the index with `index_addWord()` for every word in the webpage's html. Pseudocode:
```
while webpage_getNextWord() returns a word
    call index_addWord() with the word and docID
    free the word
```

## Other modules

### pagedir
In addition to the `pagedir` functions implemented for the crawler, several new helper functions are added for file related operations used by the indexer.

`pagedir_loadPageFromFile`: creates a webpage from the contents in a file generated by `pagedir_save()`.
```
given a docID, construct the pathname for the corresponding file
read the URL, depth, and html from the file
if any part cannot be read, return NULL
otherwise, return a webpage initialized with the file contents
```

`pagedir_isCrawlerDirectory`: checks if a directory is marked by the crawler with a `.crawler` file.
```
construct the pathname for the .crawler file in the directory
return whether such a file exists
```

`pagedir_isFileWriteable`: checks if a path is writeable


### index
This module defines a `struct index` and provides wrapper functions around a `hashtable` to update an index, including adding a word from a document, saving to and reading from an index file, and some private helper functions.

`struct index`: contains a `hashtable` of `char*` keys and `counters_t*` items, which will look like
```
typedef struct index {
  hashtable_t* ht;
} index_t
```

`index_new`: initialize a new index.
```
create a new index_t* pointer
initialize the index's hashtable to a default of 500 slots
return the index
```

`index_delete`: free all memory used by the index.
```
call hashtable_delete() on the index's hashtable
free the index_t* pointer
```

`counterDelete`: wrapper for `counters_delete()` for use by `hashtable_delete()`.

`index_addWord`: given a word that appears in file `docID`, update the index.
```
ignore words that have length < 3
call normalizeWord() on the word
find the counter for the word in the index, initialize a new
  counter and add into hashtable if first time adding the word
increment the count of docID in the counter
```

`index_readIndexFile`: loads the contents of a file into an index.
```
initialize an index with 1.5 * number of words slots in the hashtable
for each line in the file
    tokenize the line to get the word
    read (docID, count) pairs
    for the word, set the count of docID
    free the line
return the index
```

`index_newWithNumSlots`: helper function to initialize an index with a certain number of slots in the hashtable.
```
create a new index_t* pointer
initialize the index's hashtable with the number of slots
return the index
```

`str2int`: converts a string to an int, returns whether conversion is successful.
```
read an int and any extra characters from the string
return whether exactly one int is read, without trailing characters
```

`index_setWordDocCount`: helper function to set a specific (docID, count) pair for a word.
```
find the counter for the word in the index, initialize a new
  counter and add into hashtable if first time adding the word
set the count of docID in the counter
```

`index_saveToFile`: save an index to a file, where each line consists of a word and the corresponding (docID, count) pairs.
```
call hashtable_iterate() to save each word entry to a file
```

`saveHashtableEntry`: helper function to save an entry in the hashtable.
```
print the word
call counters_iterate() on the counter stored in the entry
```

`saveCounterEntry`: helper function passed into `counters_iterate`, which prints the (docID, count) pairs on the same line.

### word
This module contains functions for processing words found in the HTML of a webpage.

`normalizeWord`: turns all alphabet characters in a word to lower case, modified in-place.

### libcs50
The indexer uses the `hashtable`, `counters`, and `webpage` modules in libcs50. Refer to the corresponding header files for details. `webpage` is mainly used to store and extract the contents of files in the `pageDirectory`.

## Function prototypes

### indexer
Refer to `indexer.c` for more details regarding each function.
```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory_p, char** indexFilename_p);
index_t* indexBuild(const char* pageDirectory);
static void indexPage(index_t* idx, webpage_t* page, const int docID);
```

### pagedir
Refer to `pagedir.h` for details.

```c
webpage_t* pagedir_loadPageFromFile(const char* pageDirectory, const int docID);
bool pagedir_isCrawlerDirectory(char* pageDirectory);
bool pagedir_isFileWriteable(char* filePath);
```

### index
Refer to `index.h` for details.
```c
index_t* index_new();
void index_delete(index_t* idx);
void index_addWord(index_t* idx, char* word, const int docID);
index_t* index_readIndexFile(const char* filePath);
void index_saveToFile(index_t* idx, const char* filePath);
```

### word
Refer to `word.h` for details.
```c
char* normalizeWord(char* word);
```

## Error handling and recovery

Command line arguments are checked by `parseArgs` before the indexer processes any pages. Any errors will be printed to stderr and the indexer exits with non-zero status.

All functions will check for NULL pointers before executing. If there is not enough memory to initiate a new index, the indexer will exit normally without doing anything. When indexing a page, the indexer will skip the current page on memory allocation failures or formatting errors in the file.

## Testing plan

### Regression testing
`testing.sh` will save the indexes produced in the `indexes` directory, and will create a backup if such a directory already exists. This allows comparison between the results produced from different runs

### System testing
`testing.sh` will first test `indexer` with invalid arguments, including:
* invalid numbers of arguments
* nonexistent/invalid (non-crawler) `pageDirectory`
* nonexistent `indexFile`
* `indexFile` is in read-only directory, or is a read-only file

It then runs `indexer` on different valid `pageDirectories`, and uses `indextest` to check if `index.c` properly handles writing to and reading from index files. These tests are run with `valgrind` to ensure there are no memory leaks or errors.
