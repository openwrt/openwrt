#!/usr/bin/env bash
export LANG=C
export LC_ALL=C
[ -n "$TOPDIR" ] && cd $TOPDIR

GET_REV=$1

try_version() {
	[ -f version ] || return 1
	REV="$(cat version)"
	[ -n "$REV" ]
}

try_git() {
	git rev-parse --git-dir >/dev/null 2>&1 || return 1

	[ -n "$GET_REV" ] || GET_REV="HEAD"

	case "$GET_REV" in
	r*)
		GET_REV="$(echo $GET_REV | tr -d 'r')"
		BASE_REV="$(git rev-list reboot..HEAD | wc -l)"
		REV="$(git rev-parse HEAD~$((BASE_REV - GET_REV)))"
		;;
	*)
		UPSTREAM_BASE="$(git merge-base $GET_REV origin/master)"
		UPSTREAM_REV="$(git rev-list reboot..$UPSTREAM_BASE | wc -l | awk '{print $1}')"
		REV="$(git rev-list reboot..$GET_REV | wc -l | awk '{print $1}')"
		if [ -n "$REV" -a -n "$UPSTREAM_REV" -a "$REV" -gt "$UPSTREAM_REV" ]; then
			REV="r${UPSTREAM_REV}+$((REV - UPSTREAM_REV))"
		else
			REV="${REV:+r$REV}"
		fi
		;;
	esac

	[ -n "$REV" ]
}

try_hg() {
	[ -d .hg ] || return 1
	REV="$(hg log -r-1 --template '{desc}' | awk '{print $2}' | sed 's/\].*//')"
	REV="${REV:+r$REV}"
	[ -n "$REV" ]
}

try_version || try_git || try_hg || REV="unknown"
echo "$REV"
