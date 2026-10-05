REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk fit_check_sign'

platform_do_upgrade() {
  local board=$(board_name)

  case "$board" in
  quantum,c6500xk-v1)
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
