#!/bin/sh
/bin/led-button -l 41 &
/sbin/jffs2reset -y && /sbin/reboot
