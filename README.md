## Hugo Fang - CS50 Winter 2024, Lab 4
GitHub username: hugo2f


## Assumptions & Notes
`indexer` and `querier` are commented out in the root Makefile, as they are not finished yet.

As per REQUIREMENTS.md, we assume `pageDirectory` to not contain any files whose name is an integer. If the `crawler` tries to write to a file that already exists, the file will be overwritten.

On any type of error, the crawler will exit with a corresponding errno described in `crawler.c`. In particular, if `pagedir_init()` returns false (see `pagedir.h` for details), the crawler will not run. When a given URL is unretrievable, the crawler will skip it and continue with any remaining URLs to explore.
 
