#!/usr/bin/env bash
# Copyright (C) 2006-2010 OpenWrt.org
. ./gen_image_generic.sh

which chpax >/dev/null && chpax -zp $(which grub)
grub --batch --no-curses --no-floppy --device-map=/dev/null <<EOF
device (hd0) $OUTPUT
geometry (hd0) $cyl $head $sect
root (hd0,0)
setup (hd0)
quit
EOF

