#!/bin/sh

ssh root@47.96.30.235 "rm -rf /var/www/html/gateboard-one/repo/openwrt-21.02.5"
scp -r ./bin/targets/ramips/mt7621 root@47.96.30.235:/var/www/html/gateboard-one/repo/openwrt-21.02.5
