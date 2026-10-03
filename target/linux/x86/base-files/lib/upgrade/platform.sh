RAMFS_COPY_BIN='grub-bios-setup grub-editenv'

find_partname_dev() {
	local partname="$1"
	local uevent dev
	for uevent in /sys/class/block/*/uevent; do
		grep -q "^PARTNAME=$partname\$" "$uevent" 2>/dev/null || continue
		dev=$(sed -n 's/^DEVNAME=//p' "$uevent")
		[ -n "$dev" ] && { echo "/dev/$dev"; return 0; }
	done
	return 1
}

# True when the boot disk hosts ONIE: both ONIE-BOOT and OPENWRT-ROOT
# GPT-labelled partitions present.
is_onie_install() {
	grep -q PARTNAME=ONIE-BOOT /sys/class/block/*/uevent 2>/dev/null || return 1
	grep -q PARTNAME=OPENWRT-ROOT /sys/class/block/*/uevent 2>/dev/null || return 1
	return 0
}

# Reinstall via ONIE: append the preserved-config tarball to the installer
# image (installer detects it via size check), drop the combined file on
# OPENWRT-ROOT as onie-installer-x86_64, and flip both grubenv files so the
# next boot chainloads ONIE in install mode. ONIE re-runs our installer,
# which recreates OPENWRT-ROOT, extracts the new rootfs, and stashes
# /sysupgrade.tgz for OpenWrt's preinit to restore.
platform_do_upgrade_onie() {
	local image="$1"
	local backup="$UPGRADE_BACKUP"

	local root_dev onie_dev
	root_dev=$(find_partname_dev OPENWRT-ROOT) || {
		v "ONIE upgrade: OPENWRT-ROOT not found"; return 1;
	}
	onie_dev=$(find_partname_dev ONIE-BOOT) || {
		v "ONIE upgrade: ONIE-BOOT not found"; return 1;
	}

	local root_mnt=/tmp/upgrade-root
	local onie_mnt=/tmp/upgrade-onie
	mkdir -p "$root_mnt" "$onie_mnt"

	mount -t ext4 -o rw,noatime "$root_dev" "$root_mnt" || {
		v "ONIE upgrade: mount $root_dev failed"
		return 1
	}

	local bundle="$root_mnt/onie-installer-x86_64"
	v "ONIE upgrade: writing bundle to $bundle"
	if [ -s "$backup" ]; then
		cat "$image" "$backup" > "$bundle"
	else
		cp "$image" "$bundle"
	fi
	chmod +x "$bundle"

	# One-shot: OpenWrt's grub.cfg reads next_entry and chainloads ONIE.
	grub-editenv "$root_mnt/boot/grub/grubenv" set next_entry=ONIE || {
		v "ONIE upgrade: failed to set next_entry in OPENWRT-ROOT grubenv"
		sync; umount "$root_mnt"
		return 1
	}
	sync
	umount "$root_mnt"

	mount -o rw,noatime "$onie_dev" "$onie_mnt" || {
		v "ONIE upgrade: mount $onie_dev failed"
		return 1
	}
	grub-editenv "$onie_mnt/grub/grubenv" set onie_mode=install || {
		v "ONIE upgrade: failed to set onie_mode in ONIE-BOOT grubenv"
		sync; umount "$onie_mnt"
		return 1
	}
	sync
	umount "$onie_mnt"

	v "ONIE upgrade: ready; stage2 will reboot"
	return 0
}

platform_check_image() {
	local diskdev partdev diff extra_parts
	[ "$#" -gt 1 ] && return 1

	if is_onie_install; then
		[ "$(get_magic_word "$1")" = "2321" ] || {
			v "Invalid image: expected ONIE installer (shell script)"
			return 1
		}
		head -c 4096 "$1" | grep -q '^PAYLOAD_OFFSET=' || {
			v "Invalid image: no PAYLOAD_OFFSET header"
			return 1
		}
		return 0
	fi

	case "$(get_magic_word "$1")" in
		eb48|eb63) ;;
		*)
			v "Invalid image type"
			return 1
		;;
	esac

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	get_partitions "/dev/$diskdev" bootdisk

	v "Extract boot sector from the image"
	get_image_dd "$1" of=/tmp/image.bs count=63 bs=512b

	get_partitions /tmp/image.bs image

	#compare tables
	diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"

	# Detect partitions beyond the two defaults
	extra_parts="$(awk '$1 > 2 { printf "%s ", $1 }' /tmp/partmap.bootdisk)"

	rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image

	if [ -n "$diff" ]; then
		# Applying a changed layout requires rewriting the whole disk. If
		# non-default partitions exist, that destroys them, so refuse
		# unless the user opted in with -p (SAVE_PARTITIONS=0). The refusal
		# marks the image invalid-but-forceable, so sysupgrade aborts
		# unless -p or -F/--force is given.
		#
		# SAVE_PARTITIONS is exported by sysupgrade so it reaches this
		# (validate_firmware_image child) process. If it is unset, this is
		# not a sysupgrade run (e.g. a GUI image check), so default to the
		# safe choice and refuse: "$SAVE_PARTITIONS" != "0" is then true.
		if [ -n "$extra_parts" ] && [ "$SAVE_PARTITIONS" != "0" ]; then
			# Report the reason to GUI/RPC callers, which only see the
			# JSON test results and not the messages printed below.
			notify_firmware_test_result "partition_layout" 0
			v "Partition layout changed; refusing to erase non-default partition(s): ${extra_parts}"
			echo "The new image uses a different partition layout than the one"       >&2
			echo "currently installed. Applying it requires rewriting the partition"  >&2
			echo "table, which will DESTROY these non-default partition(s) and all"   >&2
			echo "data on them: ${extra_parts}"                                        >&2
			echo "Re-run sysupgrade with -p to proceed and erase them."                >&2
			return 1
		fi
		# Either only the default partitions are present or the user opted in with -p.
		notify_firmware_test_result "partition_layout" 1
		v "Partition layout has changed, full image will be written."
	fi

	return 0
}

platform_copy_config() {
	local partdev parttype=ext4

	# ONIE upgrade bundles sysupgrade.tgz into the installer itself.
	is_onie_install && return 0

	if export_partdevice partdev 1; then
		part_magic_fat "/dev/$partdev" && parttype=vfat
		mount -t $parttype -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	fi
}

platform_do_bootloader_upgrade() {
	local bootpart parttable=msdos
	local diskdev="$1"

	if export_partdevice bootpart 1; then
		mkdir -p /tmp/boot
		mount -o rw,noatime "/dev/$bootpart" /tmp/boot
		echo "(hd0) /dev/$diskdev" > /tmp/device.map
		part_magic_efi "/dev/$diskdev" && parttable=gpt

		v "Upgrading bootloader on /dev/$diskdev..."
		grub-bios-setup \
			-m "/tmp/device.map" \
			-d "/tmp/boot/boot/grub" \
			-r "hd0,${parttable}1" \
			"/dev/$diskdev"

		umount /tmp/boot
	fi
}

platform_do_upgrade() {
	local diskdev partdev diff

	if is_onie_install; then
		platform_do_upgrade_onie "$1"
		return $?
	fi

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	sync

	if [ "$UPGRADE_OPT_SAVE_PARTITIONS" = "1" ]; then
		get_partitions "/dev/$diskdev" bootdisk

		v "Extract boot sector from the image"
		get_image_dd "$1" of=/tmp/image.bs count=63 bs=512b

		get_partitions /tmp/image.bs image

		#compare tables
		diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"
	else
		diff=1
	fi

	if [ -n "$diff" ]; then
		get_image_dd "$1" of="/dev/$diskdev" bs=4096 conv=fsync

		# Separate removal and addtion is necessary; otherwise, partition 1
		# will be missing if it overlaps with the old partition 2
		partx -d - "/dev/$diskdev"
		partx -a - "/dev/$diskdev"

		return 0
	fi

	#iterate over each partition from the image and write it to the boot disk
	while read part start size; do
		if export_partdevice partdev $part; then
			v "Writing image to /dev/$partdev..."
			get_image_dd "$1" of="/dev/$partdev" ibs=512 obs=1M skip="$start" count="$size" conv=fsync
		else
			v "Unable to find partition $part device, skipped."
		fi
	done < /tmp/partmap.image

	v "Writing new UUID to /dev/$diskdev..."
	get_image_dd "$1" of="/dev/$diskdev" bs=1 skip=440 count=4 seek=440 conv=fsync

	platform_do_bootloader_upgrade "$diskdev"
	local parttype=ext4
	part_magic_efi "/dev/$diskdev" || return 0

	if export_partdevice partdev 1; then
		part_magic_fat "/dev/$partdev" && parttype=vfat
		mount -t $parttype -o rw,noatime "/dev/$partdev" /mnt
		set -- $(dd if="/dev/$diskdev" bs=1 skip=1168 count=16 2>/dev/null | hexdump -v -e '8/1 "%02x "" "2/1 "%02x""-"6/1 "%02x"')
		sed -i "s/\(PARTUUID=\)[a-f0-9-]\+/\1$4$3$2$1-$6$5-$8$7-$9/ig" /mnt/boot/grub/grub.cfg
		umount /mnt
	fi
}
