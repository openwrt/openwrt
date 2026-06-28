REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk fit_check_sign'

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
    PART_NAME=firmware
    nand_do_upgrade "$1"
    ;;
  *)
    nand_do_upgrade "$1"
    ;;
  esac
}

platform_check_image() {
  return 0
}
