/*
 * portcompat.h -- shared 2.6.36 -> 6.18 kernel-API compat layer for the
 * econet-xpon driver.  Force-included into every translation unit via the
 * Kbuild ccflags ( -include .../compat/portcompat.h ), so individual source
 * files need no edits for the global renames below.
 */
#ifndef _ECONET_XPON_PORTCOMPAT_H_
#define _ECONET_XPON_PORTCOMPAT_H_

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/io.h>
#include <linux/uaccess.h>

/* ---- vendor integer alias ----
 * The lowercase uint8 typedef lives in <modules/eth_global_def.h>, but a few
 * TUs (e.g. xpondrv.c) use it without pulling that header in.  uint8 is
 * unsigned char everywhere in this tree, so this identical typedef is safe
 * (C11 allows redundant identical typedefs).  NB: do NOT add uint16/uint32
 * here -- xpon_netif.c locally typedefs uint32 as `unsigned int` while
 * eth_global_def.h uses `unsigned long`, so a global one would conflict. */
typedef unsigned char  uint8;

/* ---- EPON command-table type ----
 * EPON is dropped from this GPON-only build, but inc/epon/epon.h is still pulled
 * transitively and references cmds_t in extern prototypes that are never linked.
 * A trivial alias lets those headers parse.  TODO EN7528: real type if EPON is
 * ever ported. */
typedef int cmds_t;

/* ---- MMIO ----
 * ioremap_nocache() was removed; ioremap() is uncached/device by default. */
#ifndef ioremap_nocache
#define ioremap_nocache(addr, size)	ioremap((addr), (size))
#endif

/* ---- timers ----
 * del_timer()/del_timer_sync() were renamed to timer_delete()/_sync().
 * Note: init_timer() and the old t.function/t.data callback ABI are NOT
 * shimmed here -- the callback signature changed (struct timer_list *), which
 * must be ported per-file with timer_setup()/from_timer(). */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 2, 0)
#ifndef del_timer
#define del_timer(t)		timer_delete(t)
#endif
#ifndef del_timer_sync
#define del_timer_sync(t)	timer_delete_sync(t)
#endif
#endif

/*
 * ---- proc_fs ----
 * create_proc_entry() was removed in 3.10.  We provide a wrapper that maps to
 * proc_create() so the *call* resolves, but the legacy ->read_proc/->write_proc
 * member assignments that follow each call still require per-file porting to
 * struct proc_ops + seq_file.  Those member assignments remain the documented
 * "proc_fs" error bucket.
 */
#ifndef create_proc_entry
#define create_proc_entry(name, mode, parent) \
	proc_create_data((name), (mode), (parent), NULL, NULL)
#endif

/* ---- PON sk_buff metadata ----
 * The vendor patched struct sk_buff with PON bitfields (gem_type:1, gem_port:12,
 * pon_mark, v_if, pon_*_flag, pon_tag_num) that the FE/QDMA driver sets on
 * ingress and the PON datapath reads.  Mainline sk_buff has no such members.
 * For this compile/link milestone the PON datapath is NOT yet wired to the
 * econet_eth FE seam (see econet-eth 100-xpon-bench-spike.patch), so the
 * vendor `skb->gem_port` etc. accesses are redirected to the always-present
 * 48-byte skb->cb scratch area via XPON_SKB(skb).
 * TODO (FE datapath wiring): replace skb->cb with the real cross-driver tag
 * (skb extension or a kernel sk_buff field) once econet_eth marks GEM frames. */
struct xpon_skb_cb {
	u16 gem_port;			/* vendor :12 */
	u8  gem_type;			/* vendor :1  */
	u8  v_if;
	u8  pon_mark;
	u8  pon_mac_filter_flag;
	u8  pon_vlan_flag;
	u8  pon_tag_num;
};
#define XPON_SKB(skb)	((struct xpon_skb_cb *)((skb)->cb))

/* isEPONFWID: vendor build-time firmware-ID predicate (true when the running
 * image is EPON firmware).  Its defining header is generated at vendor build
 * time and is not part of the GPL drop.  This is the GPON-only build, so it is
 * compile-time false.  TODO EN7528: wire to the real fw-id if EPON is ported. */
#ifndef isEPONFWID
#define isEPONFWID	(0)
#endif

/* SDK symbol stubs (flash_base, GetMacAddr, skbmgr, sw-NAT hooks, ...) */
#include "sdk_stubs.h"

#endif /* _ECONET_XPON_PORTCOMPAT_H_ */
