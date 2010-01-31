#ifndef BCM63XX_DEV_SPI_H
#define BCM63XX_DEV_SPI_H

#include <linux/types.h>

int __init bcm63xx_spi_register(void);

struct bcm63xx_spi_pdata {
	unsigned int	fifo_size;
	int		bus_num;
	int		num_chipselect;
	u32		speed_hz;
};

#endif /* BCM63XX_DEV_SPI_H */
