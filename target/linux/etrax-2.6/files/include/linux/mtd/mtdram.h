#ifndef __MTD_MTDRAM_H__
#define __MTD_MTDRAM_H__

#ifdef __KERNEL__
#include <linux/mtd/mtd.h>
int mtdram_init_device(struct mtd_info *mtd, void *mapped_address, 
                       unsigned long size, char *name);

#endif /* __KERNEL__ */
#endif /* __MTD_MTDRAM_H__ */
