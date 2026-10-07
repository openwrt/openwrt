# Kernel 7.3 testing kernel (x86/64 only)

Branch: `kernel-7.3`, based on vanilla upstream (`origin` = openwrt/openwrt).
Push: `git push graysky2 kernel-7.3` (add `--force-with-lease` after a rebase).
Model: the 6.18 bring-up (generic by namiltd, x86 by graysky2), Nov 2025 to Apr 2026.

## Steps

1. `kernel: add define for kernel 7.3`
   New `target/linux/generic/kernel-7.3`:
   `LINUX_VERSION-7.3 = -rc6` plus `LINUX_KERNEL_HASH-7.3-rc6 = <sha256>`.
   RC tarballs come from git.kernel.org/torvalds/t (include/kernel.mk handles -rc).
2. `scripts/kernel_bump.sh -s v6.18 -t v7.3`
   Makes the two auto commits: create files for v7.3, restore files for v6.18.
   Run on the clean tree only, so nothing local gets copied into 7.3 files.
3. generic backport-7.3: remove obsolete (already in 7.3), manually rebuild, refresh
   (`make target/linux/refresh`). One commit each.
4. generic pending-7.3: same three commits.
5. generic hack-7.3: manually rebuild, refresh.
6. `generic: 7.3: update kernel symbols` (config-7.3: drop dead, add new).
7. `scripts/kernel_bump.sh -p x86 -s v6.18 -t v7.3`
8. `x86: enable testing kernel for x86` (`KERNEL_TESTING_PATCHVER:=7.3`).
9. `x86: 7.3: import new options` (`make kernel_oldconfig`, CONFIG_TESTING_KERNEL=y).
10. Build x86/64, fix fallout:
    - `package/kernel/linux/modules/*.mk` (packages: 7.3: update modules files)
    - in-tree packages and drivers hitting API changes
    - kernel.mk / kbuild breakage
    - mac80211, mt76 and other out-of-tree modules
11. Each new RC: bump version and hash, refresh. At final, `-rcN` becomes `.0`/`.y`.

## 6.18 reference commits

| Step | 6.18 commit |
|---|---|
| define | e843bd8766 |
| generic copy | b70d9a15af, 141cb99b41 |
| backport | 5c73f28bfd, 4e16e53913, 6534e6bd54 |
| pending | e504ee0283, a3f4d1e0c6, 168710bbd5 |
| hack | 31f7baacf5, 7be35813ed |
| symbols | 71c180460a |
| packages/drivers | 7f97b2665c, c1599ce82e, 460afd1045, 2569a339ed, 354a26b094, 94c7f4a25f |
| build system | 1934927449, 666fee6f48 |
| mac80211/mt76 | 364be1fd7f, e0a8c4fc82, 0584503bab, ac9ea1be34 |
| x86 copy | 9547fd3647, 31f9f36625 |
| x86 enable | dc0389172b |
| x86 config | 3a4b01f154 |

## Later, before calling it merge-ready

- Build with CONFIG_ALL_KMODS=y to shake out kmod .mk fallout
  (deferred for the first draft; feed kmod breakage goes to the feeds, not this branch).
- One build with KERNEL_WERROR on.

## Optional

malta (MIPS, QEMU, no target patches) as a second-arch smoke test of generic
once x86/64 is green: symbols + enable testing kernel, two commits.

## Status log

- 2026-10-06: plan written. Baseline x86/64 build (toolchain + host tools) running.
  Nothing committed yet.
  7.3.config = user baseline + CONFIG_TESTING_KERNEL=y; copy to .config only after step 8.
- 2026-10-06 (later): steps 1-9 done through the patch refresh (20 commits, not pushed).
  Order differs from 6.18 on purpose: x86 copy + testing kernel enabled before the
  generic refresh, because refresh needs a target that builds 7.3.
  Backports: 169 removed (verified by upstream sha/subject vs v7.3-rc6), 6 v7.4 kept;
  441 and 786-01 rebuilt from their upstream originals.
  Dropped as obsolete in 7.3: pending 699, 707, 711-05, 812, 895-00; hack 250, 260, 970.
  Manual rebuilds: 22 pending, 10 hack (lists in the commit bodies).
  Symbols: generic 268 dead removed / 74 added, BOOTPARAM_*_PANIC now int (=0);
  x86 dead removed, 6 new x86/64 symbols. listnewconfig is empty after this.
  kmods: LINUX_6_18 conditions rekeyed to LINUX_6_12 (6.18-or-newer); broken
  "LINUX_x_y:mod" AUTOLOAD entries switched to mod@ge6.18 / mod@lt6.18 (bug on 6.18 too).
  Compile-tested with COMPILE_TEST every non-x86 file touched by a manual rebuild.
  760-09 needed a follow-up commit (duplicate teardown, tag value 36).
  Build fallout fixed: 7 more symbols (only visible with kmod overrides),
  linux-atm (7.3 removed SVC/LANE ioctls; ppp build-depends on it),
  kmods (mdio_bus rename, wmi move, phy-package and net-selftests deps).
  Full x86/64 build OK on 7.3-rc6: ext4/squashfs, BIOS/EFI images.
  Next: boot test in QEMU (needs qemu-system-x86 in the sandbox).
- 2026-10-07: user's live config (glibc, gcc 15.3, LTO, mold, many kmods) builds on
  7.3-rc6, ext4 BIOS/EFI images OK. 6 commits on top of cc03e2fa28:
  PMBus symbols; NFS_V4_0=y / NFSD_V4_POSIX_ACLS=n in kmods; 645 needs net/dsfield.h;
  kmod fixes (sha3 and blake2b renamed + lib split, dm-crypt needs libmd5,
  rpcsec_gss_krb5 needs crypto/krb5 + krb5enc); x86 iTCO_vendor_support gone;
  include/kernel.mk: out-of-tree kmod versions on -rc kernels were invalid for apk
  (7.3_rc6.1.14), now 7.3.1.14_rc6. Not 7.3 specific, upstreamable on its own.
  Version gates use 7.3 (verified absent in 6.18; exact upstream release not checked).
  Sandbox: 8 GB memcg OOM and 4096 pids cap both broke builds; now 64 GB / 8192.

## Broken packages (TODO)

Found with CONFIG_ALL_KMODS=y on 7.3-rc6 (x86/64, glibc, gcc 15.3).

In-tree, fix on this branch:

- [x] package/kernel/mac80211
- [x] package/kernel/nat46
- [x] package/kernel/ntfs
- [x] package/kernel/r8101

packages feed (fix upstream in the feed, not here):

- [ ] jool
- [ ] libpfring: proto_ops bind() now takes struct sockaddr_unsized *
- [ ] netatop: strncpy() removed from the kernel
- [ ] xtables-addons

telephony feed:

- [ ] dahdi-linux
- [ ] rtpengine (no-transcode variant, kernel module)

## Follow-ups found during the refresh

- 739-03 makes mtk_pcs_lynxi_create() return ERR_PTR; drivers/net/dsa/mt7530-mdio.c
  still checks !pcs. Same in 6.18.
- 720-04 rtl822x_init_phycr1() derefs phydev->priv, but the RTL8226B_RTL8221B and
  RTL8226B-CG driver entries have no .probe (NULL), RTL8226-CG uses rtl8226_probe.
  Latent oops on those PHYs. Same in 6.18.
- (resolved) nfnetlink and mux-core are built in on 7.3; kmod install already
  skips modules listed in modules.builtin, no gate needed.
- MUST-FIX before other targets use 7.3: upstream 7.3 has its own MTD_VIRT_CONCAT
  (bool, depends on MTD_PARTITIONED_MASTER, part-concat-next binding). Pending 497
  defines a different driver under the same symbol. Generic leaves
  MTD_PARTITIONED_MASTER unset, so the 18 targets using mtd-concat (81 DTS) would
  silently lose it, and enabling it is a hard build failure (virt_concat.c vs the
  new linux/mtd/concat.h). Fix: rename the 497 symbol (e.g. MTD_VIRT_CONCAT_DT).
- Host tools built outside the sandbox bake /scratch/7.3-openwrt into 22 tools;
  rebuilt inside the sandbox (tools only, toolchain is path clean).
- (done) Compile-tested non-x86 code touched by manual rebuilds with COMPILE_TEST.
- 760-09: DSA_TAG_PROTO_MXL862_8021Q_VALUE moved from 31 to 36 (31 is MXL_GSW1XX in 7.3).
