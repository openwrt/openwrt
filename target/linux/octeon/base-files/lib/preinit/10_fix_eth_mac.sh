#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

. /lib/functions/system.sh

preinit_set_mac_address() {
	case "$(board_name)" in
	dlink,dap-2690-a1)
		ip link set dev lan address "$(mtd_get_mac_ascii bdcfg lanmac)"
		;;
	esac
}

boot_hook_add preinit_main preinit_set_mac_address
