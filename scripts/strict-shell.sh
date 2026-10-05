#!/usr/bin/env bash
#
# Shell for make recipes that must not ignore failing commands.
# make runs it as "strict-shell.sh [--skip-if-exists <file>] -c <recipe line>".
#
# errexit and pipefail stop the recipe line at the first failing command,
# including a failing command in a pipeline. bash ignores the exit status of
# a command substitution in the arguments of a command, so these options do
# not catch the failing false in
#   echo "$(false)" x
#
# With errtrace, subshells and command substitutions inherit the ERR trap.
# The trap in a failing subshell signals the recipe shell, which exits with
# an error after its current command has finished. bash does not run the ERR
# trap in the places where errexit is ignored, so failures in if conditions
# and "||" lists remain allowed. bash 3.2 runs the trap in a command
# substitution even if it is part of an if condition or a "||" list, so the
# trap is only set with bash 4 and later.
#
# With --skip-if-exists, the recipe line is not run if <file> exists.
# Image/drop in include/image.mk creates this file to skip the remaining
# recipe lines of a dropped image.
#
# make also runs the $(shell) calls of these recipes through this script, so
# a $(shell a; b) stops at the first failing command. make expands all lines
# of a recipe, including their $(shell) calls, before it runs the first line,
# so Image/drop does not skip the $(shell) calls of its own recipe. A file
# left by an earlier build would skip the recipe lines and make $(shell)
# return an empty string, so image_prepare removes these files.

if [ "$1" = --skip-if-exists ]; then
	[ ! -e "$2" ] || exit 0
	shift 2
fi

set -eE -o pipefail

strict_shell_err() {
	[ "$BASH_SUBSHELL" -ne 0 ] || return 0
	echo "Command in subshell failed with exit code $1" >&2
	kill -USR1 $$
}

if [ "${BASH_VERSINFO[0]}" -ge 4 ]; then
	trap 'strict_shell_err $?' ERR
	trap 'exit 1' USR1
fi

# The recipe line gets no positional parameters, as with "bash -c".
strict_shell_cmd=$2
set --
eval "$strict_shell_cmd"
