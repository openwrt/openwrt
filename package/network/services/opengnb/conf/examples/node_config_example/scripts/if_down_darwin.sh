#!/bin/sh

if [ -n "$GNB_IF_NAME" ] ; then

    echo $GNB_MTU > "/sys/class/net/$GNB_IF_NAME/mtu"

fi


#if [ -n "$GNB_TUN_IPV6" ] ; then
   
#	ip -6 address add "$GNB_TUN_IPV6/96" dev $GNB_IF_NAME

#fi

 

