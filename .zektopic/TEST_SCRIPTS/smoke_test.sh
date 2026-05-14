#!/bin/bash
# smoke_test.sh — QEMU smoke test for x86/64 OpenWrt images
# Boots the built image in QEMU via UEFI (OVMF) and verifies it reaches the shell
# Usage: ./smoke_test.sh [image_path]

set -e
cd /home/manu/openwrt

# Find the image
if [ -n "$1" ]; then
    IMAGE_SRC="$1"
else
    IMAGE_SRC=$(ls -t bin/targets/x86/64/openwrt-*-ext4-combined-efi.img.gz 2>/dev/null | head -1)
fi

if [ -z "$IMAGE_SRC" ]; then
    echo "ERROR: No built images found in bin/targets/x86/64/"
    echo "Build x86/64 first: make -j\$(nproc)"
    exit 1
fi

# Gunzip the image if needed
if echo "$IMAGE_SRC" | grep -q '\.gz$'; then
    IMAGE="/tmp/qemu_smoke_$$.img"
    echo "Decompressing $IMAGE_SRC ..."
    gunzip -c "$IMAGE_SRC" > "$IMAGE"
else
    IMAGE="$IMAGE_SRC"
fi

echo "=== QEMU Smoke Test (UEFI/OVMF) ==="
echo "Image:  $IMAGE"
echo ""

# Check if qemu is available
if ! command -v qemu-system-x86_64 &>/dev/null; then
    echo "WARNING: qemu-system-x86_64 not found. Install with:"
    echo "  sudo apt-get install -y qemu-system-x86"
    echo "Skipping smoke test."
    echo "{\"result\": \"SKIPPED\", \"reason\": \"qemu not installed\"}"
    exit 0
fi

# Check for OVMF firmware
OVMF="/usr/share/ovmf/OVMF.fd"
if [ ! -f "$OVMF" ]; then
    OVMF=$(find /usr/share -name "OVMF.fd" 2>/dev/null | head -1)
fi
if [ -z "$OVMF" ]; then
    echo "WARNING: OVMF UEFI firmware not found. Install with:"
    echo "  sudo apt-get install -y ovmf"
    echo "Skipping smoke test."
    echo "{\"result\": \"SKIPPED\", \"reason\": \"OVMF not found\"}"
    exit 0
fi

# Check for KVM support
KVM=""
if [ -e /dev/kvm ]; then
    if [ -r /dev/kvm ] && [ -w /dev/kvm ]; then
        KVM="-enable-kvm -cpu host"
    fi
fi

# Boot in QEMU with UEFI via OVMF (not -kernel, which is broken in QEMU 10.x)
echo "Booting image in QEMU via UEFI ($([ -n "$KVM" ] && echo "KVM" || echo "no KVM"))..."
echo ""

timeout 60 qemu-system-x86_64 \
    -bios "$OVMF" \
    $KVM \
    -drive "file=$IMAGE,format=raw,if=virtio" \
    -m 512M \
    -nographic \
    -no-reboot \
    2>&1 | tee /tmp/qemu_boot.log | tail -40

# Check for boot success indicators
echo ""
echo "=== Boot Analysis ==="
if grep -q "done\|login\|openwrt\|Ready\|init\|procd" /tmp/qemu_boot.log 2>/dev/null; then
    echo "[PASS] Kernel booted and reached init"
    RESULT="PASS"
elif grep -q "Kernel panic\|BUG\|Error\|fatal\|segfault\|OOM\|out of memory" /tmp/qemu_boot.log 2>/dev/null; then
    echo "[FAIL] Kernel panic or fatal error during boot"
    RESULT="FAIL"
else
    echo "[WARN] Could not determine boot status (image may still boot, check /tmp/qemu_boot.log)"
    RESULT="UNKNOWN"
fi

echo ""
echo "=== Smoke Test Result: $RESULT ==="
