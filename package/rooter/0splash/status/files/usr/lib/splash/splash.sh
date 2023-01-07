#!/bin/sh

log() {
	logger -t "Splash Screen : " "$@"
}

sleep 5 

ENB=$(uci get splash.settings.enabled)
FULL=$(uci get splash.settings.full)

if [ $ENB = "0" ]; then
	rm -f /www/splash_files/check.gif
	rm -f /www/splash_files/full.gif
else
	cp /usr/lib/splash/check.gif /www/splash_files
	rm -f /www/splash_files/full.gif
	if [ $FULL = "1" ]; then
		cp /usr/lib/splash/full.gif /www/splash_files
	fi
fi