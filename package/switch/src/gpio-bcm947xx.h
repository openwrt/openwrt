#ifndef __SWITCH_GPIO_H
#define __SWITCH_GPIO_H
#include <linux/interrupt.h>

#ifndef BCMDRIVER
#include <linux/ssb/ssb_embedded.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
#define ssb ssb_bcm47xx
#endif

extern struct ssb_bus ssb;


static inline u32 gpio_in(void)
{
	return ssb_gpio_in(&ssb, ~0);
}

static inline u32 gpio_out(u32 mask, u32 value)
{
	return ssb_gpio_out(&ssb, mask, value);
}

static inline u32 gpio_outen(u32 mask, u32 value)
{
	return ssb_gpio_outen(&ssb, mask, value);
}

static inline u32 gpio_control(u32 mask, u32 value)
{
	return ssb_gpio_control(&ssb, mask, value);
}

static inline u32 gpio_intmask(u32 mask, u32 value)
{
	return ssb_gpio_intmask(&ssb, mask, value);
}

static inline u32 gpio_intpolarity(u32 mask, u32 value)
{
	return ssb_gpio_polarity(&ssb, mask, value);
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
