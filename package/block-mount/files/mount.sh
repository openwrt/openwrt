#!/bin/sh
# Copyright 2010 Vertical Communications
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


. /lib/functions/block.sh
. /lib/functions/fsck.sh

config_mount_by_section() {
	local cfg="$1"
	local find_rootfs="$2"

	mount_cb() {
		local cfg="$1"
		local device="$2"
		shift
		local target="$2"
		local cfgdevice="$3"
		local fstype="$4"
		local options="$5"
		local enabled="$6"
		local enabled_fsck="$7"
		local uuid="$8"
		local label="$9"
		shift
		local is_rootfs="$9"
		shift
		local found_device=""
		local fsck_type=""
		
		found_device="$(libmount_find_device_by_id "$uuid" "$label" "$device" "$cfgdevice")"
		if [ -n "$found_device" ]; then
			if [ -z "$find_rootfs" ] || [ "$find_rootfs" -eq 0 ] || [ "$is_rootfs" -eq 1 ]; then
				[ "$enabled_fsck" -eq 1 ] && {
					grep -q "$found_device" /proc/swaps || grep -q "$found_device" /proc/mounts || {
						libmount_fsck "$found_device" "$fsck_type" "$enabled_fsck"
					}
				}								
	
				[ "$is_rootfs" -eq 1 ] && [ "$find_rootfs" -eq 1 ] && {
					target=/overlay
				}
				config_create_mount_fstab_entry "$found_device" "$target" "$fstype" "$options" "$enabled" 
				grep -q "$found_device" /proc/swaps || grep -q "$found_device" /proc/mounts || {
					[ "$enabled" -eq 1 ] && mkdir -p "$target" && mount "$target" 2>&1 | tee /proc/self/fd/2 | logger -t 'fstab'
				}
				
			fi
		fi
		[ "$is_rootfs" -eq 1 ] && [ "$find_rootfs" -eq 1 ] && {
			rootfs_found=1
		}
		return 0	
	}
	config_get_mount "$cfg"
	reset_block_cb
}

config_swapon_by_section() {
	local cfg="$1"

	swap_cb() {
		local cfg="$1"
		local device="$2"
		local cfgdevice="$3"
		local enabled="$4"
		local uuid="$5"
		local label="$6"
		local uuid
		local label
		
		local found_device=""
		local fsck_type=""
		
		found_device="$(libmount_find_device_by_id "$uuid" "$label" "$device" "$cfgdevice")"

		if [ -n "$found_device" ]; then
			config_create_swap_fstab_entry "$found_device" "$enabled"
			grep -q "$found_device" /proc/swaps || grep -q "$found_device" /proc/mounts || {
				[ "$enabled" -eq 1 ] && swapon "$found_device" | tee /proc/self/fd/2 | logger -t	 'fstab'
			}
		fi
		return 0	
	}
	config_get_swap "$cfg"
	reset_block_cb
}
