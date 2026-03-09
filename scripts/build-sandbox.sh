#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only
#
# Sandbox wrapper for package Build/Compile and Build/Install steps.
#
# Creates a restricted environment where:
# - The entire filesystem is read-only
# - Only PKG_BUILD_DIR is writable (plus any SANDBOX_EXTRA_RW paths)
# - Network access is blocked
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

	exec "$BWRAP" \
		--ro-bind / / \
		--bind "$PKG_BUILD_DIR" "$PKG_BUILD_DIR" \
		"${EXTRA_BIND_ARGS[@]}" \
		--dev /dev \
		--proc /proc \
		--tmpfs /tmp \
		--unshare-net \
		--die-with-parent \
		-- "$@"
	;;
Darwin)
	PROFILE="${PKG_BUILD_DIR}/.sandbox-profile.sb"

	EXTRA_WRITE_RULES=""
	if [ -n "$SANDBOX_EXTRA_RW" ]; then
		IFS=: read -ra EXTRA_RW <<< "$SANDBOX_EXTRA_RW"
		for path in "${EXTRA_RW[@]}"; do
			if [ -n "$path" ] && [ -e "$path" ]; then
				EXTRA_WRITE_RULES="${EXTRA_WRITE_RULES}
(allow file-write* (subpath \"${path}\"))"
			fi
		done
	fi

	cat > "$PROFILE" <<-SBPL_EOF
	(version 1)
	(deny default)

	;; Allow all reads (toolchain, headers, system libs, scripts)
	(allow file-read*)

	;; Allow writes ONLY to the package build directory
	(allow file-write* (subpath "${PKG_BUILD_DIR}"))

	;; Allow writes to device nodes (/dev/null, /dev/zero, etc.)
	(allow file-write* (subpath "/dev"))

	;; Allow writes to /private/var/folders (used by macOS system APIs)
	(allow file-write* (subpath "/private/var/folders"))
	${EXTRA_WRITE_RULES}

	;; Allow process execution and forking
	(allow process-exec*)
	(allow process-fork)

	;; Allow system operations needed by build tools
	(allow sysctl-read)
	(allow mach-lookup)
	(allow signal (target self))

	;; Block all network access
	(deny network*)
	SBPL_EOF

	exec sandbox-exec -f "$PROFILE" "$@"
	;;
*)
	echo "build-sandbox.sh: error: unsupported platform '$(uname -s)'" >&2
	exit 1
	;;
esac
