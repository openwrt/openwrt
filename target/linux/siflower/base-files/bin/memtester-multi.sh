#!/bin/sh
# Memory aTester Scripts

#for file in `ls "/sys/class/leds/"`;do
#	echo "default-on" > /sys/class/leds/$file/trigger
#done
# by Baif
# version = date
VERSION="Fri Oct 19 11:56:57 CST 2007"

# trap for irruptions
MEMTESTER=${PWD}/memtester
PPIDKILL=$$
SIDKILL=$$
trap "pkill -9 -P ${PPIDKILL};kill -9 $$" INT
trap "pkill -9 -P ${PPIDKILL};kill -9 $$" KILL


cat <<-EOF_vpps >&2

Version: ${VERSION}
PID: $$
PPIDKILL: ${PPIDKILL}
SIDKILL: ${PPIDKILL}

EOF_vpps

CORE_NUM=$(grep -i ^processor /proc/cpuinfo|wc -l)
MEMTESTERCOPY=${CORE_NUM}
MEM_TOTAL_K=$(awk '/^MemTotal/{print $2}' /proc/meminfo)
MEM_RESERVE_PERCENTAGE=$((1000*50/1024)) # 95%

MEM_RESERVED=$((MEM_TOTAL_K/1024*MEM_RESERVE_PERCENTAGE/1000))
MEM_TOTAL_TOBETESTED=$((MEM_TOTAL_K/1024-MEM_RESERVED))
MEM_PER_COPY=$((MEM_TOTAL_TOBETESTED/MEMTESTERCOPY))

RUN_DURATION_TIME=0
RUN_LOOPS=-1
RUN_DURATION_TIME_FLAG=0
RUN_LOOPS_FLAG=0
DDPERCOPY_TIME=6s

LOGDIR=/tmp/memtester-log-${$}
mkdir -p ${LOGDIR}
#rm /tmp/memtester-logdir
echo $LOGDIR > /tmp/memtester-logdir

#mkdir -p /judge



show_help ()
{
	cat <<-EOFshow_HELP >&2

	Version: ${VERSION}

	Usage: $(basename ${0})
	-r Directory: the root location of memtester binary file
	-c NUMBER: the copies of memtester should be run
	-m NUMBER: how many memory should be tested totally (in MB)
	-t TIME: duration mode, how long will the tests go
	-l NUMBER: loops mode,how many loops will each memtester should go

	The option -t and -l are exclusive, which means tests could work
	only with 1. duration mode or 2. loops mode

	RUN 4 copies memtester with in 24 hours, to test total 4000 MB memory:

	$(basename ${0}) -t 24h -c 4 -m 4000

	RUN 2 copies memtester with in 1 hours, to test total 4000 MB memory:

	$(basename ${0}) -t 1h -c 4 -m 4000

	RUN 4 copies memtester with in 2 loops, to test total 3600 MB memory:

	$(basename ${0}) -l 2 -c 4 -m 3600

	-V/-h/-H: show this info.

	EOFshow_HELP

	exit 0
}

while getopts :c:m:t:l:r:p:hHVvx OPTION
do
	case ${OPTION} in
		c)
			#echo "-c ${OPTARG}"
			MEMTESTERCOPY=${OPTARG}
			;;
		m)
			#echo "-m ${OPTARG} MB"
			MEM_TOTAL_TOBETESTED=${OPTARG}
			MEM_RESERVED=$((MEM_TOTAL_K/1024-MEM_TOTAL_TOBETESTED))
			;;
		t)
			#echo "-t ${OPTARG}"
			[ 0 -ne ${RUN_LOOPS_FLAG} ] && echo "-t and -l are exclusive." && exit 222
			RUN_DURATION_TIME=${OPTARG}
			RUN_DURATION_TIME_FLAG=1
			;;
		l)
			#echo "-l ${OPTARG}"
			[ 0 -ne ${RUN_DURATION_TIME_FLAG} ] && echo && echo "-t and -l are exclusive." && show_help && echo && exit 223
			RUN_LOOPS=${OPTARG};
			RUN_LOOPS_FLAG=1
			;;
		d)
			#echo "-r ${OPTARG}"
			MEMTESTER=${OPTARG}/memtester
			;;
		p)
			#echo "-p ${OPTARG}"
			MEMTESTER=${OPTARG}
			;;
		V|h|H)
			show_help
			;;
		v)
			set -v
			;;
		x)
			set -x
			;;
		?) echo "Error...";
			echo "?Unknown args..."
			exit 224
			;;
		*) #echo "*Unknown args..."
	esac
done
#exit
[ 0 -eq ${RUN_DURATION_TIME_FLAG} ] && [ 0 -eq ${RUN_LOOPS_FLAG} ] && echo && echo "Please specified which mode should we run... -t or -l" && show_help && echo && exit 225

MEM_PER_COPY=$((MEM_TOTAL_TOBETESTED/MEMTESTERCOPY))

echo "Mem total: " $((MEM_TOTAL_K/1024)) MB
echo "Core total: "${CORE_NUM}
echo "Memtester copys: " ${MEMTESTERCOPY}
echo "Mem per copy: "${MEM_PER_COPY}
echo "Mem total to used: "${MEM_TOTAL_TOBETESTED} MB
if [ ${MEM_RESERVED} -lt 1 ];
then
	echo "Mem reserved: -- No more memory reserved..."
else
	echo "Mem reserved: "${MEM_RESERVED} MB
fi
#exit


# GOGOGO
if [ 0 -ne ${RUN_DURATION_TIME_FLAG} ]; then
	echo "Run within a duration: ${RUN_DURATION_TIME}"
elif [ 0 -ne ${RUN_LOOPS_FLAG} ];
then
	echo "Run within a loop: ${RUN_LOOPS}"
fi
echo "Working directory: " $PWD
echo "Memtester: " ${MEMTESTER}
echo "LOGs directory: " $LOGDIR
echo
echo -n "Jobs started at date: "
date #+%Y/%m/%d\ %H:%M
echo
#exit

########################
# Run testing within a duration time.

sh /bin/date_output.sh &

if [ 0 -ne ${RUN_DURATION_TIME_FLAG} ]
then
	# prepareing the sleeping killers
	sleep ${RUN_DURATION_TIME}
	echo -n "End of testing(TIMEOUT)... "
	echo "KILL CHILD" && kill -9 $(pgrep -P ${PPIDKILL} memtester) && echo "Childen processes - KILLED."
	# attention to how the memtesters are forked...
echo "KILL PARENT" && kill $$ && echo "KILLED." &
	echo "Finished the memtester"
	echo -n "Jobs finished at date: "
	date #+%Y/%m/%d\ %H:%M

fi &

echo -n "Waiting (PID: $$) for ${MEMTESTERCOPY} memtesters(${MEM_PER_COPY}MB for each). "
if [ 0 -ne ${RUN_DURATION_TIME_FLAG} ];
then
	echo -n "For time: ${RUN_DURATION_TIME} "
fi
if [ 0 -ne ${RUN_LOOPS_FLAG} ];
then
	echo -n "For loops: ${RUN_LOOPS} "
fi
echo "..."



#kernel panic
while true
do
	MEMTESTER_NUM=0

	echo -n "{"
	while [ ${MEMTESTER_NUM} -lt ${MEMTESTERCOPY} ]
	do
		echo -n " ${MEMTESTER_NUM} "

		if [ 0 -ne ${RUN_DURATION_TIME_FLAG} ]
		then
			RUN_LOOPS=0
		fi

		${MEMTESTER} ${MEM_PER_COPY} ${RUN_LOOPS} 2>&1 >> ${LOGDIR}/${MEMTESTER_NUM}.log &
		# set loops = 0 to make memtester run loop infinitely...
		# .pogo version will run only one loop by default

		sleep ${DDPERCOPY_TIME}
		MEMTESTER_NUM=$(expr $MEMTESTER_NUM + 1)
	done
	echo -n "}"
	wait

	[ 0 -ne ${RUN_LOOPS_FLAG} ] && break
	# memtesters' loops...
done

########################

echo
echo -n "End of testing(Excution ended)... "
killall -9 -P ${PPIDKILL}
kill $$
echo "Finished the memtester"


echo -n "Jobs finished at date: "
date #+%Y/%m/%d\ %H:%M
