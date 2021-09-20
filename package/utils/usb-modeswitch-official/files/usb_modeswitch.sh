#!/bin/sh

if [ "${ACTION}" = "add" ]; then

export PATH=/sbin:/usr/sbin:/bin:/usr/bin
FOLDER=/etc/usb_modeswitch.d
TOOL=/usr/sbin/usb_modeswitch

#echo "PRODUCT:   ${PRODUCT}" >> /dev/ttyS0
#echo "ACTION:    ${ACTION}" >> /dev/ttyS0
#echo "INTERFACE: ${INTERFACE}" >> /dev/ttyS0
#echo "DEVICE:    ${DEVICE}" >> /dev/ttyS0

VID=`echo $PRODUCT | awk '{split($0,p,"/" ); print p[1]}'`
PID=`echo $PRODUCT | awk '{split($0,p,"/" ); print p[2]}'`

LV=${#VID}
LP=${#PID}

if [ "$LV" = "3" ]; then
    VID="0"$VID
elif [ "$LV" = "2" ]; then
    VID="00"$VID
elif [ "$LV" = "1" ]; then
    VID="000"$VID    
fi

if [ "$LP" = "3" ]; then
    PID="0"$PID
elif [ "$LP" = "2" ]; then
    PID="00"$PID
elif [ "$LP" = "1" ]; then
    PID="000"$PID    
fi
#echo ${PRODUCT} $VID $PID >> /dev/ttyS0

FILES="$FOLDER/$VID:$PID"*
for FILE in $FILES; do
#echo $FILE >> /dev/ttyS0

if [ -f $FILE ]; then
    # echo "Config file existed." >> /dev/ttyS0
    TVID=`awk -F "=" '/TargetVendor/ {print $2}' $FILE`
    echo $TVID
    TPIDS=`awk -F "=" '/TargetProductList/ {print $2}' $FILE`
    echo $TPIDS
    MSG=`awk -F "=" '/MessageContent/ {print $2}' $FILE | sed -e 's/\"//g'`
    echo $MSG
    TPIDV=`echo $TPIDS | awk '{split($0,p,"," ); print p[1]}' | sed -e 's/\"//g'`
    echo $TPIDV
    # /usr/sbin/usb_modeswitch -v 12d1 -p 1446 -V 12d1 -P 140c -M 55534243123456780000000000000011062000000101000100000000000000
    # echo "$TOOL -v $VID -p $PID -V $TVID -P $TPIDV -M $MSG" >> /dev/ttyS0
    $TOOL -v $VID -p $PID -c $FILE
fi
done
fi
