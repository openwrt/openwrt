#!/bin/sh

net48xx_board_name () {
    local name
    local pci=`wc -l /proc/bus/pci/devices`

    case "$pci" in
	*"8"*)
	    name="net4826"
	    ;;
	*1[0-4]*)
	    name="net4801"
	    ;;
	*)
	    name="net4826"
	    ;;
    esac
    echo $name
}
