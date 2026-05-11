RAMFS_COPY_BIN='grub-bios-setup'

platform_check_image() {
	local diskdev partdev diff
	[ "$#" -gt 1 ] && return 1

	if is_efi_system; then
		#extract the boot sector from the image
		get_image "$@" | dd of=/tmp/image.bs count=63 bs=512b 2>/dev/null

		if part_magic_efi /tmp/image.bs; then
			rm -f /tmp/image.bs
			export_partdevice_label bootpart openwrt_boot && \
				export_partdevice_label partdev openwrt_rootfs && \
				export_partdevice_label partdevalt openwrt_rootfs_alt || {
					echo "Unable to determine upgrade device"
					return 1
				}
			return 0
		else
			echo "Invalid image type"
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

	if (is_efi_system && export_partdevice_label partdev openwrt_boot) \
		|| export_partdevice partdev 1 ; then
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

	if is_efi_system; then
		export_partdevice_label efipart "EFI system partition" || {
			echo "Cannot find 'EFI system partition'" >&2
			ask_bool 0 "Abort" && exit 1
			return
		}

		mkdir -p /tmp/.efi /tmp/.tmp
		mount -o ro $diskdev /tmp/.tmp
		mount -o rw,noatime "/dev/$efipart" /tmp/.efi
		new=$( ls /tmp/.tmp/EFI/boot/boot???.efi )
		old="${new/\/tmp\/.tmp//tmp/.efi}"

		if diff -q "$old" "$new" | grep -q ' differ$'; then
			echo "Bootloader on /dev/$efipart needs to be updated."
			ask_bool 0 "Skip" || cp -a "$new" "$old"
		else
			echo "Bootloader on /dev/$efipart does not need to be updated."
		fi

		umount /tmp/.efi /tmp/.tmp
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
	local _alt altboot bootpart defboot diff diskdev partdev partdevalt part size start

	#extract the boot sector from the image
	get_image "$@" | dd of=/tmp/image.bs count=63 bs=512b 2>/dev/null

	if is_efi_system; then
		export_partdevice_label bootpart openwrt_boot \
			&& export_partdevice_label partdev openwrt_rootfs \
			&& export_partdevice_label partdevalt openwrt_rootfs_alt || {
				echo "Unable to determine upgrade device" >&2
				return 1
			}

		grep -q 'root=PARTLABEL=openwrt_rootfs_alt' /proc/cmdline && _alt= || _alt=_alt
		get_partitions /tmp/image.bs image
		while read part start size; do
			case "$part" in
			1)
				echo "Extracting bootloader from image..."
				get_image "$@" | dd of=/tmp/.grub.img ibs="512" obs=1M skip="$start" count="$size" conv=fsync
			;;
			2)
				echo "Writing new kernel to /boot/vmlinuz$_alt..."
				if [ -n "$_alt" ]; then defboot=1; altboot=0; else defboot=0; altboot=1; fi
				get_image "$@" | dd of=/tmp/.bootkernel.img ibs="512" obs=1M skip="$start" count="$size" conv=fsync
				mkdir -p /tmp/boot /tmp/.bootkernel \
					&& mount -o rw,noatime "/dev/$bootpart" /tmp/boot \
					&& mount -o ro /tmp/.bootkernel.img /tmp/.bootkernel \
					&& cp -af /tmp/.bootkernel/boot/vmlinuz /tmp/boot/boot/vmlinuz$_alt \
					&& sed -e "s:^set default.\+$:set default=\"$defboot\":" \
						-e "s:^set fallback.\+$:set fallback=\"$altboot\":" -i /tmp/boot/boot/grub/grub.cfg
				umount /tmp/boot; umount /tmp/.bootkernel; rm -f /tmp/.bootkernel.img
				sync
			;;
			3)
				[ -n "$_alt" ] && partdev=$partdevalt
				echo "Writing new rootfs to /dev/$partdev..."
				get_image "$@" | dd of="/dev/$partdev" ibs="512" obs=1M skip="$start" count="$size" conv=fsync
			;;
			esac
		done < /tmp/partmap.image

		platform_do_bootloader_upgrade /tmp/.grub.img
		rm -f /tmp/.grub.img /tmp/image.bs /tmp/partmap.image /tmp/sysupgrade.*
		sync

		return 0
	fi

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	sync

	if [ "$UPGRADE_OPT_SAVE_PARTITIONS" = "1" ]; then
		get_partitions "/dev/$diskdev" bootdisk

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
}
