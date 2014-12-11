#!/bin/sh

au1000_yamonenv_getvar()
{
	local varname="$1"
	local partition

	. /lib/functions.sh

	partition="$( find_mtd_part 'yamon env' )"
	YAMONENVFILE="$partition" yamonenv "$varname"
}

au1000_detect()
{
	local line board_name model

	while read line; do
		case "$line" in
			'system type'*)
				break
			;;
		esac
	done <'/proc/cpuinfo'

	case "$line" in
		*' MTX-1')
			# both models appear nearly similar: the 'InternetBox' has
			# the same design but shrinked to 1 PCB and only 1 x miniPCI
			# for WiFi/ath5k and 1 x miniPCI for CardBus/UMTS, they differ
			# in BogoMIPS but there are old MeshCubes with 330 Mhz instead
			# of 400 MHz and no Cube has 'imei' (for UMTS) set in bootloader-env

			if [ -n "$( au1000_yamonenv_getvar 'imei' )" ]; then
				board_name='internetbox'
				model='T-Mobile InternetBox TMD SB1-S'
			else
				board_name='meshcube'
				model='4G Systems AccessCube/MeshCube'
			fi
		;;
		*)
			board_name='unknown'
			model='unknown'
		;;
	esac

	mkdir -p '/tmp/sysinfo'
	echo "$board_name" >'/tmp/sysinfo/board_name'
	echo "$model" >'/tmp/sysinfo/model'
}

au1000_board_name()
{
	local file='/tmp/sysinfo/board_name'

	[ -e "$file" ] || au1000_detect
	cat "$file"
}
