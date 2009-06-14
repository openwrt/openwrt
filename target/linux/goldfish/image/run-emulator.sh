PREFIX=openwrt-goldfish-
touch ${PREFIX}data.bin
./goldfish-qemu/emulator -skindir ./goldfish-qemu/skins -skin HVGA -sysdir . -ramdisk ${PREFIX}ramdisk.bin -kernel ${PREFIX}kernel.bin -system ${PREFIX}system.bin -data ${PREFIX}data.bin -show-kernel -partition-size 100 -qemu -append "console=ttyS0 ubi.mtd=0 root=/dev/mtdblock3 rootfstype=jffs2 bootdelay=1 init=/etc/preinit" -net nic,vlan=0,model=smc91c111 -net user,vlan=0,hostname=OpenWrt
