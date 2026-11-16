#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Usage: cache-run.sh <slot> <input> <output> <command> [<arg>...]
#
# Run a command that creates <output> from <input>. If the content of the
# input, the command line and the command binary are the same as in the
# last run for <slot>, copy the stored output instead of running the
# command again. Only use it for deterministic commands, such as the
# compression of kernel and initramfs images, which are slow and get the
# same input on most rebuilds.

[ $# -ge 4 ] || {
	echo "Usage: $0 <slot> <input> <output> <command> [<arg>...]" >&2
	exit 1
}
[ -n "$MKHASH" ] || {
	echo "$0: MKHASH is not set" >&2
	exit 1
}

slot="$1"
input="$2"
output="$3"
shift 3

[ -f "$input" ] || {
	echo "$0: $input does not exist" >&2
	exit 1
}
tool="$(command -v "$1")" || {
	echo "$0: $1 not found" >&2
	exit 1
}

input_hash="$("$MKHASH" sha256 "$input")" || exit 1
tool_hash="$("$MKHASH" sha256 "$tool")" || exit 1
key="$(printf '%s\n' "$input_hash" "$tool_hash" "$@" | "$MKHASH" sha256)" || exit 1

mkdir -p "$(dirname "$slot")" || exit 1

# Two image recipes can hold the same slot at the same time. Hold a lock over
# the whole slot, so that a reader never sees a half written pair.
exec 9>"$slot.lock" || exit 1
flock 9 || exit 1

if [ -f "$slot.data" ] && [ "$(cat "$slot.key" 2>/dev/null)" = "$key" ]; then
	cmp -s "$slot.data" "$output" && exit 0
	exec cp "$slot.data" "$output"
fi

rm -f "$slot.key"
"$@" || exit
cp "$output" "$slot.data.new" && mv -f "$slot.data.new" "$slot.data" &&
	echo "$key" > "$slot.key"
