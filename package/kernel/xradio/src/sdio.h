#ifndef __XRADIO_SDIO_H
#define __XRADIO_SDIO_H

size_t sdio_align_len(struct xradio_common *self, size_t size);
void sdio_lock(struct xradio_common *self);
void sdio_unlock(struct xradio_common *self);
int sdio_set_blk_size(struct xradio_common *self, size_t size);
int sdio_data_read(struct xradio_common *self, unsigned int addr, void *dst,
		int count);
int sdio_data_write(struct xradio_common *self, unsigned int addr, const void *src,
		int count);
int sdio_pm(struct xradio_common *self, bool  suspend);

int xradio_sdio_register(void);
void xradio_sdio_unregister(void);

#endif
