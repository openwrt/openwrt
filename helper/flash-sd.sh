#!/bin/bash
DRV="/dev/sdb"
TYPE="ext4"
umount "${DRV}1"
umount "${DRV}2"
umount "${DRV}3"
umount "${DRV}4"
gzip -dkc bin/targets/ramips/mt7621/*-ramips-mt7621-hatlab_gateboard-one-${TYPE}-combined.img.gz | dd of="${DRV}" bs=4k
sync
gparted
