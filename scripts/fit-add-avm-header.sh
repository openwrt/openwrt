#! /bin/sh
# vim: set tabstop=4 syntax=sh :
# SPDX-License-Identifier: GPL-2.0-or-later
#######################################################################################################
#                                                                                                     #
# add AVM's header to a FIT image file                                                                #
#                                                                                                     #
###################################################################################################VER#
#                                                                                                     #
# fit-add-avm-header, version 0.2                                                                     #
#                                                                                                     #
# This script is a part of the YourFritz project from https://github.com/PeterPawn/YourFritz.         #
#                                                                                                     #
###################################################################################################CPY#
#                                                                                                     #
# Copyright (C) 2022 P.Haemmerlein (peterpawn@yourfritz.de)                                           #
#                                                                                                     #
###################################################################################################LIC#
#                                                                                                     #
# This project is free software, you can redistribute it and/or modify it under the terms of the GNU  #
# General Public License as published by the Free Software Foundation; either version 2 of the        #
# License, or (at your option) any later version.                                                     #
#                                                                                                     #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without   #
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       #
# General Public License under http://www.gnu.org/licenses/gpl-2.0.html for more details.             #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# This script takes a FIT image built with U-boot's 'mkimage' command and adds a header using AVM's   #
# extension format.                                                                                   #
# The data part, which looks like a cryptographic signature to me, will be filled with zeros.         #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# the whole logic as a sealed sub-function                                                            #
#                                                                                                     #
#######################################################################################################
add_avm_header()
(
	printf_ss() {
		mask="$1"
		shift
		# shellcheck disable=SC2059
		printf -- "$mask" "$@"
	}
	result() {
		[ "$dbg" = "1" ] || return 0
		if [ "$1" = "0" ]; then
			color="$__yf_ansi_bright_green__"
		elif [ "$1" = "1" ]; then
			color="$__yf_ansi_bright_red__"
		else
			color="$__yf_ansi_yellow__"
		fi
		shift
		printf -- "%s" "$color" 1>&2
		debug "$@"
		printf -- "%s\n" "$__yf_ansi_reset__" 1>&2
	}
	debug() {
		[ "$dbg" = "1" ] || return 0
		printf_ss "$@" 1>&2
	}
	sbo() (
		sbo_ro()
		{
			v1=0; v2=0; v3=0; v4=0
			while read -r p _ rt; do
				[ "$p" -gt 5 ] && exit 1
				[ "$p" -eq 5 ] && [ "$rt" -ne 0377 ] && exit 1
				if [ "$p" -eq 5 ]; then
					for i in $v4 $v3 $v2 $v1; do
						printf -- "%b" "\0$(printf -- "%o" "$i")"
					done
					exit 0
				fi
				eval "v$p"=$(( 0$rt ))
			done
			exit 1
		}
		( cat; printf -- "%b" "\377" ) | cmp -l -- "$zeros" - 2>"$null" | sbo_ro
	)
	b2d() (
		b2d_ro()
		{
			i=1; v=0; ff=0
			while read -r p _ rt; do
				if [ "$ff" -eq 1 ]; then
					v=$(( v * 256 ))
					ff=255
					v=$(( v + ff ))
					i=$(( i + 1 ))
					ff=0
				fi
				while [ "$i" -lt "$p" ]; do
					v=$(( v * 256 ))
					i=$(( i + 1 ))
				done
				if [ "$rt" = 377 ] && [ $ff -eq 0 ]; then
					ff=1
					continue
				fi
				v=$(( v * 256 ))
				rt=$(( 0$rt ))
				v=$(( v + rt ))
				i=$(( p + 1 ))
			done
			printf -- "%d" $v
		}
		( cat; printf -- "%b" "\377" ) | cmp -l -- "$zeros" - 2>"$null" | b2d_ro
		return 0
	)
	get_data() ( dd if="$1" bs="$3" count=$(( ( $2 / $3 ) + 1 )) skip=1 2>"$null" | dd bs=1 count="$2" 2>"$null"; )
	get_fdt32() ( get_data "$1" 4 "$2" | b2d; )
	__yf_ansi_sgr() { printf -- '\033[%sm' "$1"; }
	__yf_ansi_bold__="$(__yf_ansi_sgr 1)"
	__yf_ansi_yellow__="$(__yf_ansi_sgr 33)"
	__yf_ansi_bright_red__="$(__yf_ansi_sgr 91)"
	__yf_ansi_bright_green__="$(__yf_ansi_sgr 92)"
	__yf_ansi_reset__="$(__yf_ansi_sgr 0)"
	__yf_get_script_lines() {
		sed -n -e "/^#*${1}#\$/,/^#\{20\}.*#\$/p" -- "$0" | \
		sed -e '1d;$d' | \
		sed -e 's|# \(.*\) *#$|\1|' | \
		sed -e 's|^#*#$|--|p' | \
		sed -e '$d' | \
		sed -e 's| *$||'
	}
	__yf_show_script_name() {
		[ -n "$1" ] && printf -- '%s' "$1"
		printf -- '%s' "${0#*/}"
		[ -n "$1" ] && printf -- "%s" "${__yf_ansi_reset__}"
	}
	__yf_show_license() { __yf_get_script_lines 'LIC'; }
	__yf_show_version() {
		printf -- "\n%s%s%s, " "${__yf_ansi_bold__}" "$(__yf_get_script_lines 'VER' | sed -n -e "2s|^\([^,]*\),.*|\1|p")" "${__yf_ansi_reset__}"
		v_display="$(__yf_get_script_lines 'VER' | sed -n -e "2s|^[^,]*, \(.*\)|\1|p")"
		printf -- "%s\n" "$v_display"
	}
	__yf_show_copyright() { __yf_get_script_lines 'CPY'; }
	usage() {
		exec 1>&2
		__yf_show_version
		__yf_show_copyright
		__yf_show_license
		printf -- "\n"
		printf -- "%sfit-add-avm-header.sh%s - add AVM's header to a (standard) FIT image file\n\n" "${__yf_ansi_bold__}" "${__yf_ansi_reset__}"
		printf -- "Usage: %s [ options ] <standard-fit-image>\n\n" "$0"
		printf -- "Options:\n\n"
		printf -- "-d or --debug - show some extra info on STDERR\n"
		printf -- "\n"
		exec 1>&2
	}

	null="/dev/null"
	zeros="/dev/zero"

	fdt32_size=4

	dbg=0
	while [ "$(expr "$1" : "\(.\).*")" = "-" ]; do
		[ "$1" = "--" ] && shift && break

		if [ "$1" = "-d" ] || [ "$1" = "--debug" ]; then
			dbg=1
			shift
		elif [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
			usage
			exit 0
		else
			printf -- "Unknown option: %s\a\n" "$1" 1>&2 && exit 1
		fi
	done

	img="$1"
	if [ "$dbg" = "1" ]; then
		__yf_show_version 1>&2
		__yf_show_copyright 1>&2
		debug "\n"
	fi
	[ -z "$img" ] && printf -- "Missing input source parameter.\a\n" 1>&2 && exit 1
	debug "Input file: %s%s%s\n" "$__yf_ansi_bright_green__" "$img" "$__yf_ansi_reset__"
	debug "Looking for FDT magic value at offset 0x00 ..."
	fdt_magic="$(dd if="$img" bs=4 count=1 2>"$null" | b2d)"
	if { [ "$fdt_magic" -gt 0 ] && [ "$fdt_magic" -ne 3490578157 ]; } || { [ "$fdt_magic" -lt 0 ] && [ "$fdt_magic" -ne -804389139 ]; }; then
		debug "$(result 1 " failed")"
		printf -- "Invalid FDT magic at start of input file '%s'.\a\n" "$img" 1>&2
		exit 1
	else
		debug "$(result 0 " OK")"
	fi

	offset=$(( fdt32_size ))
	total_size="$(get_fdt32 "$img" "$offset")"
	debug "FDT total size at offset 0x04: %s%u%s (%#x)\n" "$__yf_ansi_bright_green__" "$total_size" "$__yf_ansi_reset__" "$total_size"

	[ -t 1 ] && printf -- "STDOUT is a terminal device, output suppressed.\a\n" 1>&2 && exit 1

	magic="$(printf -- "\376\355\000\015" | sbo | b2d)"
	debug "Writing magic value: 0x%08x ..." "$magic"
	if printf -- "\376\355\000\015" | sbo; then
		result 0 " OK"
	else
		result 1 " failed"
		exit 1
	fi

	debug "Writing standard FIT image size: 0x%08x ..." "$(get_data "$img" "$offset" "$fdt32_size" | sbo | b2d)"
	if dd if="$img" bs=4 skip=1 count=1 2>"$null" | sbo; then
		result 0 " OK"
	else
		result 1 " failed"
		exit 1
	fi

	# TODO: further investigations to find the algorithm used to fill this area
	debug "Writing zeros to (assumed) signature area ..."
	if dd if="$zeros" bs=64 count=1 2>"$null"; then
		result 0 " OK"
	else
		result 1 " failed"
		exit 1
	fi

	debug "Copying standard FIT data ..."
	if dd if="$img" bs=$(( 1024 * 1024 )) 2>"$null"; then
		result 0 " OK"
		debug "\n"
	else
		result 1 " failed"
		debug "\n"
		exit 1
	fi

	debug "Appending 8 zero bytes after FDT payload ..."
	if dd if="$zeros" bs=8 count=1 2>"$null"; then
		result 0 " OK"
		debug "\n"
	else
		result 1 " failed"
		debug "\n"
		exit 1
	fi

	exit 0
)
#######################################################################################################
#                                                                                                     #
# invoke sealed function from above                                                                   #
#                                                                                                     #
#######################################################################################################
add_avm_header "$@"
#######################################################################################################
#                                                                                                     #
# end of script                                                                                       #
#                                                                                                     #
#######################################################################################################
