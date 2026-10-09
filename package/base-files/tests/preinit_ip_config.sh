#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

# Regression test for boards whose DSA LAN device is registered after preinit
# starts.  The command shims make the race deterministic and keep the test
# runnable on a regular Linux host without a target board or CAP_NET_ADMIN.

set -eu

ROOT=$(CDPATH= cd -- "$(dirname "$0")/../../.." && pwd)
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

mkdir "$TMPDIR/bin"
cat > "$TMPDIR/bin/grep" <<'EOF'
#!/bin/sh

if [ "$#" -eq 3 ] && [ "$1" = "-q" ] &&
   [ "$2" = "lan1" ] && [ "$3" = "/proc/net/dev" ]; then
	count_file=${PREINIT_TEST_TMP}/grep-count
	count=$(cat "$count_file" 2>/dev/null || echo 0)
	count=$((count + 1))
	echo "$count" > "$count_file"
	[ "$count" -ge 3 ]
	exit $?
fi

exec /usr/bin/grep "$@"
EOF
cat > "$TMPDIR/bin/ip" <<'EOF'
#!/bin/sh
echo "$*" >> "$PREINIT_TEST_TMP/ip.log"
EOF
cat > "$TMPDIR/bin/sleep" <<'EOF'
#!/bin/sh
# Avoid making the deterministic retry test take five seconds.
:
EOF
chmod +x "$TMPDIR/bin/grep" "$TMPDIR/bin/ip" "$TMPDIR/bin/sleep"

export PREINIT_TEST_TMP="$TMPDIR"
export PATH="$TMPDIR/bin:$PATH"

boot_hook_add() { :; }
. "$ROOT/package/base-files/files/lib/preinit/10_indicate_preinit"

pi_ip=192.168.1.1
pi_netmask=255.255.255.0
pi_broadcast=192.168.1.255
preinit_ip_config lan1

/usr/bin/grep -q '^link set dev lan1 up$' "$TMPDIR/ip.log"
expected='-4 address add 192.168.1.1/255.255.255.0 broadcast 192.168.1.255 dev lan1'
/usr/bin/grep -Fqx -- "$expected" "$TMPDIR/ip.log"
[ "$(cat "$TMPDIR/grep-count")" -ge 3 ]

# Keep the board contract covered alongside the preinit race: the reset key
# must remain the active-low GPIO 9 KEY_RESTART input used by failsafe.
DTS="$ROOT/target/linux/mediatek/dts/mt7986a-glinet-gl-mt6000.dts"
/usr/bin/grep -Fq 'label = "reset";' "$DTS"
/usr/bin/grep -Fq 'linux,code = <KEY_RESTART>;' "$DTS"
/usr/bin/grep -Fq 'gpios = <&pio 9 GPIO_ACTIVE_LOW>;' "$DTS"

echo "preinit_ip_config and GL-MT6000 reset metadata: PASS"
