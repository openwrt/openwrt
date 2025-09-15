    #!/bin/sh
    #By Zy143L
    #Icey:add module test function
    echo "SIM INIT..." >/tmp/simcardstat
    PROGRAM="RM520N_MODEM"
    printMsg() {
        local msg="$1"
        logger -t "${PROGRAM}" "${msg}"
    } #日志输出调用API

    # 检查是否存在锁文件 @Icey
    lock_file="/tmp/rm520n.lock"

    if [ -e "$lock_file" ]; then
    # 锁文件存在，获取锁定的进程 ID，并终止它
    locked_pid=$(cat "$lock_file")
    if [ -n "$locked_pid" ]; then
        echo "Terminating existing rm520n.sh process (PID: $locked_pid)" 
        kill "$locked_pid"
        sleep 2  # 等待一段时间确保进程终止
    fi
    fi

    ipcheckLOCKFILE="/tmp/ipcheck.lock"
    if [ -e "$ipcheckLOCKFILE" ]; then
    # 锁文件存在，获取锁定的进程 ID，并终止它
    ipcheckLOCKFILElocked_pid=$(cat "$ipcheckLOCKFILE")
    if [ -n "$ipcheckLOCKFILElocked_pid" ]; then
        printMsg "Terminating existing ipcheck.sh process (PID: $ipcheckLOCKFILElocked_pid)" 
        kill "$ipcheckLOCKFILElocked_pid"
        sleep 2  # 等待一段时间确保进程终止
    fi
    fi


    # 创建新的锁文件，记录当前进程 ID
    echo "$$" > "$lock_file"
    sleep 2 && /sbin/uci commit
    Modem_Enable=`uci -q get modem.@ndis[0].enable` || Modem_Enable=1
    #模块启动
    #模块开关
    if [ "$Modem_Enable" == 0 ]; then
        echo 0 >/sys/class/gpio/cpe-pwr/value
        printMsg "禁用模块，退出"
        rm $lock_file
        exit 0
    else
        printMsg "模块启用"
        echo 1 >/sys/class/gpio/cpe-pwr/value
        echo 1 > /sys/class/gpio/cpe-sel0/value
    fi

    Sim_Sel=`uci -q get modem.@ndis[0].simsel`|| Sim_Sel=0
    echo "simsel: $Sim_Sel" >> /tmp/moduleInit
    #SIM选择

    Enable_IMEI=`uci -q get modem.@ndis[0].enable_imei` || Enable_IMEI=0
    #IMEI修改开关

    RF_Mode=`uci -q get modem.@ndis[0].smode` || RF_Mode=0
    #网络制式 0: Auto, 1: 4G, 2: 5G
    NR_Mode=`uci -q get modem.@ndis[0].nrmode` || NR_Mode=0
    #0: Auto, 1: SA, 2: NSA
    Band_LTE=`uci -q get modem.@ndis[0].bandlist_lte` || Band_LTE=0
    Band_SA=`uci -q get modem.@ndis[0].bandlist_sa` || Band_SA=0
    Band_NSA=`uci -q get modem.@ndis[0].bandlist_nsa` || Band_NSA=0
    #Enable_PING=`uci -q get modem.@ndis[0].pingen` || Enable_PING=0
    PING_Addr=`uci -q get modem.@ndis[0].pingaddr` || PING_Addr="119.29.29.29"
    PING_Count=`uci -q get modem.@ndis[0].count` || PING_Count=10
    #FCN CI LOCK
    Earfcn=`uci -q get modem.@ndis[0].earfcn` || Earfcn=0
    Cellid=` uci -q get modem.@ndis[0].cellid` || Cellid=0
    Freqlock=` uci -q get modem.@ndis[0].freqlock` || Freqlock=0
    switchNetwork=` uci -q get modem.@ndis[0].switchNetwork` || switchNetwork=0
    RF_Mode2=`uci -q get modem.@ndis[0].smode2` || RF_Mode2=0
    Autoswitchtime=` uci -q get modem.@ndis[0].Autoswitchtime` || Autoswitchtime="00-07"

    #if [ ${Enable_PING} == 1 ];then
        #/usr/share/modem/pingCheck.sh &
    #else 
        #process=`ps -ef | grep "pingCheck" | grep -v grep | awk '{print $1}'` 
        #if [[ -n "$process" ]]; then
            #kill -9 "$process" >/dev/null 2>&1
        #fi
        #rm -rf /tmp/pingCheck.lock
    #fi

     #-----------------SIM Card switch
     #attention！ims enable and autosel enable will make some card work under 4G network
        case "$Sim_Sel" in
            0)
                printMsg "外置SIM卡"
                sendat 2 "AT+QUIMSLOT=1"
                echo 1 > /etc/simsel
                sleep 2
                sendat 2 'AT+QCFG="ims",1'
                sendat 2 'AT+QMBNCFG="autosel",1' 
                echo "外置SIM卡" >> /tmp/moduleInit
                echo 0 > /tmp/sim_sel
            ;;
            1)
                printMsg "内置SIM1"
                echo 1 > /sys/class/gpio/cpe-sel0/value
                sendat 2 "AT+QUIMSLOT=2"
                echo 2 > /etc/simsel
                sleep 2
                sendat 2 'AT+QCFG="ims",0'
                sendat 2 'AT+QMBNCFG="autosel",0' 
                echo "内置SIM卡1" >> /tmp/moduleInit
                echo 1 > /tmp/sim_sel
            ;;
            2)
                printMsg "内置SIM2"
                echo 0 > /sys/class/gpio/cpe-sel0/value
                sendat 2 "AT+QUIMSLOT=2"
                echo 2 > /etc/simsel
                sleep 2
                sendat 2 'AT+QCFG="ims",0'
                sendat 2 'AT+QMBNCFG="autosel",0' 
                echo "内置SIM卡2" >> /tmp/moduleInit
                echo 2 > /tmp/sim_sel
            ;;
            *)
                printMsg "错误状态"
                sendat 2 "AT+QUIMSLOT=1"
                sleep 2
                echo 3 > /tmp/Sim_Sel
                echo "SIM状态错误" >> /tmp/moduleInit
            ;;
            esac

    #IPV6 support chk
    enable_native_ipv6_flag=$(uci get modem.@ndis[0].enable_native_ipv6) || enable_native_ipv6_flag=0
    if [ "$enable_native_ipv6_flag" -eq 1 ]; then
        echo "正在初始化，请稍后刷新查看状态" >/tmp/ipv6prefix
    fi

    if [ ${Enable_IMEI} == 1 ];then
        IMEI_file="/tmp/IMEI"
        if [ -e "$IMEI_file" ]; then
            last_IMEI=$(cat "$IMEI_file")
        else
            last_IMEI=-1
        fi
        IMEI=`uci -q get modem.@ndis[0].modify_imei`
        if [ "$IMEI" != "$last_IMEI" ]; then
            /usr/share/modem/moimei ${IMEI} 1>/dev/null 2>&1
            printMsg "IMEI: ${IMEI}"
            echo "修改IMEI $IMEI" >> /tmp/moduleInit
            echo "$IMEI" > "$IMEI_file"
        else
            echo "IMEI未变动, 不执行操作" >> /tmp/moduleInit
        fi
    fi
    # 网络模式选择
    #---------------------------------
    RF_Mode_file="/tmp/RF_Mode"
    if [ -e "$RF_Mode_file" ]; then
        last_RF_Mode=$(cat "$RF_Mode_file")
    else
        last_RF_Mode=-1
    fi
    #--
    if [ "$RF_Mode" != "$last_RF_Mode" ]; then
        if [ "$RF_Mode" == 0 ]; then
            echo "RF_Mode: $RF_Mode 自动网络" >> /tmp/moduleInit
            sendat 2 'AT+QNWPREFCFG="mode_pref",AUTO' >> /tmp/moduleInit
        elif [ "$RF_Mode" == 1 ]; then
            echo "RF_Mode: $RF_Mode 4G网络" >> /tmp/moduleInit
            sendat 2 'AT+QNWPREFCFG="mode_pref",LTE' >> /tmp/moduleInit
        elif [ "$RF_Mode" = 2 ]; then
            echo "RF_Mode: $RF_Mode 5G网络" >> /tmp/moduleInit
            sendat 2 'AT+QNWPREFCFG="mode_pref",NR5G' >> /tmp/moduleInit
        fi
        echo "$RF_Mode" > "$RF_Mode_file"
    else
        echo "RF_Mode未变动, 不执行操作" >> /tmp/moduleInit
    fi
    #-------------------------

    # LTE锁频
    #-------------------------
    Band_LTE_file="/tmp/Band_LTE"
    if [ -e "$Band_LTE_file" ]; then
        last_Band_LTE=$(cat "$Band_LTE_file")
    else
        last_Band_LTE=-1
    fi
    #--
    if [ "$Band_LTE" != "$last_Band_LTE" ]; then
        if [ "$Band_LTE" == 0 ]; then
            sendat_command='AT+QNWPREFCFG="lte_band",1:3:5:8:34:38:39:40:41'
            sendat_result=$(sendat 2 "$sendat_command")
            echo "LTE自动: $sendat_result" >> /tmp/moduleInit
        else
            sendat_command="AT+QNWPREFCFG=\"lte_band\",$Band_LTE"
            sendat_result=$(sendat 2 "$sendat_command")
            echo "LTE锁频: $sendat_result" >> /tmp/moduleInit
        fi
        echo "$Band_LTE" > "$Band_LTE_file"
    else
        echo "Band_LTE未变动, 不执行操作" >> /tmp/moduleInit
    fi
    #----------------------

    # SA/NSA模式切换
    #----------------------
    NR_Mode_file="/tmp/NR_Mode"
    if [ -e "$NR_Mode_file" ]; then
        last_NR_Mode=$(cat "$NR_Mode_file")
    else
        last_NR_Mode=-1
    fi
    #--
    if [ "$NR_Mode" != "$last_NR_Mode" ]; then
        if [ "$NR_Mode" == 0 ]; then
            echo "NR_Mode: $NR_Mode 自动网络" >> /tmp/moduleInit
            sendat 2 'AT+QNWPREFCFG="nr5g_disable_mode",0' >> /tmp/moduleInit
        elif [ "$NR_Mode" = 1 ]; then
            echo "NR_Mode: $NR_Mode SA网络" >> /tmp/moduleInit
            sendat 2 'AT+QNWPREFCFG="nr5g_disable_mode",2' >> /tmp/moduleInit
        elif [ "$NR_Mode" = 2 ]; then
            echo "NR_Mode: $NR_Mode NSA网络" >> /tmp/moduleInit
            sendat 2 'AT+QNWPREFCFG="nr5g_disable_mode",1' >> /tmp/moduleInit
        fi
        echo "$NR_Mode" > "$NR_Mode_file"
    else
        echo "NR_Mode未变动, 不执行操作" >> /tmp/moduleInit
    fi
    #----------------------

    # SA锁频
    #----------------------
    band_sa_file="/tmp/Band_SA"
    if [ -e "$band_sa_file" ]; then
        last_Band_SA=$(cat "$band_sa_file")
    else
        last_Band_SA=-1
    fi
    #--
    if [ "$Band_SA" != "$last_Band_SA" ]; then
        if [ "$Band_SA" == 0 ]; then
            sendat_command='AT+QNWPREFCFG="nr5g_band",1:3:8:28:41:78:79'
            sendat_result=$(sendat 2 "$sendat_command")
            echo "SA自动: $sendat_result" >> /tmp/moduleInit
        else
            sendat_command="AT+QNWPREFCFG=\"nr5g_band\",$Band_SA"
            sendat_result=$(sendat 2 "$sendat_command")
            echo "SA锁频: $sendat_result" >> /tmp/moduleInit
        fi
        echo "$Band_SA" > "$band_sa_file"
    else
        echo "Band_SA未变动, 不执行操作" >> /tmp/moduleInit
    fi
    #-------------------

    # NSA锁频
    #-------------------
    band_nsa_file="/tmp/Band_NSA"
    if [ -e "$band_nsa_file" ]; then
        last_Band_NSA=$(cat "$band_nsa_file")
    else
        last_Band_NSA=-1
    fi

    if [ "$Band_NSA" != "$last_Band_NSA" ]; then
        if [ "$Band_NSA" == 0 ]; then
            sendat_command='AT+QNWPREFCFG="nsa_nr5g_band",41:78:79'
            sendat_result=$(sendat 2 "$sendat_command")
            echo "NSA自动: $sendat_result" >> /tmp/moduleInit
            echo 0 > /tmp/Band_NSA
        else
            sendat_command="AT+QNWPREFCFG=\"nsa_nr5g_band\",$Band_SA"
            sendat_result=$(sendat 2 "$sendat_command")
            echo "NSA锁频: $sendat_result" >> /tmp/moduleInit
            echo 1 > /tmp/Band_NSA
        fi
        echo "$Band_NSA" > "$band_nsa_file"
    else
        echo "Band_NSA未变动, 不执行操作" >> /tmp/moduleInit
    fi

    #数据漫游
    if [ -n "$Dataroaming" ] && [ "$Dataroaming" = "1" ]; then
        sendat 2 'AT+QNWCFG="data_roaming",0'
        printMsg "DataRoaming Enable"
    else
        sendat 2 'AT+QNWCFG="data_roaming",1'
         printMsg "DataRoaming Disable"
    fi


    #锁定频率
    #------------------------@Icey
    unlock() {
        printMsg "Unlock Band"
        sendat 2 'at+qnwlock="common/5g",0'
        sleep 2
        sendat 2 'at+qnwlock="common/4g",0'
        sleep 2
        return 0
    }

     #网络定时切换
    #------------------------
    Autoswitch(){
        RF_Mode2_file="/tmp/RF_Mode2"
        Auto_witch_file="/tmp/Auto_witch"
        Auto_witch_date_file="/tmp/Autoswitchdate"
        #是否确认
        if [ -e "$Auto_witch_file" ]; then
            last_Auto_witch=$(cat "$Auto_witch_file")
        else
            last_Auto_witch=-1
        fi
        if [ "$switchNetwork" != "$last_Auto_witch" ]; then

            if [ "$switchNetwork" == 0 ]; then
                #杀死自动切换网络程序 
                Autoswitch_lock_file="/tmp/Autoswitch.lock"
                if [ -e "$Autoswitch_lock_file" ]; then
                    # 锁文件存在，获取锁定的进程 ID，并终止它
                    locked_pid=$(cat "$Autoswitch_lock_file")
                    if [ -n "$locked_pid" ]; then
                     echo "Terminating existing Autoswitch.sh process (PID: $locked_pid)" 
                    kill "$locked_pid"
                        sleep 1  # 等待一段时间确保进程终止
                    fi
                fi               
                echo "杀死自动切换网络程序" >> /tmp/moduleInit
                echo 0 > /tmp/Auto_witch
                #记录网络格式
                echo "0" > "$RF_Mode2_file"
                echo "00-07" > "$Auto_witch_date_file"
            else
                #记录网络格式,和时间数据
                echo "$RF_Mode2" > "$RF_Mode2_file"
                echo "$Autoswitchtime" > "$Auto_witch_date_file"
                /usr/share/modem/autoswitch.sh &
            #启动自动切换网络程序！ 
            echo "启动自动切换网络程序！" >> /tmp/moduleInit
            echo 1 > /tmp/Auto_witch
            fi
                echo "$switchNetwork" > "$Auto_witch_file"
            else
            echo "switchNetwork未变动, 不执行操作" >> /tmp/moduleInit
        fi

        #网络模式是否变动
        if [ -e "$RF_Mode2_file" ]; then
            last_RF_Mode2=$(cat "$RF_Mode2_file")
        else
            last_RF_Mode2=-1
        fi
        if [ "$RF_Mode2" != "$last_RF_Mode2" ]; then
                 Autoswitch_lock_file="/tmp/Autoswitch.lock"
                if [ -e "$Autoswitch_lock_file" ]; then
                    # 锁文件存在，获取锁定的进程 ID，并终止它
                     locked_pid=$(cat "$Autoswitch_lock_file")
                    if [ -n "$locked_pid" ]; then
                        echo "Terminating existing Autoswitch.sh process (PID: $locked_pid)" 
                        kill "$locked_pid"
                        sleep 1  # 等待一段时间确保进程终止
                    fi
                fi
                /usr/share/modem/autoswitch.sh &
            echo "$RF_Mode2" > "$RF_Mode2_file"
        else
            echo "RF_Mode2未变动, 不执行操作" >> /tmp/moduleInit
        fi
        

        #时间参数是否变动
        if [ -e "$Auto_witch_date_file" ]; then
            last_Auto_witch_date=$(cat "$Auto_witch_date_file")
        else
            last_Auto_witch_date=-1
        fi
        if [ "$Autoswitchtime" != "$last_Auto_witch_date" ]; then
                 Autoswitch_lock_file="/tmp/Autoswitch.lock"
                if [ -e "$Autoswitch_lock_file" ]; then
                    # 锁文件存在，获取锁定的进程 ID，并终止它
                     locked_pid=$(cat "$Autoswitch_lock_file")
                    if [ -n "$locked_pid" ]; then
                        echo "Terminating existing autoswitch.sh process (PID: $locked_pid)" 
                        kill "$locked_pid"
                        sleep 1  # 等待一段时间确保进程终止
                    fi
                fi
                /usr/share/modem/autoswitch.sh &
            echo "$Autoswitchtime" > "$Auto_witch_date_file"
        else
            echo "Autoswitchtime未变动, 不执行操作" >> /tmp/moduleInit
        fi
    }

    band_lock() {
        printMsg "Start Band Lock"
        if [ "$Freqlock" -eq 0 ]; then
            if [ ! -e "/tmp/freq.run" ]; then
                printMsg "Restore band lock at boot"
                unlock
                return 0
            fi
            printMsg "Setting Will restore at next boot"
        fi 
            case "$RF_Mode" in
                0)
                    return 0
                    ;;
                1)
                    if [ "$Band_LTE" -ne 0 ] && [ "$Earfcn" -ne 0 ] && [ "$Cellid" -ne 0 ]; then
                        printMsg "BAND LOCK AT COMMON4G $Cellid,$Earfcn"
                        sendat 2 "at+qnwlock=\"common/4g\",1,$Cellid,$Earfcn"
                        sleep 3
                        sendat 2 'at+qnwlock="save_ctrl",1,1'
                    else
                        unlock
                    fi
                    ;;
                2)
                    if [ "$NR_Mode" -ne 0 ] && [ "$Band_SA" -ne 0 -o "$Band_NSA" -ne 0 ] && [ "$Earfcn" -ne 0 ] && [ "$Cellid" -ne 0 ]; then
                            case "$Band_SA" in
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
                        printMsg "BAND LOCK AT COMMON5G $Cellid,$Earfcn,$scs,$Band_SA"
                        sendat 2 "at+qnwlock=\"common/5g\",$Cellid,$Earfcn,$scs,$Band_SA"
                        sleep 3
                        sendat 2 'at+qnwlock="save_ctrl",1,1'
                    else
                        unlock
                    fi
                    ;;
                *)
                    printMsg "BANDLOCKFAILURE"
                    return 0
                    ;;
            esac
    }
   


    #Check if SIM or esim exist
    chkSimExt() {
        simStat=$(sendat 2 'at+qsimstat?' | grep '+QSIMSTAT' | awk -F, '{print $2}'| tr -d '\r\n')
        case $simStat in
            1)
                printMsg "SIM card is inserted."
                echo "SIM卡已插入" > /tmp/simcardstat
                #APN CONFiG 
                apnconfig=`uci -q get modem.@ndis[0].apnconfig` || apnconfig=""
                sendat_result=$(sendat 2  'AT+CGDCONT=1,"IPV4V6","'$apnconfig'"')
                return 0
                ;;
            0)
                printMsg "SIM card is not inserted. Exiting program."
                echo "SIM卡未插入" > /tmp/simcardstat
                exit 0
                ;;
            *)
                printMsg "Unknown SIM card Insert status. Sleep 10，Retrying..."
                sleep 3
                chkSimExt
                ;;
        esac
    }

    #Check if SIM Ready to use

    chkSimReady() {
        chkSimReadyMAX_RETRIES=30
        local simReady=$(sendat 2 'at+qinistat' | grep '+QINISTAT' | awk '{print $2}' | tr -d '\r\n')
        while [ $moduleSetChkMAX_RETRIES -gt 0 ]; do
            case $simReady in
                7)
                    printMsg "SIM card is ready."
                    return 0
                    ;;
                *)
                        printMsg "Unknown SIM card Init status. Retrying..."
                        chkSimReadyMAX_RETRIES=$((chkSimReadyMAX_RETRIES - 1))
                        sleep 2
                    ;;
            esac
        done
    }


    #Check Module Hardware Set,pre check befroe everything
    moduleSetChk(){
        macchk=0
        moduleSetChkMAX_RETRIES=5
        printMsg "Start Modem Hardware Check"
        sendat 2 'at+qeth="rgmii","enable",1"'
        while [ $macchk -lt 30 ]; do
            mac_address=$(ifconfig | grep eth1 | awk '{print $5}' | tr -d '\r\n')

            case "$mac_address" in
            *:*:*:*:*:*)
                printMsg "WAN MAC $mac_address"
                success=true
                break
                ;;
            *)
                sleep 1
                macchk=$((macchk + 1))
                if [ $macchk -eq 30 ]; then
                    printMsg "获取WAN MAC失败!!"
                    exit 1
                fi
                ;;
            esac
        done

        while [ $moduleSetChkMAX_RETRIES -gt 0 ]; do
        success=true
        
        dataInterfaceChk=$(sendat 2 'AT+QCFG="data_interface"'|grep '+QCFG:'|awk -F \" {'print $3'}|tr -d '\r\n')
            printMsg "dataInterfaceChk: $dataInterfaceChk"
            if [ "$dataInterfaceChk" != ",1,0" ]; then
                printMsg "dataInterfaceChk Status check failed."
                sendat 2 'AT+QCFG="data_interface",1,0'
                printMsg "Maybe need to reboot"
                echo "模块数据接口模式变更或模块异常，请手动断电后重启。" >/tmp/simcardstat 
                exit
                success=false
                #todo
            fi
            
        pcieStat=$(sendat 2 'AT+QCFG="pcie/mode"' | grep '+QCFG' | awk -F, '{print $2}' | tr -d '\r\n')
            printMsg "PCIe Status: $pcieStat"
            if [ "$pcieStat" != "1" ]; then
                printMsg "PCIe Status check failed."
                sendat 2 'AT+QCFG="pcie/mode",1'
                success=false
                #todo
            fi

            ethR8125Stat=$(sendat 2 'at+qeth="eth_driver"'|grep '"r8125",1'|awk -F , {'print $3'}|tr -d '\r\n')
            printMsg "ethR8125Stat Status: $ethR8125Stat"
            if [ "$ethR8125Stat" != "1" ]; then
                printMsg "ethR8125Stat Status check failed."
                sendat 2 'AT+QETH="eth_driver","r8125",1'
                success=false
            fi

            #ipptMac=$(sendat 2 'at+qeth="ipptmac"'|grep '+QETH'|awk -F , {'print $2'}|tr -d '\r\n')
            #printMsg "ipptMac Status: $ipptMac"
            #if [ "$ipptMac" != "$mac_address" ]; then
            #    printMsg "ipptMac Status check failed."
            #    sendat 2 'AT+QETH="ipptmac",'$mac_address''
            #    success=false
            #fi

            #ipptNatStat=$(sendat 2 'at+qmap="ippt_nat"' |grep '+QMAP'|awk -F , '{print $2}'|tr -d '\r\n')
            #printMsg "ipptNatStat Status: $ipptNatStat"
            #if [ "$ipptNatStat" != "0" ]; then
            #    printMsg "ipptNatStat Status check failed."
            #    sendat 2 'AT+QMAP="ippt_nat",0'
            #    success=false
            #fi

            mpdnruleStat=$(sendat 2 'at+qmap="mpdn_rule"' | grep '+QMAP: "MPDN_rule",0,1,0,1,1' | tr -d '\r\n')
            printMsg "mpdnruleStat Status: $mpdnruleStat"
            if ! echo "$mpdnruleStat" | grep -q "0,1,0,1,1"; then
                printMsg "mpdnruleStat Status check failed."
                sendat 2 'at+qmap="mpdn_rule",0'
                sleep 5
                #sendat 2 'AT+QMAP="mpdn_rule",0,1,0,1,1,"'$mac_address'"'
                sendat 2 'AT+QMAP="mpdn_rule",0,1,0,1,1,"FF:FF:FF:FF:FF:FF"'
                sleep 8
                success=false
            fi

            cmgfStat=$(sendat 2 'at+cmgf?'|grep '+CMGF'|awk -F : {'print $2'}|tr -d '\r\n')
            printMsg "cmgfStat Status: $cmgfStat"
            if [ "$cmgfStat" != " 0" ]; then
                printMsg "cmgfStat Status check failed."
                sendat 2 'at+cmgf=0'
                success=false
            fi

            #imsStat=$(sendat 2 'AT+QCFG="ims",1'|grep '+QCFG'|awk -F , {'print $2'}|tr -d '\r\n')
            #printMsg "imsStat Status: $imsStat"
            #if [ "$imsStat" != "1" ]; then
            #    printMsg "imsStat Status check failed."
             #   sendat 2 'AT+QCFG="ims",1' 
             #   success=false
            #fi

           # autosel=$(sendat 2 'AT+QMBNCFG="autosel",1'|grep '+QMBNCFG'|awk -F , {'print $2'}|tr -d '\r\n')
            #printMsg "autosel Status: $autosel"
           # if [ "$autosel" != "1" ]; then
           #     printMsg "imsStat Status check failed."
           #     sendat 2 'AT+QMBNCFG="autosel",1' 
             #   success=false
            #fi


            if [ "$success" = false ]; then
                    moduleSetChkMAX_RETRIES=$(($moduleSetChkMAX_RETRIES - 1))
                    printMsg "Recheck Hardware Set...."
                    sleep 2
            else
                printMsg "Hardware Check Complete."
                return 0
            fi
            
        done

    }

    #Check if wan work
    check_and_activate_wan() {
    max_retries=60
    retry_interval=3
    retries=0

    while [ "$retries" -lt "$max_retries" ]; do
            sendat 2  'AT+QMAP="connect",0,1'
            sleep 1
            ipv4_info=$(sendat 2 'at+qmap="wwan"' | grep IPV4|awk -F \" {'print $6'}|tr -d '\r\n')
            printMsg "Modem ip is now $ipv4_info"
        if [ -z "$ipv4_info" ] || ! valid_ip "$ipv4_info" ; then
            printMsg "Retry $((retries + 1)): IPv4 address not obtained or invalid. Retrying in $retry_interval seconds..."
            sleep 1
            sleep "$retry_interval"
            retries=$((retries + 1))
        else
            /sbin/ifup wan
            /sbin/ifup wan6
            wantstcount=0
            while [ $wantstcount -lt 20 ]; do
                http_codeChk0=$(curl -o /dev/null -s -w %{http_code} http://connect.rom.miui.com/generate_204)
                http_codeChk1=$(curl -o /dev/null -s -w %{http_code} http://connectivitycheck.platform.hicloud.com/generate_204)

                if [ "$http_codeChk0" -eq 204 ] || [ "$http_codeChk1" -eq 204 ]; then
                    /usr/share/modem/netmodeled.sh &
                    printMsg "Internet Connection Ready."
                    break
                else
                    printMsg "HTTP status code is not 204 for either URL. Retry $((count + 1))."
                fi
                sleep 2
                wantstcount=$((wantstcount + 1))
                if [ "$wantstcount" -eq 20 ]; then
                    printMsg "Failed to Init Modem after $wantstcount"
                    exit 1
                fi
            done
            printMsg "IPV4 WAN UP!Ready to go Internet!"
            rm $lock_file
            /usr/share/modem/enableipv6.sh
            Autoswitch 
            #/usr/share/modem/ipcheck.sh &
            exit 0
        fi
    done

    if [ "$retries" -eq "$max_retries" ]; then
        printMsg "Failed to obtain valid IPv4 address after $max_retries retries. Exiting program." >>/tmp/moduleInit
        printMsg "Job is FAILURE"

        exit 1
    fi
    }

    valid_ip() {
    local ip=$1

    # 包含IP地址的正则表达式
    if echo "$ip" | grep -q -E '([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)'; then
        if echo "$ip" | grep -q "0\.0\.0\.0"; then
            return 1  # 返回 1 表示错误
            else
        return 0
        fi
        
    fi

    return 1  # 返回 1 表示错误
    }

    sim_pin_chk() {
        sim_card_pin_status=$(sendat 2 'at+cpin?' |grep '+CPIN:'|awk -F ':' {'print $2'}|tr -d ' \r\n')
        pincode=$(uci get modem.@ndis[0].pincode)
        case "$sim_card_pin_status" in
            "READY")
                printMsg "SIM card is ready."
                echo "SIM卡已正常工作">/tmp/simcardstat 
                return 0
                ;;
            "SIMPIN"|"SIMPIN2")
                if echo "$pincode" | grep -qE '^[0-9]+$'; then
                    sendat 2 "at+cpin=\"$pincode\""
                    sim_card_pin_status=$(sendat 2 'at+cpin?' |grep '+CPIN:'|awk -F ':' {'print $2'}|tr -d ' \r\n')
                    if [ "$sim_card_pin_status" != "READY" ]; then
                        printMsg "Failed to unlock SIM card with PIN."
                        echo "SIM PIN错误，请注意，多次错误将会导致锁卡">/tmp/simcardstat 
                        exit 1
                    else
                        printMsg "SIM card is ready."
                        echo "SIM卡已正常工作">/tmp/simcardstat 
                        sleep 3
                        return 0
                    fi
                else
                    printMsg "Invalid PIN code."
                    echo "需要PIN。PIN不存在或者错误！">/tmp/simcardstat 
                    exit 1
                fi
                ;;
            "SIMPUK"|"SIMPUK2")
                printMsg "SIM card requires PUK."
                echo "SIM卡已锁，请在其他设备上插入此卡输入PUK解锁">/tmp/simcardstat 
                exit 1
                ;;
            *)
                printMsg "Unknown SIM card status."
                echo "SIM卡状态异常">/tmp/simcardstat 
                exit 1
                ;;
        esac
}


    moduleStartCheckLine(){
        sleep 15  #Must wait 15 seconds for sim init itself 
        chkSimExt
        sim_pin_chk
        echo "chkSimExt $?" 
        chkSimReady
        echo "chkSimReady $?" 
        moduleSetChk
        band_lock
        echo "moduleSetChk $?" 
        check_and_activate_wan
    }


    #start
    moduleStartCheckLine

    exit


