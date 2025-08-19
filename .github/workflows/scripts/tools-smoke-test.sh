#!/usr/bin/env bash

set -e

self="$0"

__errmsg() {
	echo "$*" >&2
}

usage() {
	cat >&2 <<EOF
Usage: $self [-h|--help]
       $self <all>|<sdk>|<imagebuilder>|<toolchain>|<bpf> <target> [<subtarget>]
EOF
}

cleanup() {
	local exit_code=$?
	[ -n "$tmpdir" ] && [ -d "$tmpdir" ] && rm -rf "$tmpdir"
	exit $exit_code
}

trap cleanup EXIT INT TERM

tmpdir="$(mktemp -d)"
[ -d "$tmpdir" ] || {
	__errmsg "Failed to create temporary directory $tmpdir"
	exit 1
}

parse_args() {
	while [ "$#" -gt 0 ]; do
		case "$1" in
			--help|-h)
				usage
				exit 0
				;;
			*)
				if [ -z "$o_test" ]; then
					o_test="$1"
				elif [ -z "$o_target" ]; then
					o_target="$1"
				elif [ -z "$o_subtarget" ]; then
					o_subtarget="$1"
				else
					o_extra+=("$1")
				fi

				shift
				;;
		esac
	done

	[ -n "$o_test" ] || [ -n "$o_target" ] || {
		usage
		return 1
	}

	[ -n "$o_subtarget" ] || o_subtarget="generic"

	eval "$(grep ^CONFIG_BINARY_FOLDER= .config 2>/dev/null)"
	o_bindir="${CONFIG_BINARY_FOLDER:-bin}"
	o_targetdir="${o_bindir}/targets/$o_target/$o_subtarget"
}

test_sdk() {
	local sdk

	sdk=$(find "$o_targetdir" -name "openwrt-sdk*.Linux-x86_64.tar.zst" -type f)
	[ -n "$sdk" ] || {
		__errmsg "SDK file not found in $o_targetdir"
		return 1
	}

	tar -C "$tmpdir" -xf "$sdk"
	pushd "$tmpdir"/openwrt-sdk* > /dev/null
	make defconfig
	sed -i 's|git.openwrt.org|github.com|' feeds.conf.default
	./scripts/feeds update base
	./scripts/feeds install -p base busybox
	make -j "$(nproc)" package/busybox/compile V=sc
	find "bin/packages" -name "busybox*.ipk" -o -name "busybox*.apk"

	echo "[*] sdk test passed"
	popd > /dev/null
}

test_imagebuilder() {
	local ib

	ib=$(find "$o_targetdir" -name "openwrt-imagebuilder*.Linux-x86_64.tar.zst" -type f)
	[ -n "$ib" ] || {
		__errmsg "ImageBuilder file not found in $o_targetdir"
		return 1
	}

	tar -C "$tmpdir" -xf "$ib"
	pushd "$tmpdir"/openwrt-imagebuilder* > /dev/null
	make image PACKAGES="tcpdump"
	find "bin/targets/$o_target/$o_subtarget" -name "openwrt-*rootfs.tar.gz"

	echo "[*] imagebuilder test passed"
	popd > /dev/null
}

test_bpf() {
	local bpf

	bpf=$(find "$o_targetdir" -name "llvm-bpf-*.Linux-x86_64.tar.zst" -type f)
	[ -n "$bpf" ] || {
		__errmsg "BPF toolchain file not found in $o_targetdir"
		return 1
	}

	tar -C "$tmpdir" -xf "$bpf"
	pushd "$tmpdir"/llvm-bpf*Linux-x86_64 > /dev/null

cat > test.c << 'EOF'
	typedef unsigned int __u32;
	#define SEC(NAME) __attribute__((section(NAME), used))
	#define XDP_PASS 2
	struct xdp_md {
		__u32 data;
		__u32 data_end;
		__u32 data_meta;
		__u32 ingress_ifindex;
		__u32 rx_queue_index;
		__u32 egress_ifindex;
	};

	SEC("xdp")
	int xdp_test(struct xdp_md *ctx) {
		return XDP_PASS;
	}
EOF

	./bin/clang -target bpf -c test.c -o test.o
	./bin/llvm-objdump -S test.o | grep "<xdp_test>:"
	./bin/llvm-objdump -h test.o | grep "file format elf.*-bpf$"
	./bin/llvm-objdump -h test.o | grep "xdp .* TEXT"

	echo "[*] bpf test passed"
	popd > /dev/null
}

test_toolchain() {
	local toolchain

	toolchain=$(find "$o_targetdir" -name "openwrt-toolchain*.Linux-x86_64.tar.zst" -type f)
	[ -n "$toolchain" ] || {
		__errmsg "Toolchain file not found in $o_targetdir"
		return 1
	}

	tar -C "$tmpdir" -xf "$toolchain"
	pushd "$tmpdir"/openwrt-toolchain*/toolchain* > /dev/null

	compiler=$(find ./bin -name '*-openwrt-linux-gcc' -type l)
	objdump=$(find ./bin -name '*-openwrt-linux-objdump' -type l)

	[ -n "$compiler" ] || {
		__errmsg "gcc not found in $tmpdir/openwrt-toolchain*/toolchain*"
		return 1
	}

	[ -n "$objdump" ] || {
		__errmsg "objdump not found in $tmpdir/openwrt-toolchain*/toolchain*"
		return 1
	}

cat > test.c << 'EOF'
	#include <stdio.h>

	int main(void) {
		printf("Hello world!\n");
		return 0;
	}
EOF

	"$compiler" test.c -o test
	"$objdump" -S ./test | grep "file format elf"
	"$objdump" -S ./test | grep "<main>:"

	echo "[*] toolchain test passed"
	popd > /dev/null
}

tools_smoke_test() {
	case "$o_test" in
		sdk)			test_sdk			;;
		bpf)			test_bpf			;;
		imagebuilder)	test_imagebuilder	;;
		toolchain)		test_toolchain		;;
		all)
			test_bpf
			test_toolchain
			test_imagebuilder
			test_sdk
			echo "[*] all tests passed"
			;;
		*)
			__errmsg "smoke test of $o_test is not supported yet"
			return 1
			;;
	esac
}

parse_args "$@" && tools_smoke_test
