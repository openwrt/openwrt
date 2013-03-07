#!/bin/sh
ver=$(uname -r)
arch=$(uname -m)
echo "Preparing OpenBSD $arch $ver for OpenWrt"
PKG_PATH="http://ftp.openbsd.org/pub/OpenBSD/${ver}/packages/${arch}/"
export PKG_PATH
pkg_add -v gmake
pkg_add -v findutils
pkg_add -v coreutils
pkg_add -v gdiff
pkg_add -v rsync--
pkg_add -v git
pkg_add -v bash
pkg_add -v unzip
pkg_add -v wget
pkg_add -v gtar--
pkg_add -v ggrep
pkg_add -v gawk
pkg_add -v gsed
pkg_add -v xz
pkg_add -v apr-util--
pkg_add -v cyrus-sasl--
pkg_add -v subversion
pkg_add -v gnugetopt
