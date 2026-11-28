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

	if is_gpt_dualboot; then
		local esppart bootpart partdev partdevalt slot slotdev suffix ret

		platform_dualboot_devices || return 1
		platform_dualboot_partitions "$1" || return 1
		platform_dualboot_check_sizes
		ret=$?
		rm -f /tmp/partmap.image
		return $ret
	fi

	case "$(get_magic_word "$1")" in
		eb48|eb63) ;;
		*)
			v "Invalid image type"
			return 1
		;;
	esac

	legacy_sdcard_check_image "$1"
}

# Sets esppart, bootpart, partdev and partdevalt, and for the slot that is
# not running: slot (its grubenv target_slot), slotdev and suffix (of its
# vmlinuz).
platform_dualboot_devices() {
	export_partdevice_esp esppart && \
		export_partdevice_label bootpart openwrt_boot && \
		export_partdevice_label partdev openwrt_rootfs && \
		export_partdevice_label partdevalt openwrt_rootfs_alt || {
			v "Unable to determine upgrade device"
			return 1
		}

	if grep -q 'root=PARTLABEL=openwrt_rootfs_alt' /proc/cmdline; then
		slot=0 slotdev="$partdev" suffix=
	else
		slot=1 slotdev="$partdevalt" suffix=_alt
	fi
}

platform_dualboot_partitions() { # <image>
	get_image_dd "$1" of=/tmp/image.bs count=1 bs=512b

	part_magic_efi /tmp/image.bs || {
		v "Invalid image type"
		rm -f /tmp/image.bs
		return 1
	}

	get_partitions /tmp/image.bs image
	rm -f /tmp/image.bs

	[ "$(awk '$1 >= 1 && $1 <= 3' /tmp/partmap.image | wc -l)" -eq 3 ] || {
		v "Invalid image: expected an ESP, openwrt_boot and openwrt_rootfs"
		rm -f /tmp/partmap.image
		return 1
	}
}

# Checked before writing: dd would only fail at the end of the slot, after
# overwriting all of it
platform_dualboot_check_sizes() {
	local part start size have

	while read part start size; do
		[ "$part" = 3 ] || continue
		have="$(cat "/sys/class/block/$slotdev/size")"
		[ "$size" -le "$have" ] && return 0
		v "Image rootfs ($size sectors) does not fit /dev/$slotdev ($have sectors)"
		return 1
	done < /tmp/partmap.image

	return 1
}

# get_image_dd returns the status of its message filter, not that of dd
platform_dualboot_dd() { # <image> <dd arguments>
	local image="$1"; shift

	get_image "$image" 2>/dev/null | dd "$@" 2>/dev/null
}

# The slot becomes the boot target only after everything else was written
platform_dualboot_write() { # <image>
	local part start size

	while read part start size; do
		case "$part" in
		1)
			v "Extracting bootloader from image..."
			platform_dualboot_dd "$1" of=/tmp/.grub.img ibs=512 obs=1M skip="$start" count="$size" || return 1
			;;
		2)
			platform_dualboot_dd "$1" of=/tmp/.bootkernel.img ibs=512 obs=1M skip="$start" count="$size" || return 1
			;;
		3)
			v "Writing new rootfs to /dev/$slotdev..."
			platform_dualboot_dd "$1" of="/dev/$slotdev" ibs=512 obs=1M skip="$start" count="$size" conv=fsync || return 1
			;;
		esac
	done < /tmp/partmap.image

	v "Writing new kernel to /boot/vmlinuz$suffix..."
	mkdir -p /tmp/boot /tmp/.bootkernel
	mount -o ro,loop /tmp/.bootkernel.img /tmp/.bootkernel && \
		mount -o rw,noatime "/dev/$bootpart" /tmp/boot && \
		cp -af /tmp/.bootkernel/boot/vmlinuz "/tmp/boot/boot/vmlinuz$suffix" || {
			v "Unable to write the new kernel"
			return 1
		}

	platform_do_bootloader_upgrade /tmp/.grub.img || return 1

	grub-editenv /tmp/boot/boot/grub/grubenv set attempt=0 target_slot=$slot
}

platform_do_upgrade_dualboot() { # <image>
	local esppart bootpart partdev partdevalt slot slotdev suffix ret

	platform_dualboot_devices || return 1
	platform_dualboot_partitions "$1" || return 1
	platform_dualboot_check_sizes && platform_dualboot_write "$1"
	ret=$?

	umount /tmp/boot /tmp/.bootkernel 2>/dev/null
	rm -f /tmp/.grub.img /tmp/.bootkernel.img /tmp/partmap.image
	sync

	return $ret
}

platform_copy_config() {
	local partdev parttype=ext4

	# ONIE upgrade bundles sysupgrade.tgz into the installer itself.
	is_onie_install && return 0

	if { is_gpt_dualboot && export_partdevice_label partdev openwrt_boot; } || export_partdevice partdev 1; then
		mkdir -p /tmp/boot
		part_magic_fat "/dev/$partdev" && parttype=vfat
		mount -t $parttype -o rw,noatime "/dev/$partdev" /tmp/boot
		cp -af "$UPGRADE_BACKUP" "/tmp/boot/$BACKUP_FILE"
		umount /tmp/boot
	fi
}

platform_do_bootloader_upgrade() {
	local bootpart parttable=msdos
	local diskdev="$1"

	if is_gpt_dualboot; then
		local esppart new old ret=0

		export_partdevice_esp esppart || {
			v "Unable to find the EFI system partition"
			return 1
		}

		mkdir -p /tmp/.esp /tmp/.tmp
		mount -o ro,loop "$diskdev" /tmp/.tmp || return 1
		mount -o rw,noatime "/dev/$esppart" /tmp/.esp || {
			umount /tmp/.tmp
			return 1
		}

		set -- /tmp/.tmp/efi/boot/boot???.efi
		new=$1; old="${new/\/tmp\/.tmp//tmp/.esp}"
		[ -f "$new" ] || v "Unable to find EFI bootloader in the update image"
		[ -f "$old" ] || v "Unable to find EFI bootloader on /dev/$esppart"

		if [ ! -f "$new" ] || [ ! -f "$old" ]; then
			ret=1
		elif ! cmp -s "$old" "$new"; then
			v "Bootloader on /dev/$esppart needs to be updated"
			ask_bool 0 "Skip" || {
				v "Upgrading bootloader on /dev/$esppart..."
				cp -a "$new" "$old" || ret=1
			}
		else
			v "Bootloader on /dev/$esppart does not need to be updated"
		fi

		umount /tmp/.esp /tmp/.tmp
		return $ret
	elif export_partdevice bootpart 1; then
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

platform_legacy_sdcard_post_upgrade() {
	local diskdev="$1"
	local partdev parttype=ext4

	platform_do_bootloader_upgrade "$diskdev"
	part_magic_efi "/dev/$diskdev" || return 0

	if export_partdevice partdev 1; then
		part_magic_fat "/dev/$partdev" && parttype=vfat
		mount -t $parttype -o rw,noatime "/dev/$partdev" /mnt
		set -- $(dd if="/dev/$diskdev" bs=1 skip=1168 count=16 2>/dev/null | hexdump -v -e '8/1 "%02x "" "2/1 "%02x""-"6/1 "%02x"')
		sed -i "s/\(PARTUUID=\)[a-f0-9-]\+/\1$4$3$2$1-$6$5-$8$7-$9/ig" /mnt/boot/grub/grub.cfg
		umount /mnt
	fi
}

platform_do_upgrade() {
	if is_gpt_dualboot; then
		platform_do_upgrade_dualboot "$1"
		return $?
	fi

	if is_onie_install; then
		platform_do_upgrade_onie "$1"
		return $?
	fi

	legacy_sdcard_do_upgrade "$1"
}
