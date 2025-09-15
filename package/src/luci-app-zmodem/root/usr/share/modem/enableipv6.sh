#!/bin/sh
# Use adb to add route on 5G module   @Icey 
PROGRAM="RM520N_Enable_IPV6"
printMsg() {
    local msg="$1"
    logger -t "${PROGRAM}" "${msg}"
} #日志输出调用API

adb_device=$(adb devices | awk 'NR>1 {print $1}' | head -n -1 | tr -d '\r\n')
unlock_code=$(uci get modem.@ndis[0].adbunlockkey) || unlock_code=0
enable_native_ipv6_flag=$(uci get modem.@ndis[0].enable_native_ipv6) || enable_native_ipv6_flag=0

unlock_module_adb() {
    printMsg "Begin Module ADB Unlock.."
    # Execute the unlock command and check the result
    unlock_result=$(sendat 2 'AT+QADBKEY="'$unlock_code'"')
    if echo "$unlock_result" | grep -q 'OK'; then
        # If unlock is successful, execute the next command
        usbcfg_result=$(sendat 2 'AT+QCFG="usbcfg"'|grep "+QCFG:"|tr -d '\r\n')
        # Extract vid and pid from the result
        vid=$(echo $usbcfg_result | cut -d ',' -f 2)
        pid=$(echo $usbcfg_result | cut -d ',' -f 3)
        # Construct and execute the sendat command
        sendat_command='AT+QCFG="usbcfg",'$vid','$pid',1,1,1,1,1,1,0'
        sendat_result=$(sendat 2 "$sendat_command")
        # Check if the result contains "ok"
        if echo "$sendat_result" | grep -q 'OK'; then
            # Reboot the module and system
            sendat 2 'at+cfun=1,1'
            reboot
        else
            printMsg "Error: usbcfg command did not return ok"
        fi
    else
        printMsg "Error: unlock code did not return ok"
    fi
}

ipv6_uci_set() {
    printMsg "Set UCI Conf"
    # /etc/config/dhcp
    uci set dhcp.wan6=dhcp
    uci set dhcp.wan6.interface='wan6'
    uci set dhcp.wan6.ignore='1'
    uci set dhcp.wan6.master='1'
    uci set dhcp.wan6.ra='relay'
    uci add_list dhcp.wan6.ra_flags='none'
    uci set dhcp.wan6.dhcpv6='relay'
    uci set dhcp.wan6.ndp='relay'
    # /etc/config/network
    uci set network.wan6.reqaddress='try'
    uci set network.wan6.reqprefix='auto'
    uci del network.globals.ula_prefix
    uci del dhcp.lan.ra_slaac
    uci set dhcp.lan.ra='relay'
    uci del dhcp.lan.ra_flags
    uci add_list dhcp.lan.ra_flags='none'
    uci set dhcp.lan.dhcpv6='relay'
    uci set dhcp.lan.ndp='relay'
    uci commit
}

route_walker() {
    ipv6=$(sendat 2 'at+qmap="wwan"' | grep IPV6 | awk -F\" '{print $6}')
    ipv6prefix=$(echo $ipv6 | cut -d: -f1-4)
    ipv6prefix="${ipv6prefix}::/64"

    if [ ! -e "/tmp/ipv6prefix" ]; then
        adb shell ip -6 route add $ipv6prefix dev bridge0 metric 100 pref medium
        echo "$ipv6prefix" > /tmp/ipv6prefix
        printMsg "Add route to module $ipv6prefix"
    else
        stored_ipv6prefix=$(cat /tmp/ipv6prefix)
        if [ "$stored_ipv6prefix" != "$ipv6prefix" ]; then
            adb shell ip -6 route add $ipv6prefix dev bridge0 metric 100 pref medium
            echo "$ipv6prefix" > /tmp/ipv6prefix
            printMsg "Update route to module,new is $ipv6prefix "
        else
            printMsg "no need to update IPv6 route"
            return 0
        fi
    fi

}

check_http_status() {
    attempt=0
    max_attempts=5
    sleep 30

    while [ $attempt -lt $max_attempts ]
    do
        http_status=$(curl -o /dev/null -s -w "%{http_code}" http://v6.ipip.net --connect-timeout 3)
        if [ $http_status -eq 200 ]
        then
            printMsg "IPV6 HTTP status is 200. Success!"
            route_walker
            return 0
        else
            printMsg "HTTP status chk FAILURE TRY AGAIN"
        fi
        attempt=$((attempt+1))
        #acc this！！
        if [ $attempt -lt $max_attempts ]
        then
            sleep 20
        fi
    done
    printMsg "IPV6 Connection FAILURE"
    echo "IPV6连接检测失败，请尝试拔掉电源等待至少5分钟后重新上电" >/tmp/ipv6prefix
    return 1
}



if [ -z "$adb_device" ]; then
    printMsg "Device not enable ADB"
    if [ "$unlock_code" = "0" ]; then
        printMsg "Nothing to do, just exit"
        return 0
    else
        unlock_module_adb
    fi
else
    printMsg "Device already enable ADB"
    if [ "$enable_native_ipv6_flag" -eq 1 ]; then
        wan6_set_chk="/etc/wan6set.lock"
        if [ ! -e "$wan6_set_chk" ]; then
            ipv6_uci_set
            touch $wan6_set_chk
            sleep 5
        fi
        check_http_status
    fi
fi
