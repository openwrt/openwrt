# Configuration update functions - AWK API
#
# Copyright (C) 2006 by Fokus Fraunhofer <carsten.tittel@fokus.fraunhofer.de>
# Copyright (C) 2006 by Felix Fietkau <nbd@openwrt.org>
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

# parameters: 1
function config_load(package, var) {
	while (("/bin/ash -c '. /etc/functions.sh; unset NO_EXPORT; config_load \""package"\"; env | grep \"^CONFIG_\"'" | getline) == 1) {
		sub("^CONFIG_", "")
		if (match($0, "=") == 0) {
			if (var != "") CONFIG[var] = CONFIG[var] "\n" $0
			next
		}
		var=substr($0, 1, RSTART-1)
		CONFIG[var] = substr($0, RSTART+1, length($0) - RSTART)
	}
}

# parameters: 2
function config_get(package, option) {
	return CONFIG[package "_" option]
}

# parameters: 3
function config_get_bool(package, option, default, var) {
	var = config_get(package, option);
	if ((var == "enabled") || (var == "1") || (var == "on")) return 1
	if ((var == "disabled") || (var == "0") || (var == "off")) return 1
	return (var && var != "0" ? 1 : 0)
}


function read_file(filename,  result) {
	while ((getline <filename) == 1) {
		result = result $0 "\n"
	}
	gsub(/\n*$/, "", result)
	return result
}

function uci_cmd2option(str,  tmp) {
	if (match(str,"=")!=0) {
		res = "\toption " substr(str,1,RSTART-1) "\t'" substr(str,RSTART+1) "'"
	} else {
		res= ""
	}
	return res
}

function uci_cmd2config(atype,  aname) {
	return "config \"" atype "\" \"" aname "\""
}

function uci_update_config(cfg, update,  \
  lines, line, l, n, i, i2, section, scnt, remove, tmp, aidx, rest) {
	scnt = 1
	linecnt=split(cfg "\n", lines, "\n")

	cfg = ""
	for (n = 1; n < linecnt; n++) {
		# stupid parser for quoted arguments (e.g. for the type string).
		# not to be used to gather variable values (backslash escaping doesn't work)
		line = lines[n]
		gsub(/^[ \t]*/, "", line)
		gsub(/#.*$/, "", line)
		i2 = 1
		delete l
		rest = line
        	while (length(rest)) {
			if (match(rest, /[ \t\"]+/)) {
				if (RSTART>1) {
					l[i2] = substr(rest,1,RSTART-1)
					i2++
				}
				aidx=index(rest,"\"")
				if (aidx>=RSTART && aidx<=RSTART+RLENGTH) {
					rest=substr(rest,aidx+1)
					# find the end of the string
					match(rest,/\"/)
					l[i2]=substr(rest,1,RSTART-1)
					i2++
				}
				rest=substr(rest,RSTART+RLENGTH)
			} else {
				l[i2] = rest
				i2++
				rest = ""
			}
		}
		line = lines[n]
		
		# when a command wants to set a config value for the current
		# section and a blank line is encountered before an option with
		# the same name, insert it here to maintain some coherency between
		# manually and automatically created option lines
		# if an option with the same name appears after this point, simply
		# ignore it, because it is already set.
		if ((section != "") && (l[1] != "option")) {
			if (line ~ /^[ \t]*$/) {
				if (update ~ "^" section "\\.") {
					gsub("^" section ".", "", update)
					cfg = cfg uci_cmd2option(update) "\n"
					gsub(/=.*$/, "", update)
					update = "-" section "." update
				}
			}
		}

		if (l[1] == "config") {
			# look for all unset values
			if (section != "") {
				flag=0
				if (update ~ "^" section "\\.") {
					flag=1
					gsub("^" section ".", "", update)
					cfg = cfg uci_cmd2option(update) "\n"
					
					update = "-" section "." update
				} 
				if (flag!=0) cfg = cfg "\n"
			}
			
			remove = ""
			section = l[3]
			if (!length(section)) {
				section = "cfg" scnt
			}	
			scnt++
			if (update == "-" section) {
				remove = "section"
				update = ""
			} else if (update ~ "^@" section "=") {
				update = ""
			} else if (update ~ "^&" section "=") {
				gsub("^&" section "=", "", update)
				line = uci_cmd2config(l[2],update) 
				update = ""
			}
		}
		if (remove == "option") remove = ""
		if (l[1] == "option") {
			if (update ~ "^-" section "\\." l[2] "$") remove = "option"
			# if a supplied config value already exists, replace the whole line
			if (match(update, "^" section "." l[2] "=")) {
				gsub("^" section ".", "", update)
				line = uci_cmd2option(update)
				update = ""
			}
		}
		if (remove == "") cfg = cfg line "\n"
	}
	
	# any new options for the last section??
	if (section != "") {
		if (update ~ "^" section "\\.") {
			gsub("^" section ".", "", update)
			cfg = cfg uci_cmd2option(update) "\n"

			update = "-" section "." update
		} 
	}

	if (update ~ "^@") {
		# new section
		section = stype = substr(update,2)
		gsub(/=.*$/,"",section)
		gsub(/^.*=/,"",stype)
		cfg = cfg "\nconfig \"" stype "\" \"" section "\"\n"
	}

	return cfg
}
