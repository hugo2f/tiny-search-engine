#!/bin/bash

make clean
make all

# backup data
directory="../data"
if [ -d "$directory" ]; then
  mv -r $directory ${directory}_backup
fi

# Invalid arguments:
# number of arguments
./crawler arg1 arg2 arg3 arg4
./crawler arg
# bad seedURL, pageDirectory, maxDepth, nonexistent seedURL
mkdir -p ../data/args
./crawler http://www.4399.com/ ../data/args 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ./nonexistent 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/args -1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/nonexistent/ ../data/args 1
# rm -rf ../data/args


# # Valgrind test
# rm -rf ../data
# mkdir -p ../data/valgrind
# valgrind --leak-check=full --show-leak-kinds=all ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/valgrind 1


# letters, toscrape, wikipedia
rm -rf ../data/letters ../data/toscrape ../data/wikipedia
mkdir ../data/letters ../data/toscrape ../data/wikipedia
# letters - 0 TODO: what if I don't rm -rf?
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 0
# rm -f ../data/letters/*
# letters - 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 1
rm -f ../data/letters/*
# # letters - 2
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 2
# rm -f ../data/letters/*
# # letters - 10
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 10
# rm -f ../data/letters/*


# # toscrape - 0
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape 0
# rm -f ../data/toscrape/*
# # toscrape - 1
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape 1
# rm -f ../data/toscrape/*
# # toscrape - 2
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape 2
# rm -f ../data/toscrape/*
# # toscrape - 3
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape 3
# rm -f ../data/toscrape/*



# # wikipedia - 0
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia 0
# rm -f ../data/wikipedia/*
# # wikipedia - 1
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia 1
# rm -f ../data/wikipedia/*
# # wikipedia - 2
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia 2
# rm -f ../data/wikipedia/*

# TODO: try different seed urls in each of letters, toscrape, wikipedia