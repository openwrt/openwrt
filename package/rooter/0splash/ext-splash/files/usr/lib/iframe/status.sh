#!/bin/sh

/usr/lib/rooter/signal/status.sh 1 "No Modem Present"

while [ true ]
do
	result=`ps | grep -i "stupdate.sh" | grep -v "grep" | wc -l`
	if [ $result -lt 1 ]; then
		/usr/lib/iframe/stupdate.sh
		/usr/lib/iframe/bwdays.sh
	fi
	splash=$(uci -q get iframe.iframe.splashpage)
	if [ $splash = "1" ]; then
		mv /www/splash_files/check1.gif /www/splash_files/check.gif
		ln -s /tmp/www/splash.html /www/splash.html
	else
		mv /www/splash_files/check.gif /www/splash_files/check1.gif
		rm -f /www/splash.html
	fi
	sleep 10
done