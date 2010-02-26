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
	[ -d $ER_ROOT/etc/modules.d ] && {
	    cd $ER_ROOT/etc/modules.d && {
	    	local modules="$(grep -l '# May be required for rootfs' *)"
	    	cat $modules | sed 's/^\([^#]\)/insmod \1/' | sh 2>&- || : 
	    }
	}
}


