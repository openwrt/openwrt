REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk fit_check_sign'

platform_do_upgrade() {
  local board=$(board_name)

  case "$board" in
  dasan,h660gm-a-airtel|\
  dasan,h660gm-a-generic|\
  jiofiber,jcow407|\
  jiofiber,jcow414)
    CI_KERNPART="tclinux_kernel"
    nand_do_upgrade "$1"
    ;;
  tplink,xc220-g3v)
    # Raw NOR, no UBI: the whole sysupgrade.bin (FIT kernel + squashfs
    # rootfs + metadata) goes straight into the "firmware" MTD partition.
    # nand_do_upgrade (the default below) assumes a NAND/UBI layout and
    # will ubiformat this partition instead -- wrong for this device.
    PART_NAME="firmware"
    default_do_upgrade "$1"
    ;;
  *)
    nand_do_upgrade "$1"
    ;;
  esac
  sync
}

platform_check_image() {
  return 0
}
