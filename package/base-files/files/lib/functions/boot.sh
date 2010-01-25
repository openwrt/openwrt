#!/bin/sh
# Copyright (C) 2006-2010 OpenWrt.org
# Copyright (C) 2010 Vertical Communications

boot_hook_add() {
    	local hook="${1}_hook"
	local value="$2"
	local sep=" "
	
	eval "$hook=\"\${$hook:+\${$hook}\${value:+\$sep}}\$value\""
}

boot_run_hook() {
    local boot_func
    for boot_func in $(eval "echo \"\$${1}_hook\""); do
	$boot_func "$1" "$2"
    done
}

find_mtd_part() {
	local PART="$(grep "\"$1\"" /proc/mtd | awk -F: '{print $1}')"
	local PREFIX=/dev/mtdblock
	
	PART="${PART##mtd}"
	[ -d /dev/mtdblock ] && PREFIX=/dev/mtdblock/
	echo "${PART:+$PREFIX$PART}"
}

jffs2_ready () {
    mtdpart="$(find_mtd_part rootfs_data)"
    magic=$(hexdump $mtdpart -n 4 -e '4/1 "%02x"')
    [ "$magic" != "deadc0de" ]
}

dupe() { # <new_root> <old_root>
	cd $1
	echo -n "creating directories... "
	{
		cd $2 
		find . -xdev -type d
		echo "./dev ./jffs ./mnt ./proc ./tmp"
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
	mount -o move /proc $1/proc && \
	pivot_root $1 $1$2 && {
		mount -o move $2/dev /dev
		mount -o move $2/tmp /tmp
		mount -o move $2/sys /sys 2>&-
		mount -o move $2/jffs /jffs 2>&-
		return 0
	}
}

fopivot() { # <rw_root> <ro_root> <dupe?>
	root=$1
	{
		if grep -q mini_fo /proc/filesystems; then
			mount -t mini_fo -o base=/,sto=$1 "mini_fo:$1" /mnt 2>&- && root=/mnt
		else
			mount --bind / /mnt
			mount --bind -o union "$1" /mnt && root=/mnt 
		fi
	} || {
		[ "$3" = "1" ] && {
		mount | grep "on $1 type" 2>&- 1>&- || mount -o bind $1 $1
		dupe $1 $rom
		}
	}
	pivot $root $2
}

ramoverlay() {
	mkdir -p /tmp/root
	mount -t tmpfs root /tmp/root
	fopivot /tmp/root /rom 1
}
