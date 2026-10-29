#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Upgrade for the UniFi Cloud Gateway Fiber.
#
# OpenWrt runs next to the stock firmware from two slots of raw eMMC
# partitions, A (owrt_kernel_a, owrt_rootfs_a) and B (owrt_kernel_b,
# owrt_rootfs_b). Each rootfs partition holds the squashfs and, behind it,
# that slot's own overlay (fstools rootdisk). U-Boot boots what the env
# variable ucg_boot names (owrt_a, owrt_b or stock) and falls back to the
# other slot, then to stock. A sysupgrade writes the slot that is not
# running with the generic eMMC upgrade, makes it durable, checks it, and
# switches ucg_boot to it as the last step, as a trial: U-Boot counts the
# boots of the new slot (upgrade_available, bootcount) and goes back to the
# other one after bootlimit boots that did not reach the end of the boot
# (ucgfiber_boot_done, from /etc/init.d/bootcount). An interrupted upgrade
# leaves the running slot active. Stock's partitions are never written.

UCGFIBER_OVERLAY_MIN=$((16 * 1024 * 1024))

ucgfiber_gz() { # <image>
	[ "$(dd if="$1" bs=2 count=1 2>/dev/null | hexdump -v -e '2/1 "%02x"')" = 1f8b ] && echo z
}

ucgfiber_member() { # <image> <member>   writes the member to stdout
	tar x$(ucgfiber_gz "$1")f "$1" "$2" -O 2>/dev/null
}

ucgfiber_board_dir() { # <image>
	local dir

	dir="$(tar t$(ucgfiber_gz "$1")f "$1" 2>/dev/null |
		grep -m 1 '^sysupgrade-.*/$')" || return 1
	echo "${dir%/}"
}

# slot the running system was booted from (a or b), from root= on the
# kernel command line that U-Boot's load line sets
ucgfiber_running_slot() {
	local w

	for w in $(cat /proc/cmdline); do
		case "$w" in
		root=PARTLABEL=owrt_rootfs_a) echo a; return 0 ;;
		root=PARTLABEL=owrt_rootfs_b) echo b; return 0 ;;
		esac
	done
	return 1
}

ucgfiber_target_slot() {
	case "$(ucgfiber_running_slot)" in
	a) echo b ;;
	b) echo a ;;
	*) return 1 ;;
	esac
}

# block device of the GPT partition named <label>; exactly one may exist
# (a second disk with the same name, e.g. a USB stick, is refused)
ucgfiber_find_part() { # <label>
	local u dev found=

	for u in /sys/class/block/*/uevent; do
		grep -qx "PARTNAME=$1" "$u" 2>/dev/null || continue
		[ -z "$found" ] || {
			echo "ucgfiber: more than one partition named $1" >&2
			return 1
		}
		dev="$(sed -n 's/^DEVNAME=//p' "$u")"
		found="/dev/$dev"
	done
	[ -n "$found" ] && [ -b "$found" ] && echo "$found"
}

ucgfiber_part_bytes() { # <dev>
	echo $(($(cat "/sys/class/block/${1##*/}/size") * 512))
}

ucgfiber_check_image() { # <image>
	local img="$1" dir magic target kdev rdev ksize rsize

	dir="$(ucgfiber_board_dir "$img")" || {
		echo "ucgfiber: no sysupgrade-* directory in image" >&2
		return 1
	}
	[ "$dir" = sysupgrade-ubnt_ucgfiber ] || {
		echo "ucgfiber: image is for $dir" >&2
		return 1
	}

	# kernel: flattened device tree (FIT); root: squashfs
	magic="$(ucgfiber_member "$img" "$dir/kernel" |
		hexdump -v -n 4 -e '4/1 "%02x"')"
	[ "$magic" = d00dfeed ] || {
		echo "ucgfiber: kernel is not a FIT image" >&2
		return 1
	}
	magic="$(ucgfiber_member "$img" "$dir/root" |
		hexdump -v -n 4 -e '4/1 "%02x"')"
	[ "$magic" = 68737173 ] || {
		echo "ucgfiber: root is not a squashfs image" >&2
		return 1
	}

	target="$(ucgfiber_target_slot)" || {
		echo "ucgfiber: not running from an OpenWrt slot (root=PARTLABEL=owrt_rootfs_a/b); install with the installer" >&2
		return 1
	}
	kdev="$(ucgfiber_find_part owrt_kernel_$target)" &&
	rdev="$(ucgfiber_find_part owrt_rootfs_$target)" || {
		echo "ucgfiber: partitions of slot $target not found" >&2
		return 1
	}

	ksize="$(ucgfiber_member "$img" "$dir/kernel" | wc -c)"
	rsize="$(ucgfiber_member "$img" "$dir/root" | wc -c)"
	[ "$ksize" -le "$(ucgfiber_part_bytes "$kdev")" ] || {
		echo "ucgfiber: kernel ($ksize bytes) does not fit $kdev" >&2
		return 1
	}
	# squashfs rounded up to 64 KiB (fstools' overlay offset), then room for the overlay
	[ $(( ((rsize + 65535) / 65536) * 65536 + UCGFIBER_OVERLAY_MIN )) -le "$(ucgfiber_part_bytes "$rdev")" ] || {
		echo "ucgfiber: root ($rsize bytes) leaves no room for the overlay on $rdev" >&2
		return 1
	}
	return 0
}

# make the writes durable: sync only hands raw block device writes to the
# device; an fsync on the device also sends a cache flush, without which an
# eMMC (or any device with a volatile write cache) may still lose them at a
# power cut. Then drop the page cache, so the check reads the device.
ucgfiber_flush() { # <dev>...
	local d

	for d in "$@"; do
		dd if=/dev/null of="$d" bs=512 count=0 conv=notrunc,fsync 2>/dev/null || {
			echo "ucgfiber: cache flush of $d failed" >&2
			return 1
		}
	done
	echo 3 >/proc/sys/vm/drop_caches
}

# the members on the target partitions equal the ones in the image
ucgfiber_verify() { # <image> <dir> <kdev> <rdev>
	local m dev n

	for m in kernel root; do
		[ "$m" = kernel ] && dev="$3" || dev="$4"
		n="$(ucgfiber_member "$1" "$2/$m" | wc -c)"
		[ "$(ucgfiber_member "$1" "$2/$m" | sha256sum | cut -d' ' -f1)" = \
		  "$(head -c "$n" "$dev" | sha256sum | cut -d' ' -f1)" ] || {
			echo "ucgfiber: $m on $dev does not match the image" >&2
			return 1
		}
	done
}

ucgfiber_switch() { # <a|b>
	ucgfiber-envset --trial "owrt_$1" || {
		echo "ucgfiber: slot $1 written but not activated (ucg_boot unchanged)" >&2
		return 1
	}
}

ucgfiber_do_upgrade() { # <image>
	local img="$1" dir target

	UCGFIBER_WRITTEN=
	dir="$(ucgfiber_board_dir "$img")" || return 1
	target="$(ucgfiber_target_slot)" || {
		echo "ucgfiber: not running from an OpenWrt slot" >&2
		return 1
	}
	export EMMC_KERN_DEV="$(ucgfiber_find_part owrt_kernel_$target)"
	export EMMC_ROOT_DEV="$(ucgfiber_find_part owrt_rootfs_$target)"
	[ -b "$EMMC_KERN_DEV" ] && [ -b "$EMMC_ROOT_DEV" ] || {
		echo "ucgfiber: partitions of slot $target not found" >&2
		return 1
	}

	# generic eMMC upgrade: invalidates the target kernel, writes root and
	# kernel; without a config backup it also clears the target overlay
	emmc_do_upgrade "$img"
	sync
	ucgfiber_flush "$EMMC_KERN_DEV" "$EMMC_ROOT_DEV" || return 1
	ucgfiber_verify "$img" "$dir" "$EMMC_KERN_DEV" "$EMMC_ROOT_DEV" || return 1
	UCGFIBER_TARGET="$target"
	UCGFIBER_WRITTEN=1

	# with a config backup, the switch follows in ucgfiber_copy_config
	[ -n "$UPGRADE_BACKUP" ] || ucgfiber_switch "$target"
}

# the config backup goes behind the new squashfs, where fstools looks for
# it on the first boot of the new slot; then the switch
ucgfiber_copy_config() {
	[ "$UCGFIBER_WRITTEN" = 1 ] || {
		echo "ucgfiber: upgrade not written, settings not copied" >&2
		return 1
	}
	emmc_copy_config
	sync
	ucgfiber_flush "$EMMC_ROOT_DEV" || return 1
	ucgfiber_switch "$UCGFIBER_TARGET"
}

# end of the boot (/etc/init.d/bootcount): confirm a trial boot, keeping the
# slot that runs; when U-Boot fell back to this slot (the other one did not
# load), keep this one too, and log it either way
ucgfiber_boot_done() {
	local run want trial

	run="$(ucgfiber_running_slot)" || return 0
	want="$(fw_printenv -n ucg_boot 2>/dev/null)"
	trial="$(fw_printenv -n upgrade_available 2>/dev/null)"
	[ "$want" = "owrt_$run" ] ||
		logger -t ucgfiber -p daemon.warn \
			"running slot $run, but ucg_boot is '${want:-unset}': U-Boot fell back to this slot"
	[ "$trial" = 1 ] || return 0
	if ucgfiber-envset ucg_boot "owrt_$run" >/dev/null; then
		logger -t ucgfiber "trial boot of slot $run confirmed"
	else
		logger -t ucgfiber -p daemon.err "trial boot of slot $run: confirming failed"
		return 1
	fi
}
