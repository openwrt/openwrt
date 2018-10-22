#!/bin/sh
case $(uname) in
(Darwin)
	# force config.guess run
	GNU_HOST_NAME=
	;;
(*)
	GNU_HOST_NAME=$(gcc -dumpmachine)
	;;
esac
test -n "$GNU_HOST_NAME" || \
    GNU_HOST_NAME=$("$(dirname "$0")/config.guess")
printf '%s\n' "$GNU_HOST_NAME"
