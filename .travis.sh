#!/bin/bash

# marking duration in seconds
MARK_DURATION=300

# marking process
do_mark () {
    while true; do
        sleep $MARK_DURATION
        echo "MARK" >&2
    done
}

# main process
do_main () {
    make V=w -j 20
    return $?
}

# start marking process
do_mark &
PID=$!

# run main process
do_main
EXIT_CODE=$?

# terminate marking process
kill $PID

# return main process error code
exit $EXIT_CODE
