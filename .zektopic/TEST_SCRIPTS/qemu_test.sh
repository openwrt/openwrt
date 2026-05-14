#!/bin/bash
# qemu_test.sh — Universal QEMU boot test for OpenWrt images
# Tests x86/64, MIPS (malta), ARM, and AArch64 images in QEMU
# Usage: ./qemu_test.sh <target> [image_dir]
#   target: x86_64 | malta_be | malta_le | armvirt_32 | armvirt_64

set -e

TARGET="${1:-x86_64}"
IMGDIR="${2:-bin/targets}"
TIMEOUT="${TIMEOUT:-60}"
PASS=0
FAIL=0

case "$TARGET" in
  x86_64)
    TARGET_DIR="$IMGDIR/x86/64"
    IMAGE=$(ls "$TARGET_DIR"/openwrt-*-ext4-combined-efi.img.gz 2>/dev/null | head -1)
    if [ -z "$IMAGE" ]; then
      echo "ERROR: x86/64 images not found in $TARGET_DIR"
      exit 1
    fi
    echo "=== x86/64 QEMU Test (UEFI/OVMF) ==="
    echo "Image:  $(basename $IMAGE)"

    # Decompress image
    IMG="/tmp/qemu_x86_64_$$.img"
    gunzip -c "$IMAGE" > "$IMG"

    # Find OVMF firmware
    OVMF="/usr/share/ovmf/OVMF.fd"
    if [ ! -f "$OVMF" ]; then
      OVMF=$(find /usr/share -name "OVMF.fd" 2>/dev/null | head -1)
    fi

    # KVM support
    KVM=""
    if [ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ]; then
      KVM="-enable-kvm -cpu host"
    fi

    timeout $TIMEOUT qemu-system-x86_64 \
      -bios "$OVMF" \
      $KVM \
      -drive "file=$IMG,format=raw,if=virtio" \
      -m 512M \
      -nographic \
      -no-reboot \
      2>&1 | tee /tmp/qemu_${TARGET}_boot.log | tail -30

    rm -f "$IMG"
    ;;

  malta_be)
    TARGET_DIR="$IMGDIR/malta/be"
    # Try initramfs kernel first
    IMAGE=$(ls "$TARGET_DIR"/openwrt-*-vmlinux-initramfs* 2>/dev/null | head -1)
    if [ -z "$IMAGE" ]; then
      IMAGE=$(ls "$TARGET_DIR"/openwrt-*-vmlinux* 2>/dev/null | grep -v initramfs | head -1)
    fi
    if [ -z "$IMAGE" ]; then
      echo "ERROR: malta/be images not found in $TARGET_DIR"
      exit 1
    fi
    echo "=== malta/be (MIPS BE) QEMU Test ==="
    echo "Image: $(basename $IMAGE)"

    timeout $TIMEOUT qemu-system-mips \
      -M malta -m 256M \
      -kernel "$IMAGE" \
      -nographic \
      -append "console=ttyS0" \
      2>&1 | tee /tmp/qemu_${TARGET}_boot.log | tail -30
    ;;

  malta_le)
    TARGET_DIR="$IMGDIR/malta/le"
    IMAGE=$(ls "$TARGET_DIR"/openwrt-*-vmlinux-initramfs* 2>/dev/null | head -1)
    if [ -z "$IMAGE" ]; then
      IMAGE=$(ls "$TARGET_DIR"/openwrt-*-vmlinux* 2>/dev/null | grep -v initramfs | head -1)
    fi
    if [ -z "$IMAGE" ]; then
      echo "ERROR: malta/le images not found in $TARGET_DIR"
      exit 1
    fi
    echo "=== malta/le (MIPS LE) QEMU Test ==="
    echo "Image: $(basename $IMAGE)"

    timeout $TIMEOUT qemu-system-mipsel \
      -M malta -m 256M \
      -kernel "$IMAGE" \
      -nographic \
      -append "console=ttyS0" \
      2>&1 | tee /tmp/qemu_${TARGET}_boot.log | tail -30
    ;;

  armvirt_32)
    TARGET_DIR="$IMGDIR/armvirt/32"
    IMAGE=$(ls "$TARGET_DIR"/openwrt-*-Image-initramfs* 2>/dev/null | head -1)
    if [ -z "$IMAGE" ]; then
      IMAGE=$(ls "$TARGET_DIR"/openwrt-*-Image* 2>/dev/null | head -1)
    fi
    if [ -z "$IMAGE" ]; then
      echo "ERROR: armvirt/32 images not found in $TARGET_DIR"
      exit 1
    fi
    echo "=== arm/virt (ARM) QEMU Test ==="
    echo "Image: $(basename $IMAGE)"

    timeout $TIMEOUT qemu-system-arm \
      -M virt -m 256M -cpu cortex-a15 \
      -kernel "$IMAGE" \
      -nographic \
      -append "console=ttyAMA0" \
      2>&1 | tee /tmp/qemu_${TARGET}_boot.log | tail -30
    ;;

  armvirt_64)
    TARGET_DIR="$IMGDIR/armvirt/64"
    IMAGE=$(ls "$TARGET_DIR"/openwrt-*-Image-initramfs* 2>/dev/null | head -1)
    if [ -z "$IMAGE" ]; then
      IMAGE=$(ls "$TARGET_DIR"/openwrt-*-Image* 2>/dev/null | head -1)
    fi
    if [ -z "$IMAGE" ]; then
      echo "ERROR: armvirt/64 images not found in $TARGET_DIR"
      exit 1
    fi
    echo "=== aarch64/virt (AArch64) QEMU Test ==="
    echo "Image: $(basename $IMAGE)"

    timeout $TIMEOUT qemu-system-aarch64 \
      -M virt -m 256M -cpu cortex-a57 \
      -kernel "$IMAGE" \
      -nographic \
      -append "console=ttyAMA0" \
      2>&1 | tee /tmp/qemu_${TARGET}_boot.log | tail -30
    ;;

  *)
    echo "Usage: $0 <target> [image_dir]"
    echo "Targets: x86_64, malta_be, malta_le, armvirt_32, armvirt_64"
    exit 1
    ;;
esac

# Analyze boot log
echo ""
echo "=== Boot Analysis ==="
LOG="/tmp/qemu_${TARGET}_boot.log"
if grep -qE "done|login|openwrt|Ready|init|/ #|ash|BusyBox|initramfs" "$LOG" 2>/dev/null; then
  echo "[PASS] $TARGET — kernel booted successfully, init started"
  RESULT="PASS"
elif grep -qE "Kernel panic|BUG|Error|fatal|segfault" "$LOG" 2>/dev/null; then
  echo "[FAIL] $TARGET — kernel panic or fatal error during boot"
  RESULT="FAIL"
else
  echo "[WARN] $TARGET — could not determine boot status (timeout or incomplete boot)"
  RESULT="UNKNOWN"
fi

echo "=== Result: $RESULT ==="
