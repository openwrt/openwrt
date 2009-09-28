#!/usr/bin/env bash
export LANG=C
export LC_ALL=C
[ -n "$TOPDIR" ] && cd $TOPDIR

try_version() {
	[ -f version ] || return 1
	REV="$(cat version)"
	[ -n "$REV" ]
}

try_svn() {
	[ -d .svn ] || return 1
	REV="$(svn info -r COMMITTED | awk '/^Revision:/ { print $2 }')"
	REV="${REV:+r$REV}"
	[ -n "$REV" ]
}

try_git() {
	[ -d .git ] || return 1
	REV="$(git log | grep -m 1 git-svn-id | awk '{ gsub(/.*@/, "", $2); print $2 }')"
	REV="${REV:+r$REV}"
	[ -n "$REV" ]
}

try_hg() {
	[ -d .hg ] || return 1
	REV="$(hg log -r-1 --template '{desc}' | awk '{print $2}' | sed 's/\].*//')"
	REV="${REV:+$REV}"
	[ -n "$REV" ]
}

try_version || try_svn || try_git || try_hg || REV="unknown"
echo "$REV"
