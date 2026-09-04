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
  *)
    nand_do_upgrade "$1"
    ;;
  esac
  sync
}

platform_check_image() {
  return 0
}
