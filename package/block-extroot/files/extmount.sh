#!/bin/sh
# Copyright 2010 Vertical Communications

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
	ln -sf /etc/modules.d/* /tmp/overlay/etc/modules.d/* /tmp/extroot_modules/modules.d
	ln -sf /lib/modules/*/* /tmp/overlay/lib/modules/*/* /tmp/extroot_modules/modules
    	local modules="$(grep -l '# May be required for rootfs' /tmp/extroot_modules/modules.d/* 2>/dev/null)"
	cd /tmp/extroot_modules/modules && [ -n "$modules" ] && {
		cat $modules | sed -e 's/^\([^#].*\)/insmod \.\/\1.ko/'| sh 2>&- || :
	}
	rm -rf /tmp/extroot_modules
}

