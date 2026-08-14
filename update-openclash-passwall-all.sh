#!/usr/bin/env bash
set -Eeuo pipefail

export LC_ALL=C

ROOT="${OPENWRT_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)}"
MODE=all
JOBS="${JOBS:-12}"

usage() {
	cat <<'EOF'
Usage: ./update-openclash-passwall-all.sh [--core-only|--sources-only|--recover] [--jobs N]

Default: update all third-party sources and alpha core, then build E87N firmware.
  --core-only    Update only the alpha core recipe, then rebuild firmware.
  --sources-only Update all source pins/fixes/core recipe without compiling.
  --recover      Restore the newest updater backup; do not compile.
  --jobs N       Parallel build jobs (default: 12).
EOF
}

while [ "$#" -gt 0 ]; do
	case "$1" in
		--core-only) MODE=core; shift ;;
		--sources-only) MODE=sources; shift ;;
		--recover) MODE=recover; shift ;;
		--jobs)
			[ "$#" -ge 2 ] || { usage >&2; exit 2; }
			JOBS=$2
			shift 2
			;;
		-h|--help) usage; exit 0 ;;
		*) usage >&2; exit 2 ;;
	esac
done

[[ "$JOBS" =~ ^[1-9][0-9]*$ ]] || { printf 'ERROR: invalid jobs: %s\n' "$JOBS" >&2; exit 2; }

cd "$ROOT"
[ -x ./update-openclash-passwall-sources.sh ] || {
	printf 'ERROR: source updater is missing or not executable\n' >&2
	exit 1
}

case "$MODE" in
	recover)
	exec ./update-openclash-passwall-sources.sh --recover
	;;
	core)
	./update-openclash-passwall-sources.sh --core-only
	;;
	all|sources)
	./update-openclash-passwall-sources.sh --all
	;;
esac

if [ "$MODE" = sources ]; then
	printf 'E87N_THIRDPARTY_ONECLICK=PASS mode=sources-only\n'
	exit 0
fi

cp .config.e87n .config
make defconfig 2>&1 | tee e87n-thirdparty-defconfig.log
! grep -q 'recursive dependency detected' e87n-thirdparty-defconfig.log
! grep -q 'which does not exist' e87n-thirdparty-defconfig.log
grep -q '^CONFIG_TARGET_mediatek_filogic_DEVICE_edgepi_e87n=y$' .config
grep -q '^CONFIG_PACKAGE_luci-app-openclash=y$' .config
grep -q '^CONFIG_PACKAGE_luci-app-passwall=y$' .config
grep -q '^CONFIG_PACKAGE_luci-app-passwall_INCLUDE_Hysteria=y$' .config
grep -q '^CONFIG_PACKAGE_openclash-core-meta=y$' .config

make package/openclash-core-meta/clean
make package/openclash-core-meta/compile -j"$JOBS" V=sc

if [ "$MODE" = all ]; then
	make package/luci-app-openclash/clean
	make package/luci-app-passwall/clean
	make package/luci-app-openclash/compile -j"$JOBS" V=sc
	make package/luci-app-passwall/compile -j"$JOBS" V=sc
fi

set +e
set -o pipefail
make -j"$JOBS" V=sc 2>&1 | tee build-e87n-thirdparty.log
build_rc=${PIPESTATUS[0]}
set -e
printf 'E87N_FULL_BUILD_RC=%s\n' "$build_rc"
[ "$build_rc" -eq 0 ] || exit "$build_rc"

image_dir='bin/targets/mediatek/filogic'
manifest="$image_dir/openwrt-mediatek-filogic-edgepi_e87n.manifest"
sysupgrade="$image_dir/openwrt-mediatek-filogic-edgepi_e87n-squashfs-sysupgrade.bin"
initramfs="$image_dir/openwrt-mediatek-filogic-edgepi_e87n-initramfs-kernel.bin"
[ -s "$manifest" ]
[ -s "$sysupgrade" ]
[ -s "$initramfs" ]

for package_name in \
	openclash-core-meta luci-app-openclash luci-app-passwall \
	hysteria sing-box xray-core shadowsocks-rust-sslocal; do
	grep -q "^${package_name} " "$manifest" || {
		printf 'ERROR: manifest is missing %s\n' "$package_name" >&2
		exit 1
	}
done

rootfs_core=$(find build_dir/target-*/root-mediatek/etc/openclash/core \
	-type f -name clash_meta -print -quit 2>/dev/null)
[ -n "$rootfs_core" ] && [ -x "$rootfs_core" ] || {
	printf 'ERROR: final rootfs OpenClash core is missing\n' >&2
	exit 1
}
file -L "$rootfs_core" | grep -Eq 'ELF 64-bit LSB executable.*ARM aarch64.*statically linked'

(
	cd "$image_dir"
	sha256sum -c sha256sums --ignore-missing
)

printf 'E87N_THIRDPARTY_ONECLICK=PASS mode=%s jobs=%s\n' "$MODE" "$JOBS"
printf 'sysupgrade=%s\ninitramfs=%s\n' "$sysupgrade" "$initramfs"
