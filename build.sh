#!/bin/bash

# OpenWrt 24.10 Build Script for Oolite V1.0 Testing
# Kernel 6.6.93 with modern userland

set -e

echo "=== OpenWrt 24.10 Oolite V1.0 Build Script ==="
echo "Target: Gainstrong Oolite V1.0"
echo "Kernel: 6.6.93"
echo "OpenWrt: 24.10.2"
echo ""

# Update feeds
echo "=== Updating feeds ==="
./scripts/feeds update -a
./scripts/feeds install -a

# Configure for Oolite V1.0
echo "=== Configuring for Oolite V1.0 ==="
make defconfig
echo "CONFIG_TARGET_ath79=y" > .config
echo "CONFIG_TARGET_ath79_generic=y" >> .config
echo "CONFIG_TARGET_ath79_generic_DEVICE_gainstrong_oolite-v1.0=y" >> .config

# Add essential packages
echo "CONFIG_PACKAGE_luci=y" >> .config
echo "CONFIG_PACKAGE_luci-ssl=y" >> .config
echo "CONFIG_PACKAGE_kmod-usb-chipidea2=y" >> .config

# Apply configuration
make defconfig

echo "=== Configuration Summary ==="
grep "CONFIG_TARGET" .config
echo ""

# Build
echo "=== Starting build ==="
make -j$(nproc) V=s

echo "=== Build Complete ==="
echo "Images available in bin/targets/ath79/generic/"
ls -la bin/targets/ath79/generic/*oolite*
