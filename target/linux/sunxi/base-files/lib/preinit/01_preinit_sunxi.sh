#!/bin/sh

do_sunxi() {
        . /lib/sunxi.sh

        sunxi_board_detect
}

boot_hook_add preinit_main do_sunxi
