echo -----------------------------
echo u-boot.cmd
echo  Kernel: MMC0P1:uImage
echo  RootFS: MMC0P2
echo  VideoMode: 640x480@60
echo  VideoSpec: omapfb:vram:2M,vram:4M
echo -----------------------------
setenv bootargs 'console=ttyS2,115200n8 console=tty0 root=/dev/mmcblk0p2 rw rootwait nohz=off video=omapfb:vram:1M,vxres:640,vyres:480'
fatload mmc 0 83000000 uImage
bootm 83000000
