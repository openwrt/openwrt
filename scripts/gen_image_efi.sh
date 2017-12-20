#!/usr/bin/env bash
set -x
[ $# == 6 -o $# == 7 ] || {
    echo "SYNTAX: $0 <file> <efi size> <efi directory> <efi uuid> <rootfs size> <rootfs image> [<align>]"
    exit 1
}

OUTPUT="$1"
EFISIZE="$2"
EFIDIR="$3"
EFIUUID="${4//-}"  # strip dashes from the EFI part UUID
ROOTFSSIZE="$5"
ROOTFSIMAGE="$6"
ALIGN="$7"

rm -f "$OUTPUT"

head=16
sect=63
cyl=$(( ($EFISIZE + $ROOTFSSIZE) * 1024 * 1024 / ($head * $sect * 512) ))

# create partition table
set `ptgen -o "$OUTPUT" -h $head -s $sect -p ${EFISIZE}m -p ${ROOTFSSIZE}m ${ALIGN:+-l $ALIGN} ${SIGNATURE:+-S 0x$SIGNATURE}`

EFIOFFSET="$(($1 / 512))"
EFISIZE="$(($2 / 1024))" # mkfs.fat takes the number of bytes, not blocks
ROOTFSOFFSET="$(($3 / 512))"
ROOTFSSIZE="$(($4 / 512))"

dd if=/dev/zero of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc count="$ROOTFSSIZE"
dd if="$ROOTFSIMAGE" of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc

[ -n "$NOGRUB" ] && exit 0

# The EFI partition has to be FAT32, which has a minimum size of 32MiB on 512-byte sector drives
rm -f "$OUTPUT.efipart"
mkfs.fat -F 32 -i $EFIUUID -C "$OUTPUT.efipart" $EFISIZE
ls -la "$OUTPUT.efipart"
mcopy -s -i "${OUTPUT}.efipart" $EFIDIR/* "::"

dd if="$OUTPUT.efipart" of="$OUTPUT" bs=512 seek="$EFIOFFSET" conv=notrunc
rm "${OUTPUT}.efipart"
