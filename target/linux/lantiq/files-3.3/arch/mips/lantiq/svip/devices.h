#ifndef _SVIP_DEVICES_H__
#define _SVIP_DEVICES_H__

#include <linux/mtd/physmap.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <svip_mux.h>
#include "../devices.h"

extern void __init svip_register_asc(int port);
extern void __init svip_register_eth(void);
extern void __init svip_register_virtual_eth(void);
extern void __init svip_register_spi(void);
extern void __init svip_register_spi_flash(struct spi_board_info *bdinfo);
extern void __init svip_register_gpio(void);
extern void __init svip_register_mux(const struct ltq_mux_pin mux_p0[LTQ_MUX_P0_PINS],
				     const struct ltq_mux_pin mux_p1[LTQ_MUX_P1_PINS],
				     const struct ltq_mux_pin mux_p2[LTQ_MUX_P2_PINS],
				     const struct ltq_mux_pin mux_p3[LTQ_MUX_P3_PINS],
				     const struct ltq_mux_pin mux_p4[LTQ_MUX_P4_PINS]);
extern void __init svip_register_nand(void);

#endif
