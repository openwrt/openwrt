/*
 * sdk_stubs.h -- declarations for vendor-SDK symbols the GPON driver expects
 * from the tclinux_phoenix platform (flash, MAC, skb manager).  Definitions
 * live in sdk_stubs.c (linked into econet-xpon.ko).
 */
#ifndef _ECONET_XPON_SDK_STUBS_H_
#define _ECONET_XPON_SDK_STUBS_H_

#include <linux/types.h>
#include <linux/skbuff.h>

/* ---- on-board SPI-flash window (factory data: onu type, MAC, ...) ---- */
extern unsigned long flash_base;
/* READ_FLASH_BYTE(addr): byte read from the (KSEG1) flash window.
 * flash_base is a stub (0) until wired to mtd/nvmem, so a raw deref of
 * flash_base+offset faults (NULL-region access -> kernel oops at init).
 * For the load milestone, return 0 (callers default: onu type -> SFU,
 * flash MAC -> zeroed, overridden by nvmem elsewhere).
 * TODO EN7528: route via mtd / nvmem and restore the real read when
 * flash_base points at a valid ioremap'd window. */
#ifndef READ_FLASH_BYTE
#define READ_FLASH_BYTE(addr) \
	(flash_base ? (*(volatile unsigned char *)(unsigned long)(addr)) \
		    : (unsigned char)0)
#endif

/* ---- board MAC ---- */
uint8_t *GetMacAddr(void);

/* ---- FE / WAN2LAN datapath glue (tcphy) ----
 * macSend() is the Frame Engine transmit entry the vendor PON sniffer path uses
 * to copy frames to LAN; the constants are the channel id + skb->mark bits it
 * tags.  Real wiring lands with the econet_eth FE seam; stubbed no-op for now.
 * Values recovered from the vendor preprocessed source. */
#ifndef WAN2LAN_CH_ID
#define WAN2LAN_CH_ID		(1<<31)
#endif
#ifndef SKBUF_COPYTOLAN
#define SKBUF_COPYTOLAN		(1<<26)
#endif
#ifndef DS_PKT_FORM_WAN
#define DS_PKT_FORM_WAN		0x80
#endif
void macSend(unsigned int chanId, struct sk_buff *skb);	/* TODO: FE seam */

/* ---- vendor skb buffer manager: map onto the standard skb allocators ---- */
/* RX buffer sizes the skbmgr alloc helpers below provide (2k / 4k). */
#ifndef SKBMGR_RX_BUF_LEN
#define SKBMGR_RX_BUF_LEN	2048
#endif
#ifndef SKBMGR_4K_RX_BUF_LEN
#define SKBMGR_4K_RX_BUF_LEN	4096
#endif
static inline struct sk_buff *skbmgr_alloc_skb2k(void)
{
	return dev_alloc_skb(2048);
}
static inline struct sk_buff *skbmgr_dev_alloc_skb2k(void)
{
	return dev_alloc_skb(2048);
}
static inline struct sk_buff *skbmgr_dev_alloc_skb4k(void)
{
	return dev_alloc_skb(4096);
}

#endif /* _ECONET_XPON_SDK_STUBS_H_ */
