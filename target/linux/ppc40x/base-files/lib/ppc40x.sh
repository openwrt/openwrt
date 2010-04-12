#!/bin/sh
#
# Copyright (C) 2010 OpenWrt.org
#

ppc40x_board_name() {
	local model

	model=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /model/ {print $2}' /proc/cpuinfo)
	echo $model
}
