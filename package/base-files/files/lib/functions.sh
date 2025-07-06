# Copyright (C) 2006-2014 OpenWrt.org
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

# xor multiple hex values of the same length
xor() {
	local val
	local ret="0x$1"
	local retlen=${#1}

	shift
	while [ -n "$1" ]; do
		val="0x$1"
		ret=$((ret ^ val))
		shift
	done

	printf "%0${retlen}x" "$ret"
}

data_2bin() {
	local data=$1
	local len=${#1}
	local bin_data

	for i in $(seq 0 2 $(($len - 1))); do
		bin_data="${bin_data}\x${data:i:2}"
	done

	echo -ne $bin_data
}

data_2xor_val() {
	local data=$1
	local len=${#1}
	local xor_data

	for i in $(seq 0 4 $(($len - 1))); do
		xor_data="${xor_data}${data:i:4} "
	done

	echo -n ${xor_data:0:-1}
}

append() {
	local var="$1"
	local value="$2"
	local sep="${3:- }"

	eval "export ${NO_EXPORT:+-n} -- \"$var=\${$var:+\${$var}\${value:+\$sep}}\$value\""
}

prepend() {
	local var="$1"
	local value="$2"
	local sep="${3:- }"

	eval "export ${NO_EXPORT:+-n} -- \"$var=\$value\${$var:+\${sep}\${$var}}\""
}

list_contains() {
	local var="$1"
	local str="$2"
	local val

	eval "val=\" \${$var} \""
	[ "${val%% $str *}" != "$val" ]
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

	export ${NO_EXPORT:+-n} CONFIG_NUM_SECTIONS=$((CONFIG_NUM_SECTIONS + 1))
	name="${name:-cfg$CONFIG_NUM_SECTIONS}"
	append CONFIG_SECTIONS "$name"
	export ${NO_EXPORT:+-n} CONFIG_SECTION="$name"
	config_set "$CONFIG_SECTION" "TYPE" "${cfgtype}"
	[ -n "$NO_CALLBACK" ] || config_cb "$cfgtype" "$name"
}

option () {
	local varname="$1"; shift
	local value="$*"

	config_set "$CONFIG_SECTION" "${varname}" "${value}"
	[ -n "$NO_CALLBACK" ] || option_cb "$varname" "$*"
}

list() {
	local varname="$1"; shift
	local value="$*"
	local len

	config_get len "$CONFIG_SECTION" "${varname}_LENGTH" 0
	[ $len = 0 ] && append CONFIG_LIST_STATE "${CONFIG_SECTION}_${varname}"
	len=$((len + 1))
	config_set "$CONFIG_SECTION" "${varname}_ITEM$len" "$value"
	config_set "$CONFIG_SECTION" "${varname}_LENGTH" "$len"
	append "CONFIG_${CONFIG_SECTION}_${varname}" "$value" "$LIST_SEP"
	[ -n "$NO_CALLBACK" ] || list_cb "$varname" "$*"
}

config_unset() {
	config_set "$1" "$2" ""
}

# config_get <variable> <section> <option> [<default>]
# config_get <section> <option>
config_get() {
	case "$2${3:-$1}" in
		*[!A-Za-z0-9_]*) : ;;
		*)
			case "$3" in
				"") eval echo "\"\${CONFIG_${1}_${2}:-\${4}}\"";;
				*)  eval export ${NO_EXPORT:+-n} -- "${1}=\${CONFIG_${2}_${3}:-\${4}}";;
			esac
		;;
	esac
}

# get_bool <value> [<default>]
get_bool() {
	local _tmp="$1"
	case "$_tmp" in
		1|on|true|yes|enabled) _tmp=1;;
		0|off|false|no|disabled) _tmp=0;;
		*) _tmp="$2";;
	esac
	echo -n "$_tmp"
}

# config_get_bool <variable> <section> <option> [<default>]
config_get_bool() {
	local _tmp
	config_get _tmp "$2" "$3" "$4"
	_tmp="$(get_bool "$_tmp" "$4")"
	export ${NO_EXPORT:+-n} "$1=$_tmp"
}

config_set() {
	local section="$1"
	local option="$2"
	local value="$3"

	export ${NO_EXPORT:+-n} "CONFIG_${section}_${option}=${value}"
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
		[ -n "$___type" ] && [ "x$cfgtype" != "x$___type" ] && continue
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
		c="$((c + 1))"
	done
}

default_prerm() {
	local root="${IPKG_INSTROOT}"
	[ -z "$pkgname" ] && local pkgname="$(basename ${1%.*})"
	local ret=0
	local filelist="${root}/usr/lib/opkg/info/${pkgname}.list"
	[ -f "$root/lib/apk/packages/${pkgname}.list" ] && filelist="$root/lib/apk/packages/${pkgname}.list"

	if [ -e "$root/lib/apk/packages/${pkgname}.alternatives" ]; then
		update_alternatives remove "${pkgname}"
	fi

	if [ -f "$root/usr/lib/opkg/info/${pkgname}.prerm-pkg" ]; then
		( . "$root/usr/lib/opkg/info/${pkgname}.prerm-pkg" )
		ret=$?
	fi

	local shell="$(command -v bash)"
	for i in $(grep -s "^/etc/init.d/" "$filelist"); do
		if [ -n "$root" ]; then
			${shell:-/bin/sh} "$root/etc/rc.common" "$root$i" disable
		else
			if [ "$PKG_UPGRADE" != "1" ]; then
				"$i" disable
			fi
			"$i" stop
		fi
	done

	return $ret
}

add_group_and_user() {
	[ -z "$pkgname" ] && local pkgname="$(basename ${1%.*})"
	local rusers="$(sed -ne 's/^Require-User: *//p' $root/usr/lib/opkg/info/${pkgname}.control 2>/dev/null)"
	if [ -f "$root/lib/apk/packages/${pkgname}.rusers" ]; then
		local rusers="$(cat $root/lib/apk/packages/${pkgname}.rusers)"
	fi

	if [ -n "$rusers" ]; then
		local tuple oIFS="$IFS"
		for tuple in $rusers; do
			local uid gid uname gname addngroups addngroup addngname addngid

			IFS=":"
			set -- $tuple; uname="$1"; gname="$2"; addngroups="$3"
			IFS="="
			set -- $uname; uname="$1"; uid="$2"
			set -- $gname; gname="$1"; gid="$2"
			IFS="$oIFS"

			if [ -n "$gname" ] && [ -n "$gid" ]; then
				group_exists "$gname" || group_add "$gname" "$gid"
			elif [ -n "$gname" ]; then
				gid="$(group_add_next "$gname")"
			fi

			if [ -n "$uname" ]; then
				user_exists "$uname" || user_add "$uname" "$uid" "$gid"
			fi

			if [ -n "$uname" ] && [ -n "$gname" ]; then
				group_add_user "$gname" "$uname"
			fi

			if [ -n "$uname" ] &&  [ -n "$addngroups" ]; then
				oIFS="$IFS"
				IFS=","
				for addngroup in $addngroups ; do
					IFS="="
					set -- $addngroup; addngname="$1"; addngid="$2"
					if [ -n "$addngid" ]; then
						group_exists "$addngname" || group_add "$addngname" "$addngid"
					else
						group_add_next "$addngname"
					fi

					group_add_user "$addngname" "$uname"
				done
				IFS="$oIFS"
			fi

			unset uid gid uname gname addngroups addngroup addngname addngid
		done
	fi
}

update_alternatives() {
	local root="${IPKG_INSTROOT}"
	local action="$1"
	local pkgname="$2"

	if [ -f "$root/lib/apk/packages/${pkgname}.alternatives" ]; then
		for pkg_alt in $(cat $root/lib/apk/packages/${pkgname}.alternatives); do
			local best_prio=0;
			local best_src="/bin/busybox";
			pkg_prio=${pkg_alt%%:*};
			pkg_target=${pkg_alt#*:};
			pkg_target=${pkg_target%:*};
			pkg_src=${pkg_alt##*:};

			if [ -e "$root/$target" ]; then
				for alts in $root/lib/apk/packages/*.alternatives; do
					for alt in $(cat $alts); do
						prio=${alt%%:*};
						target=${alt#*:};
						target=${target%:*};
						src=${alt##*:};

						if [ "$target" = "$pkg_target" ] &&
						   [ "$src" != "$pkg_src" ] &&
						   [ "$best_prio" -lt "$prio" ]; then
							best_prio=$prio;
							best_src=$src;
						fi
					done
				done
			fi
			case "$action" in
				install)
					if [ "$best_prio" -lt "$pkg_prio" ]; then
						ln -sf "$pkg_src" "$root/$pkg_target"
						echo "add alternative: $pkg_target -> $pkg_src"
					fi
				;;
				remove)
					if [ "$best_prio" -lt "$pkg_prio" ]; then
						ln -sf "$best_src" "$root/$pkg_target"
						echo "add alternative: $pkg_target -> $best_src"
					fi
				;;
			esac
		done
	fi
}

default_postinst() {
	local root="${IPKG_INSTROOT}"
	[ -z "$pkgname" ] && local pkgname="$(basename ${1%.*})"
	local filelist="${root}/usr/lib/opkg/info/${pkgname}.list"
	[ -f "$root/lib/apk/packages/${pkgname}.list" ] && filelist="$root/lib/apk/packages/${pkgname}.list"
	local ret=0

	if [ -e "${root}/usr/lib/opkg/info/${pkgname}.list" ]; then
		filelist="${root}/usr/lib/opkg/info/${pkgname}.list"
		add_group_and_user "${pkgname}"
	fi

	if [ -e "${root}/lib/apk/packages/${pkgname}.alternatives" ]; then
		update_alternatives install "${pkgname}"
	fi

	if [ -d "$root/rootfs-overlay" ]; then
		cp -R $root/rootfs-overlay/. $root/
		rm -fR $root/rootfs-overlay/
	fi

	if [ -z "$root" ]; then
		if grep -m1 -q -s "^/etc/modules.d/" "$filelist"; then
			kmodloader
		fi

		if grep -m1 -q -s "^/etc/sysctl.d/" "$filelist"; then
			/etc/init.d/sysctl restart
		fi

		if grep -m1 -q -s "^/etc/uci-defaults/" "$filelist"; then
			[ -d /tmp/.uci ] || mkdir -p /tmp/.uci
			for i in $(grep -s "^/etc/uci-defaults/" "$filelist"); do
				( [ -f "$i" ] && cd "$(dirname $i)" && . "$i" ) && rm -f "$i"
			done
			uci commit
		fi

		rm -f /tmp/luci-indexcache
	fi

	if [ -f "$root/usr/lib/opkg/info/${pkgname}.postinst-pkg" ]; then
		( . "$root/usr/lib/opkg/info/${pkgname}.postinst-pkg" )
		ret=$?
	fi

	local shell="$(command -v bash)"
	for i in $(grep -s "^/etc/init.d/" "$filelist"); do
		if [ -n "$root" ]; then
			${shell:-/bin/sh} "$root/etc/rc.common" "$root$i" enable
		else
			if [ "$PKG_UPGRADE" != "1" ]; then
				"$i" enable
			fi
			"$i" start
		fi
	done

	return $ret
}

include() {
	local file

	for file in $(ls $1/*.sh 2>/dev/null); do
		. $file
	done
}

ipcalc() {
	set -- $(ipcalc.sh "$@")
	[ $? -eq 0 ] && export -- "$@"
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

find_mmc_part() {
	local DEVNAME PARTNAME ROOTDEV

	if grep -q "$1" /proc/mtd; then
		echo "" && return 0
	fi

	if [ -n "$2" ]; then
		ROOTDEV="$2"
	else
		ROOTDEV="mmcblk*"
	fi

	for DEVNAME in /sys/block/$ROOTDEV/mmcblk*p*; do
		PARTNAME="$(grep PARTNAME ${DEVNAME}/uevent | cut -f2 -d'=')"
		[ "$PARTNAME" = "$1" ] && echo "/dev/$(basename $DEVNAME)" && return 0
	done
}

group_add() {
	local name="$1"
	local gid="$2"
	local rc
	[ -f "${IPKG_INSTROOT}/etc/group" ] || return 1
	[ -n "$IPKG_INSTROOT" ] || lock /var/lock/group
	echo "${name}:x:${gid}:" >> ${IPKG_INSTROOT}/etc/group
	[ -n "$IPKG_INSTROOT" ] || lock -u /var/lock/group
}

group_exists() {
	grep -qs "^${1}:" ${IPKG_INSTROOT}/etc/group
}

group_add_next() {
	local gid gids
	gid=$(grep -s "^${1}:" ${IPKG_INSTROOT}/etc/group | cut -d: -f3)
	if [ -n "$gid" ]; then
		echo $gid
		return
	fi
	gids=$(cut -d: -f3 ${IPKG_INSTROOT}/etc/group)
	gid=32768
	while echo "$gids" | grep -q "^$gid$"; do
		gid=$((gid + 1))
	done
	group_add $1 $gid
	echo $gid
}

group_add_user() {
	local grp delim=","
	grp=$(grep -s "^${1}:" ${IPKG_INSTROOT}/etc/group)
	echo "$grp" | cut -d: -f4 | grep -q $2 && return
	echo "$grp" | grep -q ":$" && delim=""
	[ -n "$IPKG_INSTROOT" ] || lock /var/lock/passwd
	sed -i "s/$grp/$grp$delim$2/g" ${IPKG_INSTROOT}/etc/group
	if [ -z "$IPKG_INSTROOT" ] && [ -x /usr/sbin/selinuxenabled ] && selinuxenabled; then
		restorecon /etc/group
	fi
	[ -n "$IPKG_INSTROOT" ] || lock -u /var/lock/passwd
}

user_add() {
	local name="${1}"
	local uid="${2}"
	local gid="${3}"
	local desc="${4:-$1}"
	local home="${5:-/var/run/$1}"
	local shell="${6:-/bin/false}"
	local rc
	[ -z "$uid" ] && {
		uids=$(cut -d: -f3 ${IPKG_INSTROOT}/etc/passwd)
		uid=32768
		while echo "$uids" | grep -q "^$uid$"; do
			uid=$((uid + 1))
		done
	}
	[ -z "$gid" ] && gid=$uid
	[ -f "${IPKG_INSTROOT}/etc/passwd" ] || return 1
	[ -n "$IPKG_INSTROOT" ] || lock /var/lock/passwd
	echo "${name}:x:${uid}:${gid}:${desc}:${home}:${shell}" >> ${IPKG_INSTROOT}/etc/passwd
	echo "${name}:x:0:0:99999:7:::" >> ${IPKG_INSTROOT}/etc/shadow
	[ -n "$IPKG_INSTROOT" ] || lock -u /var/lock/passwd
}

user_exists() {
	grep -qs "^${1}:" ${IPKG_INSTROOT}/etc/passwd
}

board_name() {
	[ -e /tmp/sysinfo/board_name ] && cat /tmp/sysinfo/board_name || echo "generic"
}

cmdline_get_var() {
	local var=$1
	local cmdlinevar tmp

	for cmdlinevar in $(cat /proc/cmdline); do
		tmp=${cmdlinevar##${var}}
		[ "=" = "${tmp:0:1}" ] && echo ${tmp:1}
	done
}

[ -z "$IPKG_INSTROOT" ] && [ -f /lib/config/uci.sh ] && . /lib/config/uci.sh || true
