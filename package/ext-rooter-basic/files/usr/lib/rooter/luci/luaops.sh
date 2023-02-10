#!/bin/sh

COMMAND=$1
FILE=$2

if [ $COMMAND = delete ]; then
	rm -f $FILE
fi