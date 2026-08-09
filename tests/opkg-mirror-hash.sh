#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Keep the opkg source mirror hash synchronized with the pinned upstream
# commit.  This catches accidental reintroduction of the stale hash that
# makes a clean package download fail before the build starts.

set -eu

TOPDIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
makefile=$TOPDIR/package/system/opkg/Makefile

source_version=$(sed -n 's/^PKG_SOURCE_VERSION:=//p' "$makefile")
mirror_hash=$(sed -n 's/^PKG_MIRROR_HASH:=//p' "$makefile")

test "$source_version" = 80503d94e356476250adaf1f669ee955ec26de76
test "$mirror_hash" = dbe5cb21e881d60733587cad22e01aab52ab5261b5f21003d32d06ff88442add
