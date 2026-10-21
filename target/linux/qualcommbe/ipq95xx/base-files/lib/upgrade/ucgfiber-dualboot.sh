# SPDX-License-Identifier: GPL-2.0-only
#
# Dual boot of stock and OpenWrt on the UniFi Cloud Gateway Fiber.
# Sourced by the switch script (on stock and on OpenWrt) and by sysupgrade.
# POSIX sh only (busybox ash on OpenWrt, dash on stock).
#
# U-Boot boots the file "uImage" in the kernel partition (p1). Each system
# keeps its kernel under its own name and "uImage" is the active one:
#   stock active:   uImage = stock kernel
#   OpenWrt active: uImage = a copy of uImage.owrt, stock's kernel renamed
#                   to uImage.stock
# OpenWrt's system is rootfs.owrt in the root partition (p3). Stock's own
# files (uImage.bkp, rootfs, rootfs.bkp) are never written here.
#
# Integrity: every OpenWrt file has a <file>.sha256 next to it, taken from
# the installed image; uImage.stock gets one when it is renamed. A kernel is
# "good" when it is a FIT whose header length equals the file size and,
# where a .sha256 exists, the hash matches. No file becomes uImage, and no
# uImage is replaced, unless the new one is good. Data is written to *.new,
# read back and checked before any name changes; names change by rename.
#
# Every function takes the mounted kernel partition (K) and, where needed,
# the mounted root partition (R). db_repair finishes or undoes what an
# interruption left; it never reboots.
#
# DB_STOP_AFTER=N (tests only): exit 99 right after step N.

DB_MARKER=owrt-upgrade.pending

_db_n=0
db_step() {
	sync
	_db_n=$((_db_n + 1))
	[ "$DB_STOP_AFTER" = "$_db_n" ] && exit 99
	return 0
}

db_log() {
	echo "ucgfiber-dualboot: $*" >&2
}

db_sha() { # <file>
	sha256sum <"$1" | cut -c1-64
}

# <n> bytes at offset <off> as lowercase hex; od on stock (Debian), hexdump
# on OpenWrt (its busybox has no od)
db_hex() { # <file> <off> <n>
	if command -v od >/dev/null 2>&1; then
		od -An -tx1 -j"$2" -N"$3" "$1" 2>/dev/null | tr -d ' \n'
	else
		hexdump -v -s "$2" -n "$3" -e '1/1 "%02x"' "$1" 2>/dev/null
	fi
}

# big-endian 32-bit number at byte offset <off>
db_be32() { # <file> <off>
	local h

	h="$(db_hex "$1" "$2" 4)"
	[ "${#h}" = 8 ] && echo $((0x$h))
}

db_size() { # <file>
	wc -c <"$1" | tr -d ' '
}

# a FIT image whose header length equals the file size (not truncated)
db_is_fit() { # <file>
	[ -f "$1" ] || return 1
	[ "$(db_hex "$1" 0 4)" = d00dfeed ] || return 1
	[ "$(db_be32 "$1" 4)" = "$(db_size "$1")" ]
}

db_is_squashfs() { # <file>
	[ "$(db_hex "$1" 0 4)" = 68737173 ]
}

# kind of a kernel file: unknown if it is not an intact FIT image (missing,
# empty, half written), owrt if "openwrt" (any case) is in its header (the
# FIT descriptions), else stock. No stock kernel contains "openwrt" anywhere.
db_kind() { # <file>
	db_is_fit "$1" || { echo unknown; return; }
	if head -c 4096 "$1" | grep -qi openwrt; then
		echo owrt
	else
		echo stock
	fi
}

# the hash stored for <file> matches, or none is stored
db_hash_ok() { # <file>
	[ -f "$1.sha256" ] || return 0
	[ "$(db_sha "$1")" = "$(cat "$1.sha256")" ]
}

# good kernel of the given kind (owrt or stock)
db_good_kernel() { # <file> <kind>
	[ "$(db_kind "$1")" = "$2" ] && db_hash_ok "$1"
}

db_good_rootfs() { # <file>
	db_is_squashfs "$1" && db_hash_ok "$1"
}

db_good_owrt_file() { # <file>
	case "$1" in
	*/rootfs.owrt) db_good_rootfs "$1" ;;
	*) db_good_kernel "$1" owrt ;;
	esac
}

# block device of the GPT partition named <label>, from sysfs (works on
# stock and on OpenWrt, with or without /dev/disk/by-partlabel)
db_find_part() { # <label>
	local u dev

	for u in /sys/class/block/*/uevent; do
		grep -qx "PARTNAME=$1" "$u" 2>/dev/null || continue
		dev="$(sed -n 's/^DEVNAME=//p' "$u")"
		[ -b "/dev/$dev" ] && { echo "/dev/$dev"; return 0; }
	done
	return 1
}

# active system: stock, openwrt or none (no intact uImage)
db_state() { # <K>
	case "$(db_kind "$1/uImage")" in
	owrt) echo openwrt ;;
	stock) echo stock ;;
	*) echo none ;;
	esac
}

# copy <src> to <dst>.new, flush and check it against <sha>
db_stage_copy() { # <src> <dst> <sha>
	rm -f "$2.new"
	cp "$1" "$2.new" || { rm -f "$2.new"; return 1; }
	sync
	{ echo 3 >/proc/sys/vm/drop_caches; } 2>/dev/null
	[ "$(db_sha "$2.new")" = "$3" ] || { db_log "$2.new does not match after writing"; rm -f "$2.new"; return 1; }
}

# the OpenWrt kernel to make active: uImage.owrt; with "emergency" also
# uImage.owrt.bkp when uImage.owrt is damaged (it may not match rootfs.owrt,
# so only when there is no bootable uImage at all)
db_owrt_source() { # <K> [emergency]
	if db_good_kernel "$1/uImage.owrt" owrt; then
		echo "$1/uImage.owrt"
	elif [ "$2" = emergency ] && db_good_kernel "$1/uImage.owrt.bkp" owrt; then
		db_log "uImage.owrt is damaged, using uImage.owrt.bkp"
		echo "$1/uImage.owrt.bkp"
	else
		return 1
	fi
}

# make uImage a fresh copy of the good OpenWrt kernel
db_refresh_uimage() { # <K> [emergency]
	local src sha

	src="$(db_owrt_source "$1" "$2")" || { db_log "no good OpenWrt kernel"; return 1; }
	sha="$(db_sha "$src")"
	[ "$(db_kind "$1/uImage")" = owrt ] && [ "$(db_sha "$1/uImage")" = "$sha" ] && return 0
	db_stage_copy "$src" "$1/uImage" "$sha" || return 1
	db_step
	mv -f "$1/uImage.new" "$1/uImage" || return 1
	db_step
}

db_switch_openwrt() { # <K>
	local k="$1" src sha

	db_repair "$k" "" || return 1
	case "$(db_state "$k")" in
	openwrt) db_log "OpenWrt is already active"; return 0 ;;
	stock) ;;
	*) db_log "no bootable uImage, not switching"; return 1 ;;
	esac
	src="$(db_owrt_source "$k")" || { db_log "no good OpenWrt kernel (uImage.owrt)"; return 1; }
	sha="$(db_sha "$src")"

	db_stage_copy "$src" "$k/uImage" "$sha" || return 1
	db_sha "$k/uImage" >"$k/uImage.stock.sha256.new" || return 1
	db_step
	ln -f "$k/uImage" "$k/uImage.stock" || return 1
	mv -f "$k/uImage.stock.sha256.new" "$k/uImage.stock.sha256" || return 1
	db_step
	mv -f "$k/uImage.new" "$k/uImage" || return 1
	db_step
}

db_switch_stock() { # <K>
	local k="$1"

	db_repair "$k" "" || return 1
	case "$(db_state "$k")" in
	stock) db_log "stock is already active"; return 0 ;;
	openwrt) ;;
	*) db_log "no bootable uImage, not switching"; return 1 ;;
	esac
	db_good_kernel "$k/uImage.stock" stock || { db_log "no good stock kernel (uImage.stock)"; return 1; }

	mv -f "$k/uImage.stock" "$k/uImage" || return 1
	rm -f "$k/uImage.stock.sha256"
	db_step
}

# install (or upgrade) OpenWrt's kernel and system; used by sysupgrade and
# the installer. The sources come from an image whose integrity the caller
# has checked; their hashes become the stored ones.
db_install_owrt() { # <K> <R> <kernel> <rootfs>
	local k="$1" r="$2" ks rs

	[ "$(db_kind "$3")" = owrt ] || { db_log "$3 is not an intact OpenWrt kernel"; return 1; }
	db_is_squashfs "$4" || { db_log "$4 is not a squashfs"; return 1; }
	db_repair "$k" "$r" || return 1
	ks="$(db_sha "$3")"
	rs="$(db_sha "$4")"

	if ! db_stage_copy "$3" "$k/uImage.owrt" "$ks" || ! db_stage_copy "$4" "$r/rootfs.owrt" "$rs" ||
	   ! echo "$ks" >"$k/uImage.owrt.sha256.new" || ! echo "$rs" >"$r/rootfs.owrt.sha256.new"; then
		rm -f "$k/uImage.owrt.new" "$r/rootfs.owrt.new" "$k/uImage.owrt.sha256.new" "$r/rootfs.owrt.sha256.new"
		db_log "staging failed, nothing changed"
		return 1
	fi
	db_step
	: >"$k/$DB_MARKER" || return 1
	db_step
	db_finish_install "$k" "$r"
}

# swap the staged files in; also the roll-forward after an interruption
db_finish_install() { # <K> <R>
	local k="$1" r="$2" f

	for f in "$r/rootfs.owrt" "$k/uImage.owrt"; do
		if [ -f "$f.new" ]; then
			# the current file becomes the backup, unless it is damaged:
			# then the existing backup stays
			if [ -f "$f" ] && db_good_owrt_file "$f"; then
				ln -f "$f" "$f.bkp" || return 1
				if [ -f "$f.sha256" ]; then
					ln -f "$f.sha256" "$f.bkp.sha256" || return 1
				else
					rm -f "$f.bkp.sha256"
				fi
				db_step
			elif [ -f "$f" ]; then
				db_log "not keeping the damaged $(basename "$f") as backup"
			fi
			mv -f "$f.new" "$f" || return 1
			db_step
		fi
		if [ -f "$f.sha256.new" ]; then
			mv -f "$f.sha256.new" "$f.sha256" || return 1
			db_step
		fi
	done
	rm -f "$k/$DB_MARKER"
	db_step
	[ "$(db_state "$k")" = openwrt ] && { db_refresh_uimage "$k" || return 1; }
	return 0
}

# bring the partitions back to a consistent state after an interruption or
# after stock's tools ran; R may be empty (kernel side only). Never reboots.
db_repair() { # <K> <R>
	local k="$1" r="$2" src

	if [ -f "$k/$DB_MARKER" ]; then
		[ -n "$r" ] || { db_log "unfinished OpenWrt upgrade; needs the root partition"; return 1; }
		db_log "finishing an interrupted OpenWrt upgrade"
		db_finish_install "$k" "$r" || return 1
	fi
	rm -f "$k/uImage.new" "$k/uImage.owrt.new" "$k/uImage.owrt.sha256.new" "$k/uImage.stock.sha256.new"
	[ -n "$r" ] && rm -f "$r/rootfs.owrt.new" "$r/rootfs.owrt.sha256.new"

	case "$(db_state "$k")" in
	stock)
		# an interrupted switch, or stock's recovery wrote a new uImage
		# while OpenWrt was active: uImage.stock is outdated
		if [ -e "$k/uImage.stock" ] || [ -e "$k/uImage.stock.sha256" ]; then
			db_log "removing outdated uImage.stock"
			rm -f "$k/uImage.stock" "$k/uImage.stock.sha256"
		fi
		;;
	openwrt)
		# an interrupted upgrade left an older copy as uImage; keep the
		# running uImage if no good OpenWrt kernel is there to replace it
		if db_owrt_source "$k" >/dev/null 2>&1; then
			db_refresh_uimage "$k" || return 1
		else
			db_log "uImage.owrt and uImage.owrt.bkp are damaged; keeping the current uImage"
		fi
		;;
	*)
		if db_good_kernel "$k/uImage.stock" stock; then
			db_log "no bootable uImage, restoring stock's kernel"
			mv -f "$k/uImage.stock" "$k/uImage" || return 1
			rm -f "$k/uImage.stock.sha256"
			db_step
		elif db_owrt_source "$k" emergency >/dev/null 2>&1; then
			db_log "no bootable uImage and no good stock kernel, making OpenWrt bootable"
			db_refresh_uimage "$k" emergency || return 1
		else
			db_log "no bootable uImage and no good kernel to restore"
			return 1
		fi
		;;
	esac
	return 0
}
