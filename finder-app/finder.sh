#!/bin/sh

validNumArg=2

if [ $# -ne $validNumArg ]
then
  echo Total number of arguments should be $validNumArg
  exit 1
fi

filesDir=$1
searchStr=$2

if [ ! -d $filesDir ]
then
  echo The first argument is not directory
  exit 1
fi

numFiles=$(find $filesDir -type f | wc -l)
numMatchingLines=$(grep -r $searchStr $filesDir | wc -l)

echo The number of files are $numFiles and the number of matching lines are $numMatchingLines
