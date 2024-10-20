#!/bin/sh

. /lib/functions.sh

include /lib/upgrade

. /usr/share/libubox/jshn.sh

BOARD="$(board_name | sed 's/,/_/g')"

# Use snapshots until a stable 24.x release
SITE="https://downloads.openwrt.org/snapshots/targets/ramips/mt7621"
FILE="openwrt-ramips-mt7621-${BOARD}-squashfs-sysupgrade.bin"
tar_file="/tmp/sysupgrade.img"
SITE=${TESTSITE:-$SITE}


confirm_migration() {
    case "$(board_name)" in
        ubnt,edgerouter-x|\
        ubnt,edgerouter-x-sfp)
            compat=$(uci -q get system.@system[0].compat_version)
            if [ "$compat" != "1.1" ]; then
                echo "Incompatible compat version $compat" >&2
                exit 1
            fi
            ;;
        *)
            echo "Incompatible board $(board_name)" >&2
            exit 1
            ;;
    esac

    echo -e "\033[0;31mWARNING:\033[0m This script will migrate your OpenWrt system to a new layout as"
    echo "required for the linux 6.6 kernel. This process will erase all your current settings."
    echo "It is recommended to back up your system before proceeding."
    echo ""

    read -p "Do you want to proceed with the migration? (y/n)" confirm
    if [ "$confirm" != "y" ]; then
        echo "Migration canceled."
        exit 0
    fi
}

download_image(){
    wget -qO "$tar_file" "$SITE/$FILE"
    sha256=$(wget -qO- "$SITE/sha256sums" | grep "$BOARD-squashfs-sysupgrade.bin" | cut -d ' ' -f1)
    sha256local=$(sha256sum "$tar_file" | cut -d ' ' -f1)
    if [ "$sha256" != "$sha256local" ]; then
            echo "Downloaded image checksum mismatch" >&2
            exit 1
    fi

    board_dir=$( (tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$') 2> /dev/null)
    export board_dir="${board_dir%/}"

    tar xC /tmp -f "$tar_file" 2> /dev/null
    if [ ! -f /tmp/$board_dir/kernel ] || [ ! -f /tmp/$board_dir/root ]; then
        echo "Invalid image file" >&2
        exit 1
    fi
    cp /sbin/ubnt_erx_stage2.sh /tmp

}

confirm_migration
download_image

install_bin /sbin/upgraded

RAM_ROOT="/tmp/root"
COMMAND="sh /tmp/ubnt_erx_stage2.sh"

json_init
json_add_string prefix "$RAM_ROOT"
json_add_string path "$tar_file"
json_add_boolean force 1
json_add_string command "$COMMAND"
json_add_object options
json_add_int save_partitions "$SAVE_PARTITIONS"
json_close_object

ubus call system sysupgrade "$(json_dump)"


