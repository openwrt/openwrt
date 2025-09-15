#!/bin/sh
# by Icey
PROGRAM="AutoFreqLock"
printMsg() {
    local msg="$1"
    logger -t "${PROGRAM}" "${msg}"
} #日志输出调用API
killall autofreqlock.sh

networkMode=`sendat 2 'at+qeng="servingcell"'|grep "+QENG"|awk -F ',' '{print $3}'|tr -d '\r\n'`
printMsg "Network Mode $networkMode"
if [ -n "$networkMode" ]; then
    if echo "$networkMode" | grep -q "5G"; then
        #NR-5G
        earfcn=`sendat 2 'at+qeng="servingcell"'|grep "+QENG"|awk -F ',' '{print $10}'|tr -d '\r\n'`
        pci=`sendat 2 'at+qeng="servingcell"'|grep "+QENG"|awk -F ',' '{print $8}'|tr -d '\r\n'`
        band=`sendat 2 'at+qeng="servingcell"'|grep "+QENG"|awk -F ',' '{print $11}'`
        signalLevel=`sendat 2 'at+qeng="servingcell"'|grep "+QENG"|awk -F ',' '{print $13}'`
    elif echo "$networkMode" | grep -q "LTE"; then
        #LTE
        earfcn=`sendat 2 'at+qeng="servingcell"'|grep "+QENG"|awk -F ',' '{print $9}'|tr -d '\r\n'`
        pci=`sendat 2 'at+qeng="servingcell"'|grep "+QENG"|awk -F ',' '{print $8}'|tr -d '\r\n'`
        band=`sendat 2 'at+qeng="servingcell"'|grep "+QENG"|awk -F ',' '{print $10}'`
        signalLevel=`sendat 2 'at+qeng="servingcell"'|grep "+QENG"|awk -F ',' '{print $14}'`
    fi
fi
AutoLock() {
    if [ -n "$earfcn" ] && [ -n "$pci" ] && [ -n "$band" ] && [ -n "$signalLevel" ] && [ $signalLevel -gt -100 ]; then
        if echo "$networkMode" | grep -q "LTE"; then
            sendat 2 "at+qnwlock=\"common/4g\",1,$pci,$earfcn"
        elif echo "$networkMode" | grep -q "5G"; then
            case "$band" in
                1|2|3|5|7|8|12|20|25|28|66|71|75|76)
                    scs=15
                    ;;
                38|40|41|48|77|78|79)
                    scs=30
                    ;;
                257|258|260|261)
                    scs=120
                    ;;
                *)
                    printMsg "BANDLOCKFAILURE"
                    return 0
                    ;;
            esac
            sendat 2 "at+qnwlock=\"common/5g\",$pci,$earfcn,$scs,$band"
        fi
        sleep 3
        sendat 2 'at+qnwlock="save_ctrl",1,1'
        printMsg "Autolock Okay,Exit"
        printMsg "CellLockInfo,$earfcn $pci $band $signalLevel"
        exit 0
    else
        printMsg "Error,$earfcn $pci $band $signalLevel"
        echo "Error,$earfcn $pci $band $signalLevel"
        unlock
        sleep 600
        AutoLock
    fi
}

unlock() {
    sendat 2 'at+qnwlock="common/5g",0'
    sleep 2
    sendat 2 'at+qnwlock="common/4g",0'
    sleep 2
}

printMsg "Unlock Band If Exist..."
AutoLock

