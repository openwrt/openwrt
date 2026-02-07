/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _MXL862XX_HOST_H
#define _MXL862XX_HOST_H

struct mxl862xx_priv;

int mxl862xx_read(struct mxl862xx_priv *dev, u32 regaddr);
int mxl862xx_write(struct mxl862xx_priv *dev, u32 regaddr, u16 data);
int mxl862xx_api_wrap(struct mxl862xx_priv *priv, u16 cmd, void *data,
		      u16 size, bool read);

int mxl862xx_smdio_read(struct mxl862xx_priv *dev, u16 addr);
int mxl862xx_smdio_write(struct mxl862xx_priv *dev, u16 addr, u16 data);

#endif /* _MXL862XX_HOST_H */

