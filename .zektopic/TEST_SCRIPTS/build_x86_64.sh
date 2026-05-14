#!/bin/bash
# build_x86_64.sh — Build OpenWrt for x86/64 target
# Usage: ./build_x86_64.sh [clean]

set -e
cd /home/manu/openwrt

LOG="build_x86_64.log"
THREADS=$(nproc)

echo "=== x86/64 Build ===" | tee "$LOG"

if [ "$1" = "clean" ]; then
    echo "Cleaning previous build..."
    make clean >> "$LOG" 2>&1
fi

echo "Configuring x86/64..." | tee -a "$LOG"
echo "CONFIG_TARGET_x86=y" > .config
echo "CONFIG_TARGET_x86_64=y" >> .config
make defconfig >> "$LOG" 2>&1

echo "Building with -j${THREADS}..." | tee -a "$LOG"
make -j"${THREADS}" V=s >> "$LOG" 2>&1

echo "=== Build Complete ===" | tee -a "$LOG"

# Show built images
echo "Built images:" | tee -a "$LOG"
ls -lh bin/targets/x86/64/*.bin 2>/dev/null | tee -a "$LOG"
ls -lh bin/targets/x86/64/*.img 2>/dev/null | tee -a "$LOG"

echo ""
echo "Build log: $LOG"
echo "Images: bin/targets/x86/64/"
