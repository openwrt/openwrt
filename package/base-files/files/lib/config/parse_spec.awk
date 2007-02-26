# AWK file for parsing uci specification files
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
BEGIN {
	section_count=1
	section = ""
	simple_types = "int|ip|netmask|string|wep|hostname|mac|port|ports|wpapsk"
}

# function print_specification
# - prints all information about the created tables containing the
#   specification 
function print_specification() {
	for (section in vars) {
		printf("%s\n",section);
		split(vars[section],arr,",")
		for (idx in arr) {
			printf("\t%s[%s]",arr[idx],types[section "_" arr[idx]]); 
			if (length(reqs[section "_" arr[idx]])) {
				if (reqs[section "_" arr[idx]]==1) {
					printf(",req");
				}else{
					printf(", req(%s)", reqs[section "_" arr[idx]]);
				}
			}
			if (length(opts[section "_" arr[idx]])) {
				printf(", opt(%s)", opts[section "_" arr[idx]]);
			}
			printf("\n");
		}
	}
}


function reset_option() {
	# just set global variables parsed on one line back to defaults
	var = ""
	type = ""
	required = ""
	optional = ""
	found = 0
}

function store_option() {
	# save all information about a config option parsed from the spec file
	# to the relevant tables for future use

	# first check minimum requirements for storing information
	if (!length(section)) {
		print STDERR "line " NR ": section definition missing"
		exit 1
	}
	if (!length(var)) {
		print STDERR "line " NR ": invalid config option name"
		exit 1
	}
	if (!length(type)) {
		print STDERR "line " NR ": invalid config option type"
		exit 1
	}

	# add config options to the names of options available for this
	# section
	if (exists[section]!=1) {
		section_names[section_count] = section
		section_count++
		exists[section] = 1
		vars[section] = var
	} else {
		vars[section] = vars[section] "," var
	}
	
	# save the type, the requirements and the optional scope of the 
	# config option
	types[section "_" var] = type
	reqs[section "_" var] = required
	opts[section "_" var] = optional
}

/^declare -x|^export/ { 
	sub(/^declare -x /,"")
	sub(/^export /,"")
	split($0,arr,"=")
	val=substr(arr[2],2,length(arr[2])-2)
	ENVIRON[arr[1]] = val
	next
}

# main parsing function
# this is done in one function block to allow multiple semicolon separated
# definitions on one line
{
	# replace leading/trailing white space
	gsub("^[ \t\n]+","");
	gsub("[ \t\n]+$","");

	# comments are removed
	# XXX todo: check for quoted comments??
	if (match($0,/[^#]*/)) {
		rest=substr($0,RSTART,RLENGTH)
	} else {
		rest=$0
	}

	# match the config section "<section> {"
	if (match(rest,/^[^ \t\n{]+[ \t\n]*\{/)) {
		match(rest,/^[^ \t\n{]+/)
		section = substr(rest,RSTART,RLENGTH)
		rest=substr($0,RSTART+RLENGTH);
		match(rest,/[ \t\n]*\{/)
		rest=substr(rest,RSTART+RLENGTH)
		# check for array indication
		if (match(section,/\[[ \t\n]*\]/)) {
			section=substr(section,1,RSTART-1)
			multiple[section] = 1
		} else {
			multiple[section] = 0
		}
	}

	reset_option()

	# parse the remaing line as long as there is something to parse
	while (rest ~ "[^ \t\n}]+") {
		found = 0

		# get option name and option type
		# first, check for "simple" datatype definitions
		if (match(rest,"[^: \t\n]+[ \t\n]*:[ \t\n]*(" \
		                        simple_types ")")){
			match(rest,"[^: \t\n]+")
			var=substr(rest,RSTART,RLENGTH)
			rest=substr(rest,RSTART+RLENGTH)
			match(rest,"[ \t\n]*:[ \t\n]*")
			rest=substr(rest,RSTART+RLENGTH)
			match(rest,"(" simple_types ")")
			type=substr(rest,RSTART,RLENGTH)
			rest = substr(rest,RSTART+RLENGTH)
			found = 1
		# next, check for enum definitions
		} else if (match(rest,/[^: \t\n]+[ \t\n]*:[ \t\n]*enum\([^\)]+\)/ )) {
			match(rest,"[^: \t\n]+")
			var=substr(rest,RSTART,RLENGTH)
			rest=substr(rest,RSTART+RLENGTH)
			match(rest,/[ \t\n]*:[ \t\n]*enum\(/)
			rest=substr(rest,RSTART+RLENGTH)
			match(rest,/[^\)]+/)
			type="enum," substr(rest,RSTART,RLENGTH)
			rest = substr(rest,RSTART+RLENGTH+1)
			found=1
		}			

		# after the name and the type, 
		# get the option requirements/scope
		if (match(rest,/[^,]*,[ \t\n]*required\[[^]]+\]/)) {
			match(rest,"[^,]*")
			save=substr(rest,RSTART,RLENGTH)
			rest=substr(rest,RSTART+RLENGTH)
			match(rest,/,[ \t\n]*required\[/);
			rest=substr(rest,RSTART+RLENGTH)
			match(rest,/[^]]+\]/)
			required=substr(rest,RSTART,RLENGTH-1)
			save=save substr(rest,RSTART+RLENGTH)
			rest=save
			found=1
		} else if (match(rest,/[^,]*,[ \t\n]*required/)) {
			match(rest,"[^,]*")
			save=substr(rest,RSTART,RLENGTH)
			rest=substr(rest,RSTART+RLENGTH)
			match(rest,",[ \t\n]*required");
			rest=substr(rest,RSTART+RLENGTH)
			required=1
			save=save substr(rest,RSTART+RLENGTH)
			rest=save
			found=1
		}
		if (match(rest,/[^,]*,[ \t\n]*optional\[[^]]+\]/)) {
			match(rest,"[^,]*")
			save=substr(rest,RSTART,RLENGTH)
			rest=substr(rest,RSTART+RLENGTH)
			match(rest,/,[ \t\n]*optional\[/);
			rest=substr(rest,RSTART+RLENGTH)
			match(rest,/[^]]+\]/)
			optional=substr(rest,RSTART,RLENGTH-1)
			save=save substr(rest,RSTART+RLENGTH)
			rest=save
			found=1
		}
	
		# if the remaining line contains a semicolon, complete the
		# specification of the config options
		if (match(rest, "^[ \t\n]*;(.*)")) {
			match(rest,"^[ \t\n]*;")
			rest=substr(rest,RSTART+RLENGTH)
			if (found==1) {
				store_option()
			}
			reset_option()

		# if nothing matched on this line, clear the rest
		} else if (!found) {
			rest = ""
		}
	}

	# after the line is pared, store the configuration option in the
	# table if any has been defined
	if (length(var)) {
		store_option()
		reset_option()
	}
	# close the section if the line contained a closing section bracket, 
	# XXX todo: check if this has to be done more intelligent
	if ($0 ~ /\}/) {
		section=""
	}
}
