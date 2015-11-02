#!/bin/sh
#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#

do_mediatek() {
	. /lib/mediatek.sh

	mediatek_board_detect
}

boot_hook_add preinit_main do_mediatek
