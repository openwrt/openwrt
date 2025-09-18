#!/bin/bash
# 自动修复 OpenWrt 构建缺少的 host 工具

OPENWRT_DIR=$(pwd)
STAGING_BIN="$OPENWRT_DIR/staging_dir/host/bin"

mkdir -p "$STAGING_BIN"

fix_tool() {
    tool=$1
    sys_path=$(which $tool 2>/dev/null)
    if [ -z "$sys_path" ]; then
        echo "❌ 系统中没有找到 $tool，请先用 apt/yum 安装"
        return
    fi

    if [ ! -f "$STAGING_BIN/$tool" ]; then
        ln -sf "$sys_path" "$STAGING_BIN/$tool"
        echo "✅ 已修复 $tool → $STAGING_BIN/$tool"
    else
        echo "✔ $tool 已存在于 staging_dir"
    fi
}

# 常见需要的工具
for t in ninja cmake pkg-config; do
    fix_tool $t
done

echo "🎉 修复完成，可以继续 make 了。"
