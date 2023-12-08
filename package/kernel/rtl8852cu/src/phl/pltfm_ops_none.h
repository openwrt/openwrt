/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
#ifndef _PLTFM_OPS_NONE_H_
#define _PLTFM_OPS_NONE_H_
#include "phl_types.h"

static char *_os_strpbrk(const char *cs, const char *ct)
{
	const char *sc1, *sc2;

	for (sc1 = cs; *sc1 != '\0'; ++sc1) {
		for (sc2 = ct; *sc2 != '\0'; ++sc2) {
			if (*sc1 == *sc2)
				return (char *)sc1;
		}
	}
	return NULL;
}
static __inline char *_os_strsep(char **s, const char *ct)
{
	char *sbegin = *s;
	char *end;

	if (sbegin == NULL)
		return NULL;

	end = _os_strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return sbegin;
}
static __inline int _os_sscanf(const char *buf, const char *fmt, ...)
{
	return 0;
}
static __inline int _os_strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2) {
		if (*s1 == '\0')
			break;
		s1++;
		s2++;
	}
	return *s1 - *s2;
}
static __inline int _os_strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return 0;
	while (*s1 == *s2) {
		if (*s1 == '\0')
			break;
		s1++;
		s2++;
		n--;
		if (n == 0)
			return 0;
		}
	return *s1 - *s2;
}
static __inline char *_os_strcpy(char *dest, const char *src)
{
	return NULL;
}
static inline char *_os_strncpy(char *dest, const char *src, size_t n)
{
	return NULL;
}
static __inline char *_os_strchr(const char *s, int c)
{
	while (*s != (char)c)
		if (*s++ == '\0')
			return NULL;
	return (char *)s;
}
static __inline int _os_snprintf(char *str, size_t size, const char *format, ...)
{
	return 0;
}

static __inline int _os_strncat(char *dest, char *src, size_t n)
{
	return 0;
}

static __inline u32 _os_strlen(u8 *buf)
{
	return 0;
}
static __inline void _os_delay_ms(void *h, u32 ms)
{
}
static __inline void _os_delay_us(void *h, u32 us)
{
}
static __inline void _os_sleep_ms(void *h, u32 ms)
{
}
static __inline void _os_sleep_us(void *h, u32 us)
{
}
static inline u32 _os_get_cur_time_us(void)
{
	return 0;
}
static inline u32 _os_get_cur_time_ms(void)
{
	return (_os_get_cur_time_us() / 1000);
}

static inline u64 _os_modular64(u64 x, u64 y)
{
	return x % y;
}
static inline u64 _os_division64(u64 x, u64 y)
{
	u32 low, low2, high, rem, result;

	low = x & 0xFFFFFFFF;
	high = x >> 32;
	rem = high % (u32)y;
	high=  high / (u32)y;
	low2 = low >> 16;
	low2 += rem << 16;
	rem = low2 % (u32)y;
	low2 = low2 / (u32)y;
	low = low & 0xFFFFFFFF;
	low += rem << 16;
	rem = low % (u32)y;
	low = low / (u32)y;
	result = low + ((u64)low2 << 16) + ((u64)high << 32);

	return result;
}

static inline u64 _os_minus64(u64 x, u64 y)

{
	return x - y;
}

static inline u64 _os_add64(u64 x, u64 y)

{
	return x + y;
}

static inline u32 _os_div_round_up(u32 x, u32 y)
{
        return (x + y - 1) / y;
}

#ifdef CONFIG_PCI_HCI
static inline void _os_cache_inv(void *d, u32 *bus_addr_l, u32 *bus_addr_h,
					u32 buf_sz, u8 direction)
{
}
static inline void _os_cache_wback(void *d, u32 *bus_addr_l,
			u32 *bus_addr_h, u32 buf_sz, u8 direction)
{
}

/* txbd, rxbd, wd */
static inline void *_os_shmem_alloc(void *d, u32 *bus_addr_l,
				    u32 *bus_addr_h, u32 buf_sz,
				    u8 cache, u8 direction, void **os_rsvd)
{
	return NULL;
}
static inline void _os_shmem_free(void *d, u8 *vir_addr, u32 *bus_addr_l,
				  u32 *bus_addr_h, u32 buf_sz,
				  u8 cache, u8 direction, void *os_rsvd)
{
}
#endif /*CONFIG_PCI_HCI*/

static inline void *_os_pkt_buf_unmap_rx(void *d, u32 bus_addr_l, u32 bus_addr_h, u32 buf_sz)
{
	return NULL;
}

static inline void *_os_pkt_buf_map_rx(void *d, u32 *bus_addr_l, u32 *bus_addr_h,
					u32 buf_sz, void *os_priv)
{
	return NULL;
}

static inline void *_os_pkt_buf_alloc_rx(void *d, u32 *bus_addr_l,
			u32 *bus_addr_h, u32 buf_sz, void **os_priv)
{
	return NULL;
}
static inline u8 *_os_pkt_buf_free_rx(void *d, u8 *vir_addr, u32 bus_addr_l,
			u32 bus_addr_h, u32 buf_sz, void *os_priv)
{
	return NULL;
}

/* phl pre-alloc network layer buffer */
static inline void * _os_alloc_netbuf(void *d, u32 buf_sz, void **os_priv)
{
	return NULL; // windows never do this.
}

/* Free netbuf for error case. (ex. drop rx-reorder packet) */
static inline void _os_free_netbuf(void *d, u8 *vir_addr, u32 buf_sz, void *os_priv)
{
}

/* Generate an unsigned 32-bit random number */
static inline u32 _os_random32(void *d)
{
	return 0;
}

static __inline void *_os_mem_alloc(void *h, u32 buf_sz)
{
	return NULL;
}

static __inline void _os_mem_free(void *h, void *buf, u32 buf_sz)
{
}
/*physically contiguous memory if the buffer will be accessed by a DMA device*/
static __inline void *_os_kmem_alloc(void *h, u32 buf_sz)
{
	return NULL;
}

/*physically contiguous memory if the buffer will be accessed by a DMA device*/
static __inline void _os_kmem_free(void *h, void *buf, u32 buf_sz)
{
}
static __inline void _os_mem_set(void *h, void *buf, s8 value, u32 size)
{
}
static __inline void _os_mem_cpy(void *h, void *dest, const void *src, u32 size)
{
}
static __inline int _os_mem_cmp(void *h, const void *ptr1, const void *ptr2, u32 size)
{
	return 0;
}
static __inline void _os_init_timer(void *h, _os_timer *timer,
		void (*call_back_func)(void *context), void *context,
		const char *sz_id)
{
}

static __inline void _os_set_timer(void *h, _os_timer *timer, u32 ms_delay)
{
}

static __inline void _os_cancel_timer(void *h, _os_timer *timer)
{
}

static inline void _os_cancel_timer_async(void *d, _os_timer *timer)
{
}

static __inline void _os_release_timer(void *h, _os_timer *timer)
{

}
static __inline void _os_mutex_init(void *h, _os_mutex *mutex)
{
}

static __inline void _os_mutex_deinit(void *h, _os_mutex *mutex)
{
}

static __inline void _os_mutex_lock(void *h, _os_mutex *mutex)
{
}

static __inline void _os_mutex_unlock(void *h, _os_mutex *mutex)
{
}

static inline void _os_sema_init(void *d, _os_sema *sema, int int_cnt)
{
}

static inline void _os_sema_free(void *d, _os_sema *sema)
{
}

static inline void _os_sema_up(void *d, _os_sema *sema)
{
}

static inline u8 _os_sema_down(void *d, _os_sema *sema)
{
	return 0;	//success
}

/* event */
static __inline void _os_event_init(void *h, _os_event *event)
{
}
static __inline void _os_event_free(void *h, _os_event *event)
{
}
static __inline void _os_event_reset(void *h, _os_event *event)
{
}

static __inline void _os_event_set(void *h, _os_event *event)
{
}

/*
 * m_sec
 * 	== 0 : wait for completion
 * 	>  0 : wait for timeout or completion
 * return value
 * 	0:timeout
 * 	otherwise:success
 */
static __inline int _os_event_wait(void *h, _os_event *event, u32 m_sec)
{
	return 0;
}

/* spinlock */
static __inline void _os_spinlock_init(void *d, _os_lock *plock)
{
}
static __inline void _os_spinlock_free(void *d, _os_lock *plock)
{
}

static inline void _os_spinlock(void *d, _os_lock *plock,
					enum lock_type type, _os_spinlockfg *flags)
{
}
static inline void _os_spinunlock(void *d, _os_lock *plock,
					enum lock_type type, _os_spinlockfg *flags)
{
}
static inline int _os_test_and_clear_bit(int nr, unsigned long *addr)
{
	/*UNDO*/
	return 0;
}
static inline int _os_test_and_set_bit(int nr, unsigned long *addr)
{
	/*UNDO*/
	return 1;
}
/* Atomic integer operations */
static __inline void _os_atomic_set(void *d, _os_atomic *v, int i)
{
}

static __inline int _os_atomic_read(void *d, _os_atomic *v)
{
	return 0;
}

static __inline void _os_atomic_add(void *d, _os_atomic *v, int i)
{
}
static __inline void _os_atomic_sub(void *d, _os_atomic *v, int i)
{
}

static __inline void _os_atomic_inc(void *d, _os_atomic *v)
{
}

static __inline void _os_atomic_dec(void *d, _os_atomic *v)
{
}

static __inline int _os_atomic_add_return(void *d, _os_atomic *v, int i)
{
	return 0;
}

static __inline int _os_atomic_sub_return(void *d, _os_atomic *v, int i)
{
	return 0;
}

static __inline int _os_atomic_inc_return(void *d, _os_atomic *v)
{
	return 0;
}

static __inline int _os_atomic_dec_return(void *d, _os_atomic *v)
{
	return 0;
}
/*
static __inline bool _os_atomic_inc_unless(void *d, _os_atomic *v, int u)
{
	return 0;
}
*/

static inline enum rtw_phl_status _os_tasklet_init(void *drv_priv, _os_tasklet *task,
	void (*call_back_func)(void* context), void *context)
{
	return RTW_PHL_STATUS_SUCCESS;
}

static inline enum rtw_phl_status _os_tasklet_deinit(void *drv_priv, _os_tasklet *task)
{
	return RTW_PHL_STATUS_SUCCESS;
}

static inline enum rtw_phl_status _os_tasklet_schedule(void *drv_priv, _os_tasklet *task)
{
	return RTW_PHL_STATUS_SUCCESS;
}

static __inline u8 _os_thread_init(	void *drv_priv, _os_thread *thread,
					int (*call_back_func)(void * context),
					void *context,
					const char namefmt[])
{
	return RTW_PHL_STATUS_FAILURE;
}
static __inline u8 _os_thread_deinit(void *drv_priv, _os_thread *thread)
{
	return RTW_PHL_STATUS_FAILURE;
}
static __inline enum rtw_phl_status _os_thread_schedule(void *drv_priv, _os_thread *thread)
{
	return RTW_PHL_STATUS_FAILURE;
}
static inline void _os_thread_stop(void *drv_priv, _os_thread *thread)
{
}
static __inline int _os_thread_check_stop(void *drv_priv, _os_thread *thread)
{
	return 1;
}
static inline int _os_thread_wait_stop(void *drv_priv, _os_thread *thread)
{
	return RTW_PHL_STATUS_SUCCESS;
}

#if 0
static inline _os_thread _os_thread_start(int (*threadfn)(void *data),
	void *data, const char namefmt[])
{
	return 0;
}
static inline bool _os_thread_stop(_os_thread th)
{
	return 0;
}
static inline void _os_thread_wait_stop(void)
{
}
static inline int _os_thread_should_stop(void)
{
	return 0;
}
#endif

static inline enum rtw_phl_status _os_workitem_init(void *drv_priv, _os_workitem *workitem,
			void (*call_back_func)(void* context), void *context)
{
	return RTW_PHL_STATUS_SUCCESS;
}

static inline enum rtw_phl_status _os_workitem_schedule(void *drv_priv, _os_workitem *workitem)
{
	return RTW_PHL_STATUS_SUCCESS;
}

static inline enum rtw_phl_status _os_workitem_deinit(void *drv_priv, _os_workitem *workitem)
{
	return RTW_PHL_STATUS_SUCCESS;
}

/* OS handler extension */
static inline u8 _os_init_handler_ext(void *drv_priv,
                                      void *phl_handler)
{
	return RTW_PHL_STATUS_SUCCESS;
}

static inline u8 _os_deinit_handler_ext(void *drv_priv,
                                        void *phl_handler)
{
	return RTW_PHL_STATUS_SUCCESS;
}

/*
 * _os_read_file - phl read file api
 * @path: path of the file to open and read
 * @buf: the address of allocated buffer to store the file content
 * @sz: the bytes to read at most
 *
 * returns bytes read
 */
static inline u32 _os_read_file(const char *path, u8 *buf, u32 sz)
{
	/* OS Dependent API */
	return 0;
}

/* Network Function */
#ifdef CONFIG_RTW_MIRROR_DUMP
static inline u32 _os_mirror_dump(u8 *hdr, u32 hdr_len, u8 *buf, u32 sz)
{
	return 0;
}
#endif

#ifdef CONFIG_PCI_HCI
static __inline u8 _os_read8_pcie(void *h, u32 addr)
{
	return 0;
}
static __inline u16 _os_read16_pcie(void *h, u32 addr)
{
	return 0;

}
static __inline u32 _os_read32_pcie(void *h, u32 addr)
{
	return 0;
}

static __inline u32 _os_write8_pcie(void *h, u32 addr, u8 val)
{
	return 0;
}
static __inline u32 _os_write16_pcie(void *h, u32 addr, u16 val)
{
	return 0;
}
static __inline u32 _os_write32_pcie(void *h, u32 addr, u32 val)
{
	return 0;
}
#endif/*#ifdef CONFIG_PCI_HCI*/

#ifdef CONFIG_USB_HCI
static __inline u32 _os_usbctrl_vendorreq(void *h, u8 request, u16 value,
			u16 index, void *pdata, u16 len, u8 requesttype)
{
	return 0;
}

static inline int os_usb_tx(void *h, u8 *tx_buf_ptr,
			u8 bulk_id, u32 len, u8 *pkt_data_buf)
{
	return 1;
}

static __inline void os_enable_usb_out_pipes(void *drv_priv)
{
}

static __inline void os_disable_usb_out_pipes(void *drv_priv)
{
	/* Free bulkout urb */
}

static __inline u8 os_out_token_alloc(void *drv_priv)
{
	return 0; // RTW_PHL_STATUS_SUCCESS
}

static __inline void os_out_token_free(void *drv_priv)
{
}


static __inline u8 os_in_token_alloc(void *drv_priv)
{
	// Allocate in token (pUrb) list
	return 0;
}

static __inline void os_in_token_free(void *drv_priv)
{
	// Free in token (pUrb) list
}


static __inline u8 os_send_usb_in_token(void *drv_priv, void *rxobj, u8 *inbuf, u32 inbuf_len, u8 pipe_idx, u8 minLen)
{
	// send rtw_rx_buf to os
	return 0;
}

static __inline void os_enable_usb_in_pipes(void *drv_priv)
{
}

static __inline void os_disable_usb_in_pipes(void *drv_priv)
{
}

#endif /*CONFIG_USB_HCI*/

#ifdef CONFIG_SDIO_HCI
static inline u8 _os_sdio_cmd52_r8(void *d, u32 offset)
{
	return 0;
}

static inline u8 _os_sdio_cmd53_r8(void *d, u32 offset)
{
	return 0;
}

static inline u16 _os_sdio_cmd53_r16(void *d, u32 offset)
{
	return 0;
}

static inline u32 _os_sdio_cmd53_r32(void *d, u32 offset)
{
	return 0;
}

static inline u8 _os_sdio_cmd53_rn(void *d, u32 offset, u32 size, u8 *data)
{
	return 0;
}

static inline u8 _os_sdio_cmd53_r(void *d, u32 offset, u32 size, u8 *data)
{
	return 0;
}

static inline void _os_sdio_cmd52_w8(void *d, u32 offset, u8 val)
{
}

static inline void _os_sdio_cmd53_w8(void *d, u32 offset, u8 val)
{
}

static inline void _os_sdio_cmd53_w16(void *d, u32 offset, u16 val)
{
}

static inline void _os_sdio_cmd53_w32(void *d, u32 offset, u32 val)
{
}

static inline void _os_sdio_cmd53_wn(void *d, u32 offset, u32 size, u8 *data)
{
}

static inline void _os_sdio_cmd53_w(void *d, u32 offset, u32 size, u8 *data)
{
}

static inline u8 _os_sdio_f0_read(void *d, u32 addr, void *buf, size_t len)
{
	return 0;
}

static inline u8 _os_sdio_read_cia_r8(void *d, u32 addr)
{
	return 0;
}
#endif /*CONFIG_SDIO_HCI*/


/*
* Continuous bits starting from least significant bit
* Example:
* BIT_LEN_MASK_32(0) => 0x00000000
* BIT_LEN_MASK_32(1) => 0x00000001
* BIT_LEN_MASK_32(2) => 0x00000003
* BIT_LEN_MASK_32(32) => 0xFFFFFFFF
*/
#define BIT_LEN_MASK_32(__BitLen) ((u32)(0xFFFFFFFF >> (32 - (__BitLen))))
#define BIT_LEN_MASK_16(__BitLen) ((u16)(0xFFFF >> (16 - (__BitLen))))
#define BIT_LEN_MASK_8(__BitLen) ((u8)(0xFF >> (8 - (__BitLen))))

/*
* Continuous bits starting from least significant bit
* Example:
* BIT_OFFSET_LEN_MASK_32(0, 2) => 0x00000003
* BIT_OFFSET_LEN_MASK_32(16, 2) => 0x00030000
*/
#define BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) ((u32)(BIT_LEN_MASK_32(__BitLen) << (__BitOffset)))
#define BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen) ((u16)(BIT_LEN_MASK_16(__BitLen) << (__BitOffset)))
#define BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) ((u8)(BIT_LEN_MASK_8(__BitLen) << (__BitOffset)))

/*
* Convert LE data to host byte order
*/
#define EF1Byte (u8)
#define EF2Byte le16_to_cpu
#define EF4Byte le32_to_cpu

/*
* Read LE data from memory to host byte order
*/
#define ReadLE4Byte(_ptr)	le32_to_cpu(*((u32 *)(_ptr)))
#define ReadLE2Byte(_ptr)	le16_to_cpu(*((u16 *)(_ptr)))
#define ReadLE1Byte(_ptr)	(*((u8 *)(_ptr)))

/*
* Read BE data from memory to host byte order
*/
#define ReadBEE4Byte(_ptr)	be32_to_cpu(*((u32 *)(_ptr)))
#define ReadBE2Byte(_ptr)	be16_to_cpu(*((u16 *)(_ptr)))
#define ReadBE1Byte(_ptr)	(*((u8 *)(_ptr)))

/*
* Write host byte order data to memory in LE order
*/
#define WriteLE4Byte(_ptr, _val)	((*((u32 *)(_ptr))) = cpu_to_le32(_val))
#define WriteLE2Byte(_ptr, _val)	((*((u16 *)(_ptr))) = cpu_to_le16(_val))
#define WriteLE1Byte(_ptr, _val)	((*((u8 *)(_ptr))) = ((u8)(_val)))

/*
* Write host byte order data to memory in BE order
*/
#define WriteBE4Byte(_ptr, _val)	((*((u32 *)(_ptr))) = cpu_to_be32(_val))
#define WriteBE2Byte(_ptr, _val)	((*((u16 *)(_ptr))) = cpu_to_be16(_val))
#define WriteBE1Byte(_ptr, _val)	((*((u8 *)(_ptr))) = ((u8)(_val)))

/*
* Return 4-byte value in host byte ordering from 4-byte pointer in litten-endian system.
*/
#define LE_P4BYTE_TO_HOST_4BYTE(__pStart) (le32_to_cpu(*((u32 *)(__pStart))))
#define LE_P2BYTE_TO_HOST_2BYTE(__pStart) (le16_to_cpu(*((u16 *)(__pStart))))
#define LE_P1BYTE_TO_HOST_1BYTE(__pStart) ((*((u8 *)(__pStart))))

/*
* Return 4-byte value in host byte ordering from 4-byte pointer in big-endian system.
*/
#define BE_P4BYTE_TO_HOST_4BYTE(__pStart) (be32_to_cpu(*((u32 *)(__pStart))))
#define BE_P2BYTE_TO_HOST_2BYTE(__pStart) (be16_to_cpu(*((u16 *)(__pStart))))
#define BE_P1BYTE_TO_HOST_1BYTE(__pStart) ((*((u8 *)(__pStart))))

/*
* Translate subfield (continuous bits in little-endian) of 4-byte value in LE byte to
* 4-byte value in host byte ordering.
*/
#define LE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	((LE_P4BYTE_TO_HOST_4BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_32(__BitLen))

#define LE_BITS_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	((LE_P2BYTE_TO_HOST_2BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_16(__BitLen))

#define LE_BITS_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	((LE_P1BYTE_TO_HOST_1BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_8(__BitLen))

/*
* Translate subfield (continuous bits in big-endian) of 4-byte value in BE byte to
* 4-byte value in host byte ordering.
*/
#define BE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	((BE_P4BYTE_TO_HOST_4BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_32(__BitLen))

#define BE_BITS_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	((BE_P2BYTE_TO_HOST_2BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_16(__BitLen))

#define BE_BITS_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	((BE_P1BYTE_TO_HOST_1BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_8(__BitLen))

/*
* Mask subfield (continuous bits in little-endian) of 4-byte value in LE byte oredering
* and return the result in 4-byte value in host byte ordering.
*/
#define LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	(LE_P4BYTE_TO_HOST_4BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen)))

#define LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	(LE_P2BYTE_TO_HOST_2BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen)))

#define LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	(LE_P1BYTE_TO_HOST_1BYTE(__pStart) & ((u8)(~BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen))))

/*
* Mask subfield (continuous bits in big-endian) of 4-byte value in BE byte oredering
* and return the result in 4-byte value in host byte ordering.
*/
#define BE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	(BE_P4BYTE_TO_HOST_4BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen)))

#define BE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	(BE_P2BYTE_TO_HOST_2BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen)))

#define BE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	(BE_P1BYTE_TO_HOST_1BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen)))

/*
* Set subfield of little-endian 4-byte value to specified value.
*/
#define SET_BITS_TO_LE_4BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		u8 __offset = __BitOffset, __len = __BitLen; \
		if (__offset == 0 && __len == 32) \
			WriteLE4Byte(__pStart, __Value); \
		else { \
			WriteLE4Byte(__pStart, \
				LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u32)__Value) & BIT_LEN_MASK_32(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define SET_BITS_TO_LE_2BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		WriteLE2Byte(__pStart, \
			LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
			| \
			((((u16)__Value) & BIT_LEN_MASK_16(__BitLen)) << (__BitOffset)) \
		); \
	} while (0)

#define SET_BITS_TO_LE_1BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		u8 __offset = __BitOffset; u8 __len = __BitLen; \
		if (__offset == 0 && __len == 8) \
			WriteLE1Byte(__pStart, __Value); \
		else { \
			WriteLE1Byte(__pStart, \
				LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __len) \
				| \
				((((u8)__Value) & BIT_LEN_MASK_8(__len)) << (__BitOffset)) \
			); \
		} \
	} while (0)

/*
* Set subfield of big-endian 4-byte value to specified value.
*/
#define SET_BITS_TO_BE_4BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		if (__BitOffset == 0 && __BitLen == 32) \
			WriteBE4Byte(__pStart, __Value); \
		else { \
			WriteBE4Byte(__pStart, \
				BE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u32)__Value) & BIT_LEN_MASK_32(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define SET_BITS_TO_BE_2BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		if (__BitOffset == 0 && __BitLen == 16) \
			WriteBE2Byte(__pStart, __Value); \
		else { \
			WriteBE2Byte(__pStart, \
				BE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u16)__Value) & BIT_LEN_MASK_16(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define SET_BITS_TO_BE_1BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		if (__BitOffset == 0 && __BitLen == 8) \
			WriteBE1Byte(__pStart, __Value); \
		else { \
			WriteBE1Byte(__pStart, \
				BE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u8)__Value) & BIT_LEN_MASK_8(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define _os_nvm_get_info(d, info_type, value, size) 	(0)

#endif /*_PLTFM_OPS_NONE_H_*/
