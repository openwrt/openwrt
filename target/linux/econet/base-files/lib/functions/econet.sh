# SPDX-License-Identifier: GPL-2.0

get_mac_gs3101() {
	local part_name="romfile"
	local idx;

	idx=$(find_mtd_index "$part_name")
	if [ -z "$idx" ]; then
		echo "get_mac_gs3101: partition $part_name not found" >&2
		return 1
	fi

	# <Entry0 Mac="5ac876db9bf0" newHwUpBytes="0" newHwDownBytes="0"
	dd if="/dev/mtd${idx}" 2>/dev/null | \
		gunzip 2>/dev/null | \
		strings | \
		sed -n -e 's/^.*Mac="\([a-f0-9]\{12\}\)".*$/\1/p' | \
		while read mac; do
			mac=$(macaddr_canonicalize "$mac")
			[ "$mac" = "" ] && continue
			# For some reason, GS3101 stores 5a:c8:76:...
			# But the MAC address is 58:c8:76:...
			echo "$(macaddr_unsetbit "$mac" 7)"
			break
		done | \
		grep '[a-f0-9:]\{17\}' && return 0

	echo "get_mac_gs3101: could not find mac address" >&2
	return 1
}

# Boot flags in the "benv" CSID store used by the D-Link R18 bootloader.
# Block 0: 16-byte "stag" header (sum of bytes 0..13 at 14), 16-byte "csxf"
# header ("CS", sum at 18 over the csxf header with sum=0 plus the body,
# odd tail padded with 0xff), body = {u32 id, u16 len, value}..., u32 0.
# Sums are the inverted 16-bit one's complement sum. Records:
#   0x80cc0000 sw_curboot_id	slot booted this time (set by the bootloader)
#   0x80cc0001 sw_commit	known-good slot (0 = tclinux, 1 = tclinux_slave)
#   0x80cc0002 sw_active	slot to boot, reset to sw_commit after a failed try
#   0x80cc0003 sw_tryactive	one-shot: boot slot (value - 1) once
# Only existing 4-byte records are changed, the body length never changes.

BENV_BLOCK_SIZE=131072

_benv_awk() {
	local file="$1" mode="$2"
	shift 2

	hexdump -v -n 4096 -e '1/1 "%u\n"' "$file" | awk -v mode="$mode" -v sets="$*" '
	function u16(i) { return b[i] + 256 * b[i + 1] }
	function u32(i) { return u16(i) + 65536 * u16(i + 2) }
	function osum(s, e,   i, sum) {
		sum = 0
		for (i = s; i < e; i += 2) {
			sum += b[i] + 256 * ((i + 1 < e) ? b[i + 1] : 255)
			sum = (sum % 65536) + int(sum / 65536)
		}
		return sum
	}
	function csxf_sum(   s0, s1, r) {
		s0 = b[18]; s1 = b[19]; b[18] = 0; b[19] = 0
		r = 65535 - osum(16, 32 + blen)
		b[18] = s0; b[19] = s1
		return r
	}
	function fail(msg) { print "benv: " msg > "/dev/stderr"; err = 1; exit 1 }
	{ b[NR - 1] = $1 + 0 }
	END {
		if (err) exit 1
		if (NR < 64) fail("short read")
		if (b[2] != 36 || b[3] != 43) fail("bad stag magic")
		if (u16(14) != 65535 - osum(0, 14)) fail("stag checksum mismatch")
		if (u16(16) != 21315) fail("bad csxf magic")
		blen = u32(20)
		if (blen < 4 || 32 + blen > NR) fail("bad body length")
		if (u16(18) != csxf_sum()) fail("csxf checksum mismatch")

		split("sw_curboot_id sw_commit sw_active sw_tryactive", nm, " ")
		base = 2160852992	# 0x80cc0000
		n = 0
		for (o = 32; o + 4 <= 32 + blen; o += 6 + rlen[n]) {
			if (u32(o) == 0)
				break
			n++; rid[n] = u32(o); rlen[n] = u16(o + 4); roff[n] = o + 6
		}

		if (mode == "show") {
			for (i = 1; i <= n; i++)
				if (rid[i] >= base && rid[i] < base + 4 && rlen[i] == 4)
					printf "%s=%d\n", nm[rid[i] - base + 1], u32(roff[i])
			exit 0
		}

		cnt = split(sets, kv, " ")
		for (k = 1; k <= cnt; k++) {
			split(kv[k], p, "=")
			want = -1
			for (j = 1; j <= 4; j++)
				if (nm[j] == p[1])
					want = base + j - 1
			if (want < 0 || p[2] !~ /^[0-9]+$/)
				fail("bad setting " kv[k])
			found = 0
			for (i = 1; i <= n; i++) {
				if (rid[i] != want || rlen[i] != 4)
					continue
				v = p[2] + 0; found = 1
				for (x = 0; x < 4; x++) {
					nb = v % 256; v = int(v / 256)
					if (b[roff[i] + x] != nb) { b[roff[i] + x] = nb; ch[roff[i] + x] = 1 }
				}
			}
			if (!found)
				fail("record not present: " p[1])
		}
		s = csxf_sum()
		if (b[18] != s % 256) { b[18] = s % 256; ch[18] = 1 }
		if (b[19] != int(s / 256)) { b[19] = int(s / 256); ch[19] = 1 }
		for (x in ch)
			print x, b[x]
	}'
}

# econet_benv_show: print the boot flags as name=value, fail on a bad block
econet_benv_show() {
	local idx tmp ret

	idx=$(find_mtd_index benv) || return 1
	[ -n "$idx" ] || { echo "benv: partition not found" >&2; return 1; }
	tmp=$(mktemp)
	dd if=/dev/mtd${idx}ro of=$tmp bs=$BENV_BLOCK_SIZE count=1 2>/dev/null
	_benv_awk $tmp show
	ret=$?
	rm -f $tmp
	return $ret
}

# econet_benv_get <name>
econet_benv_get() {
	econet_benv_show | sed -n "s/^$1=//p"
}

# econet_benv_set <name>=<value>...: patch block 0, check, write, read back
econet_benv_set() {
	local idx cur new patch off val

	idx=$(find_mtd_index benv) || return 1
	[ -n "$idx" ] || { echo "benv: partition not found" >&2; return 1; }
	cur=$(mktemp); new=$(mktemp); patch=$(mktemp)
	dd if=/dev/mtd${idx}ro of=$cur bs=$BENV_BLOCK_SIZE count=1 2>/dev/null
	if ! _benv_awk $cur patch "$@" > $patch; then
		rm -f $cur $new $patch
		return 1
	fi
	cp $cur $new
	while read off val; do
		printf "\\$(printf %o $val)" |
			dd of=$new bs=1 seek=$off conv=notrunc 2>/dev/null
	done < $patch
	rm -f $patch

	if ! _benv_awk $new show > /dev/null; then
		echo "benv: new block invalid, not written" >&2
		rm -f $cur $new
		return 1
	fi
	if cmp -s $cur $new; then
		rm -f $cur $new
		return 0
	fi

	flash_erase -q /dev/mtd$idx 0 1 && nandwrite -q -p /dev/mtd$idx $new
	sync
	if ! dd if=/dev/mtd${idx}ro bs=$BENV_BLOCK_SIZE count=1 2>/dev/null |
	     cmp -s - $new; then
		echo "benv: read-back mismatch" >&2
		rm -f $cur $new
		return 1
	fi
	rm -f $cur $new
	return 0
}
