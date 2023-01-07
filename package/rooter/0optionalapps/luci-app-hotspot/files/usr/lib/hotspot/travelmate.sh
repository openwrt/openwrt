#!/bin/sh

. /lib/functions.sh

# travelmate, a wlan connection manager for travel router
# written by Dirk Brenken (dev@brenken.org)

# This is free software, licensed under the GNU General Public License v3.
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# prepare environment
#
LC_ALL=C
PATH="/usr/sbin:/usr/bin:/sbin:/bin"
trm_ver="0.3.0"
trm_enabled=1
trm_debug=0
trm_maxwait=20
trm_maxretry=1
trm_iw=1
trm_auto=$(uci -q get travelmate.global.trm_auto)

check_wwan() {
	uci set travelmate.global.ssid="8"
	wif=$(uci -q get travelmate.global.freq)
	if [ -z "$wif" ]; then
		uci set travelmate.global.freq="2"
	fi
	uci commit travelmate
	while [ ! -e /etc/config/wireless ]
	do
		sleep 1
	done
	sleep 3
	f_check "ap"
	cntx=0
	while [ "${trm_ifstatus}" != "true" ]; do
		sleep 1
		f_check "ap"
		let cntx=cntx+1
		if [ $cntx -ge 20 ]; then
			break
		fi
	done
	f_log "info" "AP Status   ::: $trm_ifstatus"
}

count_radio() {
	local config=$1
	local channel

	config_get channel $1 channel
	if [ $channel -gt 15 ]; then
		uci set travelmate.global.radio5="5.8 Ghz"
	else
		uci set travelmate.global.radio2="2.4 Ghz"
	fi
	let radcnt=radcnt+1
}

check_radio() {
	radcnt=0
	config_load wireless
	config_foreach count_radio wifi-device
	uci set travelmate.global.radcnt=$radcnt
	uci commit travelmate
}

f_envload()
{
    # source required system libraries
    #
    if [ -r "/lib/functions.sh" ]
    then
        . "/lib/functions.sh"
    else
        f_log "error" "required system library not found"
    fi

    # load uci config and check 'enabled' option
    #
    option_cb()
    {
        local option="${1}"
        local value="${2}"
        eval "${option}=\"${value}\""
    }
    config_load travelmate

    if [ ${trm_enabled} -ne 1 ]
    then
        f_log "info " "status  ::: Hotspot Manager is currently disabled"
        exit 0
    fi

    # check for preferred wireless tool
    #
    if [ ${trm_iw} -eq 1 ]
    then
        trm_scanner="$(which iw)"
    else
        trm_scanner="$(which iwinfo)"
    fi
    if [ -z "${trm_scanner}" ]
    then
        f_log "error" "status  ::: no wireless tool for wlan scanning found, please install 'iw' or 'iwinfo'"
    fi
}

# function to bring down all STA interfaces
#
f_prepare()
{
    local config="${1}"
    local mode="$(uci -q get wireless."${config}".mode)"
    local network="$(uci -q get wireless."${config}".network)"
    local disabled="$(uci -q get wireless."${config}".disabled)"

    if [ "${mode}" = "sta" ] && [ -n "${network}" ]
    then
        trm_stalist="${trm_stalist} ${config}_${network}"
        if [ -z "${disabled}" ] || [ "${disabled}" = "0" ]
        then
            uci -q set wireless."${config}".disabled=1
            f_log "debug" "prepare ::: config: ${config}, interface: ${network}"
        fi
    fi
}

f_check()
{
    local ifname cnt=0 mode="${1}"
    trm_ifstatus="false"

    while [ ${cnt} -lt ${trm_maxwait} ]
    do
		wif=$(uci -q get travelmate.global.freq)
		RADIO=$(uci get wireless.wwan$wif.device)
		ifname="$(ubus -S call network.wireless status | jsonfilter -l 1 -e "@.$RADIO.interfaces[@.config.mode=\"${mode}\"].ifname")"
		if [ -z $ifname ]; then
			break
		fi
        if [ "${mode}" = "sta" ]
        then
			trm_ifstatus="$(ubus -S call network.interface dump | jsonfilter -e "@.interface[@.device=\"${ifname}\"].up")"
        else
            trm_ifstatus="$(ubus -S call network.wireless status | jsonfilter -l1 -e '@.*.up')"
        fi
        if [ "${trm_ifstatus}" = "true" ]
        then
            break
        fi
        cnt=$((cnt+1))
        sleep 1
    done
    f_log "debug" "check   ::: ${mode} name: ${ifname}, status: ${trm_ifstatus}, count: ${cnt}"
}

# function to write to syslog
#
f_log()
{
    local class="${1}"
    local log_msg="${2}"

    if [ -n "${log_msg}" ] && ([ "${class}" != "debug" ] || [ ${trm_debug} -eq 1 ])
    then
        wifilog "HOTSPOT-[${trm_ver}] ${class}" "${log_msg}"
        if [ "${class}" = "error" ]
        then
			wif=$(uci -q get travelmate.global.freq)
			uci set travelmate.global.ssid="1"
			uci commit travelmate
			uci -q set wireless.wwan@wif.ssid="Hotspot Manager Interface"
			uci -q set wireless.wwan$wif.encryption="none"
			uci -q set wireless.wwan$wif.key=
			uci -q commit wireless
            #exit 255
        fi
    fi
}

ssid_list=""
ap_list=""
trm_stalist=""

f_working_ap() {
	f_check "ap"
	wif=$(uci -q get travelmate.global.freq)
	RADIO=$(uci get wireless.wwan$wif.device)
	ap_list="$(ubus -S call network.wireless status | jsonfilter -e "@.$RADIO.interfaces[@.config.mode=\"ap\"].ifname")"
	f_log "debug" "main    ::: ap-list: ${ap_list}, sta-list: ${trm_stalist}"
	if [ -z "${ap_list}" ] || [ -z "${trm_stalist}" ]
	then
		sleep 3
		f_check "ap"
		ap_list="$(ubus -S call network.wireless status | jsonfilter -e "@.$RADIO.interfaces[@.config.mode=\"ap\"].ifname")"
		f_log "debug" "main    ::: ap-list: ${ap_list}, sta-list: ${trm_stalist}"
	fi
}

f_scan_ap() {
	wif=$(uci -q get travelmate.global.freq)
	radio=$(uci get wireless.wwan$wif.device)
	# scan using AP radio
	trm_iwinfo="$(command -v iwinfo)"
	trm_maxscan="10"
	scan_dev="$(ubus -S call network.wireless status 2>/dev/null | jsonfilter -q -l1 -e "@.${radio}.interfaces[0].ifname")"
	ssid_list="$("${trm_iwinfo}" "${scan_dev:-${radio}}" scan 2>/dev/null |
			awk 'BEGIN{FS="[[:space:]]"}/Address:/{var1=$NF}/ESSID:/{var2="";for(i=12;i<=NF;i++)if(var2==""){var2=$i}else{var2=var2" "$i}}
			/Quality:/{split($NF,var0,"/")}/Encryption:/{if($NF=="none"){var3="+"}else{var3="-"};
			printf "%i %s %s %s\n",(var0[1]*100/var0[2]),var3,var1,var2}' | sort -rn | head -qn "${trm_maxscan}")"
}

f_main()
{
    local config network ssid cnt=0
	wif=$(uci -q get travelmate.global.freq)
	
	trm_stalist=""
	# check if wwan is connected
    f_check "sta"
	if [ "${trm_ifstatus}" == "true" ]; then
			wifi down $(uci -q get wireless.wwan$wif.device)
			f_check "sta"
			f_log "info" "STA ${trm_ifstatus}"
			while [ "${trm_ifstatus}" == "true" ]; do
				sleep 1
				f_check "sta"
			done
		fi
    if [ "${trm_ifstatus}" != "true" ] # not connected
    then
		uci set travelmate.global.state='1'
		uci commit travelmate
		f_check "ap"
		f_log "info" "AP ${trm_ifstatus}"
		if [ "${trm_ifstatus}" != "true" ]; then
			wifi up $(uci -q get wireless.wwan$wif.device)
			f_check "ap"
			while [ "${trm_ifstatus}" != "true" ]; do
				sleep 1
				f_check "ap"
			done
		fi
		uci set travelmate.global.bssid=""
		uci set travelmate.global.ssid="2"
		uci commit travelmate
		uci -q set wireless.wwan$wif.ssid="Hotspot Manager Interface"
		uci -q set wireless.wwan$wif.encryption="none"
		uci -q set wireless.wwan$wif.key=
		uci -q commit wireless
		ubus call network.interface.wwan$wif up
		ubus call network reload
		wifi up $(uci -q get wireless.wwan$wif.device)
		sleep 5

		# set disabled for wwan iface
        config_load wireless
        config_foreach f_prepare wifi-iface
        if [ -n "$(uci -q changes wireless)" ]
        then
            uci -q commit wireless
            ubus call network reload
        fi
		# check if AP working
		f_working_ap
		# AP or STA not working
        if [ -z "${ap_list}" ] || [ -z "${trm_stalist}" ]
        then
            f_log "error" "main    ::: no usable AP/STA configuration found"
			
        else
			# single AP in list
			for ap in ${ap_list}
			do
				#f_scan_ap
				# repeat scan and connection
				cnt=0
				delay=10
				reconn=$(uci -q get travelmate.global.reconn)
				while [ ${cnt} -lt $reconn ]
				do
					f_log "info" " Retry Count ${cnt}"
					if [ $reconn -eq 99 ]; then
						cnt=0
					fi
					f_scan_ap
					f_log "info" " SSID List ${ssid_list}"
					# get list of Hotspots present
					if [ -n "${ssid_list}" ]; then
						if [ "$trm_auto" = "1" ]; then
							FILE="/etc/hotspot"
						else
							FILE="/tmp/hotman"
						fi
						if [ -f "${FILE}" ]; then
							# read list of selected Hotspots
							while IFS='|' read -r ssid encrypt key
							do
								ssidq="\"$ssid\""
								# see if in scan list
								if [ -n "$(printf "${ssid_list}" | grep -Fo "${ssidq}")" ]; then
									# connect to Hotspot
									uci set travelmate.global.bssid="$ssid"
									uci set travelmate.global.ssid=">>> $ssid"
									uci set travelmate.global.connecting="1"
									uci commit travelmate
									uci -q set wireless.wwan$wif.ssid="$ssid"
									uci -q set wireless.wwan$wif.encryption=$encrypt
									uci -q set wireless.wwan$wif.key=$key
									uci -q set wireless.wwan$wif.disabled=0
									uci -q commit wireless
									wifi up $(uci -q get wireless.wwan$wif.device)
									ubus call network.interface.wwan$wif up
                            		ubus call network reload
									f_log "info " "main    ::: wwan interface connected to uplink ${ssid}"
									sleep 5
									# wait and check for good connection
									f_check "ap"
									f_log "info" "AP Status   ::: $trm_ifstatus"
									cntx=0
									while [ "${trm_ifstatus}" != "true" ]; do
										sleep 1
										f_check "ap"
										let cntx=cntx+1
										if [ $cntx -ge $delay ]; then
											break
										fi
										f_log "info" "AP Status   ::: $trm_ifstatus"
									done
									cntx=0
									#delay=$(uci -q get travelmate.global.delay)
									f_check "sta"
									f_log "info" "STA Status ${trm_ifstatus}"
									while [ "${trm_ifstatus}" != "true" ]; do
										sleep 1
										f_check "sta"
										let cntx=cntx+1
										if [ $cntx -ge $delay ]; then
											break
										fi
										f_log "info" "STA Status ${trm_ifstatus}"
									done

									if [ "${trm_ifstatus}" = "true" ]; then
										uci set travelmate.global.ssid="$ssid"
										uci set travelmate.global.connecting="0"
										uci set travelmate.global.lost="0"
										uci set travelmate.global.state='2'
										uci set travelmate.global.key=$key
										uci set travelmate.global.trm_auto="1"
										uci commit travelmate
										# connection good
										f_log "info" "Connected $ssid"
										exit 0
									fi
									# bad connection try next Hotspot in list
									uci set travelmate.global.bssid=""
									uci set travelmate.global.ssid="3"
									uci commit travelmate
									uci -q set wireless.wwan$wif.ssid="Hotspot Manager Interface"
									uci -q set wireless.wwan$wif.encryption="none"
									uci -q set wireless.wwan$wif.key=
									uci -q set wireless.wwan$wif.disabled=1
									uci -q commit wireless
									ubus call network.interface.wwan$wif down
									ubus call network reload
									f_log "info" "Try next in list"
								fi
							done <"${FILE}"
							wifi up $(uci -q get wireless.wwan$wif.device)
							f_check "ap"
							while [ "${trm_ifstatus}" != "true" ]; do
								f_check "ap"
								sleep 1
							done
						fi
					fi
					# No connection to any in list
					cnt=$((cnt+1))
					if [ $reconn -gt 0 ]; then
						if [ ${cnt} -lt $reconn ]; then
							f_log "info " "Sleep before retrying"
							sleep 30
						fi
					fi
					# repeat scan and connect
				done
			done
		fi
		# unable to connect
		if [ "$trm_auto" = "1" ]; then
			uci set travelmate.global.ssid="4"
		else
			uci set travelmate.global.ssid="5"
		fi
		reconn=$(uci -q get travelmate.global.reconn)
		lost=$(uci -q get travelmate.global.lost)
		if [ $reconn -eq 99 ]; then
			lost="1"
		fi
		if [ $lost -gt $reconn ]; then
			uci set travelmate.global.ssid="5"
		fi
		uci set travelmate.global.trm_enabled="0"
		uci set travelmate.global.connecting="0"
		uci set travelmate.global.lost="0"
		uci set travelmate.global.state='0'
		uci set travelmate.global.bssid=""
		uci set travelmate.global.trm_auto="1"
		uci commit travelmate
		uci -q set wireless.wwan$wif.ssid="Hotspot Manager Interface"
		uci -q set wireless.wwan$wif.encryption="none"
		uci -q set wireless.wwan$wif.key=
		uci -q commit wireless
		f_log "info " "main    ::: no wwan uplink found"
    fi
	# already connected
	exit 0
}

check_wwan
check_radio
f_envload
f_main

exit 0