#!/bin/sh
#
# Copyright 2016 by Imagination Technologies Limited and/or its affiliated group companies.
#
# All rights reserved.  No part of this software, either
# material or conceptual may be copied or distributed,
# transmitted, transcribed, stored in a retrieval system
# or translated into any human or computer language in any
# form by any means, electronic, mechanical, manual or
# other-wise, or disclosed to the third parties without the
# express written permission of Imagination Technologies
# Limited, Home Park Estate, Kings Langley, Hertfordshire,
# WD4 8LZ, U.K.
#
# common shell lib for mac generate functions

#PRODDATA=$(which proddata)
FILE=/etc/config/
WIFI_OTP_REG0=0
SOFTAP_OTP_REG0=1
BT_OTP_REG0=2
ETH_OTP_REG0=3
MESH_OTP_REG0=4
WPAN_OTP_REG0=5

read_otp() {
    local data_field=$1
    local data_value=$(which proddata read $data_field)
    if [ -z $data_value ]; then
        return 1
    else
        eval "$data_field=$data_value"
        return 0
    fi
}

read_mac_from_otp () {
    local mac_number=$1
    local mac_field="MAC_$mac_number"
    if read_otp $mac_field; then
        eval MAC=\$$mac_field
        MAC=$(echo $MAC | sed 's/../&:/g' | sed s'/.$//')
        echo "mac source: otp ($mac_field)"
        return 0
    else
        return 1
    fi
}

generate_mac_from_serial () {
    local mac_number=$1
    if read_otp "SERIAL"; then
        local var=$(echo $SERIAL | md5sum | cut -c1-9)
        MAC=$(echo "e2$mac_number$var" | sed 's/../&:/g' | sed s'/.$//')
        echo "mac source: serial number"
        return 0
    else
        return 1
    fi
}

read_mac_from_file () {
    local mac_number=$1
    local mac_field="MAC_$mac_number"
    local mac_file="$FILE$mac_field"
    if [ -f $mac_file ]; then
        MAC=$(cat $mac_file)
        echo "mac source: file"
        return 0
    fi
    return 1
}

generate_random_mac () {
    local mac_number=$1
    local mac_field="MAC_$mac_number"
    local mac_file="$FILE$mac_field"
    MAC=$(hexdump -n5 -e'/3 "E2" 5/1 ":%02X"' /dev/random)
    echo $MAC > $mac_file
    echo "mac source: random"
    return 0
}

generate_mac () {
    local mac_number=$1
    if read_mac_from_otp $mac_number || read_mac_from_file $mac_number || generate_mac_from_serial $mac_number || generate_random_mac $mac_number; then
        echo "MAC = $MAC"
        return 0
    else
        echo "Failed to generate mac"
        return 1
    fi
}
