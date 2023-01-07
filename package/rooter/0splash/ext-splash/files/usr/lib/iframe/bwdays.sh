#!/bin/sh 
. /lib/functions.sh

genline() {
	MONLIST=$MONLIST"<tr>"
		t1="<td><div align=/"center/"><strong> $START</strong></div></td>"
		t2="<td><div align=/"center/"><strong> $updata</strong></div></td>"
		t3="<td><div align=/"center/"><strong> $downdata</strong></div></td>"
		t4="<td><div align=/"center/"><strong> $totaldata</strong></div></td>"
	MONLIST=$MONLIST$t1$t2$t3$t4"</tr>"
}

bwdata() {
	START="-"
	END="-"
	header=0
	while IFS= read -r line; do
		if [ $header -eq 0 ]; then
			days=$line
			read -r line
			DOWN=$line
			read -r line
			UP=$line
			read -r line
			TOTAL=$line
			read -r line
			line=$(echo $line" " | tr "|" ",")
			END=$(echo $line | cut -d, -f1)
			START=$END
			updata=$(echo $line | cut -d, -f2)
			downdata=$(echo $line | cut -d, -f3)
			totaldata=$(echo $line | cut -d, -f4)
			genline
			read -r line
			header=1
			if [ -z "$line" ]; then
				break
			fi
		fi
		line=$(echo $line" " | tr "|" ",")
		START=$(echo $line | cut -d, -f1)
		updata=$(echo $line | cut -d, -f2)
		downdata=$(echo $line | cut -d, -f3)
		totaldata=$(echo $line | cut -d, -f4)
		genline

	done < /usr/lib/bwmon/data/monthly.data
}

currdata() {
	while IFS= read -r line; do
		if [ $line = '0' ]; then
			break
		else
			cdays=$line
			read -r line
			read -r line
			ctused=$line
			read -r line
			read -r line
			ctdwn=$line
			read -r line
			read -r line
			ctup=$line
			read -r line
			read -r line
			cproject=$line
			break
		fi
	done < /tmp/bwdata
}

	STEMP="/tmp/www/dtemp.html"
	STATUS="/usr/lib/iframe/bwtemplate.html"
	IFSTATUS="/tmp/www/daylist.html"
	
	MONLIST=""
	
	rm -f $STEMP
	cp $STATUS $STEMP
	if [ -e /usr/lib/bwmon/data/monthly.data ]; then
		#MONLIST="<table width=\"700\" border=\"1\" >"
		bwdata
		MONLIST=$MONLIST"</table>"
	else
		START="-"
		END="-"
		TOTAL="-"
		DOWN="-"
		UP="-"
	fi
	
	currdata
	
	sed -i -e "s!#START#!$START!g" $STEMP
	sed -i -e "s!#END#!$END!g" $STEMP
	sed -i -e "s!#TOTAL#!$TOTAL!g" $STEMP
	sed -i -e "s!#DOWN#!$DOWN!g" $STEMP
	sed -i -e "s!#UP#!$UP!g" $STEMP
	sed -i -e "s!#MONLIST#!$MONLIST!g" $STEMP
	
	sed -i -e "s!#CDAYS#!$cdays!g" $STEMP
	sed -i -e "s!#CTOTAL#!$ctused!g" $STEMP
	sed -i -e "s!#CDOWN#!$ctdwn!g" $STEMP
	sed -i -e "s!#CUP#!$ctup!g" $STEMP
	sed -i -e "s!#PROJECT#!$cproject!g" $STEMP
	
	mv $STEMP $IFSTATUS

