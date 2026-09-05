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
#define MSP_FW_LAN_RX_MAC_ID_MASK	0x03c00000
#define MSP_FW_LAN_RX_MAC_ID_SHIFT	22
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

static void tx_from_dqm_queue(u32 queue, u32 fifo_addr, u32 header)
{
	u32 token = dqm_pop_token(queue);

	if (!fifo_addr || !header)
		return;

	send_lan_tx_msg(header, token, fifo_addr);
}

static const volatile struct msp_4ke_port_config *
find_rx_port(const volatile struct msp_4ke_config *cfg, u32 msg_header)
{
	u32 mac_id = (msg_header & MSP_FW_LAN_RX_MAC_ID_MASK) >>
		     MSP_FW_LAN_RX_MAC_ID_SHIFT;

	for (u32 i = 0; i < cfg->enet_port_count &&
	     i < MSP_4KE_MAX_ENET_PORTS; i++) {
		const volatile struct msp_4ke_port_config *port =
			&cfg->enet_ports[i];

		if (port->valid && port->mac_id == mac_id)
			return port;
	}

	return 0;
}

static u32 valid_enet_port_count(const volatile struct msp_4ke_config *cfg)
{
	u32 valid_ports = 0;

	for (u32 i = 0; i < cfg->enet_port_count &&
	     i < MSP_4KE_MAX_ENET_PORTS; i++) {
		const volatile struct msp_4ke_port_config *port =
			&cfg->enet_ports[i];

		if (port->valid)
			valid_ports++;
	}

	return valid_ports;
}

static u32 valid_enet_ports_have_rx_space(const volatile struct msp_4ke_config *cfg)
{
	u32 base = cfg->ioproc_base;

	for (u32 i = 0; i < cfg->enet_port_count &&
	     i < MSP_4KE_MAX_ENET_PORTS; i++) {
		const volatile struct msp_4ke_port_config *port =
			&cfg->enet_ports[i];

		if (!port->valid)
			continue;
		if ((mmio_read(base + port->rx_queue_status_offset) &
		     MSP_FW_DQM_QUEUE_AVAIL_MASK) == 0)
			return 0;
	}

	return 1;
}

static void rx_to_dqm_queue(const volatile struct msp_4ke_config *cfg)
{
	u32 base = cfg->ioproc_base;
	u32 status_addr = base + cfg->in_msg_status_offset;
	u32 data_addr = base + cfg->in_msg_data_offset;
	u32 msg_header;
	u32 token;
	const volatile struct msp_4ke_port_config *port;

	if (!valid_enet_port_count(cfg))
		return;
	if (!valid_enet_ports_have_rx_space(cfg))
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

	port = find_rx_port(cfg, msg_header);
	/*
	 * Unknown MAC IDs are intentionally not returned to FPM.  The leaked
	 * token makes bad MBDMA RX routing visible through host-side FPM
	 * counters instead of silently hiding it.
	 */
	if (!port)
		mmio_write(base + cfg->host_mbox_in_offset, msg_header);
	if (!port)
		return;

	mmio_write(base + port->rx_queue_data_offset, token);
}

static u32 tx_from_enet_ports(const volatile struct msp_4ke_config *cfg,
			      u32 not_empty)
{
	for (u32 i = 0; i < cfg->enet_port_count &&
	     i < MSP_4KE_MAX_ENET_PORTS; i++) {
		const volatile struct msp_4ke_port_config *port =
			&cfg->enet_ports[i];

		if (!port->valid)
			continue;

		if (not_empty & (1u << port->tx_high_queue)) {
			tx_from_dqm_queue(port->tx_high_queue, port->tx_fifo_addr,
					  port->tx_header);
			return 1;
		}

		if (not_empty & (1u << port->tx_normal_queue)) {
			tx_from_dqm_queue(port->tx_normal_queue, port->tx_fifo_addr,
					  port->tx_header);
			return 1;
		}
	}

	return 0;
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

		if (tx_from_enet_ports(cfg, not_empty))
			continue;

		rx_to_dqm_queue(cfg);
	}
}
