#!/bin/sh
. /etc/functions.sh
cmdline="$(cat /proc/cmdline)"
kernel="$(find_mtd_part kernel)"
kexec -l $kernel --command-line=$cmdline
