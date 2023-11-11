/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef __XMIT_OSDEP_H_
#define __XMIT_OSDEP_H_


struct pkt_file {
	struct sk_buff *pkt;
	SIZE_T pkt_len;	 /* the remainder length of the open_file */
	_buffer *cur_buffer;
	u8 *buf_start;
	u8 *cur_addr;
	SIZE_T buf_len;
};
#ifndef NR_XMITFRAME
#define NR_XMITFRAME		1256
#endif /*NR_XMITFRAME*/
#define NR_XMITFRAME_EXT	32
#define SZ_XMITFRAME_EXT	1536	/*MGNT frame*/

#ifdef CONFIG_PCI_HCI
	#define SZ_ALIGN_XMITFRAME_EXT	4
#else
	#ifdef USB_XMITBUF_ALIGN_SZ
		#define SZ_ALIGN_XMITFRAME_EXT (USB_XMITBUF_ALIGN_SZ)
	#else
		#define SZ_ALIGN_XMITFRAME_EXT 512
	#endif
#endif


struct xmit_priv;
struct pkt_attrib;
struct sta_xmit_priv;
struct xmit_frame;
struct xmit_buf;


#ifdef PLATFORM_FREEBSD
extern int rtw_xmit_entry(struct sk_buff *pkt, _nic_hdl pnetdev);
extern void rtw_xmit_entry_wrap(_nic_hdl pifp);
#endif /* PLATFORM_FREEBSD */

#ifdef PLATFORM_LINUX
extern int _rtw_xmit_entry(struct sk_buff *pkt, _nic_hdl pnetdev);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32))
extern netdev_tx_t rtw_xmit_entry(struct sk_buff *pkt, _nic_hdl pnetdev);
#else
extern int rtw_xmit_entry(struct sk_buff *pkt, _nic_hdl pnetdev);
#endif

#ifdef RTW_PHL_TX
extern int rtw_os_tx(struct sk_buff *pkt, _nic_hdl pnetdev);
#endif
#endif /* PLATFORM_LINUX */

void rtw_os_xmit_schedule(_adapter *padapter);

#if 0 /*CONFIG_CORE_XMITBUF*/
int rtw_os_xmit_resource_alloc(_adapter *padapter, struct xmit_buf *pxmitbuf, u32 alloc_sz, u8 flag);
void rtw_os_xmit_resource_free(_adapter *padapter, struct xmit_buf *pxmitbuf, u32 free_sz, u8 flag);
#else
u8 rtw_os_xmit_resource_alloc(_adapter *padapter, struct xmit_frame *pxframe);
void rtw_os_xmit_resource_free(_adapter *padapter, struct xmit_frame *pxframe);
#endif
extern void rtw_set_tx_chksum_offload(struct sk_buff *pkt, struct pkt_attrib *pattrib);

extern uint rtw_remainder_len(struct pkt_file *pfile);
extern void _rtw_open_pktfile(struct sk_buff *pkt, struct pkt_file *pfile);
extern uint _rtw_pktfile_read(struct pkt_file *pfile, u8 *rmem, uint rlen);
extern sint rtw_endofpktfile(struct pkt_file *pfile);

extern void rtw_os_pkt_complete(_adapter *padapter, struct sk_buff *pkt);
extern void rtw_os_xmit_complete(_adapter *padapter, struct xmit_frame *pxframe);

void rtw_os_check_wakup_queue(_adapter *padapter, u16 os_qid);
bool rtw_os_check_stop_queue(_adapter *padapter, u16 os_qid);
void rtw_os_wake_queue_at_free_stainfo(_adapter *padapter, int *qcnt_freed);

void dump_os_queue(void *sel, _adapter *padapter);

void rtw_coalesce_tx_amsdu(_adapter *padapter, struct xmit_frame *pxframes[],
			   int xf_nr, bool amsdu, u32 *pktlen);

#endif /* __XMIT_OSDEP_H_ */
