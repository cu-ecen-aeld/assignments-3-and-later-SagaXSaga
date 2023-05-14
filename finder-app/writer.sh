#!/bin/sh

writefile=$1;
writestr=$2;

if [ "$writefile" = "" ]; then
    echo "need 2 arguments"
    exit 1
elif [ "$writestr" = "" ]; then
    echo "need 2 arguments"
    exit 1
elif [ "$#" -ne 2 ]; then
    echo "need 2 arguments"
    exit 1
else 
    mkdir -p "$(dirname "$writefile")"
    echo "$writestr" > "$writefile"
    exit 0
fi

