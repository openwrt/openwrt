#!/bin/sh

ROOTER=/usr/lib/rooter

TEXT=$1
DATE=$(date +%c)

modlog="/tmp/wifilog.log"
tmplog="/tmp/twifilog"

echo "$DATE : $TEXT" >> $modlog
lua $ROOTER/log/mrotate.lua $modlog $tmplog
mv $tmplog $modlog

