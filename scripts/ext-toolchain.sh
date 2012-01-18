#!/usr/bin/env bash
#
#   Script for various external toolchain tasks, refer to
#   the --help output for more information.
#
#   Copyright (C) 2012 Jo-Philipp Wich <jow@openwrt.org>
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

CC=""
CXX=""
CPP=""

CFLAGS=""
TOOLCHAIN="."

LIBC_TYPE=""


# Library specs
LIB_SPECS="
	c:        ld-* lib{anl,c,cidn,crypt,dl,m,nsl,nss_dns,nss_files,resolv,util}
	rt:       librt-* librt
	pthread:  libpthread-* libpthread
	cpp:      libstdc++
	gcc:      libgcc_s
	ssp:      libssp
	gfortran: libgfortran
"

# Binary specs
BIN_SPECS="
	ldd:       ldd
	ldconfig:  ldconfig
	gdb:       gdb
	gdbserver: gdbserver
"


test_c() {
	cat <<-EOT | "${CC:-false}" $CFLAGS -o /dev/null -x c - 2>/dev/null
		#include <stdio.h>

		int main(int argc, char **argv)
		{
			printf("Hello, world!\n");
			return 0;
		}
	EOT
}

test_cxx() {
	cat <<-EOT | "${CXX:-false}" $CFLAGS -o /dev/null -x c++ - 2>/dev/null
		#include <iostream>

		using namespace std;

		int main()
		{
			cout << "Hello, world!" << endl;
			return 0;
		}
	EOT
}

test_softfloat() {
	cat <<-EOT | "$CC" $CFLAGS -msoft-float -o /dev/null -x c - 2>/dev/null
		int main(int argc, char **argv)
		{
			double a = 0.1;
			double b = 0.2;
			double c = (a + b) / (a * b);
			return 1;
		}
	EOT
}

test_uclibc() {
	local sysroot="$("$CC" $CFLAGS -print-sysroot 2>/dev/null)"
	if [ -d "$sysroot" ]; then
		local lib
		for lib in "$sysroot"/{lib,usr/lib,usr/local/lib}/ld-uClibc*.so*; do
			if [ -f "$lib" ] && [ ! -h "$lib" ]; then
				return 0
			fi
		done
	fi
	return 1
}

test_feature() {
	local feature="$1"; shift

	# find compilers, libc type
	probe_cc
	probe_cxx
	probe_libc

	# common toolchain feature tests
	case "$feature" in
		c)     test_c;         return $? ;;
		c++)   test_cxx;       return $? ;;
		soft*) test_softfloat; return $? ;;
	esac

	# assume eglibc/glibc supports all libc features
	if [ "$LIBC_TYPE" != "uclibc" ]; then
		return 0
	fi

	# uclibc feature tests
	local inc
	local sysroot="$("$CC" "$@" -muclibc -print-sysroot 2>/dev/null)"
	for inc in "include" "usr/include" "usr/local/include"; do
		local conf="$sysroot/$inc/bits/uClibc_config.h"
		if [ -f "$conf" ]; then
			case "$feature" in
				lfs)     grep -q '__UCLIBC_HAS_LFS__ 1'     "$conf"; return $?;;
				ipv6)    grep -q '__UCLIBC_HAS_IPV6__ 1'    "$conf"; return $?;;
				rpc)     grep -q '__UCLIBC_HAS_RPC__ 1'     "$conf"; return $?;;
				locale)  grep -q '__UCLIBC_HAS_LOCALE__ 1'  "$conf"; return $?;;
				wchar)   grep -q '__UCLIBC_HAS_WCHAR__ 1'   "$conf"; return $?;;
				threads) grep -q '__UCLIBC_HAS_THREADS__ 1' "$conf"; return $?;;
			esac
		fi
	done

	return 1
}


find_libs() {
	local spec="$(echo "$LIB_SPECS" | sed -ne "s#^[[:space:]]*$1:##p")"

	if [ -n "$spec" ] && probe_cpp; then
		local libdir libdirs
		for libdir in $(
			"$CPP" $CFLAGS -v -x c /dev/null 2>&1 | \
				sed -ne 's#:# #g; s#^LIBRARY_PATH=##p'
		); do
			if [ -d "$libdir" ]; then
				libdirs="$libdirs $(cd "$libdir"; pwd)/"
			fi
		done

		local pattern
		for pattern in $(eval echo $spec); do
			find $libdirs -name "$pattern.so*" | sort -u
		done

		return 0
	fi

	return 1
}

find_bins() {
	local spec="$(echo "$BIN_SPECS" | sed -ne "s#^[[:space:]]*$1:##p")"

	if [ -n "$spec" ] && probe_cpp; then
		local sysroot="$("$CPP" -print-sysroot)"

		local bindir bindirs
		for bindir in $(
			echo "$sysroot/bin";
			echo "$sysroot/usr/bin";
			echo "$sysroot/usr/local/bin";
			echo "$TOOLCHAIN/bin";
			echo "$TOOLCHAIN/usr/bin";
			echo "$TOOLCHAIN/usr/local/bin";
 			"$CPP" $CFLAGS -v -x c /dev/null 2>&1 | \
				sed -ne 's#:# #g; s#^COMPILER_PATH=##p'
		); do
			if [ -d "$bindir" ]; then
				bindirs="$bindirs $(cd "$bindir"; pwd)/"
			fi
		done

		local pattern
		for pattern in $(eval echo $spec); do
			find $bindirs -name "$pattern" | sort -u
		done

		return 0
	fi

	return 1
}


wrap_bins() {
	if probe_cc; then
		mkdir -p "$1" || return 1

		local cmd
		for cmd in "${CC%-*}-"*; do
			if [ -x "$cmd" ]; then
				local out="$1/${cmd##*/}"

				echo '#!/bin/sh' > "$out"
				case "${cmd##*/}" in
					*-*cc|*-*cc-*|*-*++|*-*++-*|*-cpp)
						echo -n 'exec "'"$cmd"'" '"$CFLAGS"' '         >> "$out"
						echo -n '${STAGING_DIR:+-idirafter '           >> "$out"
						echo -n '"$STAGING_DIR/usr/include" '          >> "$out"
						echo -n '-L "$STAGING_DIR/usr/lib" '           >> "$out"
						echo -n '-Wl,-rpath-link,'                     >> "$out"
						echo    '"$STAGING_DIR/usr/lib"} "$@"'         >> "$out"
					;;
					*-ld)
						echo -n 'exec "'"$cmd"'" ${STAGING_DIR:+'      >> "$out"
						echo -n '-L "$STAGING_DIR/usr/lib" '           >> "$out"
						echo -n '-rpath-link '                         >> "$out"
						echo    '"$STAGING_DIR/usr/lib"} "$@"'         >> "$out"
					;;
					*)
						echo "exec '$cmd' \"\$@\"" >> "$out"
					;;
				esac
				chmod +x "$out"
			fi
		done

		return 0
	fi

	return 1
}


probe_cc() {
	if [ -z "$CC" ]; then
		local bin
		for bin in "bin" "usr/bin" "usr/local/bin"; do
			local cmd
			for cmd in "$TOOLCHAIN/$bin/"*-*cc*; do
				if [ -x "$cmd" ] && [ ! -h "$cmd" ]; then
					CC="$(cd "${cmd%/*}"; pwd)/${cmd##*/}"
					return 0
				fi
			done
		done
		return 1
	fi
	return 0
}

probe_cxx() {
	if [ -z "$CXX" ]; then
		local bin
		for bin in "bin" "usr/bin" "usr/local/bin"; do
			local cmd
			for cmd in "$TOOLCHAIN/$bin/"*-*++*; do
				if [ -x "$cmd" ] && [ ! -h "$cmd" ]; then
					CXX="$(cd "${cmd%/*}"; pwd)/${cmd##*/}"
					return 0
				fi
			done
		done
		return 1
	fi
	return 0
}

probe_cpp() {
	if [ -z "$CPP" ]; then
		local bin
		for bin in "bin" "usr/bin" "usr/local/bin"; do
			local cmd
			for cmd in "$TOOLCHAIN/$bin/"*-cpp*; do
				if [ -x "$cmd" ] && [ ! -h "$cmd" ]; then
					CPP="$(cd "${cmd%/*}"; pwd)/${cmd##*/}"
					return 0
				fi
			done
		done
		return 1
	fi
	return 0
}

probe_libc() {
	if [ -z "$LIBC_TYPE" ]; then
		if test_uclibc; then
			LIBC_TYPE="uclibc"
		else
			LIBC_TYPE="glibc"
		fi
	fi
	return 0
}


while [ -n "$1" ]; do
	arg="$1"; shift
	case "$arg" in
		--toolchain)
			[ -d "$1" ] || {
				echo "Toolchain directory '$1' does not exist." >&2
				exit 1
			}
			TOOLCHAIN="$(cd "$1"; pwd)"; shift
		;;

		--cflags)
			CFLAGS="${CFLAGS:+$CFLAGS }$1"; shift
		;;

		--print-libc)
			if probe_cc; then
				probe_libc
				echo "$LIBC_TYPE"
				exit 0
			fi
			echo "No C compiler found in '$TOOLCHAIN'." >&2
			exit 1
		;;

		--print-target)
			if probe_cc; then
				exec "$CC" $CFLAGS -dumpmachine
			fi
			echo "No C compiler found in '$TOOLCHAIN'." >&2
			exit 1
		;;

		--print-bin)
			if [ -z "$1" ]; then
				echo "Available programs:"                      >&2
				echo $(echo "$BIN_SPECS" | sed -ne 's#:.*$##p') >&2
				exit 1
			fi

			find_bins "$1" || exec "$0" --toolchain "$TOOLCHAIN" --print-bin
			exit 0
		;;

		--print-libs)
			if [ -z "$1" ]; then
				echo "Available libraries:"                     >&2
				echo $(echo "$LIB_SPECS" | sed -ne 's#:.*$##p') >&2
				exit 1
			fi

			find_libs "$1" || exec "$0" --toolchain "$TOOLCHAIN" --print-libs
			exit 0
		;;

		--test)
			test_feature "$1"
			exit $?
		;;

		--wrap)
			[ -n "$1" ] || exec "$0" --help
			wrap_bins "$1"
			exit $?
		;;

		-h|--help)
			me="$(basename "$0")"
			echo -e "\nUsage:\n"                                            >&2
			echo -e "  $me --toolchain {directory} --print-libc"            >&2
			echo -e "    Print the libc implementation and exit.\n"         >&2
			echo -e "  $me --toolchain {directory} --print-target"          >&2
			echo -e "    Print the GNU target name and exit.\n"             >&2
			echo -e "  $me --toolchain {directory} --print-bin {program}"   >&2
			echo -e "    Print executables belonging to given program,"     >&2
			echo -e "    omit program argument to get a list of names.\n"   >&2
			echo -e "  $me --toolchain {directory} --print-libs {library}"  >&2
			echo -e "    Print shared objects belonging to given library,"  >&2
			echo -e "    omit library argument to get a list of names.\n"   >&2
			echo -e "  $me --toolchain {directory} --test {feature}"        >&2
			echo -e "    Test given feature, exit code indicates success."  >&2
			echo -e "    Possible features are 'c', 'c++', 'softfloat',"    >&2
			echo -e "    'lfs', 'rpc', 'ipv6', 'wchar', 'locale' and "      >&2
			echo -e "    'threads'.\n"                                      >&2
			echo -e "  $me --toolchain {directory} --wrap {directory}"      >&2
			echo -e "    Create wrapper scripts for C and C++ compiler, "   >&2
			echo -e "    linker, assembler and other key executables in "   >&2
			echo -e "    the directory given with --wrap.\n"                >&2
			echo -e "  $me --help"                                          >&2
			echo -e "    Display this help text and exit.\n\n"              >&2
			echo -e "  Most commands also take a --cflags parameter which " >&2
			echo -e "  is used to specify C flags to be passed to the "     >&2
			echo -e "  cross compiler when performing tests."               >&2
			echo -e "  This paremter may be repeated multiple times."       >&2
			exit 1
		;;

		*)
			echo "Unknown argument '$arg'" >&2
			exec $0 --help
		;;
	esac
done

exec $0 --help
