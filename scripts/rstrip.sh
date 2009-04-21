#!/usr/bin/env bash
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

find_modparams() {
	FILE="$1"
	$NM "$FILE" | awk '
BEGIN {
	FS=" "
}
($3 ~ /^__module_parm_/) && ($3 !~ /^__module_parm_desc/) {
	gsub(/__module_parm_/, "", $3)
	printf "-K " $3 " "
}
($2 ~ /r/) && ($3 ~ /__param_/) {
	gsub(/__param_/, "", $3)
	printf "-K " $3 " "
}
'
}


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
	[ "${S}" = "relocatable" ] && {
		eval "$STRIP_KMOD -w -K '__param*' -K '__mod*' $(find_modparams "$F")$F"
	} || {
		b=$(stat -c '%a' $F)
		eval "$STRIP $F"
		a=$(stat -c '%a' $F)
		[ "$a" = "$b" ] || chmod $b $F
	}
  done
  true
)
