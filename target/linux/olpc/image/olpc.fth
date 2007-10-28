\ Boot script
" sd:\boot\vmlinuz" to boot-device
" block2mtd.block2mtd=/dev/mmcblk0p2,65536,rootfs root=/dev/mtdblock0 rootfstype=squashfs init=/etc/preinit  noinitrd console=tty0" to boot-file
boot
