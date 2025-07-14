#!/bin/sh
/bin/led-button -l 33 &
/sbin/jffs2reset -y && /sbin/reboot
