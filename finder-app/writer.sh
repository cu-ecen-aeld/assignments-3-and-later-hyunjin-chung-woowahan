#1/bin/sh

validNumArg=2

if [ $# -ne $validNumArg ]
then
  echo Total number of arguments should be $validNumArg
  exit 1
fi

writeFile=$1
writeStr=$2

if [ -e $writeFile ]
then
  if [ -d $writeFile]
  then
    echo $writeFile is directory
	exit 1
  fi
else
  mkdir -p $(dirname $writeFile)
fi

echo $writeStr > $writeFile

if [ $? -ne 0 ]
then
  echo Writting "$writeStr" string to $writeFile is failed
  exit 1
fi
