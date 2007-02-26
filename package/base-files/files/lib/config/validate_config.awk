# AWK file for validating uci specification files
#
# Copyright (C) 2006 by Fokus Fraunhofer <carsten.tittel@fokus.fraunhofer.de>
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
#
function is_int(value) {
	valid = 1
	if (value !~ /^[0-9]*$/) { valid = 0 }
	return valid
}

function is_netmask(value) {
	return is_ip(value)
}

function is_ip(value) {
	valid = 1
	if ((value != "") && (value !~ /^[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$/)) valid = 0
	else {
		split(value, ipaddr, "\\.")
		for (i = 1; i <= 4; i++) {
			if ((ipaddr[i] < 0) || (ipaddr[i] > 255)) valid = 0
		}
	}
	return valid
}

function is_wep(value) {
	valid = 1
	if (value !~ /^[0-9A-Fa-f]*$/) {
		valid = 0
	} else if ((length(value) != 0) && (length(value) != 10) && (length(value) != 26)) {
		valid = 0
	} else if (value ~ /0$/) {
		valid = 0
	}
	return valid
}

function is_hostname(value) {
	valid = 1
	if (value !~ /^[0-9a-zA-z\.\-]*$/) {
		valid = 0
	}
	return valid;
}

function is_string(value) {
	return 1;
}

function is_mac(value) {
	valid = 1
	if ((value != "") && (value !~ /^[0-9a-fA-F][0-9a-fA-F]:[0-9a-fA-F][0-9a-fA-F]:[0-9a-fA-F][0-9a-fA-F]:[0-9a-fA-F][0-9a-fA-F]:[0-9a-fA-F][0-9a-fA-F]:[0-9a-fA-F][0-9a-fA-F]$/)) {
		valid = 0
	}
	return valid
}

function is_port(value) {
	valid = 1
	if (value !~ /^[0-9]*$/) {
		valid = 0
	}
	return valid
}

function is_ports(value) {
	valid = 1
	n = split(value ",", ports, ",")
	for (i = 1; i <= n; i++) {
		if ((ports[i] !~ /^[0-9]*$/) && (ports[i] !~ /^[0-9][0-9]*-[0-9][0-9]*$/)) {
			valid = 0
		}
	}
	return valid
}

function is_wpapsk(value) {
	valid = 1
	if (length(value) > 64) {
		valid = 0
	}
	if ((length(value) != 0) && (length(value) < 8)) {
		valid = 0
	}
	if ((length(value) == 64) && (value ~ /[^0-9a-fA-F]/)) {
		valid = 0
	}
	return valid
}

