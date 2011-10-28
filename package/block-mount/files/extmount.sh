#!/bin/sh
# Copyright (C) 2006-2011 OpenWrt.org
# Copyright (C) 2010 Vertical Communications

# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

determine_root_device() {
	root_device="$(mount | grep ' / ' | cut -f1 -d\  | grep -v rootfs )"
}

set_jffs_mp() {
	jffs="$(awk '/jffs2/ {print $2}' /proc/mounts)"
}

er_load_modules() {
	mkdir -p /tmp/extroot_modules/modules.d
	mkdir -p /tmp/extroot_modules/modules
	cp -L /etc/modules-boot.d/* /tmp/overlay/etc/modules-boot.d/* /tmp/extroot_modules/modules.d
	ln -sf /lib/modules/*/* /tmp/overlay/lib/modules/*/* /tmp/extroot_modules/modules
	local modules="$(cat /tmp/extroot_modules/modules.d/* 2>/dev/null)"
	cd /tmp/extroot_modules/modules && [ -n "$modules" ] && {
		cat $modules | sed -e 's/^\([^#].*\)/insmod \.\/\1.ko/'| sh 2>&- || :
	}
	rm -rf /tmp/extroot_modules
}

pivot_rom() { # <new_root> <old_root>
	mount -o move /proc $1/proc && \
	pivot_root $1 $1$2 && {
		mount -o move $2/dev /dev
		mount -o move $2/tmp /tmp
		mount -o move $2/sys /sys 2>&-
		return 0
	}
}

