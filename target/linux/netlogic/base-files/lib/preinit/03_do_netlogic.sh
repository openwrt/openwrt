#!/bin/sh

do_netlogic() {
	. /lib/netlogic.sh

	netlogic_board_detect
}

boot_hook_add preinit_main do_netlogic
