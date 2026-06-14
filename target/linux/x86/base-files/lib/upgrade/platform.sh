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
	local diskdev partdev diff
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
		# extract boot sector from image
		get_image_dd "$1" of=/tmp/image.bs count=63 bs=512b

		if part_magic_efi /tmp/image.bs; then
			get_partitions /tmp/image.bs image
			local img_parts="$(wc -l < /tmp/partmap.image)"
			rm -f /tmp/image.bs /tmp/partmap.image

			if [ "$img_parts" -lt 4 ]; then
				v "Invalid image: expected 4 partitions for dual-boot, found $img_parts"
				return 1
			fi

			export_partdevice_label esppart "EFI system partition" && \
				export_partdevice_label bootpart openwrt_boot && \
				export_partdevice_label partdev openwrt_rootfs && \
				export_partdevice_label partdevalt openwrt_rootfs_alt || {
					v "Unable to determine upgrade device"
					return 1
				}
			return 0
		else
			v "Invalid image type"
			rm -f /tmp/image.bs
			return 1
		fi
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

	rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image

	if [ -n "$diff" ]; then
		v "Partition layout has changed. Full image will be written."
		ask_bool 0 "Abort" && exit 1
		return 0
	fi
}

platform_copy_config() {
	local partdev parttype=ext4

	# ONIE upgrade bundles sysupgrade.tgz into the installer itself.
	is_onie_install && return 0

	if (is_gpt_dualboot && export_partdevice_label partdev openwrt_boot) || export_partdevice partdev 1; then
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
		export_partdevice_label esppart "EFI system partition" || {
			v "Unable to find 'EFI system partition'"
			return 1
		}

		mkdir -p /tmp/.esp /tmp/.tmp
		mount -o ro,loop $diskdev /tmp/.tmp
		mount -o rw,noatime "/dev/$esppart" /tmp/.esp

		set -- /tmp/.tmp/efi/boot/boot???.efi
		new=$1; old="${new/\/tmp\/.tmp//tmp/.esp}"
		[ -f "$new" ] || v "Unable to find EFI bootloader in the update image"
		[ -f "$old" ] || v "Unable to find EFI bootloader on /dev/$esppart"
		[ -f "$new" ] && [ -f "$old" ] || {
			umount /tmp/.esp /tmp/.tmp
			return 1
		}

		if ! cmp -s "$old" "$new"; then
			v "Bootloader on /dev/$esppart needs to be updated"
			mkdir -p "$(dirname "$old")"
			ask_bool 0 "Skip" || {
				v "Upgrading bootloader on /dev/$esppart..."
				cp -a "$new" "$old"
			}
		else
			v "Bootloader on /dev/$esppart does not need to be updated"
		fi

		umount /tmp/.esp /tmp/.tmp
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

platform_do_upgrade() {
	local _alt bootpart defboot diff diskdev partdev partdevalt part size start

	if is_gpt_dualboot; then
		# extract the boot sector from the image
		get_image_dd "$1" of=/tmp/image.bs count=63 bs=512b

		export_partdevice_label esppart "EFI system partition" && \
			export_partdevice_label bootpart openwrt_boot && \
			export_partdevice_label partdev openwrt_rootfs && \
			export_partdevice_label partdevalt openwrt_rootfs_alt || {
				v "Unable to determine upgrade device"
				return 1
			}

		grep -q 'root=PARTLABEL=openwrt_rootfs_alt' /proc/cmdline && _alt= || _alt=_alt
		get_partitions /tmp/image.bs image
		while read part start size; do
			case "$part" in
			1)
				v "Extracting bootloader from image..."
				get_image_dd "$1" of=/tmp/.grub.img ibs="512" obs=1M skip="$start" count="$size"
			;;
			2)
				v "Writing new kernel to /boot/vmlinuz$_alt..."
				if [ -n "$_alt" ]; then defboot=1; else defboot=0; fi
				get_image_dd "$1" of=/tmp/.bootkernel.img ibs="512" obs=1M skip="$start" count="$size"
				mkdir -p /tmp/boot /tmp/.bootkernel && \
					mount -o rw,noatime "/dev/$bootpart" /tmp/boot && \
					mount -o ro,loop /tmp/.bootkernel.img /tmp/.bootkernel && \
					cp -af /tmp/.bootkernel/boot/vmlinuz /tmp/boot/boot/vmlinuz$_alt && \
					grub-editenv /tmp/boot/boot/grub/grubenv set attempt=0 target_slot=$defboot
				umount /tmp/boot /tmp/.bootkernel; rm -f /tmp/.bootkernel.img
				sync
			;;
			3)
				[ -n "$_alt" ] && partdev=$partdevalt
				v "Writing new rootfs to /dev/$partdev..."
				get_image_dd "$1" of="/dev/$partdev" ibs="512" obs=1M skip="$start" count="$size" conv=fsync
			;;
			esac
		done < /tmp/partmap.image

		platform_do_bootloader_upgrade /tmp/.grub.img
		rm -f /tmp/.grub.img /tmp/image.bs /tmp/partmap.image /tmp/sysupgrade.*
		sync

		return 0
	fi

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
