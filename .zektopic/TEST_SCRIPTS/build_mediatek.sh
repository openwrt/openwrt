#!/bin/bash
# build_mediatek.sh — Build OpenWrt for mediatek/filogic target (ARM)
# Usage: ./build_mediatek.sh [clean]

set -e
cd /home/manu/openwrt

LOG="build_mediatek.log"
THREADS=$(nproc)

echo "=== mediatek/filogic Build ===" | tee "$LOG"

if [ "$1" = "clean" ]; then
    echo "Cleaning previous build..."
    make clean >> "$LOG" 2>&1
fi

echo "Configuring mediatek/filogic..." | tee -a "$LOG"
echo "CONFIG_TARGET_mediatek=y" > .config
echo "CONFIG_TARGET_mediatek_filogic=y" >> .config
make defconfig >> "$LOG" 2>&1

echo "Building with -j${THREADS}..." | tee -a "$LOG"
make -j"${THREADS}" V=s >> "$LOG" 2>&1

echo "=== Build Complete ===" | tee -a "$LOG"

# Show built images
echo "Built images:" | tee -a "$LOG"
ls -lh bin/targets/mediatek/filogic/*.bin 2>/dev/null | tee -a "$LOG"

echo ""
echo "Build log: $LOG"
echo "Images: bin/targets/mediatek/filogic/"
