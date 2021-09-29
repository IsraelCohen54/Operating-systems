#!/bin/bash
# Israel Cohen

if [ "$#" -ne 3 ];
  then
  echo "Not enough parameters"
  exit 1
fi

cd $1

for i in $((ls)|LC_ALL=C sort)
do
  if [[ "$i" == *"$2"* ]];
  then
      cat $i |grep -iw $3
  fi
done
