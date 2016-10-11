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
	REV="$(svn info | awk '/^Last Changed Rev:/ { print $4 }')"
	REV="${REV:+r$REV}"
	[ -n "$REV" ]
}

try_git() {
	git rev-parse --git-dir >/dev/null 2>&1 || return 1
	REV="$(git describe --tags | sed "s/v15.05.1-\([0-9]*\)-.*/\1/g")"
	REV="${REV:+r$((REV+49254))}"
	[ -n "$REV" ]
}

try_hg() {
	[ -d .hg ] || return 1
	REV="$(hg log -r-1 --template '{desc}' | awk '{print $2}' | sed 's/\].*//')"
	REV="${REV:+$REV}"
	[ -n "$REV" ]
}

try_version || try_git || try_hg || REV="unknown"
echo "$REV"
