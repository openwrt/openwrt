#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
. /lib/functions.sh

DECRYPT=/usr/sbin/dvf99-css-decrypt
MEMBER=lib/firmware/css-loader
DAEMON=snom/media_daemon
UMT_DIR=/lib/firmware/dvf99-css
CFG=dvf99-css-firmware
LOCK=/var/lock/dvf99-css-firmware
PUBKEY=firmware1.pub
TMP=

log() {
	logger -s -t dvf99-css-firmware "$1"
}

cleanup() {
	[ -n "$TMP" ] || return
	umount "$TMP/rescuefs" 2>/dev/null
	rm -rf "$TMP"
	TMP=
}

fail() {
	log "$1"
	cleanup
	lock -u "$LOCK"
	exit 1
}

rescuefs_mount() {
	local part

	part=$(find_mtd_part rescuefs)
	[ -n "$part" ] || fail "rescuefs partition not found"
	mkdir -p "$TMP/rescuefs"
	mount -t jffs2 -o ro "$part" "$TMP/rescuefs" ||
		fail "mount of rescuefs failed"
	[ -f "$TMP/rescuefs/$PUBKEY" ] || fail "rescuefs has no $PUBKEY"
}

umt_missing() {
	local slot="${1%%:*}"

	[ -f "$UMT_DIR/wbhf-dyn-$slot.bin" ] || missing=1
}

umt_extract() {
	local slot offset len sum got
	local out="$TMP/wbhf-dyn-${1%%:*}.bin"

	IFS=: read -r slot offset len sum <<-EOF
		$1
	EOF

	dd if="$TMP/$DAEMON" of="$out" bs=1 skip=$((offset)) count="$len" 2>/dev/null
	got=$(sha256sum "$out" | cut -d' ' -f1)
	[ "$got" = "$sum" ] || fail "mode table $slot: sha256 mismatch ($got)"
}

umt_install() {
	local slot="${1%%:*}"

	cp "$TMP/wbhf-dyn-$slot.bin" "$UMT_DIR/wbhf-dyn-$slot.bin" ||
		fail "install of mode table $slot failed"
}

lock -n "$LOCK" 2>/dev/null || exit 0

config_load "$CFG"
config_get url main url
config_get file main file
config_get dest main dest /lib/firmware/css-loader
config_get sha256 main sha256

missing=
[ -f "$dest" ] || missing=1
config_list_foreach main umt umt_missing
[ -n "$missing" ] || { lock -u "$LOCK"; exit 0; }

[ -x "$DECRYPT" ] || fail "missing $DECRYPT"
[ -n "$file" ] || [ -n "$url" ] || fail "set 'url' or 'file' in /etc/config/$CFG"

log "fetching the CSS firmware"
TMP=$(mktemp -d) || fail "mktemp failed"
rescuefs_mount

{
	if [ -n "$file" ]; then
		cat "$file"
	else
		uclient-fetch -q -O - "$url"
	fi
} | "$DECRYPT" "$TMP/rescuefs/$PUBKEY" | tar -xz -C "$TMP" "$MEMBER" "$DAEMON"

[ -s "$TMP/$MEMBER" ] || fail "extraction produced no css-loader"

if [ -n "$sha256" ]; then
	got=$(sha256sum "$TMP/$MEMBER" | cut -d' ' -f1)
	[ "$got" = "$sha256" ] || fail "sha256 mismatch ($got)"
fi

config_list_foreach main umt umt_extract

mkdir -p "$UMT_DIR" "$(dirname "$dest")"
config_list_foreach main umt umt_install
cp "$TMP/$MEMBER" "$dest.new" || fail "install to $dest failed"
mv "$dest.new" "$dest" || fail "install to $dest failed"
cleanup
lock -u "$LOCK"
log "installed $dest and the mode tables, rebooting"
reboot
exit 0
