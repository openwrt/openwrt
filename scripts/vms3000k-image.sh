#!/bin/sh
set -eu
kernel=$1
rootfs=$2
output=$3
test "$(wc -c < "$kernel")" -le 8126464 || { echo "V-MS3000K kernel exceeds 64 LEBs" >&2; exit 1; }
test "$(wc -c < "$rootfs")" -le 9142272 || { echo "V-MS3000K rootfs exceeds 72 LEBs" >&2; exit 1; }
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
dtc=${4:?missing host dtc}
size=$(wc -c < "$rootfs")
crc=$(python3 -c 'import pathlib,sys,zlib; print(hex(zlib.crc32(pathlib.Path(sys.argv[1]).read_bytes())))' "$rootfs")
digest=$(sha1sum "$rootfs" | cut -d " " -f 1 | sed 's/\(........\)/0x\1 /g')
"$dtc" -q -I dtb -O dts -o "$work/kernel.dts" "$kernel"
cat >> "$work/kernel.dts" <<EOF
/ {
	rootfs {
		size = <$size>;
		hash-1 { algo = "crc32"; value = <$crc>; };
		hash-2 { algo = "sha1"; value = <$digest>; };
	};
};
EOF
"$dtc" -q -I dts -O dtb -o "$work/kernel" "$work/kernel.dts"
test "$(wc -c < "$work/kernel")" -le 8126464
cp "$rootfs" "$work/rootfs"
printf '%s\n' tenbay,v-ms3000k:ubi1:v2 > "$work/format"
(cd "$work" && sha256sum kernel rootfs > sha256)
tar --format=ustar --numeric-owner --owner=0 --group=0 --mtime="@${SOURCE_DATE_EPOCH:-0}" \
    -cf "$output" -C "$work" format kernel rootfs sha256
