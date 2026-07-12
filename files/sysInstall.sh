#!/bin/sh

echo "=> 清空MMC数据..."
dd if=/dev/zero of=/dev/mmcblk0 bs=1M count=1

echo "=> 将MMC分成两个分区..."
fdisk /dev/mmcblk0 <<EOF
n
p
1
2048
+1G
n
p
2
2099200

w
EOF

echo "=> 格式化分区为ext4文件系统..."
mkfs.ext4 -F /dev/mmcblk0p1
mkfs.ext4 -F /dev/mmcblk0p2

# 挂载目标分区
mkdir -p /mnt/mmcblk0p1 /mnt/mmcblk0p2
mount /dev/mmcblk0p1 /mnt/mmcblk0p1
mount /dev/mmcblk0p2 /mnt/mmcblk0p2

# 挂载源分区
mkdir -p /mnt/sda1 /mnt/sda2
mount /dev/sda1 /mnt/sda1
mount /dev/sda2 /mnt/sda2

# 复制文件系统到目标分区
echo "=> 复制文件系统到目标分区..."
cp -rf /mnt/sda1/* /mnt/mmcblk0p1/
cp -rf /mnt/sda2/* /mnt/mmcblk0p2/

rm /mnt/mmcblk0p2/sysInstall.sh

# 取消挂载源分区
sync;sync;sync
umount -l /mnt/sda1
umount -l /mnt/sda2
umount -l /mnt/mmcblk0p1
umount -l /mnt/mmcblk0p2
sync;sync;sync
sleep 3

rmdir /mnt/mmcblk0p1 /mnt/mmcblk0p2
rmdir /mnt/sda1      /mnt/sda2

sync;sync;sync
sleep 3
echo "=> 系统安装完成!"
