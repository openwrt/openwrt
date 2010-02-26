#!/bin/sh
# Copyright 2010 Vertical Communications
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

. /etc/functions.sh

reset_block_cb() {
	mount_cb() { 
		dmc_mount_cfg="$1"
		shift # skip optional param
		dmc_target="$2"
		dmc_mount_device="$3"
		dmc_fstype="$4"
		dmc_options="$5"
		dmc_enabled="$6"
		dmc_enabled_fsck="$7"
		dmc_uuid="$8"
		dmc_label="$9"
		shift
		dmc_is_rootfs="$9"
		return 0
	}
	swap_cb() { 
		dsc_swap_cfg="$1"
		shift # skip optional param
		dsc_swap_device="$2"
		dsc_enabled="$3"
		dsc_uuid="$4"
		dsc_label="$5"
		return 0
	}
}
reset_block_cb

reset_dev_section_cb() {
	mount_dev_section_cb() {
		dmds_mount_cfg="$1"
		dmds_mount_target="$2"
		dmds_mount_device="$3"
		dmds_mount_fstype="$4"
		dmds_mount_options="$5"
		dmds_mount_enabled="$6"
		dmds_mount_enabled_fsck="$7"
		dmds_mount_uuid="$8"
		dmds_mount_label="$9"
		shift
		dmds_is_rootfs="$9"
		return 0
	}
	swap_dev_section_cb() { 
		dsds_swap_cfg="$1"
		dsds_swap_device="$2"
		dsds_swap_enabled="$3"
		dsds_swap_uuid="$4"
		dsds_swap_label="$5"
		return 0
	}
}
reset_dev_section_cb

config_get_mount() {
	local gm_cfg="$1"
	local gm_param="$2"
	local gm_target
	local gm_device
	local gm_fstype
	local gm_options
	local gm_enabled
	local gm_enabled_fsck
	local gm_uuid
	local gm_label
	local gm_is_rootfs
	config_get gm_target "$1" target
	config_get gm_device "$1" device
	config_get gm_fstype "$1" fstype 'auto'
	config_get gm_options "$1" options 'rw'
	config_get_bool gm_enabled "$1" enabled 1
	config_get_bool gm_enabled_fsck "$1" enabled_fsck 0
	config_get gm_uuid "$1" uuid
	config_get gm_label "$1" label
	config_get_bool gm_is_rootfs "$1" is_rootfs 0
	mount_cb "$gm_cfg" "$gm_param" "$gm_target" "$gm_device" "$gm_fstype" "$gm_options" "$gm_enabled" "$gm_enabled_fsck" "$gm_uuid" "$gm_label" "$gm_is_rootfs"
}

config_get_swap() {
	local gs_cfg="$1"
	local gs_param="$2"
	local gs_device
	local gs_enabled
	local gs_uuid
	local gs_label
	config_get gs_device "$1" device
	config_get_bool gs_enabled "$1" enabled 1
	config_get gs_uuid "$1" uuid
	config_get gs_label "$1" label

	swap_cb "$gs_cfg" "$gs_param" "$gs_device" "$gs_enabled" "$gs_uuid" "$gs_label"
}

config_get_automount() {
	config_load fstab
	config_get_bool from_fstab "automount" from_fstab 1
	config_get_bool anon_mount "automount" anon_mount 1
	config_get_bool anon_fsck "automount" anon_fsck 0
}

config_get_autoswap() {
	config_load fstab
	config_get_bool from_fstab "autoswap" from_fstab 1
	config_get_bool anon_swap "autoswap" anon_swap 0
}

config_create_swap_fstab_entry() {
	local device="$1"
	local enabled="$2"

	[ -n "$device" ] || return 0

	local fstabnew="$(mktemp -t '.fstab.XXXXXXXX')"
	
	mkdir -p /var/lock
	lock /var/lock/fstab.lck
	cat /tmp/fstab | grep -E -v "^$device[[:blank:]]" >>"$fstabnew"
	[ "$enabled" -eq 1 ] && echo "$device	none	swap	sw	0	0" >> "$fstabnew"
	cat "$fstabnew" >/tmp/fstab
	lock -u /var/lock/fstab.lck
	rm -f $fstabnew
}

config_create_mount_fstab_entry() {
	local device="$1"
	local target="$2"
	local fstype="$3"
	local options="$4"
	local enabled="$5"
	options="${options:-rw}"
	[ "$enabled" -eq 0 ] && options="noauto,$options"
	[ -n "$target" ] || return 0
	[ -n "$device" ] || return 0

	local fstabnew="$(mktemp -t '.fstab.XXXXXXXX')"
	
	mkdir -p /var/lock
	lock /var/lock/fstab.lck
	cat /tmp/fstab | grep -E -v "^$device[[:blank:]]" | grep -v "$target" >>"$fstabnew"
	echo "$device	$target	$fstype	$options	0	0" >>"$fstabnew"
	cat "$fstabnew" >/tmp/fstab		
	lock -u /var/lock/fstab.lck
	rm -f $fstabnew
}

libmount_find_token() {
	local token="$1"
	local value="$2"
	local device
	device="$(blkid | grep "$token=\"$value\"" | cut -f1 -d:)"
	echo "$device"
}

libmount_find_device_by_id() {
	local uuid="$1"
	local label="$2"
	local device="$3"
	local cfg_device="$4"
	local found_device
	
	if [ -n "$uuid" ]; then
		found_device="$(libmount_find_token "UUID" "$uuid")"
	elif [ -n "$label" ]; then
		found_device="$(libmount_find_token "LABEL" "$label")"
	elif [ "$device" = "$cfg_device" ]; then
		found_device="$device"
	elif [ -z "$device" ] && [ -e "$cfg_device" ]; then
		found_device="$cfg_device"
	fi
	[ -n "$device" ] && [ "$device" != "$found_device" ] && {
		found_device=""
	}
	echo "$found_device"
}

config_get_mount_section_by_device() {
	local msbd_device="$1"
	local msbd_mount_cfg=
	local msbd_target=
	local msbd_mount_device=
	local msbd_fstype=
	local msbd_options=
	local msbd_enabled=
	local msbd_enabled_fsck=
	local msbd_uuid=
	local msbd_label=
	local msbd_is_rootfs
	local msbd_blkid_fstype_match=
	mount_cb() {
		local mc_cfg="$1"
		local mc_device="$2"
		shift
		local mc_target="$2"
		local mc_cfgdevice="$3"
		local mc_fstype="$4"
		local mc_uuid="$8"
		local mc_label="$9"
		shift
		local mc_is_rootfs="$9"
		local mc_found_device=""
		
		mc_found_device="$(libmount_find_device_by_id "$mc_uuid" "$mc_label" "$mc_device" "$mc_cfgdevice")"
		if [ -n "$mc_found_device" ]; then
			msbd_mount_cfg="$mc_cfg"
			msbd_target="$mc_target"
			msbd_mount_device="$mc_found_device"
			msbd_fstype="$mc_fstype"
			msbd_options="$4"
			msbd_enabled="$5"
			msbd_enabled_fsck="$6"
			msbd_uuid="$7"
			msbd_label="$8"
			msbd_is_rootfs="$9"
		fi
		return 0	
	}
	config_foreach config_get_mount mount "$msbd_device"
	[ -n "$msbd_mount_device" ] && config_create_mount_fstab_entry "$msbd_mount_device" "$msbd_target" "$msbd_fstype" "$msbd_options" "$msbd_enabled" 
	mount_dev_section_cb "$msbd_mount_cfg" "$msbd_target" "$msbd_mount_device" "$msbd_fstype" "$msbd_options" "$msbd_enabled" "$msbd_enabled_fsck" "$msbd_uuid" "$msbd_label" "$msbd_is_rootfs"
	reset_block_cb
}

config_get_swap_section_by_device() {
	local ssbd_device="$1"
	local ssbd_swap_cfg=
	local ssbd_swap_device=
	local ssbd_enabled=
	local ssbd_uuid=
	local ssbd_label=
	swap_cb() {
		local sc_cfg="$1"
		local sc_device="$2"
		local sc_uuid="$5"
		local sc_label="$6"
		local sc_cfgdevice="$3"
		local sc_found_device

		sc_found_device="$(libmount_find_device_by_id "$sc_uuid" "$sc_label" "$sc_device" "$sc_cfgdevice")"
		if [ -n "$sc_found_device" ]; then
			ssbd_swap_cfg="$sc_cfg"
			ssbd_swap_device="$sc_found_device"
			ssbd_enabled="$4"
			ssbd_uuid="$5"
			ssbd_label="$6"
		fi
		return 0	
	}
	config_foreach config_get_swap swap "$ssbd_device"
	[ -n "$ssbd_swap_device" ] && config_create_swap_fstab_entry "$ssbd_swap_device" "$ssbd_enabled"
	swap_dev_section_cb "$ssbd_swap_cfg" "$ssbd_swap_device" "$ssbd_enabled" "$ssbd_uuid" "$ssbd_label"
	reset_block_cb
}


