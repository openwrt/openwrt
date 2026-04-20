#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only
#
# Sandbox wrapper for package Build/Compile and Build/Install steps.
#
# Creates a restricted environment where:
# - Only standard system toolchain paths and the OpenWrt source tree are readable
# - Only PKG_BUILD_DIR is writable (plus any SANDBOX_EXTRA_RW paths)
# - Network access is blocked
# - User home (outside TOPDIR), /etc/shadow, /etc/ssh, /etc/sudoers are not exposed
#
# Usage: build-sandbox.sh <pkg_build_dir> [command args...]
#
# Environment:
#   SANDBOX_EXTRA_RW  Colon-separated list of additional read-write paths
#                     (e.g. ccache dir, build log dir)

set -e

PKG_BUILD_DIR="$1"
shift

if [ -z "$PKG_BUILD_DIR" ] || [ ! -d "$PKG_BUILD_DIR" ]; then
	echo "build-sandbox.sh: error: PKG_BUILD_DIR='$PKG_BUILD_DIR' is not a directory" >&2
	exit 1
fi

TOPDIR="$(cd "$(dirname "$0")/.." && pwd)"

case "$(uname -s)" in
Linux)
	BWRAP="${BWRAP:-bwrap}"

	EXTRA_BIND_ARGS=()
	if [ -n "$SANDBOX_EXTRA_RW" ]; then
		IFS=: read -ra EXTRA_RW <<< "$SANDBOX_EXTRA_RW"
		for path in "${EXTRA_RW[@]}"; do
			if [ -n "$path" ] && [ -e "$path" ]; then
				EXTRA_BIND_ARGS+=(--bind "$path" "$path")
			fi
		done
	fi

	# Synthesized /etc/passwd and /etc/group — contain only root, the
	# caller's uid, and nobody. Avoids leaking host usernames, home paths,
	# GECOS fields, and UID layout to the build.
	UID_N=$(id -u)
	GID_N=$(id -g)
	USER_N=$(id -un)
	GROUP_N=$(id -gn)

	exec "$BWRAP" \
		--ro-bind-try /usr /usr \
		--ro-bind-try /bin /bin \
		--ro-bind-try /sbin /sbin \
		--ro-bind-try /lib /lib \
		--ro-bind-try /lib32 /lib32 \
		--ro-bind-try /lib64 /lib64 \
		--ro-bind-try /libx32 /libx32 \
		--ro-bind-try /opt /opt \
		--tmpfs /etc \
		--ro-bind-data 10 /etc/passwd \
		--ro-bind-data 11 /etc/group \
		--ro-bind-try /etc/nsswitch.conf /etc/nsswitch.conf \
		--ro-bind-try /etc/localtime /etc/localtime \
		--ro-bind "$TOPDIR" "$TOPDIR" \
		--bind "$PKG_BUILD_DIR" "$PKG_BUILD_DIR" \
		"${EXTRA_BIND_ARGS[@]}" \
		--dev /dev \
		--proc /proc \
		--bind /tmp /tmp \
		--unshare-net \
		--unshare-pid \
		--unshare-ipc \
		--unshare-uts \
		--unshare-cgroup-try \
		--new-session \
		--hostname sandbox \
		--die-with-parent \
		-- "$@" \
		10< <(printf 'root:x:0:0:root:/:/bin/sh\n%s:x:%s:%s:builder:/:/bin/sh\nnobody:x:65534:65534:nobody:/:/bin/sh\n' "$USER_N" "$UID_N" "$GID_N") \
		11< <(printf 'root:x:0:\n%s:x:%s:\nnobody:x:65534:\n' "$GROUP_N" "$GID_N")
	;;
Darwin)
	PROFILE="${PKG_BUILD_DIR}/.sandbox-profile.sb"

	EXTRA_READ_WRITE_RULES=""
	if [ -n "$SANDBOX_EXTRA_RW" ]; then
		IFS=: read -ra EXTRA_RW <<< "$SANDBOX_EXTRA_RW"
		for path in "${EXTRA_RW[@]}"; do
			if [ -n "$path" ] && [ -e "$path" ]; then
				EXTRA_READ_WRITE_RULES="${EXTRA_READ_WRITE_RULES}
(allow file-read* (subpath \"${path}\"))
(allow file-write* (subpath \"${path}\"))"
			fi
		done
	fi

	cat > "$PROFILE" <<-SBPL_EOF
	(version 1)
	(deny default)

	;; Allow metadata reads (stat, lstat, readlink) on any path so the
	;; kernel can traverse into our allowed subpaths. Actual file contents
	;; are still restricted to the file-read-data allows below.
	(allow file-read-metadata)
	;; /bin/sh and dyld need to read the root directory listing.
	(allow file-read-data (literal "/"))
	;; Follow macOS's selector symlinks (sh for /bin/sh, developer_dir for
	;; xcode-select).
	(allow file-read* (subpath "/private/var/select"))

	;; System toolchain and frameworks (read-only).
	(allow file-read*
	    (subpath "/System")
	    (subpath "/Library")
	    (subpath "/usr")
	    (subpath "/bin")
	    (subpath "/sbin")
	    (subpath "/opt")
	    (subpath "/private/var/db/dyld"))

	;; Allow only Xcode from /Applications (keep the rest private).
	(allow file-read* (subpath "/Applications/Xcode.app"))
	(allow file-read* (subpath "/Applications/Xcode-beta.app"))

	;; Mask sensitive subtrees inside /Library.
	(deny file-read* (subpath "/Library/Keychains"))
	(deny file-read* (subpath "/Library/LaunchAgents"))
	(deny file-read* (subpath "/Library/LaunchDaemons"))

	;; A subset of /private/etc — block sensitive files specifically.
	(allow file-read* (subpath "/private/etc"))
	(deny file-read* (subpath "/private/etc/ssh"))
	(deny file-read* (subpath "/private/etc/sudoers.d"))
	(deny file-read* (literal "/private/etc/sudoers"))
	(deny file-read* (literal "/private/etc/krb5.keytab"))
	(deny file-read* (literal "/private/etc/master.passwd"))

	;; OpenWrt source tree (read-only). PKG_BUILD_DIR overrides below.
	(allow file-read* (subpath "${TOPDIR}"))

	;; Package build directory (read-write).
	(allow file-read*  (subpath "${PKG_BUILD_DIR}"))
	(allow file-write* (subpath "${PKG_BUILD_DIR}"))

	;; Device nodes (/dev/null, /dev/zero, etc.).
	(allow file-read*  (subpath "/dev"))
	(allow file-write* (subpath "/dev"))

	;; /tmp — needed for the make jobserver FIFO.
	(allow file-read*  (subpath "/private/tmp"))
	(allow file-write* (subpath "/private/tmp"))

	;; /private/var/folders — used by macOS system APIs.
	(allow file-read*  (subpath "/private/var/folders"))
	(allow file-write* (subpath "/private/var/folders"))
	${EXTRA_READ_WRITE_RULES}

	;; Allow process execution and forking.
	(allow process-exec*)
	(allow process-fork)

	;; System operations needed by build tools.
	(allow sysctl-read)
	(allow mach-lookup)
	(allow signal (target self))

	;; No network.
	(deny network*)
	SBPL_EOF

	exec sandbox-exec -f "$PROFILE" "$@"
	;;
*)
	echo "build-sandbox.sh: error: unsupported platform '$(uname -s)'" >&2
	exit 1
	;;
esac
