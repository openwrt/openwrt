#!/bin/sh

OPTIMIZED_FOR="$1"
CPU_LIST=`cat /proc/interrupts | sed -n '1p'`
NUM_OF_CPU=0; for i in $CPU_LIST; do NUM_OF_CPU=`expr $NUM_OF_CPU + 1`; done;
DEFAULT_RPS=0

. /lib/functions.sh

RPS_IF_LIST=""
NUM_WIFI_CARD=0
WIFI_RADIO1=0
WIFI_RADIO2=0
WIFI_RADIO3=0
WED_ENABLE=0

WIFI_MODULE_LIST='mt7915e mt7996e'

get_if_info()
{
	# try to get all wifi and eth net interface.
	dbg2 "try to get all wifi and eth net interface."
	NET_IF_LIST=`ls /sys/class/net`
	for vif in $NET_IF_LIST;
	do
		if [[ "$vif" == "eth"* ]] ||  \
		[[ "$vif" == "lan"* ]] || [[ "$vif" == "wan"* ]] || \
		[[ "$vif" == "wlan"* ]] || [[ "$vif" == "phy"* ]]; then
			RPS_IF_LIST="$RPS_IF_LIST $vif"
		fi
	done;
	dbg2 "RPS_IF_LIST = $RPS_IF_LIST"

	# try to get wifi physical card num.
	dbg2 "try to get wifi physical card num."
	VIRTUAL_RADIO_LIST=`ls -l /sys/class/ieee80211/ | awk -F 'devices' '{print $2}' | awk -F 'phy' '{print $1}' | uniq`
	for v in $VIRTUAL_RADIO_LIST;
	do
		NUM_WIFI_CARD=`expr $NUM_WIFI_CARD + 1`
		dbg2 "physical raido $v"
		if [[ $v == *"wmac"* ]]; then
			WIFI_RADIO1=1
		fi

		if [[ $v == *"wbsys"* ]]; then
			WIFI_RADIO1=1
		fi

		if [[ $v == *"pci0000"* ]]; then
			WIFI_RADIO2=1
		fi

		if [[ $v == *"pci0001"* ]]; then
			WIFI_RADIO3=1
		fi
	done;


	for v in $WIFI_MODULE_LIST;
	do
		if [[ -f "/sys/module/$v/parameters/wed_enable" ]]; then
			WED_ENABLE_LIST=`cat /sys/module/$v/parameters/wed_enable`
			dbg2 "wed enable ori info $v $WED_ENABLE_LIST"
			if [[ $WED_ENABLE_LIST == "Y" ]]; then
				WED_ENABLE=1
			fi
		fi
	done;
	dbg2 "NUM_WIFI_CARD = $NUM_WIFI_CARD"
	dbg2 "platform wifi enable = $WIFI_RADIO1"
	dbg2 "pcie1 wifi enable = $WIFI_RADIO2"
	dbg2 "pcie2 wifi enable = $WIFI_RADIO3"
	dbg2 "WED enable = $WED_ENABLE"
}

# $1: CPU#
# $2: irq list for added.
CPU_AFFINITY_ADD()
{
	eval oval=\$CPU${1}_AFFINITY
	eval CPU${1}_AFFINITY=\"\$CPU${1}_AFFINITY $2\"
}

# $1: CPU#
# $2: Interface name for added.
CPU_RPS_ADD()
{
	eval oval=\$CPU${1}_RPS
	eval CPU${1}_RPS=\"\$CPU${1}_RPS $2\"
	dbg2 "CPU${1}_RPS=\"\$CPU${1}_RPS $2\""
}

MT7988()
{
	num_of_wifi=$1
	DEFAULT_RPS=0

	#Physical IRQ# setting
	#Ethernet RSS feature enables 4 Rx rings
	eth_irq_rx0=221
	eth_irq_rx1=222
	eth_irq_rx2=223
	eth_irq_rx3=224
	eth_irq_tx=229
	wifi1_irq_pcie0=524288
	wifi1_irq_pcie1=134742016
	wifi2_irq_pcie0=
	wifi2_irq_pcie1=

	if [[ "$WED_ENABLE" -eq "1" ]]; then
		dbg2 "WED_ENABLE ON irq/iptable setting"
		#TCP Binding
		iptables -D FORWARD -p tcp -m conntrack --ctstate RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		iptables -I FORWARD -p tcp -m conntrack --ctstate RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		ip6tables -D FORWARD -p tcp -m conntrack --ctstate RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		ip6tables -I FORWARD -p tcp -m conntrack --ctstate RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		#UDP Binding
		iptables -D FORWARD -p udp -j FLOWOFFLOAD --hw
		iptables -I FORWARD -p udp -j FLOWOFFLOAD --hw
		ip6tables -D FORWARD -p udp -j FLOWOFFLOAD --hw
		ip6tables -I FORWARD -p udp -j FLOWOFFLOAD --hw
		#Multicast skip Binding
		iptables -D FORWARD -m pkttype --pkt-type multicast -j ACCEPT
		iptables -I FORWARD -m pkttype --pkt-type multicast -j ACCEPT
		ip6tables -D FORWARD -m pkttype --pkt-type multicast -j ACCEPT
		ip6tables -I FORWARD -m pkttype --pkt-type multicast -j ACCEPT

	else
		dbg2 "WED_ENABLE OFF irq/iptable seting"
	fi

	for vif in $NET_IF_LIST;
	do
		if [[ "$vif" == "wlan"* ]] || [[ "$vif" == "phy"* ]]; then
			WIFI_IF_LIST="$WIFI_IF_LIST $vif"
		fi
	done;
	dbg2 "$WIFI_IF_LIST = $WIFI_IF_LIST"
	# Please update the CPU binding in each cases.
	# CPU#_AFFINITY="add binding irq number here"
	# CPU#_RPS="add binding interface name here"
	if [ "$num_of_wifi" = "0" ]; then
		CPU0_AFFINITY="$eth_irq_rx0 $eth_irq_tx"
		CPU1_AFFINITY="$eth_irq_rx1"
		CPU2_AFFINITY="$eth_irq_rx2"
		CPU3_AFFINITY="$eth_irq_rx3"

		CPU0_RPS="$RPS_IF_LIST"
		CPU1_RPS="$RPS_IF_LIST"
		CPU2_RPS="$RPS_IF_LIST"
		CPU3_RPS="$RPS_IF_LIST"
	else
		#we bound all wifi card to cpu0 and bound eth to cpu
		CPU0_AFFINITY=""
		CPU1_AFFINITY=""
		CPU2_AFFINITY="$eth_irq_rx0 $eth_irq_rx1 $eth_irq_tx"
		CPU3_AFFINITY="$eth_irq_rx2 $eth_irq_rx3"

		CPU0_RPS="$WIFI_IF_LIST"
		CPU1_RPS="$WIFI_IF_LIST"
		CPU2_RPS=""
		CPU3_RPS=""
	fi
	dbg2 "CPU0_AFFINITY = $CPU0_AFFINITY"
	dbg2 "CPU1_AFFINITY = $CPU1_AFFINITY"
	dbg2 "CPU2_AFFINITY = $CPU2_AFFINITY"
	dbg2 "CPU3_AFFINITY = $CPU3_AFFINITY"
}

MT7986()
{
	num_of_wifi=$1
	DEFAULT_RPS=0

	#Physical IRQ# setting
	#Ethernet RSS feature enables 4 Rx rings
	eth_irq_rx0=221
	eth_irq_rx1=222
	eth_irq_rx2=223
	eth_irq_rx3=224
	eth_irq_tx=229
	wifi1_irq=
	wifi2_irq=
	wifi3_irq=

	if [[ "$WED_ENABLE" -eq "1" ]]; then
		dbg2 "WED_ENABLE ON irq/iptable setting"
		#TCP Binding
		iptables -D FORWARD -p tcp -m conntrack --ctstate RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		iptables -I FORWARD -p tcp -m conntrack --ctstate RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		ip6tables -D FORWARD -p tcp -m conntrack --ctstate RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		ip6tables -I FORWARD -p tcp -m conntrack --ctstate RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		#UDP Binding
		iptables -D FORWARD -p udp -j FLOWOFFLOAD --hw
		iptables -I FORWARD -p udp -j FLOWOFFLOAD --hw
		ip6tables -D FORWARD -p udp -j FLOWOFFLOAD --hw
		ip6tables -I FORWARD -p udp -j FLOWOFFLOAD --hw
		#Multicast skip Binding
		iptables -D FORWARD -m pkttype --pkt-type multicast -j ACCEPT
		iptables -I FORWARD -m pkttype --pkt-type multicast -j ACCEPT
		ip6tables -D FORWARD -m pkttype --pkt-type multicast -j ACCEPT
		ip6tables -I FORWARD -m pkttype --pkt-type multicast -j ACCEPT

		#AX6000 AX7800 - SOC
		if [[ "$WIFI_RADIO1" -eq "1" ]]; then
			wifi1_irq=238
		fi
		#AX7800 - PCIE0
		if [[ "$WIFI_RADIO2" -eq "1" ]]; then
			wifi2_irq=237
		fi
		#AX7800 - PCIE1
		#if [[ "$WIFI_RADIO3" -eq "1" ]]; then
		#	wifi3_irq=239
		#fi
	else
		dbg2 "WED_ENABLE OFF irq/iptable seting"
		#AX6000 AX7800 - SOC
		if [[ "$WIFI_RADIO1" -eq "1" ]]; then
			wifi1_irq=245
		fi
		#AX7800 - PCIE0
		if [[ "$WIFI_RADIO2" -eq "1" ]]; then
			wifi2_irq=246
		fi
		#AX7800 - PCIE1
		#if [[ "$WIFI_RADIO3" -eq "1" ]]; then
		#	wifi3_irq=247
		#fi
	fi

	for vif in $NET_IF_LIST;
	do
		if [[ "$vif" == "wlan"* ]] || [[ "$vif" == "phy"* ]]; then
			WIFI_IF_LIST="$WIFI_IF_LIST $vif"
		fi
	done;
	dbg2 "$WIFI_IF_LIST = $WIFI_IF_LIST"
	# Please update the CPU binding in each cases.
	# CPU#_AFFINITY="add binding irq number here"
	# CPU#_RPS="add binding interface name here"
	if [ "$num_of_wifi" = "0" ]; then
		CPU0_AFFINITY="$eth_irq_rx0"
		CPU1_AFFINITY="$eth_irq_rx1 $eth_irq_tx"
		CPU2_AFFINITY="$eth_irq_rx2"
		CPU3_AFFINITY="$eth_irq_rx3"

		CPU0_RPS=""
		CPU1_RPS=""
		CPU2_RPS=""
		CPU3_RPS=""
	else
		#we bound all wifi card to cpu1 and bound eth to cpu0
		CPU0_AFFINITY="$eth_irq_rx0"
		CPU1_AFFINITY="$eth_irq_rx1 $eth_irq_tx"
		CPU2_AFFINITY="$eth_irq_rx2 $wifi2_irq $wifi3_irq"
		CPU3_AFFINITY="$eth_irq_rx3 $wifi1_irq"

		CPU0_RPS="$WIFI_IF_LIST"
		CPU1_RPS="$WIFI_IF_LIST"
		CPU2_RPS="$WIFI_IF_LIST"
		CPU3_RPS="$WIFI_IF_LIST"
	fi
	dbg2 "CPU0_AFFINITY = $CPU0_AFFINITY"
	dbg2 "CPU1_AFFINITY = $CPU1_AFFINITY"
	dbg2 "CPU2_AFFINITY = $CPU2_AFFINITY"
	dbg2 "CPU3_AFFINITY = $CPU3_AFFINITY"
}

MT7981()
{
	num_of_wifi=$1
	DEFAULT_RPS=0

	#Physical IRQ# setting
	eth_irq_rx=221
	eth_irq_tx=229
	wifi1_irq=
	wifi2_irq=
	wifi3_irq=

	#AX3000
	if [[ "$WED_ENABLE" -eq "1" ]]; then
		dbg2 "WED_ENABLE ON irq/iptable setting"
		#TCP Binding
		iptables -D FORWARD -p tcp -m conntrack --ctstate	\
				RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		iptables -I FORWARD -p tcp -m conntrack --ctstate	\
				RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		ip6tables -D FORWARD -p tcp -m conntrack --ctstate	\
				RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		ip6tables -I FORWARD -p tcp -m conntrack --ctstate	\
				RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
		#UDP Binding
		iptables -D FORWARD -p udp -j FLOWOFFLOAD --hw
		iptables -I FORWARD -p udp -j FLOWOFFLOAD --hw
		ip6tables -D FORWARD -p udp -j FLOWOFFLOAD --hw
		ip6tables -I FORWARD -p udp -j FLOWOFFLOAD --hw
		#Multicast skip Binding
		iptables -D FORWARD -m pkttype --pkt-type multicast -j ACCEPT
		iptables -I FORWARD -m pkttype --pkt-type multicast -j ACCEPT
		ip6tables -D FORWARD -m pkttype --pkt-type multicast -j ACCEPT
		ip6tables -I FORWARD -m pkttype --pkt-type multicast -j ACCEPT

		if [[ "$WIFI_RADIO1" -eq "1" ]]; then
			wifi1_irq=237
		fi
	else
		if [[ "$WIFI_RADIO1" -eq "1" ]]; then
			wifi1_irq=245
		fi
	fi

	# Please update the CPU binding in each cases.
	# CPU#_AFFINITY="add binding irq number here"
	# CPU#_RPS="add binding interface name here"
	if [ "$num_of_wifi" = "0" ]; then
		CPU0_AFFINITY="$eth_irq_rx"
		CPU1_AFFINITY="$eth_irq_tx"

		CPU0_RPS="$RPS_IF_LIST"
		CPU1_RPS="$RPS_IF_LIST"
	else
		#we bound all wifi card to cpu0 and bound eth to cpu1
		CPU0_AFFINITY="$wifi1_irq $wifi2_irq $wifi3_irq"
		CPU1_AFFINITY="$eth_irq_rx $eth_irq_tx"

		CPU0_RPS="$RPS_IF_LIST"
		CPU1_RPS="$RPS_IF_LIST"
	fi
	dbg2 "CPU0_AFFINITY = $CPU0_AFFINITY"
	dbg2 "CPU1_AFFINITY = $CPU1_AFFINITY"
}

MT7622()
{
	num_of_wifi=$1
	DEFAULT_RPS=0

	#Physical IRQ# setting
	eth0_irq=224
	eth1_irq=225
	wifi1_irq=
	wifi2_irq=
	wifi3_irq=
	#AX1200 AX3200
	if [[ "$WIFI_RADIO1" -eq "1" ]]; then
		wifi1_irq=211
	fi
	#AX1800 AX3200
	if [[ "$WIFI_RADIO2" -eq "1" ]]; then
		wifi2_irq=214
	fi
	#AX3600
	if [[ "$WIFI_RADIO3" -eq "1" ]]; then
		wifi3_irq=215
	fi

	# Please update the CPU binding in each cases.
	# CPU#_AFFINITY="add binding irq number here"
	# CPU#_RPS="add binding interface name here"
	if [ "$num_of_wifi" == "0" ]; then
		CPU0_AFFINITY="$eth0_irq"
		CPU1_AFFINITY="$eth1_irq"

		CPU0_RPS="$RPS_IF_LIST"
		CPU1_RPS="$RPS_IF_LIST"
	else
		#we bound all wifi card to cpu0 and bound eth to cpu1
		CPU0_AFFINITY="$wifi1_irq $wifi2_irq $wifi3_irq"
		CPU1_AFFINITY="$eth0_irq $eth1_irq"

		CPU0_RPS="$RPS_IF_LIST"
		CPU1_RPS="$RPS_IF_LIST"
	fi

	dbg2 "CPU0_AFFINITY = $CPU0_AFFINITY"
	dbg2 "CPU1_AFFINITY = $CPU1_AFFINITY"
}

setup_model()
{
	board=$(board_name)
	num_of_wifi=$NUM_WIFI_CARD

	if [[ $board == *"7988"* ]]; then
		dbg "setup_model: MT7988 NUM_WIFI_CARD=$num_of_wifi"
		MT7988 $num_of_wifi
	elif [[ $board == *"7986"* ]]; then
		dbg "setup_model: MT7986 NUM_WIFI_CARD=$num_of_wifi"
		MT7986 $num_of_wifi
	elif [[ $board == *"7981"* ]]; then
		dbg "setup_model: MT7981 NUM_WIFI_CARD=$num_of_wifi"
		MT7981 $num_of_wifi
	elif [[ $board == *"7622"* ]]; then
		dbg "setup_model: MT7622 NUM_WIFI_CARD=$num_of_wifi"
		MT7622 $num_of_wifi
	elif [[ $board == *"bpi-r4"* ]]; then
		dbg "setup_model: bpi-r4 NUM_WIFI_CARD=$num_of_wifi"
		MT7988 $num_of_wifi
	fi
}

get_virtual_irq()
{
	PHY_POS=`expr $NUM_OF_CPU + 3` #physical irq # position in /proc/interrups may vary with the number of CPU up
	target_phy_irq=$1
	cat /proc/interrupts | sed 's/:/ /g'| awk '$1 ~ /^[0-9]+$/' | while read line
	do
		set -- $line
		phy_irq=$(eval "echo \$$PHY_POS")
		if [ $phy_irq == $target_phy_irq ]; then
			echo $1
			return
		fi
	done
}

set_rps_cpu_bitmap()
{
	dbg2 "# Scan binding interfaces of each cpu"
	# suppose the value of interface_var is null or hex
	num=0
	while [ "$num" -lt "$NUM_OF_CPU" ];do
		cpu_bit=$((2 ** $num))
		eval rps_list=\$CPU${num}_RPS
		dbg2 "# CPU$num: rps_list=$rps_list"
		for i in $rps_list; do
			var=${VAR_PREFIX}_${i//-/_}
			eval ifval=\$$var
			dbg2 "[var val before] \$$var=$ifval"
			if [ -z "$ifval" ]; then
				eval $var=$cpu_bit
			else
				eval $var=`expr $ifval + $cpu_bit`
			fi
			eval ifval=\$$var
			dbg2 "[rps val after]$i=$ifval"
		done
		num=`expr $num + 1`
	done
}

# $1: The default rps value. If rps of the interface is not setup, set $1 to it
set_rps_cpus()
{
	dbg2 "# Setup rps of the interfaces, $RPS_IF_LIST."
	for i in $RPS_IF_LIST; do
		var=${VAR_PREFIX}_${i//-/_}
		eval cpu_map=\$$var
		if [ -d /sys/class/net/$i ]; then
			if [ ! -z $cpu_map ]; then
				cpu_map=`printf '%x' $cpu_map`
				dbg "echo $cpu_map > /sys/class/net/$i/queues/rx-0/rps_cpus"
				echo $cpu_map > /sys/class/net/$i/queues/rx-0/rps_cpus
			elif [ ! -z $1 ]; then
				dbg "echo $1 > /sys/class/net/$i/queues/rx-0/rps_cpus"
				echo $1 > /sys/class/net/$i/queues/rx-0/rps_cpus
			fi
		fi
	done
}

set_smp_affinity()
{
	dbg2 "# Setup affinity of each physical irq."
	num=0
	while [ "$num" -lt "$NUM_OF_CPU" ];do
		eval smp_list=\$CPU${num}_AFFINITY
		for i in $smp_list; do
			cpu_bit=$((2 ** $num))
			virq=$(get_virtual_irq $i)
			#virq=$i
			dbg2 "irq p2v $i --> $virq"
			if [ ! -z $virq ]; then
				dbg "echo $cpu_bit > /proc/irq/$virq/smp_affinity"
				echo $cpu_bit > /proc/irq/$virq/smp_affinity
			fi
		done
		num=`expr $num + 1`
	done
}

if [ "$1" = "dbg" ]; then
	DBG=1
elif [ "$1" = "dbg2" ]; then
	DBG=2
else
	DBG=0
fi

# Usage: dbg "the output string"
dbg()
{
	if [ "$DBG" -ge "1" ]; then
		echo -e $1
	fi
}

# Usage: dbg2 "the output string"
dbg2()
{
	if [ "$DBG" -ge "2" ]; then
		echo -e $1
	fi
}

dbg "# RPS and AFFINITY Setting"
dbg "# NUM_OF_CPU=$NUM_OF_CPU"
VAR_PREFIX="autogen"
get_if_info
setup_model
set_rps_cpu_bitmap
set_rps_cpus $DEFAULT_RPS
set_smp_affinity
#end of file
