#!/usr/bin/env bash
# ============================================================================
# diy-part2-fanchm.sh — feeds install 后, make defconfig 后
#   校验 Fanchm 增量包是否被 feed 正确解析 (选入 .config)
#   防止 "编译成功但 fwx 没编进去" 的静默失败
#
#   注: luci-app-fwx.json 由 fwxd 包自动安装 (fwxd/Makefile install 段),
#       无需手动放到 files/
# ============================================================================
set -euo pipefail
OWRT="${1:-$PWD}"
cd "$OWRT"
echo "==> [diy-part2-fanchm] OWRT=$OWRT"

EXPECTED_PKGS=(libfwx_common fwx fwxd fullconenat-nft luci-theme-fanchmwrt)
ALL_OK=1
for pkg in "${EXPECTED_PKGS[@]}"; do
    if grep -q "^CONFIG_PACKAGE_${pkg}=[ym]$" .config; then
        echo "    [OK]   CONFIG_PACKAGE_${pkg}"
    else
        echo "    [MISS] CONFIG_PACKAGE_${pkg}"
        ALL_OK=0
    fi
done

# 核心包必须存在
CORE_PKGS=(fwx fwxd)
for pkg in "${CORE_PKGS[@]}"; do
    if ! grep -q "^CONFIG_PACKAGE_${pkg}=[ym]$" .config; then
        echo "::error::核心 Fanchm 包 CONFIG_PACKAGE_${pkg} 未被选中"
        echo "  可能原因: fanchm feed 未成功 update/install, 或分支不含该包"
        echo "  已选 fanchm 相关行:"
        grep -E "CONFIG_PACKAGE_.*(fwx|fanchm).*=" .config || true
        exit 1
    fi
done

[ "$ALL_OK" = "1" ] && echo "==> 全部 Fanchm 增量包已选入 .config" \
                     || echo "::warning::部分非核心包未选中, 继续构建 (功能可能缩减)"
echo "==> [diy-part2-fanchm] done"
