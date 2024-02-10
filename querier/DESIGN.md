# CSS50 TSE Querier

## Design Spec

According to the requirements spec, the TSE `querier` reads in an index file produced by the TSE Indexer, the page files produced by the TSE Crawler, and answers search queriers in stdin.


### User interface
The querier is executed on the command line with exactly two arguments:
```
./querier pageDirectory indexFilename
```

For example,
```
./querier ../data/letters ../data/letters.index
```
where `letters` contains crawled pages, and `letters.index` contains the index.


### Inputs and outputs
**Input:** the querier reads search queries from stdin, one per line, until EOF.

**Output:** for each query, print to stdout the query result. For detailed query and output formats, see the Requirements.


### Functional decomposition into modules
We anticipate the following modules or functions:
1. `main`, which parses arguments and initializes other modules 
2. `processQuery`, which processes a query and outputs the search results
3. `intersectCounters`, which finds the intersection between two `counters`, setting the `score` items to be the minimum of the scores in each counter
4. `unionCounter`, which finds the union of two `counters`, setting the `score` items to be the sum of scores in each counter
5. `sortByScore`, which sorts a list of pages in decreasing order of score
# TODO: check this after

And some helper modules:
1. `index`, which provides the data structure to represent an index, and functions to read and write index files
2. `word`, which provides the functions to verify the format of and tokenize a query
# TODO: add anything missing


### Pseudocode for logic/algorithmic flow
The querier will run as follows:
```
parse the command line, validate arguments, initialize other modules
call parseQuery on each line from stdin
```
where `parseQuery`:
```
verifies the query format
builds the list of search results for the query
sorts and outputs the results
```


### Major data structures
For details regarding the representation of the `index`, see Major data structures in `indexer`'s design spec.

The query results are stored in a `counters`, with `docID` keys and `score` items. For a word in the query, the results are stored in the `counters` item in the index's `hashtable`. These results are intersected and unioned together to form the query results.


### Testing plan

#### Integration testing
#TODO: fuzzquery?
The `indexer` will be tested by

1. Test `querier` with various invalid arguments:
  a. no arguments
  b. one argument
  c. three or more arguments
  d. nonexistent `pageDirectory`
  e. non-crawler `pageDirectory`
  f. nonexistent `indexFilename`
  g. `indexFilename` is in read-only directory
  h. `indexFilename` is read-only
2. Run `querier` on a variety of `pageDirectories` and corresponding `indexFilenames`
3. Run `valgrind` on the above tests to ensure no memory leaks or errors
