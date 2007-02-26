# AWK file for validating uci specification files
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
#
#
# general: unfortunately, the development was done using gawk providing
#  a different match() functions than e.g. mawk on debian systems
#  - therefore, the script was changed to run on most awk's 
#  - even things like [:space:] are not used
#
# - script  parses the config section definition contained in one 
#   specification file
# global variables:
# * section  - contains the current config section name
# * var      - contains the name of the current config option
# * type     - contains the type of the current config option
# * required - contains the requirements of the current config option
# * optional - contains the optional scope of the current config option
# * vars[]  - array, contains the name of all config options valid within
#	      a certain config section, format: csv
#
# XXX todo: more than one config option with the same in different section
# will clash for the following tables
# * types[] - contains the type of a config option
# * reqs[]  - contains the requirements of a config option
# * opts[]  - contains the optional scope of a config option
#

# - check requirement validates, if the config option is required in
#   the config section type and if so, if it is defined
# - the functions exits with error in case of non-conforming 
#   behaviour
# XXX todo: use return instead of exit
#
function check_requirements(vsec,var) {
	# check, if config option is required in all cases
	if (reqs[vsec "_" var] == 1) {
		# option is always required, is it defined?
		if (!length(ENVIRON["CONFIG_" vsec "_" var])) {
			print STDERR "Error: missing config option " var " in " vsec
			exit 1
		}

	# check, if config option is required only when other options
	# have certain values
	} else if (length(reqs[vsec "_" var])) {
		# - check all requirements, e.g. proto=static,proto=pptp
		# - note, that the required flag is tiggered if at least one
		#   of the conditions is met
		split(reqs[vsec "_" var],arr,",");
		for (idx in arr) {
			# parse the condition space tolerant
			if (!match(arr[idx],"^[ \t\n]*[^ \t\n=]+"\
				"[ \t\n]*=.+")) {
				print STDERR "Error: invalid requirement "\
					"in spec file for " var " : " arr[idx]
				exit 1
			}
			# get the name of the variable
			match(arr[idx],"[^ \t\n=]+");
			name=substr(arr[idx],RSTART,RLENGTH)
			mrest=substr(arr[idx],RSTART+RLENGTH)
			# get the spaces
			match(mrest,"[ \t\n]*=[ \t\n]*")
			val=substr(mrest,RSTART+RLENGTH)
			# check the condition
			if (ENVIRON["CONFIG_" vsec "_" name] == val) {
				# condition is met, check requirement
				if (!length(ENVIRON["CONFIG_" vsec "_" var])) {
					print STDERR "Error: missing config " \
						"option " var " in " vsec 
					exit 1
				}
			}
		}
	}
}

# is_valid just returns true(1)/false(0) if the
# given value is conform with the type definition 
# NOTE: this function needs the type validating function from
# validate_config.awk
#
function is_valid(type,value) {

	# the enum type contains a definition of all allowed values as csv
	# e.g. enum,alpha,beta,gamma
	if (type ~ "enum" ) {
		split(type,tarr,",")
		for (num in tarr) {
			if (num > 0) {
				gsub("^[ \t\n]*","",tarr[num]);
				gsub("[ \t\n]*$","",tarr[num]);
				if (tarr[num] == value) {
					return 1
				}	
			}
		}
		return 0;
	}

	# all other types are checked as defined in the former validate.awk
	if (type ~ "int") return is_int(value)
	if (type ~ "ip" ) return is_ip(value)
	if (type ~ "netmask" ) return is_netmask(value)
	if (type ~ "string" ) return is_string(value)
	if (type ~ "wep" ) return is_wep(value)
	if (type ~ "hostname" ) return is_hostname(value)
	if (type ~ "mac" ) return is_mac(value)
	if (type ~ "port" ) return is_port(value)
	if (type ~ "ports" ) return is_ports(value)
	if (type ~ "wpapsk" ) return is_wpapsk(value)
}

# validate_config compares the specification as parsed from the spec file
# with the environment variables
# CONFIG_SECTION contains the relevant config section name, e.g. wan
# CONFIG_<section>_TYPE contains the type of the config, e.g. interface
# CONFIG_<section>_<var> contains the value of the config option <var>
#
function validate_config() {
	# get the config section name
	vname=ENVIRON["CONFIG_SECTION"]
	if (!length(vname)) {
		print STDERR "Error: no current configuration"
		exit 1
	}
	# get the config section type
	vsec=ENVIRON["CONFIG_" vname "_TYPE"]
	if (!length(vsec)) {
		print STDERR "Error: section " vsec " not found"
		exit 1
	}

	# loop through all config options specified for this section type
	split(vars[vsec],options,",")
	for (oidx in options) {
		# first, look for all required attributes
		var=options[oidx]
		check_requirements(vname,var)

		# next look at each option and validate it
		val=ENVIRON["CONFIG_" vname "_" var]
		if (length(val)) {
			if (!is_valid(types[vsec "_" var],val)) {
				print "Error: type validation error for '" var "' in section '" vname "'"
				exit 1
			}
		}
	}
}


END {
	validate_config()
}
