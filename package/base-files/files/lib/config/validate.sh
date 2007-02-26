# Shell script defining validating configuration macros
#
# Copyright (C) 2006 by Fokus Fraunhofer <carsten.tittel@fokus.fraunhofer.de>
# Copyright (C) 2007 by Felix Fietkau <nbd@openwrt.org>
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


validate_spec() {
	export | grep 'CONFIG_' | cat - "$@" | awk \
		-f $UCI_ROOT/lib/config/validate_config.awk \
		-f $UCI_ROOT/lib/config/parse_spec.awk \
		-f $UCI_ROOT/lib/config/validate_spec.awk
}

validate_config_cb () {
	local TYPE
	local res=
	
	[ -n "${CONFIG_SECTION}" ] || return 0
	
	config_get TYPE ${CONFIG_SECTION} TYPE
	[ -n "$TYPE" ] || return 0
	
	if type validate_${PACKAGE}_${TYPE} >/dev/null 2>&1; then
		validate_${PACKAGE}_${TYPE}
		res="$?"
	else 
		if [ -f $UCI_ROOT/lib/config/specs/${PACKAGE}.spec ]; then
			# no special defined, use default one
			validate_spec $UCI_ROOT/lib/config/specs/${PACKAGE}.spec
			res="$?"
		fi
	fi
	
	VALIDATE_RES="${VALIDATE_RES:-$res}"
}

uci_validate() {(
	PACKAGE="$1"
	FILE="$2"
	VALIDATE_RES=

	[ -z "${PACKAGE}" ] && {
		echo "Error: no package defined"
		return 1
	}

	reset_cb
	config_cb() {
		validate_config_cb "$@"
	}
	unset NO_EXPORT
	if [ -n "$FILE" ]; then
		. "$FILE"
		config
	else
		config_load "$1"
	fi

	return ${VALIDATE_RES:-0}
)}
