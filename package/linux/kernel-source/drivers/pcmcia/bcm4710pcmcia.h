/*
 *
 * bcm47xx pcmcia driver
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * Based on sa1100.h and include/asm-arm/arch-sa1100/pcmica.h
 *	from www.handhelds.org,
 * and au1000_generic.c from oss.sgi.com.
 *
 * $Id$
 */

#if !defined(_BCM4710PCMCIA_H)
#define _BCM4710PCMCIA_H

#include <pcmcia/cs_types.h>
#include <pcmcia/ss.h>
#include <pcmcia/bulkmem.h>
#include <pcmcia/cistpl.h>
#include "cs_internal.h"


/* The 47xx can only support one socket */
#define BCM47XX_PCMCIA_MAX_SOCK		1

/* In the bcm947xx gpio's are used for some pcmcia functions */
#define	BCM47XX_PCMCIA_WP		0x01		/* Bit 0 is WP input */
#define	BCM47XX_PCMCIA_STSCHG		0x20		/* Bit 5 is STSCHG input/interrupt */
#define	BCM47XX_PCMCIA_RESET		0x80		/* Bit 7 is RESET */

#define	BCM47XX_PCMCIA_IRQ		2

/* The socket driver actually works nicely in interrupt-driven form,
 * so the (relatively infrequent) polling is "just to be sure."
 */
#define BCM47XX_PCMCIA_POLL_PERIOD    (2 * HZ)

#define BCM47XX_PCMCIA_IO_SPEED       (255)
#define BCM47XX_PCMCIA_MEM_SPEED      (300)


struct pcmcia_state {
	unsigned detect: 1,
		ready: 1,
		bvd1: 1,
		bvd2: 1,
		wrprot: 1,
		vs_3v: 1,
		vs_Xv: 1;
};


struct pcmcia_configure {
	unsigned sock: 8,
		vcc: 8,
		vpp: 8,
		output: 1,
		speaker: 1,
		reset: 1;
};

struct pcmcia_irq_info {
	unsigned int sock;
	unsigned int irq;
};

/* This structure encapsulates per-socket state which we might need to
 * use when responding to a Card Services query of some kind.
 */
struct bcm47xx_pcmcia_socket {
  socket_state_t        cs_state;
  struct pcmcia_state   k_state;
  unsigned int          irq;
  void                  (*handler)(void *, unsigned int);
  void                  *handler_info;
  pccard_io_map         io_map[MAX_IO_WIN];
  pccard_mem_map        mem_map[MAX_WIN];
  ioaddr_t              virt_io, phys_attr, phys_mem;
  unsigned short        speed_io, speed_attr, speed_mem;
};

struct pcmcia_init {
	void (*handler)(int irq, void *dev, struct pt_regs *regs);
};

struct pcmcia_low_level {
	int (*init)(struct pcmcia_init *);
	int (*shutdown)(void);
	int (*socket_state)(unsigned sock, struct pcmcia_state *);
	int (*get_irq_info)(struct pcmcia_irq_info *);
	int (*configure_socket)(const struct pcmcia_configure *);
};

extern struct pcmcia_low_level bcm47xx_pcmcia_ops;

/* I/O pins replacing memory pins
 * (PCMCIA System Architecture, 2nd ed., by Don Anderson, p.75)
 *
 * These signals change meaning when going from memory-only to 
 * memory-or-I/O interface:
 */
#define iostschg bvd1
#define iospkr   bvd2


/*
 * Declaration for implementation specific low_level operations.
 */
extern struct pcmcia_low_level bcm4710_pcmcia_ops;

#endif  /* !defined(_BCM4710PCMCIA_H) */
