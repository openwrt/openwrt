#!/bin/sh

set_preinit_boot_count() {
        . /lib/functions.sh

        case $(board_name) in
        linksys,ea6350v3)
                fw_setenv boot_count 0
                ;;
        *)
                ;;
        esac
}

boot_hook_add preinit_main set_preinit_boot_count
