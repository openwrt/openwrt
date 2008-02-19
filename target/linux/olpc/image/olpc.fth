\ Boot script
" u:\boot\vmlinuz" to boot-device
" block2mtd.block2mtd=/dev/sda2,65536,rootfs root=/dev/mtdblock1 rootfstype=squashfs init=/etc/preinit noinitrd console=tty0" to boot-file
boot
