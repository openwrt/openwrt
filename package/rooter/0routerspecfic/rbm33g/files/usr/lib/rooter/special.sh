#!/bin/sh 
. /lib/functions.sh

VL=0
do_vlan() {
	local config=$1
	config_get ports $1 ports
	if [ "$ports" = "1 2 6t" ]; then
		uci set network."$config".ports="0 1 6t"
		VL=1
	fi
	if [ "$ports" = "0 6t" ]; then
		uci set network."$config".ports="2 6t"
		VL=1
	fi
}

if [ ! -f /etc/rbm33 ]; then
	config_load network
	config_foreach do_vlan switch_vlan

	if [ $VL -eq 1 ]; then
		uci commit network
		/etc/init.d/network restart
	fi
	echo "0" > /etc/rbm33
fi 

echo "1" > /sys/class/gpio/gpio9/value
sleep 1
echo "1" > /sys/class/gpio/gpio10/value
sleep 1
echo "1" > /sys/class/gpio/gpio12/value

# 1 Check USB Devices, Rev=0.00 is probably a boothold device, awk reverses the line order
var="`cat /sys/kernel/debug/usb/devices | grep -E '^T:|^P:|^C:' | grep -E 'Rev= 0.00$' -C1 | awk '{a[i++]=$0} END {for (j=i-1; j>=0;) print a[j--] }'`"
while read -r line; do
    case $line in 'T:  Bus='*)
        if [ $ProdID ] && [ $Vendor ]; then
            BPort="`echo $line | awk -F'[ =]' '{print $3$9}'`"
            case $BPort in
                '0101') GPIO_PIN=gpio9; ;; # pcie0
                '0100') GPIO_PIN=gpio10; ;; # pcie1/USB in USB 2.0 mode
                '0200') GPIO_PIN=gpio12; ;; # USB in USB 3.0 mode
                *) unset GPIO_PIN; ;;
            esac;
            if [ $GPIO_PIN ]; then
                echo "Modem in BOOTHOLD!" > /dev/kmsg
                echo "0" > /sys/class/gpio/$GPIO_PIN/value
                echo "1" > /sys/class/gpio/$GPIO_PIN/value
                echo "Toggled GPIO $GPIO_PIN" > /dev/kmsg
                unset GPIO_PIN
            fi
        fi
    esac
    unset Vendor
    unset ProdID
    case $line in 'P:  Vendor='*)
        if [ $trigger -eq 1 ]; then
            # 3 add logic to check against VID/PID from list
            Vendor=`echo $line | awk -F'[ =]' '{print $3}'`
            ProdID=`echo $line | awk -F'[ =]' '{print $5}'`
            trigger=0
        fi
    esac
    case $line in 'C:* #Ifs= 1'*)
        # 2 Found a device with only one interface, so we'll assume is a boothold modem for now
        trigger=1
    esac
done <<EOF
$var
EOF