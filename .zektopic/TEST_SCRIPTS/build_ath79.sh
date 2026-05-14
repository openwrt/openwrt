#!/bin/bash
# build_ath79.sh — Build OpenWrt for ath79/generic target (MIPS)
# Usage: ./build_ath79.sh [clean]

set -e
cd /home/manu/openwrt

LOG="build_ath79.log"
THREADS=$(nproc)

echo "=== ath79/generic Build ===" | tee "$LOG"

if [ "$1" = "clean" ]; then
    echo "Cleaning previous build..."
    make clean >> "$LOG" 2>&1
fi

echo "Configuring ath79/generic..." | tee -a "$LOG"
echo "CONFIG_TARGET_ath79=y" > .config
echo "CONFIG_TARGET_ath79_generic=y" >> .config
make defconfig >> "$LOG" 2>&1

echo "Building with -j${THREADS}..." | tee -a "$LOG"
make -j"${THREADS}" V=s >> "$LOG" 2>&1

echo "=== Build Complete ===" | tee -a "$LOG"

# Show built images
echo "Built images:" | tee -a "$LOG"
ls -lh bin/targets/ath79/generic/*.bin 2>/dev/null | tee -a "$LOG"

echo ""
echo "Build log: $LOG"
echo "Images: bin/targets/ath79/generic/"
