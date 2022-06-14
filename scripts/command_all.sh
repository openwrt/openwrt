#! /bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
# Reduced version of which -a using command utility

case $PATH in
	(*[!:]:) PATH="$PATH:" ;;
esac

for ELEMENT in $(echo $PATH | tr ":" "\n"); do
	find -L $ELEMENT -name "$@" -type f '(' -perm -1 -o -perm -10 -o -perm -100 ')' 2> /dev/null
done
