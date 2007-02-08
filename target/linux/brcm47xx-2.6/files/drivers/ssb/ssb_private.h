#ifndef LINUX_SSB_PRIVATE_H_
#define LINUX_SSB_PRIVATE_H_

#include <linux/ssb/ssb.h>
#include <linux/types.h>
#include <asm/io.h>


#define PFX	"ssb: "

#ifdef CONFIG_SSB_SILENT
# define ssb_printk(fmt, x...)	do { /* nothing */ } while (0)
#else
# define ssb_printk		printk
#endif /* CONFIG_SSB_SILENT */

/* dprintk: Debugging printk; vanishes for non-debug compilation */
#ifdef CONFIG_SSB_DEBUG
# define ssb_dprintk(fmt, x...)	ssb_printk(fmt ,##x)
#else
# define ssb_dprintk(fmt, x...)	do { /* nothing */ } while (0)
#endif

/* printkl: Rate limited printk */
#define ssb_printkl(fmt, x...)	do {		\
	if (printk_ratelimit())			\
		ssb_printk(fmt ,##x);		\
				} while (0)

/* dprintkl: Rate limited debugging printk */
#ifdef CONFIG_SSB_DEBUG
# define ssb_dprintkl			ssb_printkl
#else
# define ssb_dprintkl(fmt, x...)	do { /* nothing */ } while (0)
#endif

#define assert(cond)	do {						\
	if (unlikely(!(cond))) {					\
		ssb_dprintk(KERN_ERR PFX "BUG: Assertion failed (%s) "	\
			    "at: %s:%d:%s()\n",				\
			    #cond, __FILE__, __LINE__, __func__);	\
	}								\
		       } while (0)


/* pci.c */
#ifdef CONFIG_SSB_PCIHOST
extern int ssb_pci_switch_core(struct ssb_bus *bus,
			       struct ssb_device *dev);
extern int ssb_pci_switch_coreidx(struct ssb_bus *bus,
				  u8 coreidx);
extern int ssb_pci_xtal(struct ssb_bus *bus, u32 what,
			int turn_on);
extern int ssb_pci_sprom_get(struct ssb_bus *bus);
extern void ssb_pci_get_boardtype(struct ssb_bus *bus);
extern int ssb_pci_init(struct ssb_bus *bus);
extern const struct ssb_bus_ops ssb_pci_ops;

#else /* CONFIG_SSB_PCIHOST */

static inline int ssb_pci_switch_core(struct ssb_bus *bus,
				      struct ssb_device *dev)
{
	return 0;
}
static inline int ssb_pci_switch_coreidx(struct ssb_bus *bus,
					 u8 coreidx)
{
	return 0;
}
static inline int ssb_pci_xtal(struct ssb_bus *bus, u32 what,
			       int turn_on)
{
	return 0;
}
static inline int ssb_pci_sprom_get(struct ssb_bus *bus)
{
	return 0;
}
static inline void ssb_pci_get_boardtype(struct ssb_bus *bus)
{
}
static inline int ssb_pci_init(struct ssb_bus *bus)
{
	return 0;
}
#endif /* CONFIG_SSB_PCIHOST */


/* pcmcia.c */
#ifdef CONFIG_SSB_PCMCIAHOST
extern int ssb_pcmcia_switch_core(struct ssb_bus *bus,
				  struct ssb_device *dev);
extern int ssb_pcmcia_switch_coreidx(struct ssb_bus *bus,
				     u8 coreidx);
extern int ssb_pcmcia_switch_segment(struct ssb_bus *bus,
				     u8 seg);
extern int ssb_pcmcia_init(struct ssb_bus *bus);
extern const struct ssb_bus_ops ssb_pcmcia_ops;
#else /* CONFIG_SSB_PCMCIAHOST */
static inline int ssb_pcmcia_switch_core(struct ssb_bus *bus,
					 struct ssb_device *dev)
{
	return 0;
}
static inline int ssb_pcmcia_switch_coreidx(struct ssb_bus *bus,
					    u8 coreidx)
{
	return 0;
}
static inline int ssb_pcmcia_switch_segment(struct ssb_bus *bus,
					    u8 seg)
{
	return 0;
}
static inline int ssb_pcmcia_init(struct ssb_bus *bus)
{
	return 0;
}
#endif /* CONFIG_SSB_PCMCIAHOST */


/* scan.c */
extern const char * ssb_core_name(u16 coreid);
extern int ssb_bus_scan(struct ssb_bus *bus,
			unsigned long baseaddr);
extern void ssb_iounmap(struct ssb_bus *ssb);


/* core.c */
extern struct bus_type ssb_bustype;
extern u32 ssb_calc_clock_rate(u32 plltype, u32 n, u32 m);


/* Ceiling division helper. Divides x by y. */
static inline
unsigned long ceildiv(unsigned long x, unsigned long y)
{
	return ((x + (y - 1)) / y);
}


#endif /* LINUX_SSB_PRIVATE_H_ */
