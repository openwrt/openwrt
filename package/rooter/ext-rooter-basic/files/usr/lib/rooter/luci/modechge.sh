#!/bin/sh

ROOTER=/usr/lib/rooter

MODEMTYPE=$1
NETMODE=$2

# log() {
	modlog "ModeChange $CURRMODEM" "$@"
# }

CURRMODEM=$(uci get modem.general.miscnum)
uci set modem.modem$CURRMODEM.cmode="0"
uci set modem.modem$CURRMODEM.netmode="10"
uci commit modem

MODEMTYPE=$(uci get modem.modem$CURRMODEM.modemtype)
COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)

# ZTE
if [ $MODEMTYPE -eq 1 ]; then
	case $NETMODE in
		1*)
			ATC="AT+ZSNT=0,0,0" ;;
		2*)
			ATC="AT+ZSNT=0,0,1" ;;
		3*)
			ATC="AT+ZSNT=1,0,0" ;;
		4*)
			ATC="AT+ZSNT=0,0,2" ;;
		5*)
			ATC="AT+ZSNT=2,0,0" ;;
		6*)
			ATC="AT+ZSNT=0,0,6" ;;
		7*)
			ATC="AT+ZSNT=6,0,0" ;;
	esac
	ATC=$ATC";+ZBANDI=0"
fi

# Sierra
if [ $MODEMTYPE -eq 2 ]; then
	case $NETMODE in
		"3" )
			ATC="AT!SELRAT=2" ;;
		"5" )
			ATC="AT!SELRAT=1" ;;
		"7" )
			ATC="AT!SELRAT=6" ;;
		* )
			ATC="AT!SELRAT=0" ;;
	esac
fi

# Huawei LTE
if [ $MODEMTYPE -eq 3 ]; then
	case $NETMODE in
                1*)
                        ATC="AT^SYSCFGEX=\"00\",40000000,2,4,40000000,," ;;
                2*)
                        ATC="AT^SYSCFGEX=\"010203\",40000000,2,4,40000000,," ;;
                3*)
                        ATC="AT^SYSCFGEX=\"01\",40000000,2,4,40000000,," ;;
                4*)
                        ATC="AT^SYSCFGEX=\"020301\",40000000,2,4,40000000,," ;;
                5*)
                        ATC="AT^SYSCFGEX=\"02\",40000000,2,4,40000000,," ;;
                6*)
                        ATC="AT^SYSCFGEX=\"030201\",40000000,2,4,40000000,," ;;
                7*)
                        ATC="AT^SYSCFGEX=\"03\",40000000,2,4,40000000,," ;;
        esac
fi

# Huawei legacy
if [ $MODEMTYPE -eq 4 ]; then
	case $NETMODE in
		1*)
			ATC="AT^SYSCFG=2,0,40000000,2,4" ;;
		2*)
			ATC="AT^SYSCFG=2,1,40000000,2,4" ;;
		3*)
			ATC="AT^SYSCFG=13,1,40000000,2,4" ;;
		4*)
			ATC="AT^SYSCFG=2,2,40000000,2,4" ;;
		5*)
			ATC="AT^SYSCFG=14,2,40000000,2,4" ;;
	esac
fi

# ublox
if [ $MODEMTYPE -eq 5 ]; then
	case $NETMODE in
		1*)
			ATC="AT+CFUN=4;+URAT=4,3;+CFUN=1,1" ;;
		2*)
			ATC="AT+CFUN=4;+URAT=4,0;+CFUN=1,1" ;;
		3*)
			ATC="AT+CFUN=4;+URAT=0;+CFUN=1,1" ;;
		4*)
			ATC="AT+CFUN=4;+URAT=4,2;+CFUN=1,1" ;;
		5*)
			ATC="AT+CFUN=4;+URAT=2;+CFUN=1,1" ;;
		6*)
			ATC="AT+CFUN=4;+URAT=4,3;+CFUN=1,1" ;;
		7*)
			ATC="AT+CFUN=4;+URAT=4,3;+CFUN=1,1" ;;
	esac
fi

# Quectel
if [ $MODEMTYPE -eq 6 ]; then
	CURRMODEM=$(uci -q get modem.general.modemnum)
	idV=$(uci -q get modem.modem$CURRMODEM.idV)
	idP=$(uci -q get modem.modem$CURRMODEM.idP)
	ATCMDD="AT+CGMM"
	model=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	EM20=$(echo "$model" | grep "EM20")
	if [ $EM20 ]; then
		idP="0"
	fi
	NEWFMT=false
	if [ "$idV" = "2c7c" ]; then
		if [ "$idP" = "0800" -o "$idP" = "0620" -o "$idP" = "030b" -o "$idP" = "0801" -o "$idP" = "0900" ]; then
			NEWFMT=true
		fi
	fi
	case $NETMODE in
		"3")
			ATC="AT+QCFG=\"nwscanmode\",1" ;;
		"5")
			if $NEWFMT; then
				ATC="AT+QNWPREFCFG=\"mode_pref\",WCDMA"
			else
				ATC="AT+QCFG=\"nwscanmode\",2"
			fi
			;;
		"7")
			if $NEWFMT; then
				ATC="AT+QNWPREFCFG=\"mode_pref\",LTE"
			else
				ATC="AT+QCFG=\"nwscanmode\",3"
			fi
			;;
		"8")
			ATC="AT+QNWPREFCFG=\"mode_pref\",LTE:NR5G" ;;
		"9")
			ATC="AT+QNWPREFCFG=\"mode_pref\",NR5G" ;;
		*)
			if $NEWFMT; then
				ATC="AT+QNWPREFCFG=\"mode_pref\",AUTO"
			else
				ATC="AT+QCFG=\"nwscanmode\",0"
			fi
			;;
	esac
fi

# MEIG
if [ $MODEMTYPE -eq 7 ]; then
	CURRMODEM=$(uci -q get modem.general.modemnum)
	idV=$(uci -q get modem.modem$CURRMODEM.idV)
	if [ $idV == "2dee" ]; then
		case $NETMODE in
			"4")
				ATC="AT^SYSCFGEX=\"020304\"" ;;
			"5")
				ATC="AT^SYSCFGEX=\"02\"" ;;
			"6")
				ATC="AT^SYSCFGEX=\"030402\"" ;;
			"7")
				ATC="AT^SYSCFGEX=\"03\"" ;;
			"8")
				ATC="AT^SYSCFGEX=\"0403\"" ;;
			"9")
				ATC="AT^SYSCFGEX=\"04\"" ;;
			*)
				ATC="AT^SYSCFGEX=\"00\"" ;;
		esac
	else
		case $NETMODE in
			"3")
				ATC="AT+MODODR=3" ;;
			"5")
				ATC="AT+MODODR=1" ;;
			"7")
				ATC="AT+MODODR=5" ;;
			*)
				ATC="AT+MODODR=2" ;;
		esac
	fi
fi

# Telit, Foxconn, etc.
if [ $MODEMTYPE -eq 8 ]; then
	case $NETMODE in
		"3" )
			ATC="AT\$QCNSP=1,0,0" ;;
		"5" )
			ATC="AT\$QCNSP=2,0,0" ;;
		"7" )
			ATC="AT\$QCNSP=6,0,0" ;;
		* )
			ATC="AT\$QCNSP=0,0,0" ;;
	esac
fi

# Fibocom
if [ $MODEMTYPE -eq 9 ]; then
	CURRMODEM=$(uci -q get modem.general.modemnum)
	idV=$(uci -q get modem.modem$CURRMODEM.idV)
	idP=$(uci -q get modem.modem$CURRMODEM.idP)
	idPP=${idP:1:1}
	if [ "$idPP" = "1" ]; then
		case $NETMODE in
			"7")
				ATC="AT+GTRAT=3" ;;
			"8")
				ATC="AT+GTRAT=17" ;;
			"9")
				ATC="AT+GTRAT=14" ;;
			*)
				ATC="AT+GTRAT=10" ;;
		esac
	else
		case $NETMODE in
			"4")
				ATC="AT+XACT=4,1" ;;
			"5")
				ATC="AT+XACT=1" ;;
			"7")
				ATC="AT+XACT=2" ;;
			*)
				ATC="AT+XACT=4,2" ;;
		esac
	fi
fi

# SIMCom
if [ $MODEMTYPE -eq 10 ]; then
	case $NETMODE in
		"3")
			ATC="AT+CNMP=13" ;;
		"5")
			ATC="AT+CNMP=14" ;;
		"7")
			ATC="AT+CNMP=38" ;;
		"8")
			ATC="AT+CNMP=109" ;;
		"9")
			ATC="AT+CNMP=71" ;;
		*)
			ATC="AT+CNMP=2" ;;
	esac
fi

# Quanta, Megafon
if [ $MODEMTYPE -eq 11 ]; then
	case $NETMODE in
		"5")
			ATC="AT^QCNCFG=02" ;;
		"7")
			ATC="AT^QCNCFG=03" ;;
		*)
			ATC="AT^QCNCFG=00" ;;
	esac
fi

ATCMDD="$ATC"
OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")

$ROOTER/luci/celltype.sh $CURRMODEM
uci set modem.modem$CURRMODEM.cmode="1"
uci commit modem

$ROOTER/luci/restart.sh $CURRMODEM 11
