#ifndef __SWITCH_GPIO_H
#define __SWITCH_GPIO_H
#include <linux/interrupt.h>

#ifndef BCMDRIVER
#include <linux/ssb/ssb.h>
#include <linux/ssb/ssb_driver_chipcommon.h>
#include <linux/ssb/ssb_driver_extif.h>

extern struct ssb_bus ssb;

#define gpio_op(op, param...) \
	do { \
		if (ssb.chipco.dev) \
			return ssb_chipco_gpio_##op(&ssb.chipco, param); \
		else if (ssb.extif.dev) \
			return ssb_extif_gpio_##op(&ssb.extif, param); \
		else \
			return 0; \
	} while (0);
		

static inline u32 gpio_in(void)
{
	gpio_op(in, ~0);
}

static inline u32 gpio_out(u32 mask, u32 value)
{
	gpio_op(out, mask, value);
}

static inline u32 gpio_outen(u32 mask, u32 value)
{
	gpio_op(outen, mask, value);
}

static inline u32 gpio_control(u32 mask, u32 value)
{
	if (ssb.chipco.dev)
		return ssb_chipco_gpio_control(&ssb.chipco, mask, value);
	else
		return 0;
}

static inline u32 gpio_intmask(u32 mask, u32 value)
{
	gpio_op(intmask, mask, value);
}

static inline u32 gpio_intpolarity(u32 mask, u32 value)
{
	gpio_op(polarity, mask, value);
}

#else

#include <typedefs.h>
#include <osl.h>
#include <bcmdevs.h>
#include <sbutils.h>
#include <sbconfig.h>
#include <sbchipc.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <sbmips.h>
#else
#include <hndcpu.h>
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define sbh bcm947xx_sbh
#define sbh_lock bcm947xx_sbh_lock
#endif

extern void *sbh;
extern spinlock_t sbh_lock;

#define gpio_in()	sb_gpioin(sbh)
#define gpio_out(mask, value) 	sb_gpioout(sbh, mask, ((value) & (mask)), GPIO_DRV_PRIORITY)
#define gpio_outen(mask, value) 	sb_gpioouten(sbh, mask, value, GPIO_DRV_PRIORITY)
#define gpio_control(mask, value) 	sb_gpiocontrol(sbh, mask, value, GPIO_DRV_PRIORITY)
#define gpio_intmask(mask, value) 	sb_gpiointmask(sbh, mask, value, GPIO_DRV_PRIORITY)
#define gpio_intpolarity(mask, value) 	sb_gpiointpolarity(sbh, mask, value, GPIO_DRV_PRIORITY)

#endif /* BCMDRIVER */
#endif /* __SWITCH_GPIO_H */
