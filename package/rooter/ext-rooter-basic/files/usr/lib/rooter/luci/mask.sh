#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	modlog "BandMasking $CURRMODEM" "$@"
}

#
# remove for band locking
#
enb=$(uci -q get custom.bandlock.enabled)
if [ $enb == "0" ]; then
	exit 0
fi

reverse() {
	LX=$1
	length=${#LX}
	jx="${LX:2:length-2}"
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
	revstr=$str"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
}

rm -f /tmp/bmask
CURRMODEM=$(uci get modem.general.miscnum)
CPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)
uVid=$(uci get modem.modem$CURRMODEM.uVid)
uPid=$(uci get modem.modem$CURRMODEM.uPid)
ATCMDD="AT+CGMM"
model=$($ROOTER/gcom/gcom-locked "$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
L1=$(uci -q get modem.modem$CURRMODEM.L1)
L5=$(uci -q get modem.modem$CURRMODEM.L5)
L6=$(uci -q get modem.modem$CURRMODEM.L6)

if [ ! $L1 ]; then
	exit 0
fi

CA3=""
M5="x"
M6="x"
case $uVid in
	"2c7c" )
		case $uPid in
			"0125" ) # EC25
				#EUX EC25EUXGAR	B1/B3/B7/B8/B20/B28A/B38/B40/B41
				#EU EC25EUGAR	B1/B3/B7/B8/B20/B28A/B38/B40/B41
				#EC EC25ECGAR
				#E EC25EFAR		B1/B3/B5/B7/B8/B20/B38/B40/B41
				#AU EC25AUGCR
				#AF-FD EC25AFFDR	B2/B4/B5/B12/B13/B14/B66/B71 
				#AF EC25AFFAR		B2/B4/B5/B12/B13/B14/B66/B71 
				#A EC25AFAR
				CA=""
				M1='ATI'
				OX=$($ROOTER/gcom/gcom-locked "$CPORT" "run-at.gcom" "$CURRMODEM" "$M1")
				REV=$(echo $OX" " | grep -o "Revision: .\+ OK " | tr " " ",")
				MODL=$(echo $REV | cut -d, -f2)
				EC25AF=$(echo $MODL | grep "EC25AFF")
				if [ ! -z "$EC25AF" ]; then # EC25-AF
					M2='01011000000111000000000000000000000000000000000000000000000000000100001'
				else
					EC25AF=$(echo $MODL | grep "EC25E")
					if [ ! -z "$EC25AF" ]; then # EC25-E
						M2='1010101100000000000100000000000000000101100'
					else
						EC25AF=$(echo $MODL | grep "EC25AU")
						if [ ! -z "$EC25AF" ]; then # EC25-AU
							M2='111110110000000000000000000100000000000100'
						else # EC25-A
							M2='01010000000100'
						fi
					fi
				fi
			;;
			"0306" ) # EP06-A
				M1='AT+GMR'
				OX=$($ROOTER/gcom/gcom-locked "$CPORT" "run-at.gcom" "$CURRMODEM" "$M1")
				EP06E=$(echo $OX | grep "EP06E")
				if [ ! -z "$EP06E" ]; then # EP06E
					M2='101010110000000000010000000100010000010110'
					CA="ep06e-bands"
				else # EP06A
					M2='010110100001100010000000110011000000000010000000000000000000000001'
					CA="ep06a-bands"
				fi
			;;
			"030b" ) # EM060
				M2='111110110001110001110000110111000100011111100101000000000000000001000010'
				CA="em060-2xbands"
				CA3=""
			;;
			"0512" ) # EM12-G
				EM12=$(echo $model | grep "EG18")
				if [ -z "$EM12" ]; then
					M2='111110111001110011111000110111010000011110000000000000000000000001'
					CA="em12-2xbands"
					CA3="em12-3xbands"
				else # EG18
					EM12=$(echo $model | grep "EA")
					if [ -z "$EM12" ]; then # NA
						M2='01011001000111001000000011001100000000001000000000000000000000000000001000010'
					else # EA
						M2='101010110000000000010000000100000000010110'
					fi
					CA=""
					CA3=""
				fi
			;;
			"0620" ) # EM20-G
				EM20=$(echo $model | grep "EM20")
				if [ ! -z "$EM20" ]; then
					M2='111110110001110011110000110111000000011111100101000000000000000001'
					CA="em20-2xbands"
					CA3="em20-3xbands"
					CA4="em20-4xbands"
					if [ -e /etc/qfake ]; then
						M2='1111101100011100011100001101110101000111111000010000000000000000010000100'
						M5='0000000000000000000000000000000000000100100000000000000000000000000000000000111'
						M6='1110101100010000000100001001000000000101100000010000000000000000010000100000111'
						$ROOTER/luci/celltype.sh $CURRMODEM
						NET=$(uci -q get modem.modem$CURRMODEM.netmode)
						if [ $NET != "7" ]; then
							M5=$M6 # SA mode
							L5=$L6
						fi
						CA=""
						CA3=""
						CA4=""
					fi

				else
					M2='111110110001110011110000110111010000011111100101000000000000000001'
					CA="em20-2xbands"
					CA3="em20-3xbands"
					CA4="em20-4xbands"
				fi
			;;
			"0801" ) #RM520
				GL=$(echo $model | grep "GL")
				if [ ! -z "$GL" ]; then #RM520N-GL
					M2='1111101100011100111100001101110101000111111001010000000000000000010000100'
					M5='11101011000111000101000011011100000001011000000100000000000000000100011000111110'
					M6=$M5
					CA=""
					CA3=""
				else #RM520N-EU
					M2='1010101100000000000100000001000100000101111001000000000000000000000000100'
					M5='101010110000000000010000000100000000010110000000000000000000000000000010001111'
					M6=$M5
					CA=""
					CA3=""
				fi
			;;
			"0900" )
				M2='111010110000000000010000000100000100011110'
				M5='1000000000000000000000000001000000000000100000000000000000000000000000000000111'
				M6=$M5
				CA=""
				CA3=""
			;;
			"0800") # RM500
				f2=$(echo $model | grep "500")
				if [ -z "$f2" ]; then #RM502/505/510
					M2='1111101100011100111100001101110101000111111001010000000000000000010000100'
					M5='1110101100010100000100001001000000000101100000010000000000000000010000100000111'
					M6=$M5
				else
					GL=$(echo $model | grep "GL")
					if [ ! -z "$GL" ]; then #RM500-GL
						M2='1111101100011100111100001101110101000111111001010000000000000000010000100'
						M5='0000000000000000000000000000000000000000100000000000000000000000000000000000111'
						M6='1110101100010000000100001001000000000101100000010000000000000000010000100000111'
					else # RM500-AE
						M2='1111101100011100111100001101110101000111111001010000000000000000010000100'
						M5='11101011000100000001000010010000000001011000000100000000000000000100001000001110'
						M6=$M5
					fi
				fi
				CA=""
				CA3=""
			;;
		esac
	;;
	"1199" )
		case $uPid in
			"68a2" ) # MC7700
				M2='1001000000000000100000000'
				CA=""
			;;
			"68c0"|"9041"|"901f" ) # MC7354 EM/MC7355
				M2='0101100000001000100000001'
				CA=""
			;;
			"9070"|"9071"|"9078"|"9079"|"907a"|"907b" ) # EM/MC7455
				M2='11111011000110000001000011000100000000001'
				if [ -e /etc/fake ]; then
					M2='1111101100011100011100000100110100000000110001010000000000000000010'
				fi
				CA="mc7455-bands"
			;;
			"9090"|"9091"|"90b1" )
				EM7565=$(echo "$model" | grep "7565")
				if [ ! -z "$EM7565" ]; then # EM7565
					M2='111110111001100001110000010111010000000011100101000000000000000001'
					CA="em7565-2xbands"
					CA3="em7565-3xbands"
				else
					EM7511=$(echo "$model" | grep "7511")
					if [ ! -z "$EM7511" ]; then # EM7511
						M2='1111101100011100011100000100110100000000110001010000000000000000010'
						CA="em7511-2xbands"
						CA3="em7511-3xbands"
					else # EM7411
						M2='0101101000011100000000001100000000000000111000010000000000000000010000100'
						CA="em7411-2xbands"
						CA3="em7411-3xbands"
					fi
				fi
			;;
		esac
	;;
	"8087" )
		M2='111110110001100011110000010111000000011110000000000000000000000001'
		CA="l850-2xbands"
		CA3="l850-3xbands"
	;;
	"2cb7" )
		FM150=$(echo "$model" | grep "FM150")
		if [ -z "$FM150" ]; then
			M2='111110110001100011110000010111000000011110000000000000000000000001'
			CA="l850-2xbands"
			CA3="l850-3xbands"
		else
			M2='01011000000100000000000010001100000000000000000000000000000000000100001'
			M5='00001000000100000000000000000000000000001000000000000000000000000100001'
			CA=""
			CA3=""
		fi
	;;
	"413c" )
		case $uPid in

			"81d7"|"81d8"|"e0b4" |"e0b5"|"1910")
				M2='11111011000111001111000011011101000001111110010100000000000000000100'
				CA=""
				CA3=""
			;;
		esac
	;;
	"1bc7" )
		case $uPid in

			"1040"|"1041")
				L1X=$(uci -q get modem.modem$CURRMODEM.L1X)
				M2=$L1X
				CA=""
				CA3=""
			;;
		esac
	;;
	* )
		exit 0
	;;
esac

reverse $L1
echo $revstr > /tmp/bmask1
if [ ! -z $L5 ]; then
	reverse $L5
else
	revstr="x"
fi
echo $revstr >> /tmp/bmask1
if [ ! -z $L6 ]; then
	reverse $L6
else
	revstr="x"
fi
echo $revstr >> /tmp/bmask1
echo $M2 >> /tmp/bmask1
echo $M5 >> /tmp/bmask1
if [ -z "$M6" ]; then
	M6="x"
fi
echo $M6 >> /tmp/bmask1
if [ $CA ]; then
	echo $CA >> /tmp/bmask1
	if [ $CA3 ]; then
		echo $CA3 >> /tmp/bmask1
		if [ $CA4 ]; then
			echo $CA4 >> /tmp/bmask1
		fi
	fi
fi
mv /tmp/bmask1 /tmp/bmask

