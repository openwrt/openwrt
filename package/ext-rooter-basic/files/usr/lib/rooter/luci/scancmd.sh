#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	modlog "Scan Command $CURRMODEM" "$@"
}

fibdecode() {
	j=$1
	tdec=$2
	mod=$3
	length=${#j}
	jx=$j
	length=${#jx}

	str=""
	i=$((length-1))
	while [ $i -ge 0 ]
	do
		dgt="0x"${jx:$i:1}
		DecNum=`printf "%d" $dgt`
		Binary=
		Number=$DecNum
		while [ $DecNum -ne 0 ]
		do
			Bit=$(expr $DecNum % 2)
			Binary=$Bit$Binary
			DecNum=$(expr $DecNum / 2)
		done
		if [ -z $Binary ]; then
			Binary="0000"
		fi
		len=${#Binary}
		while [ $len -lt 4 ]
		do
			Binary="0"$Binary
			len=${#Binary}
		done
		revstr=""
		length=${#Binary}
		ii=$((length-1))
		while [ $ii -ge 0 ]
		do
			revstr=$revstr${Binary:$ii:1}
			ii=$((ii-1))
		done
		str=$str$revstr
		i=$((i-1))
	done
	len=${#str}
	ii=0
	lst=""
	sep=","
	hun=101
	if [ $mod = "1" ]; then
		sep=":"
		hun=1
	fi
	while [ $ii -lt $len ]
	do
		bnd=${str:$ii:1}
		if [ $bnd -eq 1 ]; then
			if [ $tdec -eq 1 ]; then
				jj=$((ii+hun))
			else
				if [ $ii -lt 9 ]; then
					jj=$((ii+501))
				else
					jj=$((ii+5001))
				fi
			fi
			if [ -z $lst ]; then
				lst=$jj
			else
				lst=$lst$sep$jj
			fi
		fi
		ii=$((ii+1))
	done
}

CURRMODEM=$(uci get modem.general.miscnum)
COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)
uVid=$(uci get modem.modem$CURRMODEM.uVid)
uPid=$(uci get modem.modem$CURRMODEM.uPid)
model=$(uci get modem.modem$CURRMODEM.model)
ACTIVE=$(uci get modem.pinginfo$CURRMODEM.alive)
uci set modem.pinginfo$CURRMODEM.alive='0'
uci commit modem
L1=$(uci get modem.modem$CURRMODEM.L1)
length=${#L1}
L1="${L1:2:length-2}"
L1=$(echo $L1 | sed 's/^0*//')
L2=$(uci get modem.modem$CURRMODEM.L2)
L1X=$(uci get modem.modem$CURRMODEM.L1X)
if [ -z $L1X ]; then
	L1X="0"
fi

case $uVid in
	"2c7c" )
		M2='AT+QENG="neighbourcell"'
		M5=""
		case $uPid in
			"0125" ) # EC25-A
				M1='ATI'
				OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M1")
				REV=$(echo $OX" " | grep -o "Revision: .\+ OK " | tr " " ",")
				MODL=$(echo $REV | cut -d, -f2)
				log "$REV $MODL"
				EC25AF=$(echo $MODL | grep "EC25AFF")
				if [ ! -z "$EC25AF" ]; then # EC25-AF
					MX='40000000000000381A'
				else
					MX='81a'
				fi
				M4='AT+QCFG="band",0,'$MX',0'
			;;
			"0306" )
				M1='AT+GMR'
				OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M1")
				EP06E=$(echo $OX | grep "EP06E")
				if [ ! -z $EP06E ]; then # EP06E
					M3='1a080800d5'
				else # EP06-A
					M3="2000001003300185A"
				fi
				M4='AT+QCFG="band",0,'$M3',0'
			;;
			"030b" ) # EM060
				M3="420000A7E23B0E38DF"
				M4='AT+QCFG="band",0,'$M3',0'
			;;
			"6005" ) # EM060
				M3="0x200000080080000df"
				M4='AT+QCFG="band",0,'$M3',0'
			;;
			"0512" ) # EM12-G
				EM12=$(echo $model | grep "EG18")
				if [ -z "$EM12" ]; then
					M3="2000001E0BB1F39DF"
				else # EG18
					EM12=$(echo $model | grep "EA")
					if [ -z "$EM12" ]; then # NA
						M3="4200000100330138A"
					else # EA
						M3="1A0080800C5"
					fi
				fi
				M4='AT+QCFG="band",0,'$M3',0'
			;;
			"0620" ) # EM20-G
				EM20=$(echo $model | grep "EM20")
				if [ ! -z $EM20 ]; then # EM20
					M3="20000A7E03B0F38DF"
					M4='AT+QCFG="band",0,'$M3',0'
					if [ -e /etc/qfake ]; then
						mask="42000087E2BB0F38DF"
						fibdecode $mask 1 1
						M4F='AT+QNWPREFCFG="lte_band",'$lst
						log "Fake RM500 $M4F"
					fi
					
				else # EM160
					mask="20000A7E0BB0F38DF"
					fibdecode $mask 1 1
					M4='AT+QNWPREFCFG="lte_band",'$lst
				fi
			;;
			"0800"|"0900"|"0801" )

			;;
			* )
				M3="AT"
				M4='AT+QCFG="band",0,'$M3',0'
			;;
		esac
		
		#OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M4")
		#log "$OX"
		if [ ! -z $M5 ]; then
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M5")
			log "$OX"
		fi
		sleep 5
	;;
	"1199" )
		M2='AT!LTEINFO?'
		case $uPid in

			"68c0"|"9041"|"901f" ) # MC7354 EM/MC7355
				M3="101101A"
				M3X="0"
				M4='AT!BAND=11,"Test",0,'$M3,$M3X
			;;
			"9070"|"9071"|"9078"|"9079"|"907a"|"907b" ) # EM/MC7455
				M3="100030818DF"
				M3X="0"
				M4='AT!BAND=11,"Test",0,'$M3,$M3X
				if [ -e /etc/fake ]; then
					M4='AT!BAND=11,"Test",0,A300BA0E38DF,2,0,0,0'
				fi
			;;
			"9090"|"9091"|"90b1" ) # EM7565
				EM7565=$(echo "$model" | grep "7565")
				if [ ! -z $EM7565 ]; then
					M3="A300BA0E38DF"
					M3X="2"
					M4='AT!BAND=11,"Test",0,'$M3","$M3X",0,0,0"
				else
					EM7511=$(echo "$model" | grep "7511")
					if [ ! -z $EM7511 ]; then # EM7511
						M3="A300BA0E38DF"
						M3X="2"
						M4='AT!BAND=11,"Test",0,'$M3","$M3X",0,0,0"
					else
						M3="87000300385A"
						M3X="42"
						M4='AT!BAND=11,"Test",0,'$M3","$M3X",0,0,0"
					fi
				fi

			;;
			* )
				M3="AT"
			;;
		esac
		log "Set full : $M4"
		if [ -e /etc/fake ]; then
			M4='AT!BAND=11,"Test",0,'$M3,$M3X
		fi
		M1='AT!ENTERCND="A710"'
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M1")
		log "$OX"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M4")
		log "$OX"
		M4='AT!BAND=00;!BAND=11'
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M4")
		log "$OX"
		ATCMDD='AT!ENTERCND="AWRONG"'
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	;;
	"8087"|"2cb7" )
		rm -f /tmp/scan
		echo "Cell Scanner Start ..." > /tmp/scan
		echo " " >> /tmp/scan
		if [ -e /tmp/scan$CURRMODEM ]; then
			SCX=$(cat /tmp/scan$CURRMODEM)
			echo "$SCX" >> /tmp/scan
		else
			echo "No Neighbouring cells were found" >> /tmp/scan
		fi
		echo " " >> /tmp/scan
		echo "Done" >> /tmp/scan
		exit 0
	;;
	* )
		rm -f /tmp/scanx
		echo "Scan for Neighbouring cells not supported" >> /tmp/scan
		uci set modem.pinginfo$CURRMODEM.alive=$ALIVE
		uci commit modem
		exit 0
	;;
esac

export TIMEOUT="10"
OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M2")
OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M2")
log "$OX"
ERR=$(echo "$OX" | grep "ERROR")
if [ ! -z $ERR ]; then
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M2")
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M2")
	log "$OX"
fi
if [ ! -z $ERR ]; then
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M2")
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M2")
	log "$OX"
fi
log "$OX"
echo "$OX" > /tmp/scanx
rm -f /tmp/scan
echo "Cell Scanner Start ..." > /tmp/scan
echo " " >> /tmp/scan
flg=0
while IFS= read -r line
do
	case $uVid in
	"2c7c" )
		qm=$(echo $line" " | grep "+QENG:" | tr -d '"' | tr " " ",")
		if [ "$qm" ]; then
			INT=$(echo $qm | cut -d, -f3)
			BND=$(echo $qm | cut -d, -f5)
			PCI=$(echo $qm | cut -d, -f6)
			RSSI=$(echo $qm | cut -d, -f9)
			BAND=$(/usr/bin/chan2band.sh $BND)
			if [ "$INT" = "intra" ]; then
				echo "Band : $BAND    Signal : $RSSI (dBm) EARFCN : $BND  PCI : $PCI (current)" >> /tmp/scan
			else
				echo "Band : $BAND    Signal : $RSSI (dBm) EARFCN : $BND  PCI : $PCI" >> /tmp/scan
			fi
			flg=1
		fi
	;;
	"1199" )
		qm=$(echo $line" " | grep "Serving:" | tr -d '"' | tr " " ",")
		if [ "$qm" ]; then
			read -r line
			qm=$(echo $line" " | tr -d '"' | tr " " ",")
			BND=$(echo $qm | cut -d, -f1)
			PCI=$(echo $qm | cut -d, -f10)
			BAND=$(/usr/bin/chan2band.sh $BND)
			RSSI=$(echo $qm | cut -d, -f13)
			echo "Band : $BAND    Signal : $RSSI (dBm) EARFCN : $BND  PCI : $PCI (current)" >> /tmp/scan
			flg=1
		else
			qm=$(echo $line" " | grep "InterFreq:" | tr -d '"' | tr " " ",")
			log "$line"
			if [ "$qm" ]; then
				while [ 1 = 1 ]
				do
					read -r line
					log "$line"
					qm=""
					qm=$(echo $line" " | grep ":" | tr -d '"' | tr " " ",")
					if [ "$qm" ]; then
						break
					fi
					qm=$(echo $line" " | grep "OK" | tr -d '"' | tr " " ",")
					if [ "$qm" ]; then
						break
					fi
					qm=$(echo $line" " | tr -d '"' | tr " " ",")
					if [ "$qm" = "," ]; then
						break
					fi
					BND=$(echo $qm | cut -d, -f1)
					PCI=$(echo $qm | cut -d, -f10)
					BAND=$(/usr/bin/chan2band.sh $BND)
					RSSI=$(echo $qm | cut -d, -f8)
					echo "Band : $BAND    Signal : $RSSI (dBm) EARFCN : $BND  PCI : $PCI" >> /tmp/scan
					flg=1
				done
				break
			fi
		fi
	;;
	* )
	
	;;
	esac
done < /tmp/scanx

rm -f /tmp/scanx
if [ $flg -eq 0 ]; then
	echo "No Neighbouring cells were found" >> /tmp/scan
fi
echo " " >> /tmp/scan
echo "Done" >> /tmp/scan

case $uVid in
	"2c7c" )
		if [ $uPid != "0800" ]; then
			if [ $uPid = 0620 -o $uPid = "0800" -o $uPid = "030b" ]; then
				EM20=$(echo $model | grep "EM20")
				if [ ! -z $EM20 ]; then # EM20
					M2='AT+QCFG="band",0,'$L1',0'
					if [ -e /etc/fake ]; then
						fibdecode $L1 1 1
						M2F='AT+QNWPREFCFG="lte_band",'$lst
						log "Fake EM160 Band Set "$M2F
					fi
				else
					fibdecode $L1 1 1
					M2='AT+QNWPREFCFG="lte_band",'$lst
				fi
			else
				M4='AT+QCFG="band",0,'$L1',0'
			fi
			#OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M4")
			#log "$OX"
		fi
	;;
	"1199" )
		M1='AT!ENTERCND="A710"'
		case $uPid in

			"68c0"|"9041"|"901f" ) # MC7354 EM/MC7355
				M4='AT!BAND=11,"Test",0,'$L1X,0
			;;
			"9070"|"9071"|"9078"|"9079"|"907a"|"907b" ) # EM/MC7455
				M4='AT!BAND=11,"Test",0,'$L1X,0
				if [ -e /etc/fake ]; then
					M4='AT!BAND=11,"Test",0,'$L1X','$L2',0,0,0'
				fi
			;;
			"9090"|"9091"|"90b1" )
				M4='AT!BAND=11,"Test",0,'$L1X','$L2',0,0,0'
			;;
		esac
		log "Set back : $M4"
		if [ -e /etc/fake ]; then
			M4='AT!BAND=11,"Test",0,00000100030818DF,0'
		fi
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M1")
		log "$OX"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M4")
		log "$OX"
		M4='AT!BAND=00;!BAND=11'
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M4")
		log "$OX"
		ATCMDD='AT!ENTERCND="AWRONG"'
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	;;
esac
uci set modem.pinginfo$CURRMODEM.alive=$ACTIVE
uci commit modem

log "Finished Scan"
