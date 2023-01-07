#!/bin/sh
. /lib/functions.sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	modlog "Connection Monitor $CURRMODEM" "$@"
}

CURRMODEM=$1

power_toggle() {
	if [ -f "/tmp/gpiopin" ]; then
		source /tmp/gpiopin
		echo "$GPIOPIN" > /sys/class/gpio/export
		if [ $? -eq 0 ]; then
			$ROOTER/pwrtoggle.sh 3
		else
			$ROOTER/pwrtoggle.sh $CURRMODEM
		fi
	else
		if [ $ACTIVE = 4 ]; then
# GPIO power-toggle not supported so re-bind USB driver, but only if power toggle is configured in connection monitoring
			if [ $(uci -q get modem.pinginfo$CURRMODEM.alive) = 4 ]; then
				$ROOTER/pwrtoggle.sh $CURRMODEM
			fi
#			if [ -L /sys/bus/usb/drivers/usb/usb1 ]; then
#				if [ $(uci get modem.pinginfo1.alive) = 4 ]; then
#					$ROOTER/pwrtoggle.sh 1
#				fi
#			fi
#			if [ -L /sys/bus/usb/drivers/usb/usb2 ]; then
#				if [ $(uci get modem.pinginfo2.alive) = 4 ]; then
#					$ROOTER/pwrtoggle.sh 2
#				fi
#			fi
		fi
	fi
}

do_down() {
	echo 'MONSTAT="'"DOWN$1"'"' > /tmp/monstat$CURRMODEM
	case $ACTIVE in
	"1" )
		log "Modem $CURRMODEM Connection is Down$1"
		;;
	"2" )
		log "Modem $CURRMODEM Connection is Down$1"
		reboot -f
		;;
	"3" )
		log "Modem $CURRMODEM Connection is Down$1"
		PROT=$(uci get modem.modem$CURRMODEM.proto)
		if [ $PROT -eq "30" ]; then
			CPORT=$(uci get modem.modem$CURRMODEM.commport)
			ATCMDD="AT+CFUN=4;+CFUN=1,1"
			$ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD"
			echo "1" > /tmp/modgone
			log "Setting Modem Removal flag"
			sleep 60
		else
			if [ -f $ROOTER_LINK/reconnect$CURRMODEM ]; then
				$ROOTER_LINK/reconnect$CURRMODEM $CURRMODEM &
			fi
		fi
		;;
	"4" )
		log "Modem $CURRMODEM Connection is Down$1"
		power_toggle
		;;
	esac
}

CURSOR="-"

log "Start Connection Monitor for Modem $CURRMODEM"

sleep 30

while [ 1 = 1 ]; do
	CP=$(uci -q get ping.ping.enable)
	if [ $CP = "1" ]; then
		echo 'MONSTAT="'"Custom Ping Test"'"' > /tmp/monstat$CURRMODEM
		sleep 60
	else
		ACTIVE=$(uci get modem.pinginfo$CURRMODEM.alive)
		if [ $ACTIVE = "0" ]; then
			echo 'MONSTAT="'"Disabled"'"' > /tmp/monstat$CURRMODEM
			sleep 60
		else
			track_ips=
			IFNAME=$(uci get modem.modem$CURRMODEM.interface)
			TIMEOUT=$(uci get modem.pinginfo$CURRMODEM.pingwait)
			INTERVAL=$(uci get modem.pinginfo$CURRMODEM.pingtime)
			RELIAB=$(uci get modem.pinginfo$CURRMODEM.reliability)
			DOWN=$(uci get modem.pinginfo$CURRMODEM.down)
			UP=$(uci get modem.pinginfo$CURRMODEM.up)
			COUNT=$(uci get modem.pinginfo$CURRMODEM.count)
			PACKETSIZE=$(uci get modem.pinginfo$CURRMODEM.packetsize)
			INTERF=$(uci get modem.modeminfo$CURRMODEM.inter)

			list_track_ips() {
				track_ips="$1 $track_ips"
			}

			config_load modem
			config_list_foreach "pinginfo$CURRMODEM" "trackip" list_track_ips

			if [ -f "/tmp/connstat$CURRMODEM" ]; then
				do_down " from Modem"
				rm -f /tmp/connstat$CURRMODEM
				sleep 20
			else
				ENB="0"
				if [ -e /etc/config/failover ]; then
					ENB=$(uci get failover.enabled.enabled)
				fi
				if [ $ENB = "1" ]; then
					if [ -e /tmp/mdown$CURRMODEM ]; then
						do_down " (using Failover)"
					else
						echo 'MONSTAT="'"Up ($CURSOR) (using Failover)"'"' > /tmp/monstat$CURRMODEM
					fi
					sleep 20
				else
					# check to see if modem iface has an IP address, if not try a reconnect/power toggle
					OX=$(ip address show $IFNAME 2>&1)
					ip4=$(echo "$OX" | grep 'inet ' | cut -d' ' -f6)
					ip6=$(echo "$OX" | grep 'inet6' | grep global | cut -d' ' -f6)
								if [ -z "$ip4" -a -z "$ip6" ]; then
										do_down " (no IP address)"
								fi

						UPDWN="0"
						host_up_count=0
						score_up=$UP
						score_dwn=$DOWN
						lost=0
						while true; do
							if [ ! -z "$track_ips" ]; then
								for track_ip in $track_ips; do
									ping -I $IFNAME -c $COUNT -W $TIMEOUT -s $PACKETSIZE -q $track_ip &> /dev/null
									if [ $? -eq 0 ]; then
										let host_up_count++
									else
										let lost++
									fi
								done
								if [ $host_up_count -lt $RELIAB ]; then
									let score_dwn--
									score_up=$UP
									if [ $score_dwn -eq 0 ]; then
										UPDWN="1"
										break
									fi
								else
									let score_up--
									score_dwn=$DOWN
									if [ $score_up -eq 0 ]; then
										UPDWN="0"
										break
									fi
								fi
							else
								UPDWN="0"
								exit
							fi
							host_up_count=0
							sleep $INTERVAL
						done
						if [ $UPDWN = "1" ]; then
							do_down " (using Ping Test)"
						else
							echo 'MONSTAT="'"UP ($CURSOR) (using Ping Test)"'"' > /tmp/monstat$CURRMODEM
						fi
						sleep $INTERVAL

				fi
			fi
			if [ $CURSOR = "-" ]; then
				CURSOR="+"
			else
				CURSOR="-"
			fi
		fi
	fi
done
