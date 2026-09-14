#!/usr/bin/env bash
# ============================================================================
# diy-part1.sh — clone/checkout 后, feeds update 前
#   Fork 场景: 官方 openwrt 仓库已有 scripts/diy-part1.sh, 本脚本**追加**到其后,
#   只做设备校验 + 覆盖准备, 不破坏官方逻辑
# ============================================================================
set -euo pipefail

OWRT="${1:-$PWD}"
cd "$OWRT"
CI_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

echo "==> [diy-part1] OWRT=$OWRT"

# ----------------------------------------------------------------------------
# 1) 校验 GL-MT3600BE 原生支持 (基线若不含则立即失败)
# ----------------------------------------------------------------------------
if ! grep -rq "gl-mt3600be\|gl_mt3600be" target/linux/mediatek/ 2>/dev/null; then
    echo "::error::当前基线未含 GL-MT3600BE 支持"
    echo "  请基于 openwrt-25.12 或 v25.12.5+ 分支运行"
    exit 1
fi
echo "==> [diy-part1] GL-MT3600BE 支持已确认"

# ----------------------------------------------------------------------------
# 2) 确认 files/usr/libexec/package-manager-call (apk/opkg 兼容层, fanchm 提供)
#    OpenWrt 25.12 官方 luci 已内置 package-manager-call, 此处用 fanchm 版覆盖
# ----------------------------------------------------------------------------
PM_CALL="$CI_ROOT/files/usr/libexec/package-manager-call"
if [ -x "$PM_CALL" ]; then
    echo "==> [diy-part1] package-manager-call 就绪"
else
    echo "::warning::[$PM_CALL] 不存在, 将用官方 luci 自带版本"
fi

# ----------------------------------------------------------------------------
# 3) (可选) 从 fanchm 主仓摘全锥内核补丁 — 个人推荐用 feed 版 fullconenat-nft
# ----------------------------------------------------------------------------
# if [ "${APPLY_FANCHM_KERNEL_PATCHES:-0}" = "1" ]; then
#     git remote add fcm https://github.com/fanchmwrt/fanchmwrt.git
#     git fetch fcm fanchmwrt-25.12.4
#     git cherry-pick <fullcone-commit-sha>
# fi

echo "==> [diy-part1] done"
