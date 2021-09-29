#!/bin/bash
# Israel Cohen

if [ "$#" -ne 4 ];
  then
  echo "Not enough parameters"
  exit 1
fi

path_to_scripts=$(pwd)"/ex21.sh"
function check_files () {
var=$($path_to_scripts $1 $2 $3) 
IFS=$'\n'

for item in $var
do  
  num=`eval "echo -e $item | wc -w"`;
  if (( 1+$num > $4 ))
  then
    #echo "resultd:" 
    echo $item
  fi
done
}

check_files $1 $2 $3 $4


cd $1

function recurssion () {
for i in $((ls)|LC_ALL=C sort);
do
  if   [ -d "$i" ]; #if its dir
  #here - If i$ is dir, enter
  then
    check_files $i $2 $3 $4
    cd $i
    recurssion $1 $2 $3 $4
    
  fi
done

#go up in path and save curr dir name:
dirname=$(pwd)
#echo 'dirname= '$dirname
result="${dirname%"${dirname##*[!/]}"}"
i="${result##*/}"

cd ..
return    
}
recurssion $1 $2 $3 $4
