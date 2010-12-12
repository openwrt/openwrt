#!/bin/sh /etc/rc.common
# Copyright (C) 2008 OpenWrt.org
START=99

start() {

	# start CPE dsl daemon in the background
	/sbin/dsl_cpe_control -i -f /lib/firmware/ModemHWE.bin &

#	PS=`ps`
#	echo $PS | grep -q dsl_cpe_control && {
#		# workaround for nfs: allow write to pipes for non-root
#		while [ ! -e /tmp/pipe/dsl_cpe1_ack ] ; do sleep 1; done
#		chmod a+w /tmp/pipe/dsl_*
#	}
	echo $PS | grep -q dsl_cpe_control || {
		echo "Start of dsl_cpe_control failed!!!"
		false
	}

}
