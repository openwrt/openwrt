#!/bin/sh
# Shell script defining macros for manipulating config files
#
# Copyright (C) 2006        Fokus Fraunhofer <carsten.tittel@fokus.fraunhofer.de>
# Copyright (C) 2006,2007   Felix Fietkau <nbd@openwrt.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

uci_load() {
	config_load "$1"
}

uci_apply_defaults() {(
	cd /etc/uci-defaults || return 0
	files="$(ls)"
	[ -z "$files" ] && return 0
	mkdir -p /tmp/.uci
	for file in $files; do
		( . "./$(basename $file)" ) && rm -f "$file"
	done
	uci commit
)}

uci_call_awk() {
	local CMD="$*"
	awk -f $UCI_ROOT/lib/config/uci.awk -f - <<EOF
BEGIN {
	$CMD
}
EOF
}

uci_do_update() {
	local FILENAME="$1"
	local UPDATE="$2"
	uci_call_awk "
	config = read_file(\"$FILENAME\")
	$UPDATE
	print config
"
}

uci_add_update() {
	local PACKAGE="$1"
	local UPDATE="$2"
	local PACKAGE_BASE="$(basename "$PACKAGE")"
	local UCIFILE

	case "$PACKAGE" in
		/*) UCIFILE="$PACKAGE";;
		*)
			UCIFILE="/tmp/.uci/$PACKAGE_BASE"
			mkdir -p "/tmp/.uci"
		;;
	esac

	# FIXME: add locking?
	echo "$UPDATE" >> "$UCIFILE"
}

uci_set() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	local VALUE="$4"

	case "$PACKAGE" in
		/*)
			uci_add_update "$PACKAGE" "config_set '$CONFIG' '$OPTION' '$VALUE'"
		;;
		*)
			( # spawn a subshell so you don't mess up the current environment
				uci_load "$PACKAGE"
				config_get OLDVAL "$CONFIG" "$OPTION"
				if [ "x$OLDVAL" != "x$VALUE" ]; then
					config_get type "$CONFIG" TYPE
					[ -z "$type" ]
				fi
			) || uci_add_update "$PACKAGE" "config_set '$CONFIG' '$OPTION' '$VALUE'"
		;;
	esac
}

uci_add() {
	local PACKAGE="$1"
	local TYPE="$2"
	local CONFIG="$3"

	uci_add_update "$PACKAGE" "config '$TYPE' '$CONFIG'"
}

uci_rename() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local VALUE="$3"

	uci_add_update "$PACKAGE" "config_rename '$CONFIG' '$VALUE'"
}

uci_remove() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"

	if [ -z "$OPTION" ]; then
		uci_add_update "$PACKAGE" "config_clear '$CONFIG'"
	else
		uci_add_update "$PACKAGE" "config_unset '$CONFIG' '$OPTION'"
	fi
}

uci_commit() {
	local PACKAGE="$1"
	local PACKAGE_BASE="$(basename "$PACKAGE")"

	case "$PACKAGE" in
		/*) return 0;;
	esac
	mkdir -p /tmp/.uci
	LOCK=`which lock` || LOCK=:
	$LOCK "/tmp/.uci/$PACKAGE_BASE.lock"
	[ -f "/tmp/.uci/$PACKAGE_BASE" ] && (
		updatestr=""
		
		# replace handlers
		config() {
			append updatestr "config = uci_update_config(config, \"@$2=$1\")" "$N"
		}
		option() {
			append updatestr "config = uci_update_config(config, \"$CONFIG_SECTION.$1=$2\")" "$N"
		}
		config_rename() {
			append updatestr "config = uci_update_config(config, \"&$1=$2\")" "$N"
		}
		config_unset() {
			append updatestr "config = uci_update_config(config, \"-$1.$2\")" "$N"
		}
		config_clear() {
			append updatestr "config = uci_update_config(config, \"-$1\")" "$N"
		}
		
		. "/tmp/.uci/$PACKAGE_BASE"

		# completely disable handlers so that they don't get in the way
		config() {
			return 0
		}
		option() {
			return 0
		}
		
		config_load "$PACKAGE"
		CONFIG_FILENAME="${CONFIG_FILENAME:-$UCI_ROOT/etc/config/$PACKAGE_BASE}"
		uci_do_update "$CONFIG_FILENAME" "$updatestr" > "/tmp/.uci/$PACKAGE_BASE.new" && {
			mv -f "/tmp/.uci/$PACKAGE_BASE.new" "$CONFIG_FILENAME" && \
			rm -f "/tmp/.uci/$PACKAGE_BASE"
		} 
	)
	$LOCK -u "/tmp/.uci/$PACKAGE_BASE.lock"
}


