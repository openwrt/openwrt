#!/bin/sh
echo " "
echo "——————————————————————————————————"
echo "鲲鹏系列l2tp拨号程式1.1 by:manper"
echo "——————————————————————————————————"
echo " "
while true; do
    echo "1. >>>>L2TP-SIM拨号"
    echo "2. >>>>断开并移除L2TP-SIM"
    echo "3. >>>>退出"
    echo " "
    echo "请输入数字并回车选择操作："
    read choice
    case $choice in
        1)
            /usr/bin/l2tp-sim.sh
            ;;
        2)
            /usr/bin/disl2tp-sim.sh
            ;;
        3)
            break
            ;;
        *)
            echo "输入错误，请重新输入。"
            ;;
    esac
done
