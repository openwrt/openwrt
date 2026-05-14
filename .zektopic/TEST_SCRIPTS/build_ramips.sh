#!/bin/bash
# build_ramips.sh — Build OpenWrt for ramips/mt7621 target (MIPS)
# Usage: ./build_ramips.sh [clean]

set -e
cd /home/manu/openwrt

LOG="build_ramips.log"
THREADS=$(nproc)

echo "=== ramips/mt7621 Build ===" | tee "$LOG"

if [ "$1" = "clean" ]; then
    echo "Cleaning previous build..."
    make clean >> "$LOG" 2>&1
fi

echo "Configuring ramips/mt7621..." | tee -a "$LOG"
echo "CONFIG_TARGET_ramips=y" > .config
echo "CONFIG_TARGET_ramips_mt7621=y" >> .config
make defconfig >> "$LOG" 2>&1

echo "Building with -j${THREADS}..." | tee -a "$LOG"
make -j"${THREADS}" V=s >> "$LOG" 2>&1

echo "=== Build Complete ===" | tee -a "$LOG"

# Show built images
echo "Built images:" | tee -a "$LOG"
ls -lh bin/targets/ramips/mt7621/*.bin 2>/dev/null | tee -a "$LOG"

echo ""
echo "Build log: $LOG"
echo "Images: bin/targets/ramips/mt7621/"
