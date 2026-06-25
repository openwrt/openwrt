. /lib/functions/system.sh

preinit_set_mac_address() {
  case $(board_name) in
  genexis,arcee |\
  genexis,laxy |\
  genexis,pixly_r1 |\
  genexis,rodimus_r1 |\
  genexis,zephyr)
    ip link set dev eth0 address $(mtd_get_mac_ascii u-boot-env mac_addr)
    ;;
  esac
}

boot_hook_add preinit_main preinit_set_mac_address
