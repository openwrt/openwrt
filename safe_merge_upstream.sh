#!/bin/bash
# 安全合并上游分支，避免丢失本地修改
# 用法: ./safe_merge_upstream.sh <本地分支> <上游分支>
# 例子: ./safe_merge_upstream.sh Nradio-C8-668 master

set -e

LOCAL_BRANCH=$1
UPSTREAM_BRANCH=$2

if [ -z "$LOCAL_BRANCH" ] || [ -z "$UPSTREAM_BRANCH" ]; then
    echo "[!] 用法: $0 <本地分支> <上游分支>"
    exit 1
fi

echo "[*] 当前分支: $(git branch --show-current)"

# 1️⃣ 备份本地分支
BACKUP_BRANCH="backup-${LOCAL_BRANCH}-$(date +%Y%m%d_%H%M%S)"
git branch $BACKUP_BRANCH
echo "[✓] 已备份本地分支为 $BACKUP_BRANCH"

# 2️⃣ 切换到本地分支
git checkout $LOCAL_BRANCH

# 3️⃣ 抓取上游最新代码
git fetch origin
echo "[✓] 已抓取 origin"

# 4️⃣ 尝试合并上游分支
echo "[*] 尝试合并 origin/$UPSTREAM_BRANCH 到 $LOCAL_BRANCH"
if git merge origin/$UPSTREAM_BRANCH; then
    echo "[✓] 合并成功，无冲突"
else
    echo "[!] 合并出现冲突！"
    echo "[*] 冲突文件列表："
    git diff --name-only --diff-filter=U
    echo ""
    echo "[!] 请手动解决冲突后执行："
    echo "    git add <解决的文件>"
    echo "    git commit"
    echo "如果想放弃合并并恢复备份分支："
    echo "    git reset --hard $BACKUP_BRANCH"
    exit 1
fi

echo "[✓] 合并完成！本地分支 $LOCAL_BRANCH 已经包含上游 $UPSTREAM_BRANCH 的最新代码"
