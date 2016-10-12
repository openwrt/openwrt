#!/usr/bin/env bash
export LANG=C
export LC_ALL=C
[ -n $1 ] && cd $1

try_git() {
	[ -e .git ] || return 1
	REV="$(git log | grep -m 1 commit | awk '{print substr ($2, 0, 7)}')"
	[ -n "$REV" ]
}

try_git || REV="unknown"
echo "$REV"
