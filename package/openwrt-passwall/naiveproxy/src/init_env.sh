#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (C) 2021 ImmortalWrt.org
# --------------------------------------------------------
# Init build dependencies for naiveproxy

# Read args from shell
target_arch="$1"
cpu_type="$2"
cpu_subtype="$3"
toolchain_dir="$4"

# Set arch info
case "${target_arch}" in
"aarch64")
	naive_arch="arm64"
	;;
"i386")
	naive_arch="x86"
	;;
"x86_64")
	naive_arch="x64"
	;;
*)
	naive_arch="${target_arch}"
	;;
esac

# OS detection
[ "$(uname)" != "Linux" -o "$(uname -m)" != "x86_64" ] && { echo -e "Support Linux AMD64 only."; exit 1; }

# Create TMP dir
mkdir -p "$PWD/tmp"
export TMPDIR="$PWD/tmp"

# Set ENV
export DEPOT_TOOLS_WIN_TOOLCHAIN=0
export naive_flags="
is_official_build=true
exclude_unwind_tables=true
enable_resource_allowlist_generation=false
symbol_level=0
is_clang=true
use_sysroot=false

fatal_linker_warnings=false
treat_warnings_as_errors=false

enable_base_tracing=false
use_udev=false
use_aura=false
use_ozone=false
use_gio=false
use_gtk=false
use_platform_icu_alternatives=true
use_glib=false

disable_file_support=true
enable_websockets=false
use_kerberos=false
enable_mdns=false
enable_reporting=false
include_transport_security_state_preload_list=false
use_nss_certs=false

enable_backup_ref_ptr_support=false
enable_dangling_raw_ptr_checks=false

target_os=\"openwrt\"
target_cpu=\"${naive_arch}\"
target_sysroot=\"${toolchain_dir}\""

case "${target_arch}" in
"arm")
	naive_flags+=" arm_version=0 arm_cpu=\"${cpu_type}\""
	case "${cpu_type}" in "arm1176jzf-s"|"arm926ej-s"|"mpcore"|"xscale") naive_flags+=" arm_use_thumb=false" ;; esac
	if [ -n "${cpu_subtype}" ]; then
		if grep -q "neon" <<< "${cpu_subtype}"; then
			neon_flag="arm_use_neon=true"
		else
			neon_flag="arm_use_neon=false"
		fi
		naive_flags+=" arm_fpu=\"${cpu_subtype}\" arm_float_abi=\"hard\" ${neon_flag}"
	else
		naive_flags+=" arm_float_abi=\"soft\" arm_use_neon=false"
	fi
	;;
"arm64")
	[ -n "${cpu_type}" ] && naive_flags+=" arm_cpu=\"${cpu_type}\""
	;;
"mipsel"|"mips64el")
	naive_flags+=" use_thin_lto=false chrome_pgo_phase=0"
	if [ -z "${cpu_type}" ]; then
		naive_flags+=" mips_arch_variant=\"r1\""
	else
		naive_flags+=" mips_arch_variant=\"r2\""
	fi
	if [ "${target_arch}" == "mipsel" ]; then
		if [ "${cpu_subtype}" == "24kf" ]; then
			naive_flags+=" mips_float_abi=\"hard\""
		else
			naive_flags+=" mips_float_abi=\"soft\""
		fi
	fi
	;;
esac
