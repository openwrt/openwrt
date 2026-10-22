#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only
#
# Sandbox wrapper for package Build/Configure, Build/Compile and
# Build/Install steps.
#
# Creates a restricted environment where:
# - Only standard system toolchain paths and the OpenWrt source tree are readable
# - Only PKG_BUILD_DIR is writable (plus any SANDBOX_EXTRA_RW paths)
# - A private /tmp is provided; only the make jobserver FIFO is shared
# - Network access is blocked
# - User home (outside TOPDIR), /etc/shadow, /etc/ssh, /etc/sudoers are not exposed
#
# Usage: build-sandbox.sh <pkg_build_dir> [command args...]
#
# Environment:
#   SANDBOX_EXTRA_RO  Colon-separated list of additional read-only paths
#                     (e.g. download dir, external toolchain)
#   SANDBOX_EXTRA_RW  Colon-separated list of additional read-write paths
#                     (e.g. ccache dir, build log dir)

set -e

if [ -z "$1" ] || [ ! -d "$1" ]; then
	echo "build-sandbox.sh: error: PKG_BUILD_DIR='$1' is not a directory" >&2
	exit 1
fi

# Both bwrap and Seatbelt match on canonical paths, so resolve symlinks
# (e.g. /tmp -> /private/tmp on macOS) before handing paths to them.
canon_dir() {
	(cd "$1" 2>/dev/null && pwd -P)
}

canon_file() {
	local dir
	dir="$(canon_dir "$(dirname "$1")")" || return 1
	echo "$dir/$(basename "$1")"
}

PKG_BUILD_DIR="$(canon_dir "$1")"
shift

TOPDIR="$(canon_dir "$(dirname "$0")/..")"

# Canonicalise the extra paths too, so e.g. a dl/ symlinked out of TOPDIR
# is bound/allowed where it really lives. Missing paths are dropped.
EXTRA_RO=()
EXTRA_RW=()
IFS=: read -ra paths <<< "${SANDBOX_EXTRA_RO:-}"
for path in "${paths[@]}"; do
	[ -n "$path" ] && path="$(canon_dir "$path")" && EXTRA_RO+=("$path")
done
IFS=: read -ra paths <<< "${SANDBOX_EXTRA_RW:-}"
for path in "${paths[@]}"; do
	[ -n "$path" ] && path="$(canon_dir "$path")" && EXTRA_RW+=("$path")
done

# GNU make >= 4.4 hands the jobserver to sub-makes as a named pipe in the
# temp dir. Expose just that FIFO instead of the whole host /tmp.
JOBSERVER_FIFO=""
case "${MAKEFLAGS:-}" in
*--jobserver-auth=fifo:*)
	JOBSERVER_FIFO="${MAKEFLAGS##*--jobserver-auth=fifo:}"
	JOBSERVER_FIFO="$(canon_file "${JOBSERVER_FIFO%% *}")"
	;;
esac

case "$(uname -s)" in
Linux)
	BWRAP="${BWRAP:-bwrap}"

	BIND_ARGS=()
	for path in "${EXTRA_RO[@]}"; do
		BIND_ARGS+=(--ro-bind "$path" "$path")
	done
	for path in "${EXTRA_RW[@]}"; do
		BIND_ARGS+=(--bind "$path" "$path")
	done
	if [ -n "$JOBSERVER_FIFO" ] && [ -p "$JOBSERVER_FIFO" ]; then
		BIND_ARGS+=(--bind "$JOBSERVER_FIFO" "$JOBSERVER_FIFO")
	fi

	# Synthesized /etc/passwd and /etc/group — contain only root, the
	# caller's uid, and nobody. Avoids leaking host usernames, home paths,
	# GECOS fields, and UID layout to the build. The uid may have no
	# passwd entry at all (e.g. containers started with --user).
	UID_N=$(id -u)
	GID_N=$(id -g)
	USER_N=$(id -un 2>/dev/null || echo builder)
	GROUP_N=$(id -gn 2>/dev/null || echo builder)

	# /etc is a tmpfs; bind only what the toolchain needs: alternatives
	# symlinks (Debian cc/awk/yacc, Fedora ld), the dynamic loader
	# configuration, NSS and the timezone.
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
		--ro-bind-try /etc/alternatives /etc/alternatives \
		--ro-bind-try /etc/ld.so.cache /etc/ld.so.cache \
		--ro-bind-try /etc/ld.so.conf /etc/ld.so.conf \
		--ro-bind-try /etc/ld.so.conf.d /etc/ld.so.conf.d \
		--ro-bind-try /etc/nsswitch.conf /etc/nsswitch.conf \
		--ro-bind-try /etc/localtime /etc/localtime \
		--ro-bind "$TOPDIR" "$TOPDIR" \
		--bind "$PKG_BUILD_DIR" "$PKG_BUILD_DIR" \
		--dev /dev \
		--proc /proc \
		--tmpfs /tmp \
		--tmpfs /var/tmp \
		"${BIND_ARGS[@]}" \
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
	EXTRA_RULES=""
	for path in "${EXTRA_RO[@]}"; do
		EXTRA_RULES="${EXTRA_RULES}
(allow file-read* (subpath \"${path}\"))"
	done
	for path in "${EXTRA_RW[@]}"; do
		EXTRA_RULES="${EXTRA_RULES}
(allow file-read* (subpath \"${path}\"))
(allow file-write* (subpath \"${path}\"))"
	done

	# Scratch space: /tmp (Seatbelt cannot provide a private one and e.g.
	# Lua's os.tmpname() ignores TMPDIR) plus the per-user temp and cache
	# dirs that system frameworks use. TMPDIR is passed in via
	# SANDBOX_EXTRA_RW.
	TMP_RULES=""
	for path in /tmp \
		"$(getconf DARWIN_USER_TEMP_DIR 2>/dev/null)" \
		"$(getconf DARWIN_USER_CACHE_DIR 2>/dev/null)"; do
		[ -n "$path" ] || continue
		path="$(canon_dir "$path")" || continue
		TMP_RULES="${TMP_RULES}
(allow file-read* (subpath \"${path}\"))
(allow file-write* (subpath \"${path}\"))"
	done
	if [ -n "$JOBSERVER_FIFO" ]; then
		TMP_RULES="${TMP_RULES}
(allow file-read* file-write* (literal \"${JOBSERVER_FIFO}\"))"
	fi

	PROFILE="$(cat <<-SBPL_EOF
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
	    (subpath "/private/var/db/dyld")
	    (subpath "/private/var/db/timezone"))

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
	${TMP_RULES}
	${EXTRA_RULES}

	;; Allow process execution and forking.
	(allow process-exec*)
	(allow process-fork)

	;; System operations needed by build tools.
	(allow sysctl-read)
	;; Unrestricted mach-lookup is needed for libSystem (opendirectoryd for
	;; getpwuid, notifyd, ...), but it also reaches securityd and friends;
	;; Seatbelt offers no good allowlist for the services libc needs.
	(allow mach-lookup)
	(allow signal (target self))

	;; No network.
	(deny network*)
	SBPL_EOF
	)"

	exec sandbox-exec -p "$PROFILE" "$@"
	;;
*)
	echo "build-sandbox.sh: error: unsupported platform '$(uname -s)'" >&2
	exit 1
	;;
esac
