#!/bin/bash
# testing script for indexer

# directory with tse data and sample outputs
tseDir="$HOME/cs50-dev/shared/tse"


# -----Invalid arguments-----
# number of arguments
./indexer
./indexer arg1 arg2 arg3
./indexer too many arguments provided

# nonexistent pageDirectory
./indexer ./nonexistent ./data

# non-crawler pageDirectory
./indexer ./ ./data

# invalid indexFilename
./indexer $tseDir/output/letters-0 ./nonexistent/asdf.index

# indexFilename is in readonly directory
mkdir ./readOnlyDir
chmod a=r ./readOnlyDir
./indexer $tseDir/output/letters-0 ./readOnlyDir
rm -rf ./readOnlyDir

# indexFilename is readonly TODO: try directories with and without slash
touch ./readOnlyFile
chmod a=r ./readOnlyFile
./indexer $tseDir/output/letters-0 ./readOnlyFile
rm -rf ./readOnlyFile


# -----Functional & Valgrind tests-----
# backup indexes
directory="../indexes"
if [[ -d "$directory" ]]; then
  rm -rf ${directory}_backup
  mv $directory ${directory}_backup
fi

mkdir $directory
myvalgrind="valgrind --leak-check=full --show-leak-kinds=all"

# indexer (create the index, then compare with sample output using indexcmp)
$myvalgrind ./indexer $tseDir/output/letters-0 ../indexes/letters-0.index
$tseDir/indexcmp ${directory}/letters-0.index $tseDir/output/letters-0.index

$myvalgrind ./indexer $tseDir/output/letters-2 ../indexes/letters-2.index
$tseDir/indexcmp ${directory}/letters-2.index $tseDir/output/letters-2.index

# $myvalgrind ./indexer $tseDir/output/toscrape-2 ../indexes/toscrape-2.index
# $tseDir/indexcmp ${directory}/toscrape-2.index $tseDir/output/toscrape-2.index

# $myvalgrind ./indexer $tseDir/output/wikipedia-2 ../indexes/wikipedia-2.index
# $tseDir/indexcmp ${directory}/wikipedia-2.index $tseDir/output/wikipedia-2.index


# indextest (copy from `name`.index to `name`-copy.index, then compare the two versions)
$myvalgrind ./indextest ../indexes/letters-0.index ../indexes/letters-0-copy.index
$tseDir/indexcmp ../indexes/letters-0.index ../indexes/letters-0-copy.index

$myvalgrind ./indextest ../indexes/letters-2.index ../indexes/letters-2-copy.index
$tseDir/indexcmp ../indexes/letters-2.index ../indexes/letters-2-copy.index

# $myvalgrind ./indextest ../indexes/toscrape-2.index ../indexes/toscrape-2-copy.index
# $tseDir/indexcmp ../indexes/toscrape-2.index ../indexes/toscrape-2-copy.index

# $myvalgrind ./indextest ../indexes/wikipedia-2.index ../indexes/wikipedia-2-copy.index
# $tseDir/indexcmp ../indexes/wikipedia-2.index ../indexes/wikipedia-2-copy.index