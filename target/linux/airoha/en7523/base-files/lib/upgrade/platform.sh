REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk fit_check_sign yafut flash_erase'

ROUTERBOOT_KERNEL_MAX_SIZE=$((7 * 1024 * 1024))
ROUTERBOOT_KERNEL_MTD_SIZE=$((8 * 1024 * 1024))

routerboot_kernel_from_sysupgrade() {
	local image="$1"
	local output="$2"
	local board_dir

	board_dir="$(tar tf "$image" | grep -m 1 '^sysupgrade-.*/$')"
	board_dir="${board_dir%/}"

	[ -n "$board_dir" ] || {
		echo "Cannot find sysupgrade directory"
		return 1
	}

	tar xOf "$image" "${board_dir}/kernel" > "$output" || {
		echo "Cannot extract RouterBOOT kernel from sysupgrade image"
		rm -f "$output"
		return 1
	}
}

platform_do_upgrade_mikrotik_nand() {
	local image="$1"
	local fw_mtd fw_mtd_name fw_mtd_size
	local kernel="/tmp/routerboot-kernel.elf"

	CI_KERNPART=none
	CI_UBIPART=ubi
	CI_ROOTPART=rootfs

	fw_mtd="$(find_mtd_part kernel)"
	fw_mtd="${fw_mtd/block/}"

	[ -n "$fw_mtd" ] || {
		echo "Cannot find kernel MTD partition"
		return 1
	}

	fw_mtd_name="${fw_mtd##*/}"
	fw_mtd_size="$(cat "/sys/class/mtd/${fw_mtd_name}/size" 2>/dev/null)"

	[ "$fw_mtd_size" = "$ROUTERBOOT_KERNEL_MTD_SIZE" ] || {
		echo "Refusing to erase unexpected kernel partition"
		echo "Device: $fw_mtd, size: ${fw_mtd_size:-unknown}"
		echo "Expected exactly $ROUTERBOOT_KERNEL_MTD_SIZE bytes"
		return 1
	}

	routerboot_kernel_from_sysupgrade "$image" "$kernel" || return 1
	flash_erase "$fw_mtd" 0 0 || return 1
	yafut \
		-d "$fw_mtd" \
		-T -E -P -S -L \
		-C 2048 \
		-B 128k \
		-w \
		-i "$kernel" \
		-o kernel \
		-m 0755 || return 1

	sync

	# The kernel member has already been installed into YAFFS2.  Only create
	# and update rootfs/rootfs_data in the separate UBI partition now.
	nand_do_upgrade "$image"
}

platform_do_upgrade() {
  local board=$(board_name)

  case "$board" in
  gemtek,w1700k-ubi |\
  genexis,arcee |\
  genexis,e650 |\
  genexis,laxy |\
  genexis,pixly_r1 |\
  genexis,rodimus_r1 |\
  genexis,zephyr |\
  tplink,ex530v-v1 |\
  tplink,xx230v-v1)
    fit_do_upgrade "$1"
    ;;
  mikrotik,e60iugs)
    platform_do_upgrade_mikrotik_nand "$1"
    ;;
  *)
    nand_do_upgrade "$1"
    ;;
  esac
}

platform_check_image() {
  return 0
}
