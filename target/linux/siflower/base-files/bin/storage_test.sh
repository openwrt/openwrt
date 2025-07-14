#!/bin/sh


LOG_FILE=/tmp/storage_test.log

#rand(){  
#    min=$1  
#    max=$(($2-$min+1))
#    num=$(cat /proc/sys/kernel/random/uuid | cksum | awk -F ' ' '{print $1}')
#    echo $(($num%$max+$min))
#}

rand(){
        min=$1
        max=$(($2-$min+1))
        num=$(cat /proc/sys/kernel/random/uuid | sed "s/[a-zA-Z]//g" | awk -F '-' '{printf"%d\n",$1+$4}')
        echo $((num%max+min))
}

rand_filename(){
        length=$(rand $1 $2)
        head /dev/urandom | tr -dc a-z0-9 > /tmp/pre.log
        prefix=$(head -c $length /tmp/pre.log)

        length=$(rand $3 $4)
        head /dev/urandom | tr -dc a-z0-9 > /tmp/suf.log
        suffix=$(head -c $length /tmp/suf.log)

        temp_filename=$prefix.$suffix

        echo $temp_filename
}

#generate a random path and creat it, $1 is the path's prefix
#max path deepth is 20
gen_random_path(){
        path=$1
        path_depth=$(rand 0 20)
        i=0

        while [ $((i)) -lt $((path_depth)) ]; do
                randname=$(rand_filename 1 10 1 10)
                path=${path}/$randname
                i=$(($i+1))
        done
        if [ ! -d $path ]; then
                mkdir -p $path
        fi
        echo $path
}


do_float_div(){
	awk -v a=$1 -v b=$2 'BEGIN {print a/b}'
}

do_one_test(){
	file=$(rand_filename 1 10 1 10)
	src=$(gen_random_path $1)/$file
	dst=$(gen_random_path $2)/$file
	TMP=/tmp/tmp-${$}
	file_len_KB=$(rand 0 $(($3-1)))
	file_len_B=$(rand 1 200)
	CMD=cp

	dd if=/dev/urandom of=${src} bs=1k count=${file_len_KB} > /dev/null 2>&1
	dd if=/dev/urandom of=${TMP} bs=${file_len_B} count=1 > /dev/null 2>&1
	dd if=${TMP} of=${src} seek=${file_len_KB} obs=1k > /dev/null 2>&1
	rm ${TMP}
	echo "$(date "+%H:%M:%S")  $CMD $((file_len_KB*200+file_len_B)) bytes data..." >> $LOG_FILE

	o_md5=$(md5sum $src | awk '{print $1}')
	origin_md5=$(cut -f 1 $src)
	$CMD $src $dst >> $LOG_FILE
	sync
	n_md5=$(md5sum $dst | awk '{print $1}')
	new_md5=$(cut -f 1 $dst)
	if [ "$new_md5" != "$origin_md5" ]; then
		echo "transmission error:" >> $LOG_FILE
		echo "origin_md5=$origin_md5" >> $LOG_FILE
		echo "new_md5=$new_md5" >> $LOG_FILE
		echo "cmp: $(cmp $src $dst)" >> $LOG_FILE
		return 1
	fi
	echo "$(date "+%H:%M:%S")  success" >> $LOG_FILE
	rm -rf ${1} ${2}
	return 0
}

get_current_time(){
	echo $(cat /proc/uptime | awk '{print $1}')
}

do_speed_test(){
	start_time=$(get_current_time)
	dd if=$1 of=$2 count=200 bs=1M $3
	end_time=$(get_current_time)
	awk -v a=$start_time -v b=$end_time 'BEGIN {print 200/(b-a)}'
}

SPEED_LOG=/storate_speed.log

speed_test(){
	write_speed=$(do_speed_test /dev/zero ${1}/test.usb conv=fsync)
	echo "write speed = $write_speed MB/s" >> $SPEED_LOG
	read_speed=$(do_speed_test ${1}/200M.bin /dev/null)
	echo "read speed = $read_speed MB/s" >> $SPEED_LOG
	rm ${1}/test.usb
	return 0
}

keep_transfering(){
	if [ ! -d $1 ]; then
		mkdir -p $1
	fi
	free_space=$(df -m $1 | awk 'NR==2 {print $4}')
	if [ $free_space -gt 200 ]; then
		free_space=200
	fi
	start_time=$(date +%s)
	now=$(date +%s)
	while [ $((${now}-${start_time})) -lt $2 ]
	do
		dd if=/dev/zero of=$1/test.bin bs=1M count=$free_space conv=fsync > /dev/null 2>&1
		dd if=$1/test.bin of=/dev/zero bs=1M count=$free_space > /dev/null 2>&1
		now=$(date +%s)
	done
	rm $1/test.bin
}

# $1: origin total bytes transfered
# $2: total bytes transfered after $3 seconds
# $3: delta seconds
# $4: total time
present_speed(){
	bytes=$((${2}-${1}))
	# calculate speed (Bytes/s)
	speed=$((${bytes}/${3}))

	if [ $speed -gt 1048576 ]
	then
		speed="$(do_float_div ${speed} 1048576) MB/s"
	elif [ $speed -gt 1024 ]; then
		speed="$(do_float_div ${speed} 1024) KB/s"
	else
		speed="$speed B/s"
	fi
	echo "$((${4}-1)) - $4 sec: ${speed}"
}

#$1: spi-flash or USB
#$2: specifies the amount of time in seconds between each report
#$3: determines the number of reports generated at interval seconds apart,
#    infinite if not specified.
show_speed(){
	#echo $(get_current_time)
	i=0
	case $1 in
	spi)
		statistic=/sys/devices/10000000.palmbus/18200000.spi/bytes
		;;
	usb)
		statistic=/sys/kernel/debug/*.usb/statistic
		;;
	*)
		echo "unknown device $1"
		exit
		;;
	esac
	while [ "$3" == "" ] || [ $i -lt $3 ]
	do
		start=$(cat ${statistic})
		sleep $2
		end=$(cat ${statistic})
		i=$((${i}+1))
		present_speed start end $2 $i &
	done
	#echo $(get_current_time)
}

case $2 in
start)
	if [ $# -lt 6 ]; then
		echo "too less parameters!!!"
	else
		echo "start storage_test"
	fi
	#keep_transfering $3 1002 &
	#show_speed $6 1 1000

	#mtd_block_test
	echo "mtd block test error" > $LOG_FILE
	timer=10;
	while [ $timer -gt 0 ];
	do
		timer=$(($timer - 1))
		echo "~~~~~~~~~~~~test $timer";
		printf "\x12\x34\x56\x78"| dd of=/dev/mtdblock3 bs=1 count=4 seek=32
	done
	rm $LOG_FILE
	#-------------------------

	echo "$$" > /tmp/storage_test_pid
	index=0
	while [ $index -lt 10 ]
	do
		do_one_test $3 $4 $5
		if [ $? -ne 0 ]; then
			echo "stop test" >> $LOG_FILE
			break
		fi
		#index=$(($index+1))
	done
;;
speed)
	speed_test $3
	if [ $? -ne 0 ]; then
		echo "error: speed test failed" >> $LOG_FILE
	else
		echo "success: speed test success" >> $LOG_FILE
	fi
;;
get_speed)
	speed_result=$(cat ${SPEED_LOG})
	echo "$speed_result"
;;
check)
	errors=$(grep "error" $LOG_FILE)
	if [ "$errors" != "" ]; then
		echo "storage test fail" >> /dev/console
	else
		echo "storage test success" >> /dev/console
	fi
	
;;

stop)
	rm -r /src /dst
	pid=$(cat /tmp/storage_test_pid)
	echo "kill $pid"
	kill $pid
;;
esac

errors=$(grep "error" $LOG_FILE)
if [ "$errors" != "" ]; then
	echo "storage test fail"
else
	echo "storage test success"
fi

if [ "$2" == "start" ];then
	pid=$(cat /tmp/storage_test_pid)
	echo "kill $pid"
	kill $pid
fi

