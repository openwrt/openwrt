#!/usr/bin/env bash
#
#   Script to install host system binaries along with required libraries.
#
#   Copyright (C) 2012-2017 Jo-Philipp Wich <jo@mein.io>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

DIR="$1"; shift

_cp() {
	cp ${VERBOSE:+-v} -L "$1" "$2" || {
		echo "cp($1 $2) failed" >&2
		exit 1
	}
}

_mv() {
	mv ${VERBOSE:+-v} "$1" "$2" || {
		echo "mv($1 $2) failed" >&2
		exit 1
	}
}

_md() {
	mkdir ${VERBOSE:+-v} -p "$1" || {
		echo "mkdir($1) failed" >&2
		exit 2
	}
}

_ln() {
	ln ${VERBOSE:+-v} -sf "$1" "$2" || {
		echo "ln($1 $2) failed" >&2
		exit 3
	}
}

_relpath() {
	local base="$(readlink -f "$1")"
	local dest="$(readlink -f "$2")"
	local up

	[ -d "$base" ] || base="${base%/*}"
	[ -d "$dest" ] || dest="${dest%/*}"

	while true; do
		case "$base"
			in "$dest"/*)
				echo "$up/${base#$dest/}"
				break
			;;
			*)
				dest="${dest%/*}"
				up="${up:+$up/}.."
			;;
		esac
	done
}

_wrapper() {
	cat <<-EOT | ${CC:-gcc} -x c -o "$1" -
		#include <unistd.h>
		#include <stdio.h>

		int main(int argc, char **argv) {
			const char *self   = argv[0];
			const char *target = argv[1];

			if (argc < 3) {
				fprintf(stderr, "Usage: %s executable arg0 [args...]\n", self);
				return 1;
			}

			return execv(target, argv + 2);
		}
	EOT

	[ -x "$1" ] || {
		echo "compiling wrapper failed" >&2
		exit 5
	}
}

for LDD in ${PATH//://ldd }/ldd; do
	"$LDD" --version >/dev/null 2>/dev/null && break
	LDD=""
done

[ -n "$LDD" -a -x "$LDD" ] || LDD=

for BIN in "$@"; do
	[ -n "$BIN" -a -n "$DIR" ] || {
		echo "Usage: $0 <destdir> <executable> ..." >&2
		exit 1
	}

	[ ! -d "$DIR/lib" ] && {
		_md "$DIR/lib"
		_md "$DIR/usr"
		_ln "../lib" "$DIR/usr/lib"
	}

	[ ! -x "$DIR/lib/runas" ] && {
		_wrapper "$DIR/lib/runas"
	}

	LDSO=""

	[ -n "$LDD" ] && [ -x "$BIN" ] && file "$BIN" | grep -sqE "ELF.*executable" && {
		for token in $("$LDD" "$BIN" 2>/dev/null); do
			case "$token" in */*.so*)
				case "$token" in
					*ld-*.so*) LDSO="${token##*/}" ;;
				esac

				dest="$DIR/lib/${token##*/}"
				ddir="${dest%/*}"

				[ -f "$token" -a ! -f "$dest" ] && {
					_md "$ddir"
					_cp "$token" "$dest"
				}
			;; esac
		done
	}

	# is a dynamically linked executable
	if [ -n "$LDSO" ]; then
		echo "Bundling ${BIN##*/}"

		RUNDIR="$(readlink -f "$BIN")"; RUNDIR="${RUNDIR%/*}"
		RUN="${LDSO#ld-}"; RUN="run-${RUN%%.so*}.sh"
		REL="$(_relpath "$DIR/lib" "$BIN")"

		_mv "$BIN" "$RUNDIR/.${BIN##*/}.bin"

		cat <<-EOF > "$BIN"
			#!/usr/bin/env bash
			dir="\$(dirname "\$0")"
			exec "\$dir/${REL:+$REL/}$LDSO" --library-path "\$dir/${REL:+$REL/}" "\$dir/${REL:+$REL/}runas" "\$dir/.${BIN##*/}.bin" "\$0" "\$@"
		EOF

		chmod ${VERBOSE:+-v} 0755 "$BIN"
	fi
done
