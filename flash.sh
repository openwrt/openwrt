#!/usr/bin/env bash
# Build a full SPI-NOR image for the TL-MR6400 v5 (raw-FIT layout):
#   0x000000   bootloader  (U-Boot with SPL, from uboot-mediatek build)
#   0x040000   firmware    (FIT kernel + squashfs, the sysupgrade.bin)
#   last 4 KiB factory     (MAC + WiFi cal blob)
#
# Usage: ./flash.sh [8|16]   -- flash chip size in MiB (default 8)
#   8  -> tplink_tl-mr6400-v5-ubootmod      (stock W25Q64,  factory @ 0x7ff000)
#   16 -> tplink_tl-mr6400-v5-ubootmod-16m  (e.g. W25Q128,  factory @ 0xfff000)
# Run from the openwrt build root, in the shell that has flashrom + the build.
set -euo pipefail
cd "$(dirname "$0")"

MB=${1:-8}
case "$MB" in
	8)  SUFFIX="";     CHIP_HINT="W25Q64BV/W25Q64CV/W25Q64FV" ;;
	16) SUFFIX="-16m"; CHIP_HINT="W25Q128.V" ;;
	*)  echo "usage: $0 [8|16]"; exit 1 ;;
esac

FLASH_SIZE=$((MB * 1024 * 1024))
FW_OFFSET=$((0x40000))
FAC_OFFSET=$((FLASH_SIZE - 0x1000))
FW_MAX=$((FAC_OFFSET - FW_OFFSET))

OUT=/tmp/mr6400v5_full_${MB}m.bin
SPL=$(find . -path '*image*' -name "mt7628_tplink_tl-mr6400-v5${SUFFIX}-u-boot-with-spl.bin" | head -1)
SYS=$(ls bin/targets/ramips/mt76x8/*-mr6400-v5-ubootmod${SUFFIX}-squashfs-sysupgrade.bin | head -1)
# device-specific MAC + WiFi cal blob; override with FAC=/path/to/blob
FAC=${FAC:-$PWD/mr6400v5_factory_repacked_4k.bin}

echo "SPL : $SPL"
echo "SYS : $SYS  ($(stat -c%s "$SYS") bytes)"
echo "FAC : $FAC"
[ -n "$SPL" ] && [ -f "$SYS" ] && [ -f "$FAC" ] || { echo "MISSING INPUT"; exit 1; }

# u-boot-with-spl must stay below the env sector at 0x30000, or the first
# saveenv would erase its tail
SPLZ=$(stat -c%s "$SPL")
[ "$SPLZ" -le $((0x30000)) ] || { echo "BOOTLOADER TOO BIG: $SPLZ > $((0x30000)) (env @ 0x30000)"; exit 1; }

# sysupgrade must fit in the firmware window (8M: 0x7bf000, 16M: 0xfbf000)
SYSZ=$(stat -c%s "$SYS")
[ "$SYSZ" -le "$FW_MAX" ] || { echo "FIRMWARE TOO BIG: $SYSZ > $FW_MAX"; exit 1; }

# full flash of 0xff
dd if=/dev/zero bs=1M count="$MB" 2>/dev/null | tr '\000' '\377' > "$OUT"
# bootloader @ 0x0
dd if="$SPL" of="$OUT" conv=notrunc
# firmware @ 0x040000
dd if="$SYS" of="$OUT" bs=4096 seek=$((FW_OFFSET / 4096)) conv=notrunc
# factory @ last 4 KiB sector
dd if="$FAC" of="$OUT" bs=4096 seek=$((FAC_OFFSET / 4096)) conv=notrunc

[ "$(stat -c%s "$OUT")" -eq "$FLASH_SIZE" ] && echo "SIZE_OK -> $OUT" || { echo SIZE_BAD; exit 1; }
echo "verify factory MAC in image:"
python3 -c "d=open('$OUT','rb').read(); print(':'.join('%02x'%b for b in d[$FAC_OFFSET:$FAC_OFFSET+6]))"
echo
echo "flash with:"
echo "  flashrom -p ch341a_spi -c $CHIP_HINT --write $OUT"
