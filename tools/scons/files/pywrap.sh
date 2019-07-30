#!/usr/bin/env bash

case "${0##*/}" in
	pywrap.sh) arg1="";;
	*) arg1="$0.py" ;;
esac

for bin in python python3; do
    case "$($bin -V 2>&1)" in
        "Python 3"*) exec $bin $arg1 "$@" ;;
    esac
done

echo "Unable to find a Python 3.x interpreter for executing ${arg1:+$arg1 }$@ !" >&2
exit 1
