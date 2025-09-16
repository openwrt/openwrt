#!/bin/sh

TMP_DIR="/tmp"
URL=""
TOTAL_DATA=0

while true
do
    if [ -z "$URL" ]; then
                echo "流量刷注意事项：请勿用于洪水请求，请确保你的用途是否正确，一直重复对一个URL进行请求可能导致被BAN。"
        echo "请输入目的URL:(示例：https://www.xxx.com/xxx.dat)"
        read URL
    fi

    # 发送 HTTP 请求，提取实时传输速率信息
    while true
    do
        SPEED=$(curl -s -w "%{speed_download}" -o /dev/null -L $URL | awk '{print int($1/1024)}')
        if [ $SPEED -eq 0 ]; then
            echo "请求失败，请尝试换一个URL:(示例：https://www.xxx.com/xxx.dat)"
            URL=""
            break
        else
            echo "正在进行流量传输...（请按下CTRL+C停止操作）"
            DATA=$(curl -s -w "%{size_download}" -o /dev/null -L $URL | awk '{print $1/1024/1024}')
            TOTAL_DATA=$(awk "BEGIN {print $TOTAL_DATA + $DATA}")
            echo "本次请求流量速度: ${SPEED} KB/s，本次请求流量: ${DATA} MB，总流量合计: ${TOTAL_DATA} MB"
        fi

        sleep 1
    done
done