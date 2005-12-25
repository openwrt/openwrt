#ifndef __GPIO_H
#define __GPIO_H

#if defined(BCMGPIO2)

#ifdef LINUX_2_4
#define sbh bcm947xx_sbh
extern void *bcm947xx_sbh;
#else
extern void *sbh;
#endif

extern __u32 sb_gpioin(void *sbh);
extern __u32 sb_gpiointpolarity(void *sbh, __u32 mask, __u32 val, __u8 prio);
extern __u32 sb_gpiointmask(void *sbh, __u32 mask, __u32 val, __u8 prio);
extern __u32 sb_gpioouten(void *sbh, __u32 mask, __u32 val, __u8 prio);
extern __u32 sb_gpioout(void *sbh, __u32 mask, __u32 val, __u8 prio);

#define gpioin() sb_gpioin(sbh)
#define gpiointpolarity(mask,val) sb_gpiointpolarity(sbh, mask, val, 0)
#define gpiointmask(mask,val) sb_gpiointmask(sbh, mask, val, 0)
#define gpioouten(mask,val) sb_gpioouten(sbh, mask, val, 0)
#define gpioout(mask,val) sb_gpioout(sbh, mask, val, 0)

#elif defined(BCMGPIO)

#define sbh bcm947xx_sbh
extern void *bcm947xx_sbh;
extern __u32 sb_gpioin(void *sbh);
extern __u32 sb_gpiointpolarity(void *sbh, __u32 mask, __u32 val);
extern __u32 sb_gpiointmask(void *sbh, __u32 mask, __u32 val);
extern __u32 sb_gpioouten(void *sbh, __u32 mask, __u32 val);
extern __u32 sb_gpioout(void *sbh, __u32 mask, __u32 val);

#define gpioin() sb_gpioin(sbh)
#define gpiointpolarity(mask,val) sb_gpiointpolarity(sbh, mask, val)
#define gpiointmask(mask,val) sb_gpiointmask(sbh, mask, val)
#define gpioouten(mask,val) sb_gpioouten(sbh, mask, val)
#define gpioout(mask,val) sb_gpioout(sbh, mask, val)

#else
#error Unsupported/unknown GPIO configuration
#endif

#endif /* __GPIO_H */
