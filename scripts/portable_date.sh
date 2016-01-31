#!/bin/sh

case $(uname) in
	NetBSD|OpenBSD|DragonFly|FreeBSD|Darwin)
		date -r $1 $2
		;;
	*)
		date -d @$1 $2
esac

exit $?
