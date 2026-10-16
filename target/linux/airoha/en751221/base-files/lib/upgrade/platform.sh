REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk fit_check_sign'

platform_do_upgrade() {
  local board=$(board_name)

  case "$board" in
  tplink,archer-xr500v-v1)
    fit_do_upgrade "$1"
    ;;
  *)
    nand_do_upgrade "$1"
    ;;
  esac
  sync
}

platform_check_image() {
  local board=$(board_name)
  [ "$#" -gt 1 ] && return 1

  case "$board" in
  tplink,archer-xr500v-v1)
    fit_check_image "$1"
    return $?
    ;;
  esac

  return 0
}
