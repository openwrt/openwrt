#!/usr/bin/env bash
set -euo pipefail

usage() {
	cat <<'EOF'
Usage:
  verify_router_overlay_swap.sh [--router root@192.168.1.1] [--url URL]

Waits for a freshly flashed router and verifies that firmware boot policy
mounted the prepared USB overlay and enabled USB swap.

Expected router state:
  /dev/sda1 ext4 label overlay mounted at /overlay
  overlayfs:/overlay mounted at /
  /dev/sda2 swap label swap active in /proc/swaps
  HTTP reachable at the configured URL

Defaults:
  --router root@192.168.1.1
  --url    http://192.168.1.1

Required host utilities:
  ./scripts/verify_router_overlay_swap.sh --get-deps
EOF
}

die() {
	local code="${2:-1}"
	echo "ERROR: $1" >&2
	exit "$code"
}

require_cmds() {
	local missing=()
	local cmd
	for cmd in "$@"; do
		command -v "$cmd" >/dev/null 2>&1 || missing+=("$cmd")
	done
	[ "${#missing[@]}" -eq 0 ] || die "missing required utilities: ${missing[*]}"
}

get_deps() {
	printf '%s\n' curl ping sed sleep ssh ssh-keygen
}

router_host() {
	printf '%s\n' "$router" | sed 's/.*@//; s/:.*//'
}

clear_known_host() {
	local host
	host="$(router_host)"
	ssh-keygen -f "$HOME/.ssh/known_hosts" -R "$host" >/dev/null 2>&1 || true
}

router_ping() {
	ping -c 1 -W 1 "$(router_host)" >/dev/null 2>&1
}

ssh_router() {
	ssh \
		-o BatchMode=yes \
		-o ConnectTimeout=3 \
		-o ConnectionAttempts=1 \
		-o LogLevel=ERROR \
		-o StrictHostKeyChecking=accept-new \
		"$router" "$@"
}

wait_for_ssh() {
	local i
	local ssh_output
	local last_error=""
	echo "Waiting for SSH on $router..."
	clear_known_host
	for ((i = 1; i <= 180; i++)); do
		if ssh_output="$(ssh_router 'echo ok' 2>&1)"; then
			echo "SSH is ready."
			return 0
		fi
		last_error="$(printf '%s\n' "$ssh_output" | tr -d '\r' | tail -n 1)"
		sleep 2
	done
	die "SSH did not become ready on $router; last error: ${last_error:-no output}"
}

wait_for_router_network_up() {
	local i
	echo "Waiting for router network..."
	for ((i = 1; i <= 300; i++)); do
		if router_ping; then
			echo "Router network is reachable."
			return 0
		fi
		sleep 1
	done
	die "router network did not become reachable"
}

http_ready() {
	local code
	code="$(curl -sS -o /dev/null --max-time 5 -w '%{http_code}' "$router_url" 2>/dev/null || true)"
	case "$code" in
		200|301|302|303|307|308|401|403)
			return 0
			;;
		*)
			return 1
			;;
	esac
}

wait_for_http() {
	local i
	echo "Waiting for HTTP: $router_url"
	for ((i = 1; i <= 180; i++)); do
		if http_ready; then
			echo "HTTP is ready."
			return 0
		fi
		sleep 2
	done
	die "HTTP did not become ready: $router_url"
}

verify_router() {
	echo "Verifying USB overlay and swap..."
	ssh_router "sh -s" <<'ROUTER_SH'
set -eu

fail() {
	echo "ERROR: $*" >&2
	exit 1
}

label_of() {
	dev="$1"
	block info "$dev" | sed -n 's/.*LABEL="\([^"]*\)".*/\1/p'
}

type_of() {
	dev="$1"
	block info "$dev" | sed -n 's/.*TYPE="\([^"]*\)".*/\1/p'
}

mount | grep -q '/dev/sda1 on /overlay type ext4' || fail "/dev/sda1 is not mounted at /overlay"
mount | grep -q 'overlayfs:/overlay on / type overlay' || fail "overlayfs:/overlay is not mounted at /"
[ -b /dev/sda2 ] || fail "/dev/sda2 does not exist"
[ "$(type_of /dev/sda1)" = "ext4" ] || fail "/dev/sda1 is not ext4"
[ "$(type_of /dev/sda2)" = "swap" ] || fail "/dev/sda2 is not swap"
[ "$(label_of /dev/sda1)" = "overlay" ] || fail "/dev/sda1 label must be overlay"
[ "$(label_of /dev/sda2)" = "swap" ] || fail "/dev/sda2 label must be swap"
grep -q '^/dev/sda2[[:space:]]' /proc/swaps || fail "/dev/sda2 swap is not active"

echo "Router state:"
mount | grep -E ' /overlay |overlayfs'
df -h
cat /proc/swaps
free
ROUTER_SH
}

router="root@192.168.1.1"
router_url="http://192.168.1.1"

while [ "$#" -gt 0 ]; do
	case "$1" in
		-h|--help)
			usage
			exit 0
			;;
		--get-deps)
			get_deps
			exit 0
			;;
		--router)
			[ "$#" -ge 2 ] || die "--router requires a value"
			router="$2"
			shift 2
			;;
		--url)
			[ "$#" -ge 2 ] || die "--url requires a value"
			router_url="$2"
			shift 2
			;;
		*)
			die "unknown argument: $1"
			;;
	esac
done

require_cmds $(get_deps)

echo "Waiting for the router to boot with USB overlay and swap active."
echo "The prepared USB flash must be inserted before the router boots."
echo "Old routers need several minutes during first boot led blinking."
wait_for_router_network_up
wait_for_ssh
wait_for_http
verify_router
echo "DONE: router USB overlay and swap verified active."
echo "WARNING: NEVER REMOVE the USB flash drive while the router is running."
echo
