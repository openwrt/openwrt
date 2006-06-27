#!/bin/sh
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


SELF=${0##*/}

[ -z "$STRIP" ] && {
  echo "$SELF: strip command not defined (STRIP variable not set)"
  exit 1
}

TARGETS=$*

[ -z "$TARGETS" ] && {
  echo "$SELF: no directories / files specified"
  echo "usage: $SELF [PATH...]"
  exit 1
}

find $TARGETS -type f -a -exec file {} \; | \
  sed -n -e 's/^\(.*\):.*ELF.*\(executable\|relocatable\|shared object\).*,.* stripped/\1:\2/p' | \
(
  IFS=":"
  while read F S; do
    echo "$SELF: $F:$S"
	[ "${F##*\.}" = "o" -o "${F##*\.}" = "ko" ] && \
		eval "$STRIP_KMOD $F" || \
		eval "$STRIP $F"
  done
)
