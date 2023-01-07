#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Start Mesh" "$@"
}

/usr/lib/mesh/checker.sh 1