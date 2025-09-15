#!/bin/bash
#This program is the first system to switch to the device 660, by Manper 20240307
TARGET_DIR="/mnt/app_data"
DEVICE="ubi1_0"
if [ ! -d "$TARGET_DIR" ]; then
    echo "The target directory does not exist, creating $TARGET_DIR ..."
    mkdir "$TARGET_DIR"
    if [ $? -ne 0 ]; then
        echo "Failed to create directory, exiting script..."
        exit 1
    fi
else
    echo "The target directory already exists, skipping the creation step..."
fi
if mount | grep "on $TARGET_DIR " > /dev/null; then
    echo "$TARGET_DIR created，skipping step..."
else
    echo "try to create $DEVICE to $TARGET_DIR ..."
    ubiattach -m 7 /dev/ubi_ctrl
    mount -t ubifs "$DEVICE" "$TARGET_DIR"
    if [ $? -ne 0 ]; then
        echo "create err，exiting..."
        exit 2
    else
        echo "complete."
    fi
fi
sleep 1
BOOT_2ND_FLAG_FILE="/mnt/app_data/boot_2nd_flag"
if [ -f "$BOOT_2ND_FLAG_FILE" ]; then
    echo "Refactoring $BOOT_2ND_FLAG_FILE ..."
    rm "$BOOT_2ND_FLAG_FILE"
    if [ $? -ne 0 ]; then
        echo "Refactoring file failed，exiting..."
        exit 3
    else
        echo "Refactoring completed."
    fi
else
    echo "文件 $BOOT_2ND_FLAG_FILE 不存在，跳过重构步骤."
    echo "切换到官方系统失败！退出..."
    exit 4
fi
echo "切换到官方系统成功！正在重启..."
reboot
exit 0
