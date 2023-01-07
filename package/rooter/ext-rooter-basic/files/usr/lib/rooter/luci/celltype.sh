#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	modlog "Cell type $CURRMODEM" "$@"
}

zte_type() {
	ATCMDD="AT+ZSNT?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	OX=$($ROOTER/common/processat.sh "$OX")
	ZSNT=$(echo "$OX" | awk -F[,\ ] '/^\+ZSNT:/ {print $2}')
	if [ "x$ZSNT" != "x" ]; then
		NETMODE="-"
		if [ $ZSNT = "0" ]; then
			ZSNTX=$(echo "$OX" | awk -F[,\ ] '/^\+ZSNT:/ {print $4}')
			case $ZSNTX in
			"0" )
				NETMODE="1"
				;;
			"1" )
				NETMODE="2"
				;;
			"2" )
				NETMODE="4"
				;;
			"6" )
				NETMODE="6"
				;;
			esac
		else
			case $ZSNT in
			"1" )
				NETMODE="3"
				;;
			"2" )
				NETMODE="5"
				;;
			"6" )
				NETMODE="7"
				;;
			esac
		fi
	fi
	uci set modem.modem$CURRMODEM.modemtype="1"
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

sierra_type() {
	ATCMDD="AT!SELRAT?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	SELRAT=$(echo $OX | grep -o "!SELRAT:[^0-9]\+[0-9]\{2\}" | grep -o "[0-9]\{2\}")
	if [ -n "$SELRAT" ]; then
		case $SELRAT in
		"01" )
			NETMODE="5"
			;;
		"02" )
			NETMODE="3"
			;;
		"06" )
			NETMODE="7"
			;;
		* )
			NETMODE="1"
			;;
		esac
	fi
	uci set modem.modem$CURRMODEM.modemtype="2"
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

huawei_type() {
	ATCMDD="AT^SYSCFGEX?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	OX=$($ROOTER/common/processat.sh "$OX")
	SYSCFG=$(echo "$OX" | awk -F[,\"] '/^\^SYSCFGEX:/ {print $2}')
	if [ "x$SYSCFG" != "x" ]; then
		NETMODE="-"
		case $SYSCFG in
		"00" )
			NETMODE="1"
			;;
		"01" )
			NETMODE="3"
			;;
		"02" )
			NETMODE="5"
			;;
		"03" )
			NETMODE="7"
			;;
		* )
			ACQ=${SYSCFG:0:2}
			case $ACQ in
			"01" )
				NETMODE="2"
				;;
			"02" )
				NETMODE="4"
				;;
			"03" )
				NETMODE="6"
				;;
			esac
			;;
		esac
		uci set modem.modem$CURRMODEM.modemtype="3"
	else
		ATCMDD="AT^SYSCFG?"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		OX=$($ROOTER/common/processat.sh "$OX")
		SYSCFG=$(echo "$OX" | awk -F[,\ ] '/^\^SYSCFG:/ {print $2}')
		if [ "x$SYSCFG" != "x" ]; then
			NETMODE="-"
			case $SYSCFG in
			"7" )
				NETMODE="1"
				;;
			"13" )
				NETMODE="3"
				;;
			"14" )
				NETMODE="5"
				;;
			* )
				SYSCFG=$(echo "$OX" | awk -F[,\ ] '/^\^SYSCFG:/ {print $3}')
				case $SYSCFG in
				"0" )
				NETMODE="1"
					;;
				"1" )
					NETMODE="2"
					;;
				"2" )
					NETMODE="4"
					;;
				esac
				;;
			esac
			uci set modem.modem$CURRMODEM.modemtype="4"
		fi
	fi
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

ublox_type() {
	ATCMDD="AT+URAT?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	URAT=$(echo $OX" " | grep -o "+URAT: .\+ OK " | tr " " ",")
	URAT1=$(echo $URAT | cut -d, -f2)
	URAT2=$(echo $URAT | cut -d, -f3)
	if [ -n "$URAT1" ]; then
		NETMODE="-"
		case $URAT1 in
		"0" )
			NETMODE="3"
			;;
		"2" )
			NETMODE="5"
			;;
		"3" )
			NETMODE="7"
			;;
		* )
			case $URAT2 in
			"0" )
				NETMODE="2"
				;;
			"2" )
				NETMODE="4"
				;;
			"3" )
				NETMODE="1"
				;;
			esac
			;;
		esac
		uci set modem.modem$CURRMODEM.modemtype="5"
	fi
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

quectel_type() {
	idVidP=$idV":"$idP
	ATCMDD="AT+CGMM"
	model=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	EM20=$(echo "$model" | grep "EM20")
	if [ $EM20 ]; then
		idVidP=$idV":"$idP"0"
	fi
	if [ "$idVidP" == "2c7c:0800" -o "$idVidP" == "2c7c:0620" -o "$idVidP" == "2c7c:030b" -o "$idVidP" == "2c7c:0801"  -o "$idVidP" == "2c7c:0900" ]; then
		ATCMDD="AT+QNWPREFCFG=\"mode_pref\""
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		QNSM=$(echo $OX | grep -o ",[AUTOLENR5GWCDM:]\+" | tr ',' ' ')
		QNSM=$(echo $QNSM)
		if [ -n "$QNSM" ]; then
			case $QNSM in
			"AUTO" )
				NETMODE="1"
				;;
			"LTE" )
				NETMODE="7"
				;;
			"LTE:NR5G" )
				NETMODE="8"
				;;
			"NR5G" )
				NETMODE="9"
				;;
			"WCDMA" )
				NETMODE="5"
				;;
			esac
			uci set modem.modem$CURRMODEM.modemtype="6"
		fi
	else
		ATCMDD="AT+QCFG=\"nwscanmode\""
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		QNSM=$(echo $OX | grep -o "+QCFG: \"nwscanmode\",[0-9]" | grep -o "[0-9]")
		if [ -n "$QNSM" ]; then
			case $QNSM in
			"0" )
				NETMODE="1"
				;;
			"1" )
				NETMODE="3"
				;;
			"2"|"5" )
				NETMODE="5"
				;;
			"3" )
				NETMODE="7"
				;;
			esac
			uci set modem.modem$CURRMODEM.modemtype="6"
		fi
	fi
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

meig_type() {
	if [ $idV == "2dee" ]; then
		ATCMDD="AT^SYSCFGEX?"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		RATs=$(echo "$OX" | grep -o "\^SYSCFGEX: \"[0-9]\{2,6\}\"" | grep -o "[0-9]\{2,6\}")
		if [ -n "$RATs" ]; then
			case $RATs in
			"02" )
				NETMODE="5" ;;
			"03" )
				NETMODE="7" ;;
			"04" )
				NETMODE="9" ;;
			"0203" | "0204" | "020304" | "020403" )
				NETMODE="4" ;;
			"0304" | "0302" | "030402" | "030204" )
				NETMODE="6" ;;
			"0403" )
				NETMODE="8" ;;
			* )
				NETMODE="1" ;;
			esac
		fi
	else
		ATCMDD="AT+MODODR?"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		MODODR=$(echo $OX | grep -o "[0-9]")
		if [ -n "$MODODR" ]; then
			case $MODODR in
			"1"|"8" )
				NETMODE="5" ;;
			"2" )
				NETMODE="1" ;;
			"3" )
				NETMODE="3" ;;
			"5" )
				NETMODE="7" ;;
			esac
		fi
	fi
	uci set modem.modem$CURRMODEM.modemtype="7"
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

telit_type() {
	ATCMDD="AT^SYSCONFIG?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	SCFG=$(echo $OX | grep -o "\^SYSCONFIG: [0-9]\{1,2\}" | grep -o "[0-9]\{1,2\}")
	if [ -n "$SCFG" ]; then
		PREF=$(echo $OX | grep -o "\^SYSCONFIG: 2,[0-9]" | grep -o ",[0-9]")
		case $SCFG in
		"13" )
			NETMODE="3" ;;
		"14" )
			NETMODE="5" ;;
		"17" )
			NETMODE="7" ;;
		* )
			NETMODE="1" ;;
		esac
		uci set modem.modem$CURRMODEM.modemtype="8"
	fi
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

fibocom_type() {
	NETMODE=""
	idPP=${idP:1:1}
	if [ "$idPP" = "1" ]; then
		ATCMDD="AT+GTRAT?"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		MRAT=$(echo $OX | grep -o "+GTRAT: [0-9]\{1,2\}" | grep -o "[0-9]\{1,2\}")
		if [ -n "$MRAT" ]; then
			case $MRAT in
				"2" )
					NETMODE="5" ;;
				"3" )
					NETMODE="7" ;;
				"14" )
					NETMODE="9" ;;
				"17" )
					NETMODE="8" ;;
				* )
					NETMODE="1" ;;
			esac
		fi
	else
		ATCMDD="AT+XACT?"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		XACT=$(echo $OX | grep -o "+XACT: [0-9]" | grep -o "[0-9]")
		if [ -n "$XACT" ]; then
			PREF=$(echo $OX | grep -o "+XACT: [0-9],[0-9]" | grep -o ",[0-9]")
			case $XACT in
			"1" )
				NETMODE="5" ;;
			"2" )
				NETMODE="7" ;;
			"4" )
				if [ "$PREF" = ",1" ]; then
					NETMODE="4"
				else
					NETMODE="6"
				fi ;;
			* )
				NETMODE="6" ;;
			esac

		fi
	fi
	uci set modem.modem$CURRMODEM.modemtype="9"
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

simcom_type() {
	ATCMDD="AT+CNMP?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	CNMP=$(echo "$OX" | grep -o "+CNMP:[ ]*[0-9]\{1,3\}" | grep -o "[0-9]\{1,3\}")
	if [ -n "$CNMP" ]; then
		case $CNMP in
		"2"|"55" )
			NETMODE="1" ;;
		"13" )
			NETMODE="3" ;;
		"14" )
			NETMODE="5" ;;
		"38" )
			NETMODE="7" ;;
		"71" )
			NETMODE="9" ;;
		"109" )
			NETMODE="8" ;;
		* )
			NETMODE="0" ;;
		esac
	fi
	uci set modem.modem$CURRMODEM.modemtype="10"
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

quanta_type() {
	ATCMDD="AT^QCNCFG?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	TECH=$(echo $OX | grep -o "\^QCNCFG: \"[0123]\{2\}\"" | grep -o "[0123]\{2\}")
	case $TECH in
		"02")
			NETMODE="5"
			;;
		"03")
			NETMODE="7"
			;;
		*)
			NETMODE="1"
			;;
	esac
	uci set modem.modem$CURRMODEM.modemtype="11"
	uci set modem.modem$CURRMODEM.netmode=$NETMODE
	uci commit modem
}

CURRMODEM=$1
COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)

idV=$(uci get modem.modem$CURRMODEM.idV)
idP=$(uci -q get modem.modem$CURRMODEM.idP)
NETMODE="-"

# This case statement should be kept in sync with: $ROOTER/signal/modemsignal.sh
case $idV in
"1199"|"0f3d" )
	sierra_type
	;;
"19d2" )
	if [ $idP = 1432 ]; then
		telit_type
	else
		zte_type
	fi
	;;
"12d1" )
	huawei_type
	;;
"2c7c" )
	quectel_type
	;;
"2cb7"|"1508"|"8087" )
	fibocom_type
	;;
"2dee" )
	meig_type
	;;
"05c6" )
	case $idP in
		"f601" )
			meig_type
		;;
		"5042" )
			telit_type
		;;
		"9090"|"9003"|"9215" )
			quectel_type
		;;
		"90db" )
			simcom_type
		;;
		* )
			:
		;;
	esac
	;;
"1bc7" )
	telit_type
	;;
"1410" )
	:
	;;
"413c" )
	case $idP in
		"81d7"|"81d8" )
			telit_type
		;;
		* )
			sierra_type
		;;
	esac
	;;
"0489" |"03f0" )
	telit_type
	;;
"1e0e" )
	simcom_type
	;;
"8087" )
	if [ $idP = "095a" ]; then
		fibocom_type
	fi
	;;
"0408" )
	quanta_type
	;;
* )
	:
	;;
esac
exit
