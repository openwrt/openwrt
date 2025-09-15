#!/bin/sh
L2TP_IFACE="WANL2TP"
stop_l2tp_connection() {
    echo "Stopping L2TP connection for $L2TP_IFACE..."
    ifdown $L2TP_IFACE
}
# Function to delete the L2TP interface
delete_l2tp_interface() {
    echo "Deleting L2TP interface $L2TP_IFACE..."
    uci delete network.$L2TP_IFACE
    uci commit network
    /etc/init.d/network reload
}
stop_l2tp_connection
delete_l2tp_interface
echo "已全部移除L2TP连接,Disconnection and cleanup for $L2TP_IFACE completed."