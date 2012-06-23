#include <mach/mcs814x.h>
                .macro  disable_fiq
                .endm

		.macro arch_ret_to_user, tmp1, tmp2
		.endm

		.macro  get_irqnr_preamble, base, tmp
		ldr	\base, =mcs814x_intc_base	@ base virtual address of AIC peripheral
		ldr	\base, [\base]
		.endm

		.macro  get_irqnr_and_base, irqnr, irqstat, base, tmp
		mov	\tmp, #MCS814X_IRQ_STS0	 @ load tmp with STS0 register offset
		ldr	\irqstat, [\base, \tmp]	 @ load value at base + tmp
		tst	\irqstat, \irqstat       @ test if no active IRQ's
		beq	1002f                    @ if no active irqs return with status 0
		mov	\irqnr, #0               @ start from irq zero
		mov	\tmp,   #1               @ the mask initially 1
1001:
		tst     \irqstat, \tmp           @ and with mask
		addeq   \irqnr, \irqnr, #1       @ if  zero then add one to nr
		moveq   \tmp,   \tmp, lsl #1     @ shift mask one to left
		beq     1001b                    @ if  zero then loop again
		mov     \irqstat, \tmp           @ save the return mask
		mov	\tmp, #MCS814X_IRQ_STS0  @ load tmp with ICR offset
		str     \irqstat,  [\base, \tmp] @ clear irq with selected mask
1002:
                .endm
