#ifndef _IFXMIPS_CGU_H__
#define _IFXMIPS_CGU_H__
u32 cgu_get_mips_clock(int cpu);
u32 cgu_get_cpu_clock(void);
u32 cgu_get_io_region_clock(void);
u32 cgu_get_fpi_bus_clock(int fpi);
u32 cgu_get_pp32_clock(void);
u32 cgu_get_ethernet_clock(int mii);
u32 cgu_get_usb_clock(void);
u32 cgu_get_clockout(int clkout);
#endif
