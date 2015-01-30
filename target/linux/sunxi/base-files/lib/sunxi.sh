#!/bin/sh

# defaults
SUNXI_BOARD_NAME="generic"
SUNXI_BOARD_MODEL="Generic sunxi board"

sunxi_board_detect() {
	local board
	local model

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	model="$( cat /proc/device-tree/model )"

	case "$model" in
		"Mele A1000")
			board="a1000"
			;;

		"BA10 tvbox")
			board="ba10-tvbox"
			;;

		"Cubietech Cubieboard")
			board="cubieboard"
			;;

		"Miniand Hackberry")
			board="hackberry"
			;;

		"INet-97F Rev 02")
			board="inet97fv2"
			;;

		"PineRiver Mini X-Plus")
			board="mini-xplus"
			;;

		"Olimex A10-OLinuXino-LIME")
			board="olinuxino-lime"
			;;

		"LinkSprite pcDuino")
			board="pcduino"
			;;

		"Olimex A10s-Olinuxino Micro")
			board="olinuxino-micro"
			;;

		"R7 A10s hdmi tv-stick")
			board="r7-tv-dongle"
			;;

		"HSG H702")
			board="hsg-h702"
			;;

		"Olimex A13-Olinuxino")
			board="olinuxino"
			;;

		"Olimex A13-Olinuxino Micro")
			board="olinuxino-micro"
			;;

		"Allwinner A31 APP4 EVB1 Evaluation Board")
			board="app4-evb1"
			;;

		"WITS A31 Colombus Evaluation Board")
			board="colombus"
			;;

		"Merrii A31 Hummingbird")
			board="hummingbird"
			;;

		"Mele M9 / A1000G Quad top set box")
			board="m9"
			;;

		"LeMaker Banana Pi")
			board="bananapi"
			;;

		"LeMaker Banana Pro")
			board="bananapro"
			;;

		"Cubietech Cubieboard2")
			board="cubieboard2"
			;;

		"Cubietech Cubietruck")
			board="cubietruck"
			;;

		"Merrii A20 Hummingbird")
			board="hummingbird"
			;;

		"I12 / Q5 / QT840A A20 tvbox")
			board="i12-tvbox"
			;;

		"Olimex A20-OLinuXino-LIME")
			board="olinuxino-lime"
			;;

		"Olimex A20-Olinuxino Micro")
			board="olinuxino-micro"
			;;

		"LinkSprite pcDuino3")
			board="pcduino3"
			;;

		"Ippo Q8H Dual Core Tablet (v5)")
			board="ippo-q8h-v5"
			;;
	esac

	if [ "$board" != "" ]; then
		SUNXI_BOARD_NAME="$board"
	fi

	if [ "$model" != "" ]; then
		SUNXI_BOARD_MODEL="$model"
	fi


	echo "$SUNXI_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$SUNXI_BOARD_MODEL" > /tmp/sysinfo/model
	echo "Detected $SUNXI_BOARD_NAME // $SUNXI_BOARD_MODEL"
}

sunxi_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name="$(cat /tmp/sysinfo/board_name)"
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
