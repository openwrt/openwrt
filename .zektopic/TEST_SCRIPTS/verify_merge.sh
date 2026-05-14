#!/bin/bash
# verify_merge.sh — Post-merge verification script
# Checks: unresolved conflicts, leftover markers, security fixes, script optimizations

set -e
cd "$(git rev-parse --show-toplevel 2>/dev/null || echo /home/manu/openwrt)"

PASS=0
FAIL=0

check() {
    local desc="$1"
    shift
    if "$@" 2>/dev/null; then
        echo "[PASS] $desc"
        PASS=$((PASS + 1))
    else
        echo "[FAIL] $desc"
        FAIL=$((FAIL + 1))
    fi
}

echo "========================================="
echo "  Zektopic Merge Verification"
echo "========================================="
echo ""

# 1. No unresolved merge conflicts
echo "--- Conflict Check ---"
check "No unresolved merge conflicts" \
    test -z "$(git diff --name-only --diff-filter=U 2>/dev/null)"

# 2. No leftover conflict markers in source files
echo ""
echo "--- Conflict Marker Check ---"
check "No <<<<<<< markers in source files" \
    test -z "$(git grep -l '<<<<<<<' -- ':!.gitignore' 2>/dev/null)"

# 3. Check wireguard security fix
echo ""
echo "--- Security Fix Verification ---"
WG_FILE="package/network/utils/wireguard-tools/files/wireguard.uc"
if [ -f "$WG_FILE" ]; then
    check "WireGuard command injection fix (regex allowlist)" \
        grep -qF 'match(iface' "$WG_FILE"
else
    echo "[SKIP] WireGuard file not found"
fi

# 4. Check button-hotplug strcpy fixes
echo ""
echo "--- Kernel Security Patch Check ---"
check "button-hotplug uses bounded strncpy" \
    grep -q 'strncpy' package/kernel/button-hotplug/src/button-hotplug.c 2>/dev/null
check "gpio-button-hotplug uses bounded strncpy" \
    grep -q 'strncpy' package/kernel/gpio-button-hotplug/src/gpio-button-hotplug.c 2>/dev/null

# 5. Check script optimizations
echo ""
echo "--- Script Optimization Check ---"
check "belkin-header.py has chunked I/O" \
    grep -q 'chunk\|stream\|CHUNK' scripts/belkin-header.py 2>/dev/null
check "b43-fwsquash.py uses set operations" \
    grep -q 'set(' tools/b43-tools/files/b43-fwsquash.py 2>/dev/null
check "dl_cleanup.py uses defaultdict" \
    grep -q 'defaultdict' scripts/dl_cleanup.py 2>/dev/null

# 6. Feeds check
echo ""
echo "--- Feeds Check ---"
check "Feeds are installed" \
    test -d feeds/packages/libs

# 7. Summary
echo ""
echo "========================================="
echo "  Results: $PASS passed, $FAIL failed"
echo "========================================="

if [ "$FAIL" -gt 0 ]; then
    echo "WARNING: Some checks failed. Review above."
    exit 1
fi

echo "All checks passed."
exit 0
