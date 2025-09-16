#!/bin/ash

PROGRAM="RM520N_CELLSCAN"

lockfile=/tmp/cellscanlock

if [ -e ${lockfile} ]; then
    if kill -9 `cat ${lockfile}`; then
        echo "Cell scanning is already Kill it."
        rm -f ${lockfile}
    else
        echo "Removing stale lock file."
        rm -f ${lockfile}
    fi
fi

echo $$ > ${lockfile}
pid=`cat ${lockfile}`
>/tmp/cellinfo
echo "正在扫描基站...(指示灯会熄灭,请勿惊扰,请等待2分钟左右)"
echo -e 'at+qscan=3,0\r\n' >/dev/ttyUSB2

timeout 180 cat /dev/ttyUSB2 | while read line
do
    case "$line" in "+QSCAN"*)
        operatorCode=$(echo $line | awk -F ',' '{print $2$3}')
        case "$operatorCode" in
            "46000" | "46002" | "46007" | "46008" | "46020")
                operator="中国移动"
                ;;
            "46001" | "46006" | "46009")
                operator="中国联通"
                ;;
            "46003" | "46005" | "46011")
                operator="中国电信"
                ;;
            "46015")
                operator="中国广电"
                ;;
            *)
                operator="未知运营商"
                ;;
        esac
        echo $line | awk -F ',' -v operator="$operator" '{printf("Mode:%s Operator:%s earfcn:%s pci:%s signal:%s\n", $1, operator, $4, $5, $6)}' >> /tmp/cellinfo
    esac
    case "$line" in *"OK"*)
        echo "基站扫描完成,请参考以下信息."
        # 格式化输出基站信息供用户选择
        awk '{print NR, $0}' /tmp/cellinfo
        rm -f ${lockfile}
        kill -9 $pid
        exit 0
    esac
done

rm -f ${lockfile}

