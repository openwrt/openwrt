/*
 * linux/arch/arm/mach-alpine/include/mach/al_fabric.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __AL_FABRIC_H__
#define __AL_FABRIC_H__

/*
 * North Bridge cause interrupt register definitions
 */
/* How many SYS Fabric IRQ Group instances exist in the system */
#define AL_FABRIC_INSTANCE_N				4

/* Number of SYS Fabric IRQ */
#define AL_FABRIC_IRQ_N					32

/* Cross trigger interrupt  */
#define AL_FABRIC_IRQ_NCTI_0				0
#define AL_FABRIC_IRQ_NCTI_1				1
#define AL_FABRIC_IRQ_NCTI_2				2
#define AL_FABRIC_IRQ_NCTI_3				3
/* Communications channel receive */
#define AL_FABRIC_IRQ_COMMRX_0				4
#define AL_FABRIC_IRQ_COMMRX_1				5
#define AL_FABRIC_IRQ_COMMRX_2				6
#define AL_FABRIC_IRQ_COMMRX_3				7
/* Communication channel transmit */
#define AL_FABRIC_IRQ_COMMTX_0				8
#define AL_FABRIC_IRQ_COMMTX_1				9
#define AL_FABRIC_IRQ_COMMTX_2				10
#define AL_FABRIC_IRQ_COMMTX_3				11
/* Write logging FIFO has valid transactions */
#define AL_FABRIC_IRQ_WR_LOG_FIFO_VALID_M0		12
/* Emulation write fifo log is wrapped */
#define AL_FABRIC_IRQ_WR_LOG_FIFO_WRAP_A0		12
/* Write logging FIFO wrap occurred */
#define AL_FABRIC_IRQ_WR_LOG_FIFO_WRAP_M0		13
/* Emulation write fifo log is full (new pushes might corrupt data) */
#define AL_FABRIC_IRQ_WR_LOG_FIFO_FULL_A0		13
/* Write logging FIFO is full */
#define AL_FABRIC_IRQ_WR_LOG_FIFO_FULL_M0		14
/* Emulation write fifo log is wrapped */
#define AL_FABRIC_IRQ_WR_LOG_FIFO_WRAP_1_A0		14
/* Reserved, read undefined must write as zeros. */
#define AL_FABRIC_IRQ_RESERVED_15_15_M0			15
/* Emulation write fifo log is full (new pushes might corrupt data) */
#define AL_FABRIC_IRQ_WR_LOG_FIFO_FULL_1_A0		15
/* Error indicator for AXI write transactions with a BRESP error */
#define AL_FABRIC_IRQ_CPU_AXIERRIRQ			16
/* Error indicator for: L2 RAM double-bit ECC error, illegal write */
#define AL_FABRIC_IRQ_CPU_INTERRIRQ			17
/* Coherent fabric error summary interrupt */
#define AL_FABRIC_IRQ_ACF_ERRORIRQ			18
/* DDR Controller ECC Correctable error summary interrupt */
#define AL_FABRIC_IRQ_MCTL_ECC_CORR_ERR			19
/* DDR Controller ECC Uncorrectable error summary interrupt */
#define AL_FABRIC_IRQ_MCTL_ECC_UNCORR_ERR		20
/* DRAM parity error interrupt */
#define AL_FABRIC_IRQ_MCTL_PARITY_ERR			21
/* Reserved, not functional */
#define AL_FABRIC_IRQ_MCTL_WDATARAM_PAR			22
/* Reserved */
#define AL_FABRIC_IRQ_MCTL_RSVRD			23
/* SB PoS error */
#define AL_FABRIC_IRQ_SB_POS_ERR			24
/* Received msix is not mapped to local GIC or IO-GIC spin */
#define AL_FABRIC_IRQ_MSIX_ERR_INT			25
/* Coresight timestamp overflow */
#define AL_FABRIC_IRQ_CORESIGHT_TS_OVERFLOW		26
/* Write data parity error from SB channel 0. */
#define AL_FABRIC_IRQ_SB0_WRDATA_PERR			27
/* Write data parity error from SB channel 1. */
#define AL_FABRIC_IRQ_SB1_WRDATA_PERR			28
/* Read data parity error from SB slaves. */
#define AL_FABRIC_IRQ_SB_SLV_RDATA_PERR			29
/* Logged read transaction is received */
#define AL_FABRIC_IRQ_RD_LOG_VALID			30
/* Reserved, read undefined must write as zeros. */
#define AL_FABRIC_IRQ_RESERVED_31_31_M0			31
/* Write logging FIFO has valid transactions */
#define AL_FABRIC_IRQ_WR_LOG_FIFO_VALID_A0		31


/**
 * Get SW interrupt index corresponding to a given sys fabric irq index.
 *
 * @param	idx
 *		The SYS Fabric IRQ Group index
 * @param	irq
 *		The SYS Fabric IRQ index (use AL_FABRIC_IRQ_*)
 *
 * @returns	Software interrupt index
 *
 * Usecase example - inside your module, get the SW irq using the API, and bind
 * it to your handler:
 *   irq = al_fabric_get_cause_irq(0, AL_FABRIC_IRQ_<SOME_IRQ>);
 *   request_irq(irq, irq_handler, ...);
 */
int al_fabric_get_cause_irq(unsigned int idx, int irq);

/**
 * Check if Hardware Cache-Coherency is enabled or not
 * @return   0 if Hardware Cache-Coherency is not enabled and a positive number
 *           otherwise
 */
int al_fabric_hwcc_enabled(void);

int al_fabric_init(void);

#endif /* __AL_FABRIC_H__ */
