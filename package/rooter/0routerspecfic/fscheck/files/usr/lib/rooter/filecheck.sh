#!/bin/sh 

rm -f /tmp/fsresult
rm -f /tmp/fsro
if [ -e /dev/mmcblk0p2 ]; then
	E2=$(e2fsck -y -v -f /dev/mmcblk0p2 2> /tmp/fsresult)
	echo "$E2" >> /tmp/fsresult
	E3=$(cat /tmp/fsresult | grep -o "aborting")
	if [ ! -z $E3 ]; then
		echo " " > /tmp/fsresult
		echo " " >> /tmp/fsresult
		echo " File System mounted as Read/Write" >> /tmp/fsresult
		exit 0
	fi
	echo "1" >> /tmp/fsro
else
	echo "Not correct file system" > /tmp/fsresult
fi

