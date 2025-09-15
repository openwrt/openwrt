#!/bin/sh
# 660/668 L2TP WAN Interface Setup.By:manper 20240318
L2TP_IFACE="WANL2TP"
L2TP_SERVER=""
L2TP_USERNAME=""
L2TP_PASSWORD=""
setup_l2tp_interface() {
    uci set network.$L2TP_IFACE=interface
    uci set network.$L2TP_IFACE.proto='l2tp'
    uci set network.$L2TP_IFACE.server=$L2TP_SERVER
    uci set network.$L2TP_IFACE.username=$L2TP_USERNAME
    uci set network.$L2TP_IFACE.password=$L2TP_PASSWORD
    uci set network.$L2TP_IFACE.ipv6='0'
    uci add_list firewall.@zone[1].network=$L2TP_IFACE
    uci commit network
    uci commit firewall
}
start_l2tp_connection() {
    echo "L2TP 拨号中..."
    ifup $L2TP_IFACE
    sleep 10
}
check_l2tp_connection() {
    local status=$(ifstatus $L2TP_IFACE | jsonfilter -e '@["up"]')
    if [ "$status" = "true" ]; then
        echo "已成功拨号并建立路由！L2TP connection is successfully established."
        exit 1
    else
        echo "Failed to establish L2TP connection. 连接失败,请检查你的配置或稍后再试。"
        exit 1
    fi
}
check_existing_connection() {
    if uci get network.$L2TP_IFACE >/dev/null 2>&1; then
        echo "接口 $L2TP_IFACE 已启用.跳过新建接口步骤, 正检查连接...."
        check_l2tp_connection
        return 0
    fi
    return 1
}
prompt_user_input() {
    echo "L2TP SIM卡拨号程式1.1 by:manper"
    echo "说明:该程式为特殊SIM卡专用拨号使用,使用了WAN口作为拨号接口,如果你需要使用有线网,需要更改一些设置."
        echo " "
    echo "请输入运营商服务器地址并回车:"
    read -r L2TP_SERVER
    echo "请输入运营商分配的账号并回车:"
    read -r L2TP_USERNAME
    echo "请输入L2TP密码:"
    read -r L2TP_PASSWORD
}
if check_existing_connection; then
    echo "L2TP WAN 设置完成."
    exit 0
fi
prompt_user_input
setup_l2tp_interface
start_l2tp_connection
check_l2tp_connection
echo "L2TP WAN 设置完成,享受专属卡带来的上网体验吧！"
