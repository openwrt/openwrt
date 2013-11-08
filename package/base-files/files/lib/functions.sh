#!/bin/sh
# Copyright (C) 2006-2013 OpenWrt.org
# Copyright (C) 2006 Fokus Fraunhofer <carsten.tittel@fokus.fraunhofer.de>
# Copyright (C) 2010 Vertical Communications


debug () {
	${DEBUG:-:} "$@"
}

# newline
N="
"

_C=0
NO_EXPORT=1
LOAD_STATE=1
LIST_SEP=" "

hotplug_dev() {
	env -i ACTION=$1 INTERFACE=$2 /sbin/hotplug-call net
}

append() {
	local var="$1"
	local value="$2"
	local sep="${3:- }"

	eval "export ${NO_EXPORT:+-n} -- \"$var=\${$var:+\${$var}\${value:+\$sep}}\$value\""
}

list_contains() {
	local var="$1"
	local str="$2"
	local val

	eval "val=\" \${$var} \""
	[ "${val%% $str *}" != "$val" ]
}

list_remove() {
	local var="$1"
	local remove="$2"
	local val

	eval "val=\" \${$var} \""
	val1="${val%% $remove *}"
	[ "$val1" = "$val" ] && return
	val2="${val##* $remove }"
	[ "$val2" = "$val" ] && return
	val="${val1## } ${val2%% }"
	val="${val%% }"
	eval "export ${NO_EXPORT:+-n} -- \"$var=\$val\""
}

config_load() {
	[ -n "$IPKG_INSTROOT" ] && return 0
	uci_load "$@"
}

reset_cb() {
	config_cb() { return 0; }
	option_cb() { return 0; }
	list_cb() { return 0; }
}
reset_cb

package() {
	return 0
}

config () {
	local cfgtype="$1"
	local name="$2"

	export ${NO_EXPORT:+-n} CONFIG_NUM_SECTIONS=$(($CONFIG_NUM_SECTIONS + 1))
	name="${name:-cfg$CONFIG_NUM_SECTIONS}"
	append CONFIG_SECTIONS "$name"
	[ -n "$NO_CALLBACK" ] || config_cb "$cfgtype" "$name"
	export ${NO_EXPORT:+-n} CONFIG_SECTION="$name"
	export ${NO_EXPORT:+-n} "CONFIG_${CONFIG_SECTION}_TYPE=$cfgtype"
}

option () {
	local varname="$1"; shift
	local value="$*"

	export ${NO_EXPORT:+-n} "CONFIG_${CONFIG_SECTION}_${varname}=$value"
	[ -n "$NO_CALLBACK" ] || option_cb "$varname" "$*"
}

list() {
	local varname="$1"; shift
	local value="$*"
	local len

	config_get len "$CONFIG_SECTION" "${varname}_LENGTH" 0
	[ $len = 0 ] && append CONFIG_LIST_STATE "${CONFIG_SECTION}_${varname}"
	len=$(($len + 1))
	config_set "$CONFIG_SECTION" "${varname}_ITEM$len" "$value"
	config_set "$CONFIG_SECTION" "${varname}_LENGTH" "$len"
	append "CONFIG_${CONFIG_SECTION}_${varname}" "$value" "$LIST_SEP"
	list_cb "$varname" "$*"
}

config_rename() {
	local OLD="$1"
	local NEW="$2"
	local oldvar
	local newvar

	[ -n "$OLD" -a -n "$NEW" ] || return
	for oldvar in `set | grep ^CONFIG_${OLD}_ | \
		sed -e 's/\(.*\)=.*$/\1/'` ; do
		newvar="CONFIG_${NEW}_${oldvar##CONFIG_${OLD}_}"
		eval "export ${NO_EXPORT:+-n} \"$newvar=\${$oldvar}\""
		unset "$oldvar"
	done
	export ${NO_EXPORT:+-n} CONFIG_SECTIONS="$(echo " $CONFIG_SECTIONS " | sed -e "s, $OLD , $NEW ,")"

	[ "$CONFIG_SECTION" = "$OLD" ] && export ${NO_EXPORT:+-n} CONFIG_SECTION="$NEW"
}

config_unset() {
	config_set "$1" "$2" ""
}

config_clear() {
	local SECTION="$1"
	local oldvar

	list_remove CONFIG_SECTIONS "$SECTION"
	export ${NO_EXPORT:+-n} CONFIG_SECTIONS="${SECTION:+$CONFIG_SECTIONS}"

	for oldvar in `set | grep ^CONFIG_${SECTION:+${SECTION}_} | \
		sed -e 's/\(.*\)=.*$/\1/'` ; do
		unset $oldvar
	done
}

# config_get <variable> <section> <option> [<default>]
# config_get <section> <option>
config_get() {
	case "$3" in
		"") eval echo "\${CONFIG_${1}_${2}:-\${4}}";;
		*)  eval export ${NO_EXPORT:+-n} -- "${1}=\${CONFIG_${2}_${3}:-\${4}}";;
	esac
}

# config_get_bool <variable> <section> <option> [<default>]
config_get_bool() {
	local _tmp
	config_get _tmp "$2" "$3" "$4"
	case "$_tmp" in
		1|on|true|enabled) _tmp=1;;
		0|off|false|disabled) _tmp=0;;
		*) _tmp="$4";;
	esac
	export ${NO_EXPORT:+-n} "$1=$_tmp"
}

config_set() {
	local section="$1"
	local option="$2"
	local value="$3"
	local old_section="$CONFIG_SECTION"

	CONFIG_SECTION="$section"
	option "$option" "$value"
	CONFIG_SECTION="$old_section"
}

config_foreach() {
	local ___function="$1"
	[ "$#" -ge 1 ] && shift
	local ___type="$1"
	[ "$#" -ge 1 ] && shift
	local section cfgtype

	[ -z "$CONFIG_SECTIONS" ] && return 0
	for section in ${CONFIG_SECTIONS}; do
		config_get cfgtype "$section" TYPE
		[ -n "$___type" -a "x$cfgtype" != "x$___type" ] && continue
		eval "$___function \"\$section\" \"\$@\""
	done
}

config_list_foreach() {
	[ "$#" -ge 3 ] || return 0
	local section="$1"; shift
	local option="$1"; shift
	local function="$1"; shift
	local val
	local len
	local c=1

	config_get len "${section}" "${option}_LENGTH"
	[ -z "$len" ] && return 0
	while [ $c -le "$len" ]; do
		config_get val "${section}" "${option}_ITEM$c"
		eval "$function \"\$val\" \"\$@\""
		c="$(($c + 1))"
	done
}

insert_modules() {
	[ -d /etc/modules.d ] && {
		cd /etc/modules.d
		sed 's/^[^#]/insmod &/' $* | ash 2>&- || :
	}
}

include() {
	local file

	for file in $(ls $1/*.sh 2>/dev/null); do
		. $file
	done
}

find_mtd_index() {
	local PART="$(grep "\"$1\"" /proc/mtd | awk -F: '{print $1}')"
	local INDEX="${PART##mtd}"

	echo ${INDEX}
}

find_mtd_part() {
	local INDEX=$(find_mtd_index "$1")
	local PREFIX=/dev/mtdblock

	[ -d /dev/mtdblock ] && PREFIX=/dev/mtdblock/
	echo "${INDEX:+$PREFIX$INDEX}"
}

find_mtd_chardev() {
	local INDEX=$(find_mtd_index "$1")
	local PREFIX=/dev/mtd

	[ -d /dev/mtd ] && PREFIX=/dev/mtd/
	echo "${INDEX:+$PREFIX$INDEX}"
}

mtd_get_mac_ascii()
{
	local mtdname="$1"
	local key="$2"
	local part
	local mac_dirty

	part=$(find_mtd_part "$mtdname")
	if [ -z "$part" ]; then
		echo "mtd_get_mac_ascii: partition $mtdname not found!" >&2
		return
	fi

	mac_dirty=$(strings "$part" | sed -n 's/^'"$key"'=//p')

	# "canonicalize" mac
	[ -n "$mac_dirty" ] && echo ${mac_dirty} | tr [A-F] [a-f]
}

mtd_get_mac_binary() {
	local mtdname="$1"
	local offset="$2"
	local part

	part=$(find_mtd_part "$mtdname")
	if [ -z "$part" ]; then
		echo "mtd_get_mac_binary: partition $mtdname not found!" >&2
		return
	fi

	dd bs=1 skip=$offset count=6 if=$part 2>/dev/null | hexdump -v -n 6 -e '5/1 "%02x:" 1/1 "%02x"'
}

mtd_get_part_size() {
	local part_name=$1
	local first dev size erasesize name
	while read dev size erasesize name; do
		name=${name#'"'}; name=${name%'"'}
		if [ "$name" = "$part_name" ]; then
			echo $((0x$size))
			break
		fi
	done < /proc/mtd
}

macaddr_add() {
	local mac=$1
	local val=$2
	local oui=${mac%:*:*:*}
	local nic=${mac#*:*:*:}

	nic=$(printf "%06x" $((0x${nic//:/} + $val & 0xffffff)) | sed 's/^\(.\{2\}\)\(.\{2\}\)\(.\{2\}\)/\1:\2:\3/')
	echo $oui:$nic
}

macaddr_setbit_la()
{
	local mac=$1

	printf "%02x:%s" $((0x${mac%%:*} | 0x02)) ${mac#*:}
}

macaddr_2bin()
{
	local mac=$1

	echo -ne \\x${mac//:/\\x}
}

strtok() { # <string> { <variable> [<separator>] ... }
	local tmp
	local val="$1"
	local count=0

	shift

	while [ $# -gt 1 ]; do
		tmp="${val%%$2*}"

		[ "$tmp" = "$val" ] && break

		val="${val#$tmp$2}"

		export ${NO_EXPORT:+-n} "$1=$tmp"; count=$((count+1))
		shift 2
	done

	if [ $# -gt 0 -a -n "$val" ]; then
		export ${NO_EXPORT:+-n} "$1=$val"; count=$((count+1))
	fi

	return $count
}


jffs2_mark_erase() {
	local part="$(find_mtd_part "$1")"
	[ -z "$part" ] && {
		echo Partition not found.
		return 1
	}
	echo -e "\xde\xad\xc0\xde" | mtd -qq write - "$1"
}

uci_apply_defaults() {
	cd /etc/uci-defaults || return 0
	files="$(ls)"
	[ -z "$files" ] && return 0
	mkdir -p /tmp/.uci
	for file in $files; do
		( . "./$(basename $file)" ) && rm -f "$file"
	done
	uci commit
}

group_add() {
	local name="$1"
	local gid="$2"
	local rc
	[ -f "${IPKG_INSTROOT}/etc/group" ] || return 1
	[ -n "$IPKG_INSTROOT" ] || lock /var/lock/group
	echo "${name}:x:${gid}:" >> ${IPKG_INSTROOT}/etc/group
	rc=$?
	[ -n "$IPKG_INSTROOT" ] || lock -u /var/lock/group
	return $rc
}

group_exists() {
	grep -qs "^${1}:" ${IPKG_INSTROOT}/etc/group
}

user_add() {
	local name="${1}"
	local uid="${2}"
	local gid="${3:-$2}"
	local desc="${4:-$1}"
	local home="${5:-/var/run/$1}"
	local shell="${6:-/bin/false}"
	local rc
	[ -f "${IPKG_INSTROOT}/etc/passwd" ] || return 1
	[ -n "$IPKG_INSTROOT" ] || lock /var/lock/passwd
	echo "${name}:x:${uid}:${gid}:${desc}:${home}:${shell}" >> ${IPKG_INSTROOT}/etc/passwd
	echo "${name}:x:0:0:99999:7:::" >> ${IPKG_INSTROOT}/etc/shadow
	rc=$?
	[ -n "$IPKG_INSTROOT" ] || lock -u /var/lock/passwd
	return $rc
}

user_exists() {
	grep -qs "^${1}:" ${IPKG_INSTROOT}/etc/passwd
}


pi_include() {
	if [ -f "/tmp/overlay/$1" ]; then
		. "/tmp/overlay/$1"
	elif [ -f "$1" ]; then
		. "$1"
	elif [ -d "/tmp/overlay/$1" ]; then
		if [ -n "$(ls /tmp/overlay/$1/*.sh 2>/dev/null)" ]; then
			for src_script in /tmp/overlay/$1/*.sh; do
				. "$src_script"
			done
		fi
	elif [ -d "$1" ]; then
		if [ -n "$(ls $1/*.sh 2>/dev/null)" ]; then
			for src_script in $1/*.sh; do
				. "$src_script"
			done
		fi
	else
		echo "WARNING: $1 not found"
		return 1
	fi
	return 0
}

boot_hook_splice_start() {
	export -n PI_HOOK_SPLICE=1
}

boot_hook_splice_finish() {
	local hook
	for hook in $PI_STACK_LIST; do
		local v; eval "v=\${${hook}_splice:+\$${hook}_splice }$hook"
		export -n "${hook}=${v% }"
		export -n "${hook}_splice="
	done
	export -n PI_HOOK_SPLICE=
}

boot_hook_init() {
	local hook="${1}_hook"
	export -n "PI_STACK_LIST=${PI_STACK_LIST:+$PI_STACK_LIST }$hook"
	export -n "$hook="
}

boot_hook_add() {
	local hook="${1}_hook${PI_HOOK_SPLICE:+_splice}"
	local func="${2}"

	[ -n "$func" ] && {
		local v; eval "v=\$$hook"
		export -n "$hook=${v:+$v }$func"
	}
}

boot_hook_shift() {
	local hook="${1}_hook"
	local rvar="${2}"

	local v; eval "v=\$$hook"
	[ -n "$v" ] && {
		local first="${v%% *}"

		[ "$v" != "${v#* }" ] && \
			export -n "$hook=${v#* }" || \
			export -n "$hook="

		export -n "$rvar=$first"
		return 0
	}

	return 1
}

boot_run_hook() {
	local hook="$1"
	local func

	while boot_hook_shift "$hook" func; do
		local ran; eval "ran=\$PI_RAN_$func"
		[ -n "$ran" ] || {
			export -n "PI_RAN_$func=1"
			$func "$1" "$2"
		}
	done
}

jffs2_ready() {
	mtdpart="$(find_mtd_part rootfs_data)"
	[ -z "$mtdpart" ] && return 1
	magic=$(hexdump $mtdpart -n 4 -e '4/1 "%02x"')
	[ "$magic" != "deadc0de" ]
}

dupe() { # <new_root> <old_root>
	cd $1
	echo -n "creating directories... "
	{
		cd $2
		find . -xdev -type d
		echo "./dev ./overlay ./mnt ./proc ./tmp"
		# xdev skips mounted directories
		cd $1
	} | xargs mkdir -p
	echo "done"

	echo -n "setting up symlinks... "
	for file in $(cd $2; find . -xdev -type f;); do
		case "$file" in
		./rom/note) ;; #nothing
		./etc/config*|\
		./usr/lib/opkg/info/*) cp -af $2/$file $file;;
		*) ln -sf /rom/${file#./*} $file;;
		esac
	done
	for file in $(cd $2; find . -xdev -type l;); do
		cp -af $2/${file#./*} $file
	done
	echo "done"
}

pivot() { # <new_root> <old_root>
	mount -o noatime,move /proc $1/proc && \
	pivot_root $1 $1$2 && {
		mount -o noatime,move $2/dev /dev
		mount -o noatime,move $2/tmp /tmp
		mount -o noatime,move $2/sys /sys 2>&-
		mount -o noatime,move $2/overlay /overlay 2>&-
		return 0
	}
}

fopivot() { # <rw_root> <ro_root> <dupe?>
	root=$1
	{
		if grep -q overlay /proc/filesystems; then
			mount -o noatime,lowerdir=/,upperdir=$1 -t overlayfs "overlayfs:$1" /mnt && root=/mnt
		elif grep -q mini_fo /proc/filesystems; then
			mount -t mini_fo -o noatime,base=/,sto=$1 "mini_fo:$1" /mnt 2>&- && root=/mnt
		else
			mount --bind -o noatime / /mnt
			mount --bind -o noatime,union "$1" /mnt && root=/mnt
		fi
	} || {
		[ "$3" = "1" ] && {
		mount | grep "on $1 type" 2>&- 1>&- || mount -o noatime,bind $1 $1
		dupe $1 $rom
		}
	}
	pivot $root $2
}

ramoverlay() {
	mkdir -p /tmp/root
	mount -t tmpfs -o noatime,mode=0755 root /tmp/root
	fopivot /tmp/root /rom 1
}

[ -z "$IPKG_INSTROOT" -a -f /lib/config/uci.sh ] && . /lib/config/uci.sh
