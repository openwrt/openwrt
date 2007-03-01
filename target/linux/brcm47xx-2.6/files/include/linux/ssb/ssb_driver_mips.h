#ifndef LINUX_SSB_MIPSCORE_H_
#define LINUX_SSB_MIPSCORE_H_

#ifdef __KERNEL__

#ifdef CONFIG_BCM947XX

struct ssb_device;

struct ssb_serial_port {
	void *regs;
	unsigned long clockspeed;
	unsigned int irq;
	unsigned int baud_base;
	unsigned int reg_shift;
};


struct ssb_mipscore {
	struct ssb_device *dev;

	int nr_serial_ports;
	struct ssb_serial_port serial_ports[4];

	int flash_buswidth;
	u32 flash_window;
	u32 flash_window_size;
};

extern void ssb_mipscore_init(struct ssb_mipscore *mcore);
extern u32 ssb_cpu_clock(struct ssb_mipscore *mcore);
extern unsigned int ssb_mips_irq(struct ssb_device *dev);


#else /* CONFIG_BCM947XX */

struct ssb_mipscore {
};

static inline
void ssb_mipscore_init(struct ssb_mipscore *mcore)
{
}

#endif /* CONFIG_BCM947XX */

#endif /* __KERNEL__ */
#endif /* LINUX_SSB_MIPSCORE_H_ */
