# PR #75 Conflict Resolution Report

## Overview

PR #75 merges `openwrt:main` (223 commits) into `Zektopic:main` (318 unique Zektopic commits). The merge was performed on `fix-pr75` branch.

## Merge Statistics

| Metric | Value |
|--------|-------|
| Upstream commits merged | 223 |
| Zektopic commits preserved | ~318 |
| Files changed (total) | 822 |
| Files with merge conflicts | 2 |
| Conflict files resolved | 2/2 |

## Conflicts Encountered

### File 1: `.github/workflows/kernel.yml`

**Conflict 1 (push paths):**
- **Zektopic side:** Had `target/linux/generic/**` in push paths
- **Upstream side:** Added broader `target/linux/**`, `branches-ignore: [master]`, and `workflow_dispatch:`
- **Resolution:** Accepted both path patterns + upstream additions. This broadens CI trigger scope appropriately.

**Conflict 2 (concurrency):**
- **Upstream added:** New `concurrency` block with `cancel-in-progress` for PR events
- **Resolution:** Accepted upstream addition — prevents duplicate CI runs on the same PR.

### File 2: `.github/workflows/packages.yml`

**Conflict 1 (branches & dispatch):**
- **Upstream added:** `branches-ignore: [master]` and `workflow_dispatch:`
- **Resolution:** Accepted upstream addition for workflow flexibility.

**Conflict 2 (concurrency):**
- **Upstream added:** Same `concurrency` block as kernel.yml
- **Resolution:** Accepted upstream addition.

## What Was Preserved

All Zektopic custom changes were preserved:
- Security fixes (wireguard.uc, ead, button-hotplug, provision.uc, trelay, nvram.c, tinysrp)
- Script optimizations (belkin-header.py, cameo-imghdr.py, b43-fwsquash.py, etc.)
- CI workflow modifications
- Device tree customizations

## What Was Absorbed from Upstream

- Kernel bumps (6.12.80→84, 6.18.21→25) with refreshed patches
- New device support (ZBT-Z8103AX-D, Cisco Meraki Z3C, etc.)
- Realtek DSA/MDIO refactoring
- Package version bumps (ca-certificates, wireless-regdb, intel-microcode)
- Build system improvements (SDK package signing, overlay hardening)
- Security patches (dnsmasq CVE fixes, musl CVE fixes)

## Files Removed by Upstream (accepted)

- `toolchain/gcc/patches-12.x/` — GCC 12 patch stack removed (EOL)
- Various ARC patches — integrated upstream
- Various mac80211 patches — backported upstream

## Verification

- `git diff --name-only --diff-filter=U` — empty (no unresolved conflicts)
- `git grep '<<<<<<<'` — no results (no leftover conflict markers)
- `make defconfig` — succeeds
- `./scripts/feeds update -a && ./scripts/feeds install -a` — succeeds
