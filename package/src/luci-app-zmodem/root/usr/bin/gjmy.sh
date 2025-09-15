#!/bin/sh
echo "1、开启国际漫游(可能产生高额费用)"
echo "2、关闭国际漫游服务"
read -p "请输入数字选择操作：" choice
case $choice in
    1)
        sendat 2 'AT+QNWCFG="data_roaming",0'
        echo "已开启国际漫游服务，请注意可能产生额外费用！"
        ;;
    2)
        sendat 2 'AT+QNWCFG="data_roaming",1'
        echo "已关闭国际漫游服务！"
        ;;
    *)
        echo "无效的选择，请重新运行脚本并输入有效数字。"
        ;;
esac