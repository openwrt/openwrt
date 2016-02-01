#!/usr/bin/env bash
export LANG=C
export LC_ALL=C
[ -n "$TOPDIR" ] && cd $TOPDIR

try_version() {
	[ -f version.date ] || return 1
	SOURCE_DATE_EPOCH="$(cat version.date)"
	[ -n "$SOURCE_DATE_EPOCH" ]
}

try_svn() {
	[ -d .svn ] || return 1
	SOURCE_DATE_EPOCH="$(./scripts/portable_date.sh "$(LC_ALL=C svn info | sed -ne 's/^Last Changed Date: //p')" +%s)"
	[ -n "$SOURCE_DATE_EPOCH" ]
}

try_git() {
	[ -e .git ] || return 1
	SOURCE_DATE_EPOCH="$(git log -1 --format=format:%ct)"
	[ -n "$SOURCE_DATE_EPOCH" ]
}

try_hg() {
	[ -d .hg ] || return 1
	SOURCE_DATE_EPOCH=""
	[ -n "$SOURCE_DATE_EPOCH" ]
}

try_version || try_svn || try_git || try_hg || SOURCE_DATE_EPOCH=""
echo "$SOURCE_DATE_EPOCH"
