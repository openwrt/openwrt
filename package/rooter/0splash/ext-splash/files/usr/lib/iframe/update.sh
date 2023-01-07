#!/bin/sh 
. /lib/functions.sh

log() {
	logger -t "Status Update" "$@"
}

levelsper="101,85,70,55,40,25,10,1,0"
namesper="Perfect,Excellent,Good,Medium,Low,Bad,Dead"
levelsrssi="113,119,100,90,70,1,0"
namesrssi="None,Bad,Poor,Medium,High,Perfect"
levelsrscp="140,136,112,100,90,70,50,1,0"
namesrscp='None,None (3G) : Poor (4G),Weak (3G) : Medium (4G),Poor (3G) : Good (4G),Medium (3G) : High (4G),High (3G) :High (4G)'

level2txt() {
	tmp="$1"
	key=$2
	front=""
	tmp1="$tmp"" "
	if [ "$tmp" = "-" ]; then
		namev="<b class='level_2'>""--""</b>"
		return
	fi
	if [ $key = "per" ]; then
		tmp=$(echo "$tmp" | sed -e "s/%//g")
		level=$levelsper
		namev=$namesper
	fi
	if [ $key = "rssi" ]; then
		front="-"
		tmp=$(echo "$tmp" | sed -e "s/-//g")
		tmp=$(echo "$tmp" | sed -e "s/dBm//g")
		tmp1="$tmp"" "
		level=$levelsrssi
		namev=$namesrssi
	fi
	if [ $key = "rscp" ]; then
		front=""
		tmp=$(echo "$tmp" | sed -e "s/dBm//g")
		tmp=$(echo "$tmp" | sed -e "s/([^)]*)//g")
		tmp1="$tmp"" "
		tmp=$(echo "$tmp" | sed -e "s/-//g")
		tmp=$(echo "$tmp" | tr " " "," | cut -d, -f1 )
		tmp=$(printf %.0f "$tmp")
		level=$levelsrscp
		namev=$namesrscp
	fi
	
	if [ $key = "single" ]; then
		desc=""
		tmp=$(echo "$tmp" | sed -e "s/dBm//g")
		tmp=$(echo "$tmp" | sed -e "s/dB//g")
		if [ $3 != "1" ];then
			tmp=$(echo "$tmp" | sed -e "s/-//g")
		fi
		if [ $3 = "1" -o $3 = "0" ];then
			desc="<br><i class='msDesc'>"."</i></br>"
		fi
		namev="<b class='level_2'>""$tmp""</b>"$desc
		return
	fi
	
	namez=$namev
	cindex=1
	nindex=0
	namev="-"

	while [ true ]
	do
		levelv=$(echo "$level" | cut -d, -f$cindex)
		if [ $levelv = "0" ]; then
			namev="-"
			break
		fi
		if [ "$tmp" -ge "$levelv" ]; then
			namev=$(echo "$namez" | cut -d, -f$nindex)
			break
		fi
		cindex=$((${cindex}+1))
		nindex=$((${nindex}+1))
	done

	css="level_"$nindex
	desc="<br><i class='msDesc'>"$namev"</i></br>"
	namev="<b class='"$css"'>""$front$tmp1""</b>"$desc
}

readstatus() {
modd=$1
	while IFS= read -r line; do
		port="$line"
		read -r line
		csq="$line"
		read -r line
		per="$line"
		read -r line
		rssi="$line"
		read -r line
		modem="$line"
		read -r line
		cops="$line"
		read -r line
		mode="$line"
		read -r line
		lac="$line"
		read -r line
		lacn="$line"
		read -r line
		cid="$line"

		read -r line
		cidn="$line"
		read -r line
		mcc="$line"
		read -r line
		mnc="$line"
		read -r line
		rnc="$line"
		read -r line
		rncn="$line"
		read -r line
		down="$line"
		read -r line
		up="$line"
		read -r line
		ecio="$line"
		read -r line
		rscp="$line"
		read -r line
		ecio1="$line"

		read -r line
		rscp1="$line"
		read -r line
		netmode="$line"
		read -r line
		cell="$line"
		read -r line
		modtype="$line"
		read -r line
		conntype="$line"
		read -r line
		channel="$line"
		read -r line
		phone="$line"
		read -r line
		read -r line
		lband="$line"
		read -r line
		tempur="$line"

		read -r line
		proto="$line"
		read -r line
		pci="$line"
		read -r line
		sinr="$line"
		break
	done < /tmp/status$modd.file
}
bwdata() {
	while IFS= read -r line; do
		if [ $line = '0' ]; then
			nodata="1"
			break
		else
			nodata="0"
			days=$line
			read -r line
			read -r line
			tused=$line
			read -r line
			read -r line
			tdwn=$line
			read -r line
			read -r line
			tup=$line
			read -r line
			read -r line
			project=$line
			break
		fi
	done < /tmp/bwdata
}

logtype=$(uci -q get iframe.login.logtype)
if [ $logtype = "1" ]; then
	STEMP="/tmp/www/itemp.html"
	STATUS="/usr/lib/iframe/mframe.html"
	IFSTATUS="/tmp/www/display.html"
	
	rm -f $STEMP
	cp $STATUS $STEMP
	readstatus 1
	level2txt "$csq" "single" 0
	sed -i -e "s!#CSQ#!$namev!g" $STEMP
	level2txt "$per" "per"
	sed -i -e "s!#PER#!$namev!g" $STEMP
	level2txt "$rssi" "rssi"
	sed -i -e "s!#RSSI#!$namev!g" $STEMP
	level2txt "$rscp" "rscp"
	sed -i -e "s!#RSCP#!$namev!g" $STEMP
	level2txt "$ecio" "single" 1
	sed -i -e "s!#RSRQ#!$namev!g" $STEMP
	level2txt "$sinr" "single" 1
	sed -i -e "s!#SINR#!$namev!g" $STEMP

	level2txt "$mode" "single"
	sed -i -e "s!#MODE#!$namev!g" $STEMP
	level2txt "$mcc" "single"
	sed -i -e "s!#MCC#!$namev!g" $STEMP
	level2txt "$mnc" "single"
	sed -i -e "s!#MNC#!$namev!g" $STEMP
	level2txt "$rnc" "single"
	sed -i -e "s!#RNC#!$namev!g" $STEMP
	level2txt "$rncn" "single"
	sed -i -e "s!#RNCN#!$namev!g" $STEMP
	level2txt "$lac" "single"
	sed -i -e "s!#LAC#!$namev!g" $STEMP
	level2txt "$lacn" "single"
	sed -i -e "s!#LACN#!$namev!g" $STEMP
	level2txt "$pci" "single"
	sed -i -e "s!#CELLID#!$namev!g" $STEMP
	level2txt "$channel" "single"
	sed -i -e "s!#CHAN#!$namev!g" $STEMP
	level2txt "$lband" "single"
	sed -i -e "s!#BAND#!$namev!g" $STEMP

	if [ -e /etc/custom ]; then
		mod="/etc/custom"
	else
		mod="/tmp/sysinfo/model"
	fi
	while IFS= read -r line; do
		ROUTER=$line
		break
	done < $mod
	level2txt "$ROUTER" "single"
	sed -i -e "s!#ROUTER#!$namev!g" $STEMP
	level2txt "$modem" "single"
	sed -i -e "s!#MODEM#!$namev!g" $STEMP
	level2txt "$cops" "single"
	namev=$(echo "$namev" | tr -d '&')
	sed -i -e "s!#PROVIDER#!$namev!g" $STEMP
	level2txt "$proto" "single"
	sed -i -e "s!#PROTO#!$namev!g" $STEMP
	level2txt "$port" "single"
	sed -i -e "s!#PORT#!$namev!g" $STEMP
	level2txt "$tempur" "single"
	sed -i -e "s!#TEMP#!$namev!g" $STEMP
	
	mv $STEMP $IFSTATUS
fi

if [ $logtype = "2" ]; then
	STEMP="/tmp/www/itemp.html"
	STATUS="/usr/lib/iframe/image.html"
	IFSTATUS="/tmp/www/display.html"
	
	rm -f $STEMP
	cp $STATUS $STEMP
	logimage=$(uci -q get iframe.login.logimage)
	sed -i -e "s!#IMAGE#!$logimage!g" $STEMP
	logimagewidth=$(uci -q get iframe.login.logimagewidth)
	sed -i -e "s!#WIDTH#!$logimagewidth!g" $STEMP
	
	mv $STEMP $IFSTATUS
fi

if [ $logtype = "4" ]; then
	STEMP="/tmp/www/itemp.html"
	STATUS="/usr/lib/iframe/speed.html"
	IFSTATUS="/tmp/www/display.html"
	
	rm -f $STEMP
	cp $STATUS $STEMP
	
	mv $STEMP $IFSTATUS
fi

if [ $logtype = "3" ]; then
	STEMP="/tmp/www/itemp.html"
	STATUS="/usr/lib/iframe/zerotier.html"
	IFSTATUS="/tmp/www/display.html"
	
	rm -f $STEMP
	cp $STATUS $STEMP
	
	ID=$(uci -q get zerotier.zerotier.secret)
	if [ -z $ID ]; then
		ID="xxxxxxxxxx"
	else
		ID=${ID:0:10}
	fi
	
	sed -i -e "s!#ID#!$ID!g" $STEMP
	source /etc/codename
	sed -i -e "s!#VER#!$CODENAME!g" $STEMP
		
	mv $STEMP $IFSTATUS
fi