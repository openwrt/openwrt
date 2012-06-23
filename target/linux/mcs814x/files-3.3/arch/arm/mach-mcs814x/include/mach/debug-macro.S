#include <mach/mcs814x.h>

                .macro  addruart, rp, rv, tmp
		ldr	\rp, =_PHYS_CONFADDR
		ldr	\rv, =_VIRT_CONFADDR
		orr	\rp, \rp, #_CONFOFFSET_UART
		orr	\rv, \rv, #_CONFOFFSET_UART
                .endm

#define UART_SHIFT	2
#include <asm/hardware/debug-8250.S>
