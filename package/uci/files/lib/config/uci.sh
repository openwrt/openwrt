#!/bin/sh
# Shell script compatibility wrappers for /sbin/uci
#
# Copyright (C) 2008  Felix Fietkau <nbd@openwrt.org>
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

CONFIG_APPEND=
uci_load() {
	local PACKAGE="$1"
	local DATA
	local RET

	_C=0
	if [ -z "$CONFIG_APPEND" ]; then
		export ${NO_EXPORT:+-n} CONFIG_SECTIONS=
		export ${NO_EXPORT:+-n} CONFIG_NUM_SECTIONS=0
		export ${NO_EXPORT:+-n} CONFIG_SECTION=
	fi

	DATA="$(/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} ${LOAD_STATE:+-P /var/state} -S -n export "$PACKAGE" 2>/dev/null)"
	RET="$?"
	[ "$RET" != 0 -o -z "$DATA" ] || eval "$DATA"
	unset DATA

	${CONFIG_SECTION:+config_cb}
	return "$RET"
}

uci_set_default() {
	local PACKAGE="$1"
	/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} -q show "$1" > /dev/null && return 0
	/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} import "$1"
	/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} commit "$1"
}

uci_revert_state() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"

	/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} -P /var/state revert "$PACKAGE${CONFIG:+.$CONFIG}${OPTION:+.$OPTION}"
}

uci_set_state() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	local VALUE="$4"

	[ "$#" = 4 ] || return 0
	/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} -P /var/state set "$PACKAGE.$CONFIG${OPTION:+.$OPTION}=$VALUE"
}

uci_set() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	local VALUE="$4"

	/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} set "$PACKAGE.$CONFIG.$OPTION=$VALUE"
}

uci_add() {
	local PACKAGE="$1"
	local TYPE="$2"
	local CONFIG="$3"

	if [ -z "$CONFIG" ]; then
		export ${NO_EXPORT:+-n} CONFIG_SECTION="$(/sbin/uci add "$PACKAGE" "$TYPE")"
	else
		/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} set "$PACKAGE.$CONFIG=$TYPE"
		export ${NO_EXPORT:+-n} CONFIG_SECTION="$CONFIG"
	fi
}

uci_rename() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local VALUE="$3"

	/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} rename "$PACKAGE.$CONFIG=$VALUE"
}

uci_remove() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"

	/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} del "$PACKAGE.$CONFIG${OPTION:+.$OPTION}"
}

uci_commit() {
	local PACKAGE="$1"
	/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} commit $PACKAGE
}
