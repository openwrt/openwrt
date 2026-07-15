// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3380/BCM3383 MSP 4KE co-processor firmware
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

typedef unsigned int u32;
typedef signed int s32;

#include "bcm3380-msp-fw.h"

#define MSP_FW_DQM_QUEUE_AVAIL_MASK	0x00003fff
#define MSP_FW_TOKEN_MSG_TYPE_SHIFT	26
#define MSP_FW_CP0_STATUS_CU2		0x40000000

extern volatile const struct msp_4ke_config msp_fw_config;
volatile const struct msp_4ke_config msp_fw_config
	__attribute__((section(".msp_fw_config"), used));

void main(void) __attribute__((noreturn));

__asm__(
"	.section .text.start,\"ax\",@progbits\n"
"	.set	noreorder\n"
"	.set	noat\n"
"	.globl	_start\n"
"_start:\n"
"	lui	$sp, %hi(__stack_top)\n"
"	addiu	$sp, $sp, %lo(__stack_top)\n"
"	j	main\n"
"	nop\n"
"	.set	at\n"
"	.set	reorder\n"
);

static inline u32 mmio_read(u32 addr)
{
	return *(volatile u32 *)addr;
}

static inline void mmio_write(u32 addr, u32 value)
{
	*(volatile u32 *)addr = value;
}

static inline void enable_cp2(void)
{
	u32 status;

	__asm__ volatile("mfc0 %0, $12" : "=r"(status));
	status |= MSP_FW_CP0_STATUS_CU2;
	__asm__ volatile(
		"mtc0 %0, $12\n"
		"nop\n"
		"nop\n"
		:
		: "r"(status)
		: "memory");
}

static inline u32 dqm_pop_token(u32 queue)
{
	u32 token;

	__asm__ volatile(
		"mtc2 %1, $1\n"
		"nop\n"
		"mfc2 %0, $27\n"
		"nop\n"
		: "=r"(token)
		: "r"(queue)
		: "memory");

	return token;
}

static inline void send_lan_tx_msg(u32 header, u32 token, u32 fifo_addr)
{
	__asm__ volatile(
		"mtc2 %0, $31, 1\n"
		"mtc2 %1, $31, 1\n"
		"mtc2 %2, $31, 1\n"
		:
		: "r"(header), "r"(token), "r"(fifo_addr)
		: "memory");
}

static void tx_from_dqm_queue(const volatile struct msp_4ke_config *cfg,
			      u32 queue)
{
	u32 base = cfg->ioproc_base;
	u32 token = dqm_pop_token(queue);
	u32 fifo_addr = mmio_read(base + cfg->host_mbox_out_offset);

	send_lan_tx_msg(cfg->tx_header, token, fifo_addr);
}

static void rx_to_dqm_queue(const volatile struct msp_4ke_config *cfg)
{
	u32 base = cfg->ioproc_base;
	u32 status_addr = base + cfg->in_msg_status_offset;
	u32 data_addr = base + cfg->in_msg_data_offset;
	u32 msg_header;
	u32 token;

	if ((mmio_read(base + cfg->rx_queue_status_offset) &
	     MSP_FW_DQM_QUEUE_AVAIL_MASK) == 0)
		return;

	if ((s32)mmio_read(status_addr) >= 0)
		return;
	msg_header = mmio_read(data_addr);

	do {
		if ((s32)mmio_read(status_addr) < 0)
			break;
	} while (1);
	token = mmio_read(data_addr);

	if (msg_header >> MSP_FW_TOKEN_MSG_TYPE_SHIFT)
		return;

	mmio_write(base + cfg->rx_queue_data_offset, token);
}

void main(void)
{
	const volatile struct msp_4ke_config *cfg = &msp_fw_config;
	u32 base;

	if (cfg->magic != MSP_4KE_CONFIG_MAGIC ||
	    cfg->version != MSP_4KE_CONFIG_VERSION) {
		for (;;)
			;
	}

	base = cfg->ioproc_base;
	mmio_write(base + cfg->host_mbox_in_offset, MSP_4KE_ALIVE_MAGIC);
	mmio_write(base + cfg->dqm_cfg_offset, cfg->dqm_cfg_value);
	enable_cp2();

	for (;;) {
		u32 not_empty = mmio_read(base + cfg->dqm_not_empty_status_offset);

		if (not_empty & (1u << cfg->tx_high_queue)) {
			tx_from_dqm_queue(cfg, cfg->tx_high_queue);
			continue;
		}

		if (not_empty & (1u << cfg->tx_normal_queue)) {
			tx_from_dqm_queue(cfg, cfg->tx_normal_queue);
			continue;
		}

		rx_to_dqm_queue(cfg);
	}
}
