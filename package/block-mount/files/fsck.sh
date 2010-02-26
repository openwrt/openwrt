#!/bin/sh
# Copyright 2010 Vertical Communications
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

libmount_fsck() {
	local device="$1"
	local fsck_fstype="$2"
	local fsck_enabled="$3"
	local known_type
	local found_fsck=0
	

	[ -n "$fsck_type" ] && [ "$fsck_type" != "swap" ] && {
		grep -q "$device" /proc/swaps || grep -q "$device" /proc/mounts || {
			[ -e "$device" ] && [ "$fsck_enabled" -eq 1 ] && {
				for known_type in $libmount_known_fsck; do
					if [ "$known_type" = "$fsck_fstype" ]; then
						fsck_${known_type} "$device"
						found_fsck=1
						break
					fi
				done
				if [ "$found_fsck" -ne 1 ]; then
					logger -t 'fstab' "Unable to check/repair $device; no known fsck for filesystem type $fstype"
				fi
			}
		}
	}
}

libmount_known_fsck=""

include /lib/functions/fsck

