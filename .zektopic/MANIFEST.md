# Change Manifest

## PR #75 — Consolidate Dropbear config and enhance kernel support

**Source:** `openwrt:main` → `Zektopic:main`
**Commits merged:** 223
**Files changed:** 822 (+36,030 / -13,855)
**Merge conflicts resolved:** 2

## File Categories Changed

| Category | Count | Examples |
|----------|-------|---------|
| Kernel patches | ~445 | target/linux/generic/{backport,hack,pending}-6.{12,18}/ |
| Device tree files | ~244 | target/linux/*/dts/ |
| Package Makefiles | ~40 | package/**/Makefile |
| Kernel configs | ~30 | target/linux/*/config-6.{12,18} |
| Build system | ~15 | include/*.mk, config/*.in |
| CI/Workflows | ~10 | .github/workflows/*.yml |
| Scripts | ~10 | scripts/*.py, scripts/*.sh |
| Toolchain | ~5 | toolchain/gcc/patches/ |
| U-Boot | ~15 | package/boot/uboot-*/ |
| Firmware | ~5 | package/firmware/*/ |
| Other | ~3 | feeds.conf, README |

## Zektopic Custom Changes Preserved

| Area | Files | Type |
|------|-------|------|
| WireGuard security | package/network/utils/wireguard-tools/files/wireguard.uc | Security fix |
| ead buffer overflow | target/linux/generic/pending-6.*/ead* | Security fix |
| button-hotplug strcpy | target/linux/generic/pending-6.*/button-hotplug* | Security fix |
| provision.uc injection | package/system/provision/files/provision.uc | Security fix |
| trelay TOCTOU | package/network/services/trelay/ | Security fix |
| nvram.c | target/linux/generic/hack-6.*/nvram* | Security fix |
| tinysrp | package/libs/tinysrp/ | Security fix |
| belkin-header.py | scripts/belkin-header.py | Optimization |
| cameo-imghdr.py | scripts/cameo-imghdr.py | Optimization |
| netgear-encrypted-factory.py | scripts/netgear-encrypted-factory.py | Optimization |
| b43-fwsquash.py | scripts/b43-fwsquash.py | Optimization |
| dl_cleanup.py | scripts/dl_cleanup.py | Optimization |
| make-index-json.py | scripts/make-index-json.py | Optimization |
| sercomm-payload.py | scripts/sercomm-payload.py | Optimization |

## Key Upstream Changes Absorbed

| Change | Impact |
|--------|--------|
| Kernel 6.12.80→84 | Security fixes, driver updates |
| Kernel 6.18.21→25 | Security fixes, driver updates |
| Realtek DSA/MDIO refactoring | Improved stability for realtek targets |
| New device support | mediatek, ipq40xx, ath79, ipq50xx |
| dnsmasq CVE fixes | 6 patches applied |
| musl CVE fixes | Backported from upstream |
