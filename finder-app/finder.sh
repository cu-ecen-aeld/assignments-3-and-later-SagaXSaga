#!/bin/sh

filesdir=$1;
searchstr=$2;

if [ "$#" -ne 2 ]; then
    echo "need 2 arguments"
    exit 1
elif ! [ -d "$filesdir" ]; then
    echo "no such directory" 
    exit 1
else
    X=$(grep -rl $searchstr $filesdir | wc -l)
    Y=$(grep -r $searchstr $filesdir | wc -l) 
    echo "The number of files are $X and the number of matching lines are $Y"
    exit 0
fi
