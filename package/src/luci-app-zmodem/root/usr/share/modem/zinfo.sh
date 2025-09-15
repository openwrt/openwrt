#!/bin/sh 

LOCK_FILE="/tmp/zinfo.lock"
if [ -e "$LOCK_FILE" ]; then
    echo "zinfo互斥" >> /tmp/rm520n.log
    exit 1
fi
touch "$LOCK_FILE"
#二次判断，避免获取信号状态和模块设置干扰
waitrm520n=$(ps -fe|grep rm520n.sh |grep -v grep)
if [ "$waitrm520n" != "" ]
then
    if [ -e "/tmp/cpe_cell.file" ]; then
    sed -i '14,26 s/.*/Loading.../g' /tmp/cpe_cell.file  
    fi
    rm -rf "$LOCK_FILE"
    exit 1
fi
#3次判断，避免获取信号状态和基站扫描干扰
waitCellScan=$(ps -fe|grep keyPairCellScan.sh |grep -v grep)
if [ "$waitCellScan" != "" ]
then
    rm -rf "$LOCK_FILE"
    exit 1
fi

# 判断第三行文本并执行相应操作
third_line=$(sendat 2 'ATI' | sed -n '3p')
if [ -n "$third_line" ]; then
    if echo "$third_line" | grep -q 'RM520'; then
        source /usr/share/modem/Quectel       
    elif echo "$third_line" | grep -q 'RM500U'; then
        source /usr/share/modem/RM500X
    fi
else
    echo "第三行文本为空或不包含指定内容"
fi
#source /usr/share/modem/Quectel

sim_sel=$(cat /tmp/sim_sel)
SIMCard=""

case $sim_sel in
    0)
        SIMCard="外置SIM卡"
        ;;
    1)
        SIMCard="内置SIM1"
        ;;
    2)
        SIMCard="内置SIM2"
        ;;
    *)
        SIMCard="SIM状态错误"
        ;;
esac


SIM_Check=$(sendat 2 AT+CPIN?)
if [ -z "$(echo "$SIM_Check" | grep "READY")" ]; then
    {    
    echo `sendat 2 "ATI" | sed -n '3p'|sed 's/\r$//'` #'RM520N-CN'
    echo `sendat 2 "ATI" | sed -n '2p'|sed 's/\r$//'` #'Quectel'
    echo `date "+%Y-%m-%d %H:%M:%S"`
    echo ''
    echo "未检测到SIM卡!"
    echo -e "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    } > /tmp/cpe_cell.file
    rm -rf "$LOCK_FILE"
    exit
fi

InitData(){
    Date=''
	CHANNEL="-" 
	ECIO="-"
	RSCP="-"
	ECIO1=" "
	RSCP1=" "
	NETMODE="-"
	LBAND="-"
	PCI="-"
	CTEMP="-"
	MODE="-"
	SINR="-"
	IMEI='-'
	IMSI='-'
	ICCID='-'
	phone='-'
	conntype=''
	Model=''

}

OutData(){
    {
    echo `sendat 2 "ATI" | sed -n '2p'|sed 's/\r$//'` #'Quectel'
    echo `sendat 2 "ATI" | sed -n '3p'|sed 's/\r$//'` #'RM520N-CN'
    echo `sendat 2 "ATI" | sed -n '4p' | cut -d ':' -f2 | tr -d ' '|sed 's/\r$//'` #'RM520NCNAAR03A03M4G
    echo "$CTEMP" # 设备温度 41°C
    echo `date "+%Y-%m-%d %H:%M:%S"` # 时间
    #----------------------------------
    echo "$SIMCard" # 卡槽
    echo "$ISP" #运营商
    echo "$IMEI" #imei
    echo "$IMSI" #imsi
    echo `sendat 2 AT+QCCID | awk -F': ' '/\:/{print $2}'|sed 's/\r$//'` #iccid
    echo `sendat 2 AT+CNUM | grep "+CNUM:" | sed 's/.*,"\(.*\)",.*/\1/'|sed 's/\r$//'` #phone
    #-----------------------------------
    echo "$MODE" #蜂窝网络类型 NR5G-SA "TDD"
    echo "$CSQ_PER" #CSQ_PER 信号质量
    echo "$CSQ_RSSI" #信号强度 RSSI 信号强度
    echo "$ECIO dB" #接收质量 RSRQ 
    echo "$RSCP dBm" #接收功率 RSRP
    echo "$SINR" #信噪比 SINR  rv["sinr"]
    #-----------------------------------
    echo "$COPS_MCC /$COPS_MNC" #MCC / MNC
    echo "$LAC"  #位置区编码
    echo "$CID"  #小区基站编码
    echo "$LBAND" # 频段 频宽
    echo "$CHANNEL" # 频点
    echo "$PCI" #物理小区标识   
    AMBR #基站速率和接入点！
    echo "$apn"
    echo "$DOWNspeed""mbps"
    echo "$UPspeed""mbps"
    } > /tmp/cpe_cell.file
}
AMBR(){
    rm -rf "$LOCK_FILE"
    if [ -n "$QENG5" ]; then
        AMB=$(sendat 2 'AT+qnwcfg="NR5G_AMBR"' | sed -n '2p')
        if [[ "$AMB" =~ "IMS" || "$AMB" =~ "ims" ]]
        then
            AMB=$(sendat 2 'AT+qnwcfg="NR5G_AMBR"' | sed -n '3p')
            apn=$(echo $AMB | cut -d, -f2)
            apn=${apn:1:-1}
            DOWNspeed=$(echo $AMB | cut -d, -f4)
            UPspeed=$(($(echo $AMB | cut -d, -f6))) 
        else
            apn=$(echo $AMB | cut -d, -f2)
            apn=${apn:1:-1}
            DOWNspeed=$(echo $AMB | cut -d, -f4)
            UPspeed=$(($(echo $AMB | cut -d, -f6)))
        fi
    else
        AMB=$(sendat 2 'AT+qnwcfg="lte_ambr"' | sed -n '2p')
        if [[ "$AMB" =~ "IMS" || "$AMB" =~ "ims" ]]
        then
            AMB=$(sendat 2 'AT+qnwcfg="lte_ambr"' | sed -n '3p')
            apn=$(echo $AMB | cut -d, -f2)
            apn=${apn:1:-1}
            DOWNspeed=$(echo $AMB | cut -d, -f3)
            DOWNspeed=`expr $DOWNspeed / 1000`             
            UPspeed=$(echo $AMB | cut -d, -f4)
            UPspeed=$(($UPspeed / 1000))
        else
            apn=$(echo $AMB | cut -d, -f2)
            apn=${apn:1:-1}
            DOWNspeed=$(echo $AMB | cut -d, -f3)
            DOWNspeed=`expr $DOWNspeed / 1000`
            UPspeed=$(echo $AMB | cut -d, -f4)
            UPspeed=$(($UPspeed / 1000))
        fi  
    fi
}

InitData
Quectel_AT
ISP=""
case $COPS in
    "CHN-CT")
        ISP="中国电信"
        ;;
    "CHN-UNICOM")
        ISP="中国联通"
        ;;
    "CHINA MOBILE")
        ISP="中国移动"
        ;;
    *)
        ISP="$COPS"
        ;;
esac
OutData
rm -rf "$LOCK_FILE"
exit 1
