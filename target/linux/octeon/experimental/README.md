# Experimental OCTEON prefetch patches (showcase only)

**Not for merging, not applied by the build.** These patches only show what the subtarget split makes possible:
with `CONFIG_CAVIUM_CN63XXP1` (the generic subtarget) the assembler turns every `pref` into the L2-only hint 28,
so none of them reach the L1; on the octeonplus and octeon3 subtargets they do. They need proper testing first.

The build applies only `patches-6.18/` of the target; nothing reads this directory. The octeon_pip patches
(077, 078, 083, 087) depend on the octeon_pip driver of #26060; the others change generic kernel code but were
only measured with that driver.

To try them: on a tree with #26060, copy `experimental/patches-6.18/*` into `target/linux/octeon/patches-6.18/`
and build the octeon3 (or octeonplus) subtarget.

## What the split is worth

EdgeRouter 4 (CN7030, 4 cores), octeon_pip driver, one boot per image, 64-byte single-flow floods, 2026-10-21:

| comparison | flood1 | fw4 NAT | fw4+flowtable | GRE tcp1 |
|---|---|---|---|---|
| the same patched kernel (these patches plus the fraglist/checksum/VLAN patches of the study) built for generic (CN63XXP1=y, hint 28) and for octeon3 | +13 % | +15 % | +11 % | +11 % |
| unpatched octeon3 -> octeon3 with all those patches | +20 % | +18 % | +19 % | +14 % |
| unpatched octeon3 -> octeon3 with only the prefetch patches below | +11 % | +8 % | +10 % | +10 % |

The first row is the gain of the subtarget split on a patched kernel. It is larger than the prefetch patches
alone on an unpatched one (last row): once the fraglist/checksum patches removed other work, the memory
misses are a bigger share of a packet's cost. It also contains -march=octeon3 (2-5 % without patches).

## Measured gains of the single prefetch patches (on top of an unpatched octeon3 kernel)

EdgeRouter 4 (CN7030, 4 cores), octeon3 subtarget (CN63XXP1 off), octeon_pip driver, one boot per image,
64-byte single-flow floods in kpps, TCP in Mbit/s, 2026-10-21. Gains of single patches inside a step are
extrapolated from their share of the L1 misses the step removed (marked ~); 081 was a step of its own.

| patch | L1 misses per packet at its target | flood1 | fw4 NAT | fw4+flowtable | GRE tcp1 |
|---|---|---|---|---|---|
| 077 rx: shared info + page struct a packet ahead | 1.24 -> 0.11 | ~+1.6 % | ~+0.6 % | ~+1.7 % | ~+1.5 % |
| 078 tx completion: what the free reads | 2.04 -> 0.86 | ~+1.7 % | ~+0.6 % | ~+1.8 % | ~+1.6 % |
| 079 next cached skb head | 0.45 -> 0.11 | ~+0.5 % | ~+0.2 % | ~+0.5 % | ~+0.5 % |
| 080 csum_partial, four chunks ahead | 9.4 -> 3.6 (GRE, per inner packet) | 0 | 0 | 0 | ~+6 % |
| 081 next segment in the fraglist passes | 4.31 -> 1.33 | +3.1 % | +3.8 % | +2.8 % | ~+0.7 % |
| 083 077 on the skb path only (XDP) | by design none | 0 | 0 | 0 | 0 |
| 084 second line of the next cached skb head | 0.68 -> 0.06 | ~+1.1 % | ~+0.8 % | ~+1.0 % | - |
| 086 gso segment skb lines two ahead | 0.92 -> 0.24 | ~+1.2 % | ~+0.9 % | ~+1.1 % | - |
| 087 page struct in the pool refill | 0.39 -> 0.01 | ~+0.7 % | ~+0.5 % | ~+0.6 % | - |

Steps as measured: baseline -> 077-080: flood1 339 -> 352, fw4 NAT 285 -> 289, fw4+flowtable 338 -> 352,
GRE 464 -> 501; + 081: 363 / 300 / 362 / 512; + 083-087: 375 / 307 / 372 (GRE of that step not usable: it
included 085, measured worse and dropped). All together: flood1 +11 %, fw4 NAT +8 %, fw4+flowtable +10 %,
GRE +10 %, WireGuard +3 %; flood1 L1 misses 14.1 -> 4.8 per packet. Line-rate TCP and the host-limited tests
are unchanged.

The split row above in kpps: flood1 360 vs 404 (octeon3 sender-capped), fw4 NAT 290 vs 334, fw4+flowtable
362 vs 403, GRE ~490 vs ~545.
