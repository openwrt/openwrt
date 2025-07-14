#!/bin/sh

echo 0 > /tmp/result.txt


dst=`cat /tmp/memtester-logdir`
echo "dst is $dst" >> /dev/conosle

while [ 1 ]
do 
	cat /proc/uptime >> /dev/console
	sleep 60

	grep -i fail /tmp/memtester-log-*/*.log > /dev/console 2>&1

	if [ $? -ne 0 ];then
		echo "not find fail in test log !" >> /dev/console
	else
		echo "find fail in test!!!" >> /bin/fail_log.txt
		break
	fi

	devmem 0x19d00304 32 1
	sleep 1
	devmem 0x19d00304 32 0
	sleep 1
	devmem 0x19d00304 32 1
	sleep 1
	devmem 0x19d00304 32 0
	sleep 1
	devmem 0x19d00304 32 1
	sleep 1
	devmem 0x19d00304 32 0
	sleep 1

	echo 1 > $dst/0.log
	echo 1 > $dst/1.log
	echo 1 > $dst/2.log
	echo 1 > $dst/3.log
done
