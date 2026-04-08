# SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause

FS_STATE_READY=2

# $(1): file to read from
# $(2): offset in bytes
get_hex_u32_le() {
	dd if="$1" skip=$2 bs=1 count=4 2>/dev/null | hexdump -v -e '1/4 "%02x"'
}

# Setup /tmp/env.config to provide "metadata" UBI volume access
#
# It can be used with "fw_printenv -c /tmp/env.config"
bcm4908_pkgtb_setup_env_config() {
	local size=$((0x$(get_hex_u32_le /dev/ubi0_1 4)))

	dd if=/dev/ubi0_1 of=/tmp/env.head count=8 iflag=count_bytes
	dd if=/dev/ubi0_1 of=/tmp/env.body skip=8 iflag=skip_bytes
	printf "%s\t0x%x\t0x%x\t0x%x" "/tmp/env.body" 0x0 $size $size > /tmp/env.config
}

bcm4908_committed_image_seq() {
	bcm4908_pkgtb_setup_env_config

	commited="$(fw_printenv -n -c /tmp/env.config COMMITTED)"
	[ -n "$commited" ] && {
		seq=$(fw_printenv -n -c /tmp/env.config SEQ | cut -d ',' -f $commited)
		[ -n "$seq" ] && {
			echo $seq
			return
		}
	}

	echo "Failed to read COMMITED and SEQ from metadata1" >&2
}

# Make sure "rootfs_data" UBI volume matches currently flashed image
#
# On mismatch "rootfs_data" will be wiped and assigned
#
# $1: UBI volume of "rootfs_data" (e.g. ubi0_123)
bcm4908_verify_rootfs_data() {
	local ubivol="$1"
	local dir=/tmp/rootfs_data
	local seq="$(bcm4908_committed_image_seq)"

	[ -z "$seq" ] && return

	mkdir $dir
	if ! mount -t ubifs /dev/$ubivol $dir; then
		echo "Failed to mount $ubivol UBI volume" >&2
		rmdir $dir
		return
	fi

	# Wipe rootfs_data if it doesn't belong to us
	[ "$(readlink $dir/.openwrt-image-seq)" != "$seq" ] && {
		echo "Removing \"rootfs_data\" content"
		rm -rf $dir/..?* $dir/.[!.]* $dir/*
	}

	# If rootfs_data is clean (or was just wiped) claim it
	[ -z "$(ls -A $dir)" ] && {
		echo "Assigning \"rootfs_data\" to the current firmware"
		# Claim this "rootfs_data"
		ln -s $seq $dir/.openwrt-image-seq
		# Mark it ready to avoid "mount_root" wiping it again
		ln -s $FS_STATE_READY $dir/.fs_state
	}

	umount $dir
	rmdir $dir
}
