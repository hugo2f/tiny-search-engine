#!/bin/bash
# testing script for querier

# directory with tse data and sample outputs
tseDir="$HOME/cs50-dev/shared/tse/output"


# -----Invalid arguments-----
# number of arguments
./querier
./querier arg1 arg2 arg3
./querier too many arguments provided

# nonexistent pageDirectory
./indexer ./nonexistent ./index

# non-crawler pageDirectory
./querier ./ ./index

# nonexistent indexFilename
./querier $tseDir/letters-0 ./nonexistent/asdf.index

# -----System & Valgrind tests-----
# invalid.txt contains the following invalid queries:
#   1. starting "and"
#   2. starting "or"
#   3. ending "and"
#   4. ending "or"
#   5. consecutive "and"
#   6. consecutive "or"
#   7. adjacent "and"/"or"
#   8. non-alphabet, non-space characters
myvalgrind="valgrind --leak-check=full --show-leak-kinds=all"
$myvalgrind cat invalid.txt | ./querier $tseDir/letters-0 $tseDir/letters-0.index

# directories to tse output
letters2="${tseDir}/letters-2"
toscrape2="${tseDir}/toscrape-2"
wikipedia2="${tseDir}/wikipedia-2"
echo $letters2
echo $toscrape2
echo $wikipedia2

# tests below have long output
$myvalgrind ./fuzzquery "${letters2}.index" 10 0 | ./querier $letters2 "${letters2}.index"
$myvalgrind ./fuzzquery "${toscrape2}.index" 10 0 | ./querier $toscrape2 "${toscrape2}.index"
$myvalgrind ./fuzzquery "${wikipedia2}.index" 10 0 | ./querier $wikipedia2 "${wikipedia2}.index"
# $myvalgrind cat testing.txt | ./querier $tseDir/wikipedia-2 $tseDir/wikipedia-2.index
