#!/bin/sh

make -j $(nproc)

if [ $? -eq 0 ] 
then
	echo "Build Success & Copy to tftproot"
	sudo cp ./bin/targets/ramips/mt76x8/openwrt-ramips-mt76x8-telix_ell200-squashfs-sysupgrade.bin /srv/tftp/op23-ell200.bin
	echo "op23-ell200.bin"
else
	echo "Build Fail!!"
fi
