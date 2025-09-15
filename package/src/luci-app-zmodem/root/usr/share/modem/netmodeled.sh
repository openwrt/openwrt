#!/bin/sh
# 检查是否已经有锁文件存在
lock_file="/var/run/network_status_update.lock"
exec 200>$lock_file
flock -n 200 || exit 1
while true
do
    # 获取网络模式信息
    if [ ! -f "/tmp/ledflag.conf" ]; then
        network_mode=$(sendat 2 'at+qnwinfo' | grep '+QNWINFO' | awk -F\" '{print $2}' | tr -d '\r\n')
        # 判断网络模式并更新LED状态
        if echo "$network_mode" | grep -qE "5G|NR5G"; then
            # 包含TDD，点亮hc:blue:cmode5，熄灭hc:blue:cmode4
            echo 1 > /sys/class/leds/hc:blue:cmode5/brightness
            echo 0 > /sys/class/leds/hc:blue:cmode4/brightness
        elif echo "$network_mode" | grep -q "LTE"; then
            # 包含LTE，点亮hc:blue:cmode4，熄灭hc:blue:cmode5
            echo 1 > /sys/class/leds/hc:blue:cmode4/brightness
            echo 0 > /sys/class/leds/hc:blue:cmode5/brightness
        else
            # 其他网络，熄灭hc:blue:cmode5和hc:blue:cmode4
            echo 0 > /sys/class/leds/hc:blue:cmode5/brightness
            echo 0 > /sys/class/leds/hc:blue:cmode4/brightness
        fi
    fi
    # 等待5秒
    sleep 5
done
# 释放锁
flock -u 200
