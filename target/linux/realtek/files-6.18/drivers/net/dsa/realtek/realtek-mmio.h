/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef _REALTEK_MMIO_H
#define _REALTEK_MMIO_H

#if IS_ENABLED(CONFIG_NET_DSA_REALTEK_MMIO)

int realtek_mmio_probe(struct platform_device *pdev);
void realtek_mmio_remove(struct platform_device *pdev);
void realtek_mmio_shutdown(struct platform_device *pdev);

#else /* IS_ENABLED(CONFIG_NET_DSA_REALTEK_MMIO) */

static inline int realtek_mmio_probe(struct platform_device *pdev)
{
	return -ENOENT;
}

static inline void realtek_mmio_remove(struct platform_device *pdev)
{
}

static inline void realtek_mmio_shutdown(struct platform_device *pdev)
{
}

#endif /* IS_ENABLED(CONFIG_NET_DSA_REALTEK_MMIO) */

#endif  /* _REALTEK_MMIO_H */
