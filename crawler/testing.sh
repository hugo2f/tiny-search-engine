#!/bin/bash

make all

# backup data
directory="../data"
if [ -d "$directory" ]; then
  rm -rf ${directory}_backup
  mv $directory ${directory}_backup
fi

# -----Invalid arguments-----
argDir="../data/args"
mkdir -p $argDir

# number of arguments
./crawler arg1 arg2 arg3 arg4
./crawler arg

# bad seedURL
./crawler asdf.fdsa.com $argDir 1
# external seedURL
./crawler http://www.4399.com/ $argDir 1
# nonexistent seedURL
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/nonexistent/ $argDir 1

# nonexistent pageDirectory
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ./nonexistent 1

# non-integer maxDepth
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ $argDir NAN
# negative maxDepth
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ $argDir -1


# -----Valgrind test-----
# valgrindDir="../data/valgrind"
# mkdir -p $valgrindDir
# valgrind --leak-check=full --show-leak-kinds=all ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ $valgrindDir 1


# -----System tests-----
lettersDir="../data/letters"
toscrapeDir="../data/toscrape"
wikiDir="../data/wikipedia"
mkdir $lettersDir $toscrapeDir $wikiDir

# ---letters---
# letters - maxDepth 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 0

# letters - maxDepth 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 1

# letters - maxDepth 2
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 2

# letters - maxDepth 10
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 10


# ---toscrape---
toscrape - maxDepth 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape 0

# toscrape - maxDepth 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape 1

# # toscrape - maxDepth 2
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape 2

# # toscrape - maxDepth 3
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape 3


# ---wikipedia---
# wikipedia - maxDepth 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia 0

# wikipedia - maxDepth 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia 1

# # wikipedia - maxDepth 2
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia 2
