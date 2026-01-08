#!/bin/sh
# Usage: verify-kernel-tar-signature.sh <tarball> <signfile>

set -eu

tarball="$1"
signfile="$2"

case "$tarball" in
  *.tar.xz)  decomp="xz -cd"    ;;
  *.tar.gz)  decomp="gzip -cd"  ;;
  *.tar.bz2) decomp="bzip2 -cd" ;;
  *.tar.zst) decomp="zstd -cd"  ;;
  *)
    echo "Unsupported kernel tarball format: $tarball" >&2
    exit 1
    ;;
esac

$decomp "$tarball" | gpg --verify "$signfile" -

