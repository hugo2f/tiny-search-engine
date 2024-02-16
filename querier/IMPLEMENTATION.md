## Implementation Spec

In this document, the following implementation details are described:
* Data structures
* Control flow
* Detailed function prototypes and parameters
* Eror handling and recovery
* Testing plan

## Data structures

The `querier` answers search queries using an `index`. See [`index.h`](../common/index.h) and the [indexer implementation spec](../indexer/IMPLEMENTATION.md) directory for details about the representation of the index.

## Control flow
The `querier` is implemented in `querier.c` with four functions:

### main
The `main` function calls `parseArgs`, then `prompt`s for queries, calling `processQuery` on each query, until EOF.

### parseArgs
Given command line arguments, extract them into the function parameters. Returns only if successful.
* for `pageDirectory`, checks that the directory exists and contains a `.crawler` file
* for `indexFilename`, verify that the path is a readable file, and can be read into an `index`
* on any error, print a message to stderr and exit non-zero

### prompt
Prints a prompt to stdout, asking for a query, if stdin is a tty.

### processQuery
Given a query, compute the matching documents, sort, and output the results. Pseudocode:
```
strip and compact spaces, normalize the query
initialize res and temp counters
for each word in the query
    if read word
        intersect counter for that word into temp
    else if read "or"
        union temp into res
union temp into res
call getSortedResults on res
output sorted results
delete temp, res
```

The following functions to intersect and union counters, and to sort and output the query results, are also implemented:

### isValidQuery
Given a query, return whether the query is valid
```
check query contains only isalpha and isspace characters
check query does not start/end with "and"/"or", nor contains
  consecutive "and"/"or"
```

### getSortedResults
Given a counter, return a sorted array of (docID, score) pairs in descending score order. `querier.c` defines a `queryResult` struct to hold a (docID, score) pair, and a `queryResArr` struct to hold a `queryResult**` array with the next `pos` to insert a pair at.
```c
typedef struct queryResult {
  int docID, score;
} queryResult_t;

typedef struct queryResArr {
  queryResult_t** arr;
  int pos;
} queryResArr_t;
```
```
get counter size using counters_iterate
initialize a queryResArr with size equal to counter size
convert the counter into a queryResArr
sort the queryResArr
```

### addIntoArray
Helper function to convert counter into queryResArr, passed into `counters_iterate`.
```
initialize a queryResult with the (docID, score) pair
update queryResArr's pos item to be the queryResult
increment pos
```

### outputQueryResults
Output a query result line corresponding to each result in a `queryResArr`.
```
print number of documents matched
for each result in queryResArr, print:
  score, docID, url
```

### queryResArr_delete
Free all memory in a `queryResArr` by freeing each `queryResult` in the array, the array pointer itself, and the `queryResArr` pointer.

### copyCounter
Returns a copy of a counter. Used to copy counters in the index to protect the contents of the index.

### copyCounterHelper
Helper function to set a (key, score) pair in the copy (passed in as the `arg`).

### intersectCounters
Merge a counter `from` into another counter `to`, taking the minimum of the scores for each key. If a key isn't present in a counter, the score becomes 0.
```
call counters_iterate on `to` to update each score
replace `to` with a new counter that copies over only
  entries with nonzero score
```

### mergeMinCount
When iterating through each entry in `to`, the helper function needs to update the score for each key with the key's score in `from`. Thus, a struct `counterPair` of both counters is passed into `counters_iterate`, allowing `mergeMinCount` to access both counters.
```c
typedef struct counterPair {
  counters_t* from;
  counters_t* to;
} counterPair_t;
```
```
get `to` and `from` in the counterPair
get the key's score in `from`
set the key's score in `to` to be the minimum of both scores
```

### copyIfNonzero
Helper function passed into `counters_iterate`, setting the (key, score) pair in the copy (passed in as the `arg`) only if the score > 0.

### unionCounters
Merge a counter `from` into another counter `to`, summing the scores for each key. The score is 0 if a key isn't found in a counter.
```
Call mergeAddCount on `from` with `to` as the argument
```

### mergeAddCount
Helper function for `unionCounter`. For a key in `from`, add the corresponding count to `to`'s count for that key.


## Other modules

### pagedir
In addition to the `pagedir` functions implemented for the crawler and indexer, several new helper functions are added for file related operations used by the querier.

`pagedir_loadUrlFromFile`: reads the first line (the URL) from a crawler-generated file.
```
given a docID, construct the pathname for the corresponding file
read and return the URL
```

`pagedir_isFileReadable`: checks if a path points to an existing, readable file.


### index
In the querier, an index is used to store and get counters corresponding to each word. For more details, refer to the [indexer implementation spec](../indexer/IMPLEMENTATION.md#index). The querier implements a getter to retrieve the counter for a given word.


### word
This module contains functions preprocessing and extracting words from the query.

`stripCompactNormalize`: removes leading and trailing spaces, and replaces consecutive spaces or tabs (satisfying `isspace`) with a single space character. Modifies the string in place.

`onlyAlphaSpaces`: returns whether a string contains only `isalpha` and `isspace` characters, used to validate a query.

`nextWord`: given a string and a position in it, returns a pointer to the next word in the string, terminated by `\0`. The string is modified in place.

### libcs50
The querier uses the `counters` module in libcs50, as they store (docID, count) pairs in the index entries. Refer to the corresponding header files for details.

## Function and struct prototypes

### querier
Refer to `querier.c` for more details regarding each function.

```c
int main(const int argc, char* argv[]);
int fileno(FILE* stream);
static index_t* parseArgs(const int argc, char* argv[], char** pageDirectory_p);
static void queryResArr_delete(queryResArr_t* resArr);

// processing queries
static void prompt();
static void processQuery(const index_t* idx, char* query, const char* pageDirectory);
static bool isValidQuery(char* query);
static bool isAndOr(char* word);

// processing results
static queryResArr_t* getSortedResults(counters_t* res);
static void getCounterSize(void* arg, const int key, const int count);
static void addIntoArray(void* arg, const int key, const int count);
static int compareQueryResult(const void* a, const void* b);
static void outputQueryResults(const queryResArr_t* resArr, const char* pageDirectory);

// combining counters
static void intersectCounters(counters_t* from, counters_t** to_p);
static void mergeMinCount(void* arg, const int key, const int item);
static void copyIfNonzero(void* arg, const int key, const int item);

static void unionCounters(counters_t* from, counters_t* to);
static void mergeAddCount(void* arg, const int key, const int item);

static counters_t* copyCounter(counters_t* counter);
static void copyCounterHelper(void* arg, const int key, const int item);
```

### index
Refer to `index.h` for details.
```c
counters_t* index_getWord(const index_t* idx, const char* word);
```

### pagedir
Refer to `pagedir.h` for details.

```c
char* pagedir_loadUrlFromFile(const char* pageDirectory, const int docID);
bool pagedir_isFileReadable(char* filePath);
```

### word
Refer to `word.h` for details.
```c
char* stripCompactNormalize(char* string);
char onlyAlphaSpaces(char* string);
char* nextWord(char* string, int* pos);
```

## Error handling and recovery

Command line arguments are checked by `parseArgs` before the querier starts processing queries. Any errors will be printed to stderr and the querier exits with non-zero status.

All functions will check for NULL pointers before executing. If there is not enough memory to initiate a new index, the querier will exit normally without doing anything. If a query is invalid, a message will be printed containing the issue, and the querier will continue to prompt for queries.

When outputting search results, if the querier fails to read a URL from a file, it will print a message to stderr and continue outputting the rest of the results.

## Testing plan

### System testing
`testing.sh` will first test `querier` with invalid arguments, including:
* invalid numbers of arguments
* nonexistent/invalid (non-crawler) `pageDirectory`
* nonexistent `indexFilename`

It then runs `querier` on different valid `pageDirectories` and corresponding `indexFilenames`, using `fuzzquery` to generate queries by combining words in `indexFilename`. These tests are run with `valgrind` to ensure there are no memory leaks or errors.
