#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Checker" "$@"
}

channel2="1,2,3,4,5,6,7,8,9,10,11,12,13,14"
channel5="36,40,44,48,149,153,157,161,165,169"
channel5dfs="36,40,44,48,52,56,60,64,100,104,104,112,116,132,136,140,144,149,153,157,161,165,169"

absolute() {
	num=$1
	if [ "$num" -lt 0 ]; then
		num=$((-num))
	fi
}

loadconfig() {
	X=$(uci -q get wireless.wmesh.device)
	uci set mesh.radio.radionumber=$(echo ${X#radio})
	
	uci set mesh.radio.dedicated=$(uci -q get wireless.default_$X.disabled)
	
	chanwidth=$(uci -q get wireless.$X.htmode)
	case $chanwidth in
		"HT20" )
			chanwidth=0
		;;
		"HT40" )
			chanwidth=1
		;;
		"VHT80" )
			chanwidth=2
		;;
		"VHT160" )
			chanwidth=2
		;;
	esac
	uci set mesh.radio.channelwidth=$chanwidth
	
	chan=$(uci -q get wireless.$X.channel)
	if [ $chan -lt 15 ]; then
		uci set mesh.radio.channellist=0
		uci set mesh.radio.channelindex=$((${chan}-1))
	else
		dfs=$(uci -q get wireless.$X.usedfs)
		if [ -z $dfs ]; then
			dfs=1
			uci set mesh.radio.usedfs=1
		else
			uci set mesh.radio.usedfs=$dfs
		fi
		if [ $dfs -eq 0 ]; then
			uci set mesh.radio.channellist=1
			clist=$channel5
		else
			uci set mesh.radio.channellist=2
			clist=$channel5dfs
		fi
		cindex=1
		while [ true ]
		do
			chan=$(echo "$clist" | cut -d, -f$cindex)
			if [ $chan -eq $channel ]; then
				uci set mesh.radio.channelindex=$((${cindex}-1))
				break
			fi
			cindex=$((${cindex}+1))
		done
	fi
	
	enc=$(uci -q get wireless.wmesh.encryption)
	if [ $enc = "sae" ]; then
		uci set mesh.network.netencrypted=1
		uci set mesh.network.netpassword=$(uci -q get wireless.wmesh.key)
	else
		uci set mesh.network.netencrypted=0
		uci set mesh.network.netpassword="password"
	fi
	
	uci set mesh.network.networkid=$(uci -q get wireless.wmesh.mesh_id)
	
	snr=$(uci -q get wireless.default_$X.ieee80211r)
	if [ ! -z $snr ]; then
		uci set mesh.roam.signalenable=$snr
		uci set mesh.roam.signalid=$(uci -q get wireless.default_$X.mobility_domain)
	else
		uci set mesh.roam.signalenable=0
		uci set mesh.roam.signalid="abcd"
	fi

	uci commit mesh
}

count_radio() {
	local config=$1
	local channel

	uci set wireless.default_radio$count.ieee80211r=$signalenable
	uci set wireless.default_radio$count.mobility_domain=$signalid
	uci set wireless.default_radio$count.ft_over_ds="1"
	uci set wireless.default_radio$count.ft_psk_generate_local="1"
	count=$((${count}+1))

}

loadmesh() {
	radionum=$(uci -q get mesh.radio.radionumber)
	dedicated=$(uci -q get mesh.radio.dedicated)
	if [ -z $dedicated ]; then
		dedicated="0"
	fi
	log "default_radio$radionum disabled = $dedicated"
	
	chanwidth=$(uci -q get mesh.radio.channelwidth)
	case $chanwidth in
		"0" )
			chanwidth=20
		;;
		"1" )
			chanwidth=40
		;;
		"2" )
			chanwidth=80
		;;
		"3" )
			chanwidth=80
		;;
	esac
	cwidth=$(uci -q get wireless.radio$radionum.htmode)
	ht=$(echo "$cwidth" | grep "VHT")
	if [ ! -z $ht ]; then
		cwidth="VHT"$chanwidth
	else
		cwidth="HT"$chanwidth
	fi
	log "radio$radionum htmode = $cwidth"
	
	clist=$(uci -q get mesh.radio.channellist)
	cindex=$(uci -q get mesh.radio.channelindex)
	cindex=$((${cindex}+1))
	case $clist in
		"0" )
			channel=$(echo "$channel2" | cut -d, -f$cindex)
		;;
		"1" )
			channel=$(echo "$channel5" | cut -d, -f$cindex)
		;;
		"2" )
			channel=$(echo "$channel5dfs" | cut -d, -f$cindex)
		;;
	esac
	log "radio$radionum channel = $channel"
	
	networkid=$(uci -q get mesh.network.networkid)
	netencrypted=$(uci -q get mesh.network.netencrypted)
	netpassword=$(uci -q get mesh.network.netpassword)
	log "mesh_id = $networkid"
	log "encryption = $netencrypted  key = $netpassword"
	
	signalenable=$(uci -q get mesh.roam.signalenable)
	signalid=$(uci -q get mesh.roam.signalid)
	log "roam enable = $signalenable"
	log "id = $signalid"
	
	ipaddr=$(uci -q get network.lan.ipaddr)
	
	uci set wireless.default_radio$radionum.disabled=$dedicated
	uci set wireless.radio$radionum.htmode=$cwidth
	uci set wireless.radio$radionum.channel=$channel
	
	count=0
	config_load wireless
	config_foreach count_radio wifi-iface
	
	uci set wireless.wmesh=wifi-iface
	uci set wireless.wmesh.device=radio$radionum
	uci set wireless.wmesh.network="mesh"
	uci set wireless.wmesh.ifname="if-mesh"
	uci set wireless.wmesh.mode="mesh"
	uci set wireless.wmesh.mesh_fwding="0"
	uci set wireless.wmesh.mesh_id=$networkid
	uci set wireless.w.encryption="none"
	if [ $netencrypted = "1" ]; then
		uci set wireless.wmesh.encryption="sae"
		uci set wireless.wmesh.key=$netpassword
	fi
	uci set wireless.wmesh.mesh_ttl='1'
    uci set wireless.wmesh.mcast_rate='24000'
    uci set wireless.wmesh.disabled='0'
	uci commit wireless
	
	uci set alfred.alfred.batmanif='bat0'
	uci set alfred.alfred.disabled='0'
	uci commit alfred
	
	uci set network.bat0=interface
	uci set network.bat0.proto='batadv'
	uci set network.bat0.routing_algo='BATMAN_IV'
	uci set network.bat0.aggregated_ogms='1'
	uci set network.bat0.ap_isolation='0'
	uci set network.bat0.bonding='0'
	uci set network.bat0.bridge_loop_avoidance='1'
	uci set network.bat0.distributed_arp_table='1'
	uci set network.bat0.fragmentation='1'
	uci set network.bat0.gw_mode='off'
	uci set network.bat0.hop_penalty='30'
	uci set network.bat0.isolation_mark='0x00000000/0x00000000'
	uci set network.bat0.log_level='0'
	uci set network.bat0.multicast_mode='1'
	uci set network.bat0.multicast_fanout='16'
	uci set network.bat0.network_coding='0'
	uci set network.bat0.orig_interval='1000'
	
	uci set network.mesh=interface
	uci set network.mesh.proto='batadv_hardif'
	uci set network.mesh.master='bat0'
	uci set network.mesh.mtu='2304'
	uci set network.mesh.throughput_override='0'
	
	uci set network.bat0_hardif_eth0=interface
	uci set network.bat0_hardif_eth0.proto='batadv_hardif'
	uci set network.bat0_hardif_eth0.master='bat0'
	uci set network.bat0_hardif_eth0.mtu='1536'
	uci set network.bat0_hardif_eth0.device='eth0'
	
	uci set network.bat0_lan=interface
	uci set network.bat0_lan.proto='static'
	uci set network.bat0_lan.ipaddr=$ipaddr
	uci set network.bat0_lan.netmask='255.255.255.0'
	uci set network.bat0_lan.ip6assign='60'
	uci set network.bat0_lan.device='bat0'
	uci commit network
}

cmd=$1
if [ -z $cmd ]; then
	WW=$(uci get wireless.wmesh)
	if [ -z $WW ]; then
		loadmesh
		reboot -f
	else
		loadconfig
		/usr/lib/mesh/ping.sh &
	fi
else
	loadmesh
fi

return
