#!/bin/bash

# -------------------------------
# 配置参数
# -------------------------------
OPENWRT_DIR=~/openwrt
OPENCLASH_FEED="src-git OpenClash https://github.com/vernesong/OpenClash.git;master"
CORE_DIR=$OPENWRT_DIR/feeds/OpenClash/luci-app-openclash/files/etc/openclash/core
BACKUP_DIR=$OPENWRT_DIR/feeds/OpenClash/luci-app-openclash/files/etc/openclash/core_backup
GITHUB_API_URL="https://api.github.com/repos/vernesong/OpenClash/releases/latest"

# -------------------------------
# 安装依赖
# -------------------------------
sudo apt update
sudo apt install -y git build-essential libncurses5-dev gawk flex gettext \
  unzip python3-distutils jq wget

# -------------------------------
# 获取最新 ARM64 内核下载链接
# -------------------------------
echo "Fetching latest OpenClash release..."
LATEST_RELEASE=$(curl -s $GITHUB_API_URL)
ARM64_CORE_URL=$(echo $LATEST_RELEASE | jq -r '.assets[] | select(.name | test("clash-linux-arm64")) | .browser_download_url')

if [ -z "$ARM64_CORE_URL" ]; then
  echo "Error: Cannot find ARM64 core download URL."
  exit 1
fi

# -------------------------------
# 添加 OpenClash feed
# -------------------------------
echo "Adding OpenClash feed..."
grep -q "OpenClash" $OPENWRT_DIR/feeds.conf.default || echo "$OPENCLASH_FEED" >> $OPENWRT_DIR/feeds.conf.default

# -------------------------------
# 更新并安装 feeds
# -------------------------------
cd $OPENWRT_DIR
./scripts/feeds update -a
./scripts/feeds install -a

# -------------------------------
# 清理旧核心并备份
# -------------------------------
mkdir -p $BACKUP_DIR
if [ -d "$CORE_DIR" ] && [ "$(ls -A $CORE_DIR)" ]; then
    echo "Backing up existing cores..."
    cp -r $CORE_DIR/* $BACKUP_DIR/
    rm -f $CORE_DIR/*
fi

# -------------------------------
# 下载最新 ARM64 内核
# -------------------------------
mkdir -p ~/openclash_core
cd ~/openclash_core
echo "Downloading ARM64 core..."
wget -O clash-linux-arm64.gz "$ARM64_CORE_URL"
gunzip -f clash-linux-arm64.gz
chmod +x clash-linux-arm64

# -------------------------------
# 拷贝到 OpenClash core 目录
# -------------------------------
mkdir -p $CORE_DIR
cp clash-linux-arm64 $CORE_DIR/
chmod +x $CORE_DIR/*

echo "ARM64 core copied to $CORE_DIR"

# -------------------------------
# 自动勾选 luci-app-openclash 编译
# -------------------------------
echo "Configuring OpenClash for automatic compilation..."
CONFIG_FILE=$OPENWRT_DIR/.config
grep -q "CONFIG_PACKAGE_luci-app-openclash=y" $CONFIG_FILE || echo "CONFIG_PACKAGE_luci-app-openclash=y" >> $CONFIG_FILE

# -------------------------------
# 编译固件
# -------------------------------
cd $OPENWRT_DIR
make defconfig
make -j$(nproc)

echo "Done! Firmware includes luci-app-openclash with latest ARM64 core in /etc/openclash/core/"
echo "Backup of previous cores is stored in $BACKUP_DIR"
