/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 *  @{
 * @file   al_hal_eth_ec_regs.h
 *
 * @brief Ethernet controller registers
 *
 */

#ifndef __AL_HAL_EC_REG_H
#define __AL_HAL_EC_REG_H

/*
* Unit Registers
*/

struct al_ec_gen {
	/* [0x0] Ethernet controller Version */
	uint32_t version;
	/* [0x4] Enable modules operation. */
	uint32_t en;
	/* [0x8] Enable FIFO operation on the EC side. */
	uint32_t fifo_en;
	/* [0xc] General L2 configuration for the Ethernet controlle ... */
	uint32_t l2;
	/* [0x10] Configure protocol index values */
	uint32_t cfg_i;
	/* [0x14] Configure protocol index values (extended protocols ... */
	uint32_t cfg_i_ext;
	/* [0x18] Enable modules operation (extended operations). */
	uint32_t en_ext;
	uint32_t rsrvd[9];
};
struct al_ec_mac {
	/* [0x0] General configuration of the MAC side of the Ethern ... */
	uint32_t gen;
	/* [0x4] Minimum packet size  */
	uint32_t min_pkt;
	/* [0x8] Maximum packet size  */
	uint32_t max_pkt;
	uint32_t rsrvd[13];
};
struct al_ec_rxf {
	/* [0x0] Rx FIFO input controller configuration 1 */
	uint32_t cfg_1;
	/* [0x4] Rx FIFO input controller configuration 2 */
	uint32_t cfg_2;
	/* [0x8] Threshold to start reading packet from the Rx FIFO */
	uint32_t rd_fifo;
	/* [0xc] Threshold to stop writing packet to the Rx FIFO */
	uint32_t wr_fifo;
	/* [0x10] Threshold to stop writing packet to the loopback FI ... */
	uint32_t lb_fifo;
	/* [0x14] Rx FIFO input controller loopback FIFO configuratio ... */
	uint32_t cfg_lb;
	/* [0x18] Configuration for dropping packet at the FIFO outpu ... */
	uint32_t out_drop;
	uint32_t rsrvd[25];
};
struct al_ec_epe {
	/* [0x0] Ethernet parsing engine configuration 1 */
	uint32_t parse_cfg;
	/* [0x4] Protocol index action table address */
	uint32_t act_table_addr;
	/* [0x8] Protocol index action table data */
	uint32_t act_table_data_1;
	/* [0xc] Protocol index action table data */
	uint32_t act_table_data_2;
	/* [0x10] Protocol index action table data */
	uint32_t act_table_data_3;
	/* [0x14] Protocol index action table data */
	uint32_t act_table_data_4;
	/* [0x18] Protocol index action table data */
	uint32_t act_table_data_5;
	/* [0x1c] Protocol index action table data */
	uint32_t act_table_data_6;
	/* [0x20] Input result vector, default values for parser inpu ... */
	uint32_t res_def;
	/* [0x24] Result input vector selection */
	uint32_t res_in;
	uint32_t rsrvd[6];
};
struct al_ec_epe_res {
	/* [0x0] Parser result vector pointer */
	uint32_t p1;
	/* [0x4] Parser result vector pointer */
	uint32_t p2;
	/* [0x8] Parser result vector pointer */
	uint32_t p3;
	/* [0xc] Parser result vector pointer */
	uint32_t p4;
	/* [0x10] Parser result vector pointer */
	uint32_t p5;
	/* [0x14] Parser result vector pointer */
	uint32_t p6;
	/* [0x18] Parser result vector pointer */
	uint32_t p7;
	/* [0x1c] Parser result vector pointer */
	uint32_t p8;
	/* [0x20] Parser result vector pointer */
	uint32_t p9;
	/* [0x24] Parser result vector pointer */
	uint32_t p10;
	/* [0x28] Parser result vector pointer */
	uint32_t p11;
	/* [0x2c] Parser result vector pointer */
	uint32_t p12;
	/* [0x30] Parser result vector pointer */
	uint32_t p13;
	/* [0x34] Parser result vector pointer */
	uint32_t p14;
	/* [0x38] Parser result vector pointer */
	uint32_t p15;
	/* [0x3c] Parser result vector pointer */
	uint32_t p16;
	/* [0x40] Parser result vector pointer */
	uint32_t p17;
	/* [0x44] Parser result vector pointer */
	uint32_t p18;
	/* [0x48] Parser result vector pointer */
	uint32_t p19;
	/* [0x4c] Parser result vector pointer */
	uint32_t p20;
	uint32_t rsrvd[12];
};
struct al_ec_epe_h {
	/* [0x0] Header length, support for header length table for  ... */
	uint32_t hdr_len;
};
struct al_ec_epe_p {
	/* [0x0] Data  for comparison */
	uint32_t comp_data;
	/* [0x4] Mask for comparison */
	uint32_t comp_mask;
	/* [0x8] Compare control */
	uint32_t comp_ctrl;
	uint32_t rsrvd[4];
};
struct al_ec_epe_a {
	/* [0x0] Protocol index action register */
	uint32_t prot_act;
};
struct al_ec_rfw {
	/* [0x0] Tuple (4/2) Hash configuration */
	uint32_t thash_cfg_1;
	/* [0x4] Tuple (4/2) Hash configuration */
	uint32_t thash_cfg_2;
	/* [0x8] MAC Hash configuration */
	uint32_t mhash_cfg_1;
	/* [0xc] MAC Hash configuration */
	uint32_t mhash_cfg_2;
	/* [0x10] MAC Hash configuration */
	uint32_t hdr_split;
	/* [0x14] Masking the errors described in  register rxf_drop  ... */
	uint32_t meta_err;
	/* [0x18] Configuration for generating the MetaData for the R ... */
	uint32_t meta;
	/* [0x1c] Configuration for generating the MetaData for the R ... */
	uint32_t filter;
	/* [0x20] 4 tupple hash table address */
	uint32_t thash_table_addr;
	/* [0x24] 4 tupple hash table data */
	uint32_t thash_table_data;
	/* [0x28] MAC hash table address */
	uint32_t mhash_table_addr;
	/* [0x2c] MAC hash table data */
	uint32_t mhash_table_data;
	/* [0x30] VLAN table address */
	uint32_t vid_table_addr;
	/* [0x34] VLAN table data */
	uint32_t vid_table_data;
	/* [0x38] VLAN p-bits table address */
	uint32_t pbits_table_addr;
	/* [0x3c] VLAN p-bits table data */
	uint32_t pbits_table_data;
	/* [0x40] DSCP table address */
	uint32_t dscp_table_addr;
	/* [0x44] DSCP table data */
	uint32_t dscp_table_data;
	/* [0x48] TC table address */
	uint32_t tc_table_addr;
	/* [0x4c] TC table data */
	uint32_t tc_table_data;
	/* [0x50] Control table address */
	uint32_t ctrl_table_addr;
	/* [0x54] Control table data */
	uint32_t ctrl_table_data;
	/* [0x58] Forwarding output configuration */
	uint32_t out_cfg;
	/* [0x5c] Flow steering mechanism,
Table address */
	uint32_t fsm_table_addr;
	/* [0x60] Flow steering mechanism,
Table data */
	uint32_t fsm_table_data;
	/* [0x64] Selection of data to be used in packet forwarding0  ... */
	uint32_t ctrl_sel;
	/* [0x68] Default VLAN data, used for untagged packets */
	uint32_t default_vlan;
	/* [0x6c] Default HASH output values */
	uint32_t default_hash;
	/* [0x70] Default override values, if a packet was filtered b ... */
	uint32_t default_or;
	/* [0x74] Latched information when a drop condition occurred */
	uint32_t drop_latch;
	/* [0x78] Check sum calculation configuration */
	uint32_t checksum;
	/* [0x7c] LRO offload engine configuration register */
	uint32_t lro_cfg_1;
	/* [0x80] LRO offload engine Check rules configurations for I ... */
	uint32_t lro_check_ipv4;
	/* [0x84] LRO offload engine IPv4 values configuration */
	uint32_t lro_ipv4;
	/* [0x88] LRO offload engine Check rules configurations for I ... */
	uint32_t lro_check_ipv6;
	/* [0x8c] LRO offload engine IPv6 values configuration */
	uint32_t lro_ipv6;
	/* [0x90] LRO offload engine Check rules configurations for T ... */
	uint32_t lro_check_tcp;
	/* [0x94] LRO offload engine IPv6 values configuration */
	uint32_t lro_tcp;
	/* [0x98] LRO offload engine Check rules configurations for U ... */
	uint32_t lro_check_udp;
	/* [0x9c] LRO offload engine Check rules configurations for U ... */
	uint32_t lro_check_l2;
	/* [0xa0] LRO offload engine Check rules configurations for U ... */
	uint32_t lro_check_gen;
	/* [0xa4] Rules for storing packet information into the cache ... */
	uint32_t lro_store;
	/* [0xa8] VLAN table default */
	uint32_t vid_table_def;
	/* [0xac] Control table default */
	uint32_t ctrl_table_def;
	/* [0xb0] Additional configuration 0 */
	uint32_t cfg_a_0;
	/* [0xb4] Tuple (4/2) Hash configuration (extended for RoCE a ... */
	uint32_t thash_cfg_3;
	/* [0xb8] Tuple (4/2) Hash configuration , mask for the input ... */
	uint32_t thash_mask_outer_ipv6;
	/* [0xbc] Tuple (4/2) Hash configuration , mask for the input ... */
	uint32_t thash_mask_outer;
	/* [0xc0] Tuple (4/2) Hash configuration , mask for the input ... */
	uint32_t thash_mask_inner_ipv6;
	/* [0xc4] Tuple (4/2) Hash configuration , mask for the input ... */
	uint32_t thash_mask_inner;
	uint32_t rsrvd[10];
};
struct al_ec_rfw_udma {
	/* [0x0] Per UDMA default configuration */
	uint32_t def_cfg;
};
struct al_ec_rfw_hash {
	/* [0x0] key configuration (320 bits) */
	uint32_t key;
};
struct al_ec_rfw_priority {
	/* [0x0] Priority to queue mapping configuration */
	uint32_t queue;
};
struct al_ec_rfw_default {
	/* [0x0] Default forwarding configuration options */
	uint32_t opt_1;
};
struct al_ec_fwd_mac {
	/* [0x0] MAC address data [31:0] */
	uint32_t data_l;
	/* [0x4] MAC address data [15:0] */
	uint32_t data_h;
	/* [0x8] MAC address mask [31:0] */
	uint32_t mask_l;
	/* [0xc] MAC address mask [15:0] */
	uint32_t mask_h;
	/* [0x10] MAC compare control */
	uint32_t ctrl;
};
struct al_ec_msw {
	/* [0x0] Configuration for unicast packets */
	uint32_t uc;
	/* [0x4] Configuration for multicast packets */
	uint32_t mc;
	/* [0x8] Configuration for broadcast packets */
	uint32_t bc;
	uint32_t rsrvd[3];
};
struct al_ec_tso {
	/* [0x0] Input configuration */
	uint32_t in_cfg;
	/* [0x4] MetaData default cache table address */
	uint32_t cache_table_addr;
	/* [0x8] MetaData default cache table data */
	uint32_t cache_table_data_1;
	/* [0xc] MetaData default cache table data */
	uint32_t cache_table_data_2;
	/* [0x10] MetaData default cache table data */
	uint32_t cache_table_data_3;
	/* [0x14] MetaData default cache table data */
	uint32_t cache_table_data_4;
	/* [0x18] TCP control bit operation for first segment */
	uint32_t ctrl_first;
	/* [0x1c] TCP control bit operation for middle segments  */
	uint32_t ctrl_middle;
	/* [0x20] TCP control bit operation for last segment */
	uint32_t ctrl_last;
	/* [0x24] Additional TSO configurations */
	uint32_t cfg_add_0;
	/* [0x28] TSO configuration for tunnelled packets */
	uint32_t cfg_tunnel;
	uint32_t rsrvd[13];
};
struct al_ec_tso_sel {
	/* [0x0] MSS value */
	uint32_t mss;
};
struct al_ec_tpe {
	/* [0x0] Parsing configuration */
	uint32_t parse;
	uint32_t rsrvd[15];
};
struct al_ec_tpm_udma {
	/* [0x0] Default VLAN data */
	uint32_t vlan_data;
	/* [0x4] UDMA MAC SA information for spoofing */
	uint32_t mac_sa_1;
	/* [0x8] UDMA MAC SA information for spoofing */
	uint32_t mac_sa_2;
};
struct al_ec_tpm_sel {
	/* [0x0] Ethertype values for VLAN modification */
	uint32_t etype;
};
struct al_ec_tfw {
	/* [0x0] Tx FIFO Wr configuration */
	uint32_t tx_wr_fifo;
	/* [0x4] VLAN table address */
	uint32_t tx_vid_table_addr;
	/* [0x8] VLAN table data */
	uint32_t tx_vid_table_data;
	/* [0xc] Tx FIFO Rd configuration */
	uint32_t tx_rd_fifo;
	/* [0x10] Tx FIFO Rd configuration, checksum insertion */
	uint32_t tx_checksum;
	/* [0x14] Tx forwarding general configuration register */
	uint32_t tx_gen;
	/* [0x18] Tx spoofing configuration */
	uint32_t tx_spf;
	/* [0x1c] TX data FIFO status */
	uint32_t data_fifo;
	/* [0x20] Tx control FIFO status */
	uint32_t ctrl_fifo;
	/* [0x24] Tx header FIFO status */
	uint32_t hdr_fifo;
	uint32_t rsrvd[14];
};
struct al_ec_tfw_udma {
	/* [0x0] Default GMDA output bitmap for unicast packet */
	uint32_t uc_udma;
	/* [0x4] Default GMDA output bitmap for multicast packet */
	uint32_t mc_udma;
	/* [0x8] Default GMDA output bitmap for broadcast packet */
	uint32_t bc_udma;
	/* [0xc] Tx spoofing configuration */
	uint32_t spf_cmd;
	/* [0x10] Forwarding decision control */
	uint32_t fwd_dec;
	uint32_t rsrvd;
};
struct al_ec_tmi {
	/* [0x0] Forward packets back to the Rx data path for local  ... */
	uint32_t tx_cfg;
	uint32_t rsrvd[3];
};
struct al_ec_efc {
	/* [0x0] Mask of pause_on  [7:0] for the Ethernet controller ... */
	uint32_t ec_pause;
	/* [0x4] Mask of Ethernet controller Almost Full indication  ... */
	uint32_t ec_xoff;
	/* [0x8] Mask for generating XON indication pulse */
	uint32_t xon;
	/* [0xc] Mask for generating GPIO output XOFF indication fro ... */
	uint32_t gpio;
	/* [0x10] Rx FIFO threshold for generating the Almost Full in ... */
	uint32_t rx_fifo_af;
	/* [0x14] Rx FIFO threshold for generating the Almost Full in ... */
	uint32_t rx_fifo_hyst;
	/* [0x18] Rx FIFO threshold for generating the Almost Full in ... */
	uint32_t stat;
	/* [0x1c] XOFF timer for the 1G MACSets the interval (in SB_C ... */
	uint32_t xoff_timer_1g;
	/* [0x20] PFC force flow control generation */
	uint32_t ec_pfc;
	uint32_t rsrvd[3];
};
struct al_ec_fc_udma {
	/* [0x0] Mask of "pause_on"  [0] for all queues */
	uint32_t q_pause_0;
	/* [0x4] Mask of "pause_on"  [1] for all queues */
	uint32_t q_pause_1;
	/* [0x8] Mask of "pause_on"  [2] for all queues */
	uint32_t q_pause_2;
	/* [0xc] Mask of "pause_on"  [3] for all queues */
	uint32_t q_pause_3;
	/* [0x10] Mask of "pause_on"  [4] for all queues */
	uint32_t q_pause_4;
	/* [0x14] Mask of "pause_on"  [5] for all queues */
	uint32_t q_pause_5;
	/* [0x18] Mask of "pause_on"  [6] for all queues */
	uint32_t q_pause_6;
	/* [0x1c] Mask of "pause_on"  [7] for all queues */
	uint32_t q_pause_7;
	/* [0x20] Mask of external GPIO input pause [0] for all queue ... */
	uint32_t q_gpio_0;
	/* [0x24] Mask of external GPIO input pause [1] for all queue ... */
	uint32_t q_gpio_1;
	/* [0x28] Mask of external GPIO input pause [2] for all queue ... */
	uint32_t q_gpio_2;
	/* [0x2c] Mask of external GPIO input pause [3] for all queue ... */
	uint32_t q_gpio_3;
	/* [0x30] Mask of external GPIO input [4] for all queues */
	uint32_t q_gpio_4;
	/* [0x34] Mask of external GPIO input [5] for all queues */
	uint32_t q_gpio_5;
	/* [0x38] Mask of external GPIO input [6] for all queues */
	uint32_t q_gpio_6;
	/* [0x3c] Mask of external GPIO input [7] for all queues */
	uint32_t q_gpio_7;
	/* [0x40] Mask of "pause_on"  [7:0] for the UDMA stream inter ... */
	uint32_t s_pause;
	/* [0x44] Mask of Rx Almost Full indication for generating XO ... */
	uint32_t q_xoff_0;
	/* [0x48] Mask of Rx Almost Full indication for generating XO ... */
	uint32_t q_xoff_1;
	/* [0x4c] Mask of Rx Almost Full indication for generating XO ... */
	uint32_t q_xoff_2;
	/* [0x50] Mask of Rx Almost Full indication for generating XO ... */
	uint32_t q_xoff_3;
	/* [0x54] Mask of Rx Almost Full indication for generating XO ... */
	uint32_t q_xoff_4;
	/* [0x58] Mask of Rx Almost Full indication for generating XO ... */
	uint32_t q_xoff_5;
	/* [0x5c] Mask of Rx Almost Full indication for generating XO ... */
	uint32_t q_xoff_6;
	/* [0x60] Mask of Rx Almost Full indication for generating XO ... */
	uint32_t q_xoff_7;
	uint32_t rsrvd[7];
};
struct al_ec_tpg_rpa_res {
	/* [0x0] NOT used */
	uint32_t not_used;
	uint32_t rsrvd[63];
};
struct al_ec_eee {
	/* [0x0] EEE configuration */
	uint32_t cfg_e;
	/* [0x4] Number of clocks to get into EEE mode. */
	uint32_t pre_cnt;
	/* [0x8] Number of clocks to stop MAC EEE mode after getting ... */
	uint32_t post_cnt;
	/* [0xc] Number of clocks to stop the Tx MAC interface after ... */
	uint32_t stop_cnt;
	/* [0x10] EEE status */
	uint32_t stat_eee;
	uint32_t rsrvd[59];
};
struct al_ec_stat {
	/* [0x0] Rx Frequency adjust FIFO input  packets */
	uint32_t faf_in_rx_pkt;
	/* [0x4] Rx Frequency adjust FIFO input short error packets */
	uint32_t faf_in_rx_short;
	/* [0x8] Rx Frequency adjust FIFO input  long error packets */
	uint32_t faf_in_rx_long;
	/* [0xc] Rx Frequency adjust FIFO output  packets */
	uint32_t faf_out_rx_pkt;
	/* [0x10] Rx Frequency adjust FIFO output short error packets ... */
	uint32_t faf_out_rx_short;
	/* [0x14] Rx Frequency adjust FIFO output long error packets */
	uint32_t faf_out_rx_long;
	/* [0x18] Rx Frequency adjust FIFO output  drop packets */
	uint32_t faf_out_drop;
	/* [0x1c] Number of packets written into the Rx FIFO (without ... */
	uint32_t rxf_in_rx_pkt;
	/* [0x20] Number of error packets written into the Rx FIFO (w ... */
	uint32_t rxf_in_fifo_err;
	/* [0x24] Number of packets written into the loopback FIFO (w ... */
	uint32_t lbf_in_rx_pkt;
	/* [0x28] Number of error packets written into the loopback F ... */
	uint32_t lbf_in_fifo_err;
	/* [0x2c] Number of packets read from Rx FIFO 1 */
	uint32_t rxf_out_rx_1_pkt;
	/* [0x30] Number of packets read from Rx FIFO 2 (loopback FIF ... */
	uint32_t rxf_out_rx_2_pkt;
	/* [0x34] Rx FIFO output drop packets from FIFO 1 */
	uint32_t rxf_out_drop_1_pkt;
	/* [0x38] Rx FIFO output drop packets from FIFO 2 (loopback) */
	uint32_t rxf_out_drop_2_pkt;
	/* [0x3c] Rx Parser 1, input packet counter */
	uint32_t rpe_1_in_rx_pkt;
	/* [0x40] Rx Parser 1, output packet counter */
	uint32_t rpe_1_out_rx_pkt;
	/* [0x44] Rx Parser 2, input packet counter */
	uint32_t rpe_2_in_rx_pkt;
	/* [0x48] Rx Parser 2, output packet counter */
	uint32_t rpe_2_out_rx_pkt;
	/* [0x4c] Rx Parser 3 (MACsec), input packet counter */
	uint32_t rpe_3_in_rx_pkt;
	/* [0x50] Rx Parser 3 (MACsec), output packet counter */
	uint32_t rpe_3_out_rx_pkt;
	/* [0x54] Tx parser, input packet counter */
	uint32_t tpe_in_tx_pkt;
	/* [0x58] Tx parser, output packet counter */
	uint32_t tpe_out_tx_pkt;
	/* [0x5c] Tx packet modification, input packet counter */
	uint32_t tpm_tx_pkt;
	/* [0x60] Tx forwarding input packet counter */
	uint32_t tfw_in_tx_pkt;
	/* [0x64] Tx forwarding input packet counter */
	uint32_t tfw_out_tx_pkt;
	/* [0x68] Rx forwarding input packet counter */
	uint32_t rfw_in_rx_pkt;
	/* [0x6c] Rx Forwarding, packet with VLAN command drop indica ... */
	uint32_t rfw_in_vlan_drop;
	/* [0x70] Rx Forwarding, packets with parse drop indication */
	uint32_t rfw_in_parse_drop;
	/* [0x74] Rx Forwarding, multicast packets */
	uint32_t rfw_in_mc;
	/* [0x78] Rx Forwarding, broadcast packets */
	uint32_t rfw_in_bc;
	/* [0x7c] Rx Forwarding, tagged packets */
	uint32_t rfw_in_vlan_exist;
	/* [0x80] Rx Forwarding, untagged packets */
	uint32_t rfw_in_vlan_nexist;
	/* [0x84] Rx Forwarding, packets with MAC address drop indica ... */
	uint32_t rfw_in_mac_drop;
	/* [0x88] Rx Forwarding, packets with undetected MAC address */
	uint32_t rfw_in_mac_ndet_drop;
	/* [0x8c] Rx Forwarding, packets with drop indication from th ... */
	uint32_t rfw_in_ctrl_drop;
	/* [0x90] Rx Forwarding, packets with L3_protocol_index drop  ... */
	uint32_t rfw_in_prot_i_drop;
	/* [0x94] EEE, number of times the system went into EEE state ... */
	uint32_t eee_in;
	uint32_t rsrvd[90];
};
struct al_ec_stat_udma {
	/* [0x0] Rx forwarding output packet counter */
	uint32_t rfw_out_rx_pkt;
	/* [0x4] Rx forwarding output drop packet counter */
	uint32_t rfw_out_drop;
	/* [0x8] Multi-stream write, number of Rx packets */
	uint32_t msw_in_rx_pkt;
	/* [0xc] Multi-stream write, number of dropped packets at SO ... */
	uint32_t msw_drop_q_full;
	/* [0x10] Multi-stream write, number of dropped packets at SO ... */
	uint32_t msw_drop_sop;
	/* [0x14] Multi-stream write, number of dropped packets at EO ... */
	uint32_t msw_drop_eop;
	/* [0x18] Multi-stream write, number of packets written to th ... */
	uint32_t msw_wr_eop;
	/* [0x1c] Multi-stream write, number of packets read from the ... */
	uint32_t msw_out_rx_pkt;
	/* [0x20] Number of transmitted packets without TSO enabled */
	uint32_t tso_no_tso_pkt;
	/* [0x24] Number of transmitted packets with TSO enabled */
	uint32_t tso_tso_pkt;
	/* [0x28] Number of TSO segments that were generated */
	uint32_t tso_seg_pkt;
	/* [0x2c] Number of TSO segments that required padding */
	uint32_t tso_pad_pkt;
	/* [0x30] Tx Packet modification, MAC SA spoof error  */
	uint32_t tpm_tx_spoof;
	/* [0x34] Tx MAC interface, input packet counter */
	uint32_t tmi_in_tx_pkt;
	/* [0x38] Tx MAC interface, number of packets forwarded to th ... */
	uint32_t tmi_out_to_mac;
	/* [0x3c] Tx MAC interface, number of packets forwarded to th ... */
	uint32_t tmi_out_to_rx;
	/* [0x40] Tx MAC interface, number of transmitted bytes */
	uint32_t tx_q0_bytes;
	/* [0x44] Tx MAC interface, number of transmitted bytes */
	uint32_t tx_q1_bytes;
	/* [0x48] Tx MAC interface, number of transmitted bytes */
	uint32_t tx_q2_bytes;
	/* [0x4c] Tx MAC interface, number of transmitted bytes */
	uint32_t tx_q3_bytes;
	/* [0x50] Tx MAC interface, number of transmitted packets */
	uint32_t tx_q0_pkts;
	/* [0x54] Tx MAC interface, number of transmitted packets */
	uint32_t tx_q1_pkts;
	/* [0x58] Tx MAC interface, number of transmitted packets */
	uint32_t tx_q2_pkts;
	/* [0x5c] Tx MAC interface, number of transmitted packets */
	uint32_t tx_q3_pkts;
	uint32_t rsrvd[40];
};
struct al_ec_msp {
	/* [0x0] Ethernet parsing engine configuration 1 */
	uint32_t p_parse_cfg;
	/* [0x4] Protocol index action table address */
	uint32_t p_act_table_addr;
	/* [0x8] Protocol index action table data */
	uint32_t p_act_table_data_1;
	/* [0xc] Protocol index action table data */
	uint32_t p_act_table_data_2;
	/* [0x10] Protocol index action table data */
	uint32_t p_act_table_data_3;
	/* [0x14] Protocol index action table data */
	uint32_t p_act_table_data_4;
	/* [0x18] Protocol index action table data */
	uint32_t p_act_table_data_5;
	/* [0x1c] Protocol index action table data */
	uint32_t p_act_table_data_6;
	/* [0x20] Input result vector, default values for parser inpu ... */
	uint32_t p_res_def;
	/* [0x24] Result input vector selection */
	uint32_t p_res_in;
	uint32_t rsrvd[6];
};
struct al_ec_msp_p {
	/* [0x0] Header length, support for header length table for  ... */
	uint32_t h_hdr_len;
};
struct al_ec_msp_c {
	/* [0x0] Data  for comparison */
	uint32_t p_comp_data;
	/* [0x4] Mask for comparison */
	uint32_t p_comp_mask;
	/* [0x8] Compare control */
	uint32_t p_comp_ctrl;
	uint32_t rsrvd[4];
};
struct al_ec_crce {
	/* [0x0] RoCE CRC init value */
	uint32_t roce_crc_init;
	/* [0x4] FCoE CRC init value */
	uint32_t fcoe_crc_init;
	/* [0x8] Packet header / trailer size - bytes not included i ... */
	uint32_t pkt_cfg;
	/* [0xc] Bit Byte Swap enable */
	uint32_t swap_en;
	/* [0x10] RoCE mask data word 0 */
	uint32_t roce_word_0;
	/* [0x14] RoCE mask data word 1 */
	uint32_t roce_word_1;
	/* [0x18] RoCE mask data word 2 */
	uint32_t roce_word_2;
	/* [0x1c] RoCE mask data word 3 */
	uint32_t roce_word_3;
	/* [0x20] General CRC engine configuration */
	uint32_t gen_cfg;
	uint32_t rsrvd[7];
};
struct al_ec_wol {
	/* [0x0] WoL enable configuration,Packet forwarding and inte ... */
	uint32_t wol_en;
	/* [0x4] Password for magic_password packet detection - bits ... */
	uint32_t magic_pswd_l;
	/* [0x8] Password for magic+password packet detection -  47: ... */
	uint32_t magic_pswd_h;
	/* [0xc] Configured L3 Destination IP address for WoL IPv6 p ... */
	uint32_t ipv6_dip_word0;
	/* [0x10] Configured L3 Destination IP address for WoL IPv6 p ... */
	uint32_t ipv6_dip_word1;
	/* [0x14] Configured L3 Destination IP address for WoL IPv6 p ... */
	uint32_t ipv6_dip_word2;
	/* [0x18] Configured L3 Destination IP address for WoL IPv6 p ... */
	uint32_t ipv6_dip_word3;
	/* [0x1c] Configured L3 Destination IP address for WoL IPv4 p ... */
	uint32_t ipv4_dip;
	/* [0x20] Configured EtherType for WoL EtherType_da/EtherType ... */
	uint32_t ethertype;
	uint32_t rsrvd[7];
};
struct al_ec_pth {
	/* [0x0] System time counter (Time of Day) */
	uint32_t system_time_seconds;
	/* [0x4] System time subseconds in a second (MSBs) */
	uint32_t system_time_subseconds_msb;
	/* [0x8] System time subseconds in a second (LSBs) */
	uint32_t system_time_subseconds_lsb;
	/* [0xc] Clock period in femtoseconds (MSB) */
	uint32_t clock_period_msb;
	/* [0x10] Clock period in femtoseconds (LSB) */
	uint32_t clock_period_lsb;
	/* [0x14] Control register for internal updates to the system ... */
	uint32_t int_update_ctrl;
	/* [0x18] Value to update system_time_seconds with */
	uint32_t int_update_seconds;
	/* [0x1c] Value to update system_time_subseconds_msb with */
	uint32_t int_update_subseconds_msb;
	/* [0x20] Value to update system_time_subseconds_lsb with */
	uint32_t int_update_subseconds_lsb;
	/* [0x24] Control register for external updates to the system ... */
	uint32_t ext_update_ctrl;
	/* [0x28] Value to update system_time_seconds with */
	uint32_t ext_update_seconds;
	/* [0x2c] Value to update system_time_subseconds_msb with */
	uint32_t ext_update_subseconds_msb;
	/* [0x30] Value to update system_time_subseconds_lsb with */
	uint32_t ext_update_subseconds_lsb;
	/* [0x34] This value represents the APB transaction delay fro ... */
	uint32_t read_compensation_subseconds_msb;
	/* [0x38] This value represents the APB transaction delay fro ... */
	uint32_t read_compensation_subseconds_lsb;
	/* [0x3c] This value is used for two purposes:1 */
	uint32_t int_write_compensation_subseconds_msb;
	/* [0x40] This value is used for two purposes:1 */
	uint32_t int_write_compensation_subseconds_lsb;
	/* [0x44] This value represents the number of cycles it for a ... */
	uint32_t ext_write_compensation_subseconds_msb;
	/* [0x48] This value represents the number of cycles it for a ... */
	uint32_t ext_write_compensation_subseconds_lsb;
	/* [0x4c] Value to be added to system_time before transferrin ... */
	uint32_t sync_compensation_subseconds_msb;
	/* [0x50] Value to be added to system_time before transferrin ... */
	uint32_t sync_compensation_subseconds_lsb;
	uint32_t rsrvd[11];
};
struct al_ec_pth_egress {
	/* [0x0] Control register for egress trigger #k */
	uint32_t trigger_ctrl;
	/* [0x4] threshold for next egress trigger (#k) - secondsWri ... */
	uint32_t trigger_seconds;
	/* [0x8] Threshold for next egress trigger (#k) - subseconds ... */
	uint32_t trigger_subseconds_msb;
	/* [0xc] threshold for next egress trigger (#k) - subseconds ... */
	uint32_t trigger_subseconds_lsb;
	/* [0x10] External output pulse width (subseconds_msb)(Atomic ... */
	uint32_t pulse_width_subseconds_msb;
	/* [0x14] External output pulse width (subseconds_lsb)(Atomic ... */
	uint32_t pulse_width_subseconds_lsb;
	uint32_t rsrvd[2];
};
struct al_ec_pth_db {
	/* [0x0] timestamp[k], in resolution of 2^18 femtosec =~ 0 */
	uint32_t ts;
	/* [0x4] Timestamp entry is valid */
	uint32_t qual;
	uint32_t rsrvd[4];
};
struct al_ec_roce {
	/* [0x0] RoCE configuration  */
	uint32_t roce_gen_cfg_1;
	/* [0x4] READ_RESPONSE  state error */
	uint32_t rr_err;
	/* [0x8] READ_RESPONSE qualification error mask */
	uint32_t rr_qual;
	/* [0xc] READ_RESPONSE packet counter mask */
	uint32_t rr_err_cnt;
	/* [0x10] READ_RESPONSE interrupt generation  error mask */
	uint32_t rr_err_int;
	/* [0x14] WRITE REQUEST  state error */
	uint32_t wr_err;
	/* [0x18] WRITE REQUEST qualification error mask */
	uint32_t wr_qual;
	/* [0x1c] WRITE REQUEST packet counter mask */
	uint32_t wr_err_cnt;
	/* [0x20] WRITE REQUEST interrupt generation  error mask */
	uint32_t wr_err_int;
	/* [0x24] RoCE configuration  */
	uint32_t roce_gen_en_1;
	/* [0x28] RoCE GRH header fields configuration  */
	uint32_t roce_hdr;
	/* [0x2c] READ RESPONSE ERROR packets */
	uint32_t rr_error;
	/* [0x30] READ RESPONSE correct packets */
	uint32_t rr_ok;
	/* [0x34] WRITE REQUEST ERROR packets */
	uint32_t wr_error;
	/* [0x38] WRITE REQUEST correct packets */
	uint32_t wr_ok;
	uint32_t rsrvd[49];
};
struct al_ec_roce_qp_db {
	/* [0x0] QP value for comparing to the received DEST_QP */
	uint32_t qp_val;
	/* [0x4] RoCE configuration  */
	uint32_t roce_qp_cfg_1;
	/* [0x8] RoCE configuration  */
	uint32_t roce_qp_cfg_2;
	/* [0xc] READ_RESPONSE Control */
	uint32_t rr_ctrl;
	/* [0x10] WRITE REQUEST Control */
	uint32_t wr_ctrl;
	/* [0x14] WRITE REQUEST PSN control */
	uint32_t wr_psn;
	/* [0x18] WRITE REQUEST, LAST PSN value that the SW sampled */
	uint32_t wr_psn_last_sw;
	/* [0x1c] WRITE REQUEST, LAST PSN value that the HW sampled ( ... */
	uint32_t wr_psn_last_hw;
	/* [0x20] WRITE REQUEST, LAST PSN value that the HW sampled ( ... */
	uint32_t wr_psn_hw;
	/* [0x24] Address translation offset high */
	uint32_t addr_offset_h;
	/* [0x28] Address translation offset low */
	uint32_t addr_offset_l;
	/* [0x2c] Address translation mask high */
	uint32_t addr_mask_h;
	/* [0x30] Address translation mask low */
	uint32_t addr_mask_l;
	/* [0x34] PKEY */
	uint32_t p_key;
	/* [0x38] RKEY */
	uint32_t r_key;
	/* [0x3c] RoCE status information */
	uint32_t roce_status;
	/* [0x40] READ REQUEST FIFO */
	uint32_t rr_fifo_1;
	/* [0x44] READ REQUEST FIFO */
	uint32_t rr_fifo_2;
	/* [0x48] READ REQUEST FIFO */
	uint32_t rr_fifo_3;
	/* [0x4c] READ REQUEST FIFO */
	uint32_t rr_fifo_4;
	/* [0x50] READ REQUEST FIFO status */
	uint32_t rr_fifo_status;
	/* [0x54] READ REQUEST FIFO control */
	uint32_t rr_fifo_ctrl;
};
struct al_ec_gen_v3 {
	/* [0x0] Bypass enable */
	uint32_t bypass;
	/* [0x4] Rx Completion descriptor */
	uint32_t rx_comp_desc;
	uint32_t rsrvd[2];
};
struct al_ec_tfw_v3 {
	/* [0x0] Generic protocol detect Cam compare table address */
	uint32_t tx_gpd_cam_addr;
	/* [0x4] Tx Generic protocol detect Cam compare data_1 (low) ... */
	uint32_t tx_gpd_cam_data_1;
	/* [0x8] Tx Generic protocol detect Cam compare data_2 (high ... */
	uint32_t tx_gpd_cam_data_2;
	/* [0xc] Tx Generic protocol detect Cam compare mask_1 (low) ... */
	uint32_t tx_gpd_cam_mask_1;
	/* [0x10] Tx Generic protocol detect Cam compare mask_1 (high ... */
	uint32_t tx_gpd_cam_mask_2;
	/* [0x14] Tx Generic protocol detect Cam compare control */
	uint32_t tx_gpd_cam_ctrl;
	/* [0x18] Tx Generic crc parameters legacy */
	uint32_t tx_gcp_legacy;
	/* [0x1c] Tx Generic crc prameters table address */
	uint32_t tx_gcp_table_addr;
	/* [0x20] Tx Generic crc prameters table general */
	uint32_t tx_gcp_table_gen;
	/* [0x24] Tx Generic crc parametrs tabel mask word 1 */
	uint32_t tx_gcp_table_mask_1;
	/* [0x28] Tx Generic crc parametrs tabel mask word 2 */
	uint32_t tx_gcp_table_mask_2;
	/* [0x2c] Tx Generic crc parametrs tabel mask word 3 */
	uint32_t tx_gcp_table_mask_3;
	/* [0x30] Tx Generic crc parametrs tabel mask word 4 */
	uint32_t tx_gcp_table_mask_4;
	/* [0x34] Tx Generic crc parametrs tabel mask word 5 */
	uint32_t tx_gcp_table_mask_5;
	/* [0x38] Tx Generic crc parametrs tabel mask word 6 */
	uint32_t tx_gcp_table_mask_6;
	/* [0x3c] Tx Generic crc parametrs tabel crc init */
	uint32_t tx_gcp_table_crc_init;
	/* [0x40] Tx Generic crc parametrs tabel result configuration ... */
	uint32_t tx_gcp_table_res;
	/* [0x44] Tx Generic crc parameters table alu opcode */
	uint32_t tx_gcp_table_alu_opcode;
	/* [0x48] Tx Generic crc parameters table alu opsel */
	uint32_t tx_gcp_table_alu_opsel;
	/* [0x4c] Tx Generic crc parameters table alu constant value */
	uint32_t tx_gcp_table_alu_val;
	/* [0x50] Tx CRC/Checksum replace */
	uint32_t crc_csum_replace;
	/* [0x54] CRC/Checksum replace table address */
	uint32_t crc_csum_replace_table_addr;
	/* [0x58] CRC/Checksum replace table */
	uint32_t crc_csum_replace_table;
	uint32_t rsrvd;
};
struct al_ec_rfw_v3 {
	/* [0x0] Rx Generic protocol detect Cam compare table addres ... */
	uint32_t rx_gpd_cam_addr;
	/* [0x4] Rx Generic protocol detect Cam compare data_1 (low) ... */
	uint32_t rx_gpd_cam_data_1;
	/* [0x8] Rx Generic protocol detect Cam compare data_2 (high ... */
	uint32_t rx_gpd_cam_data_2;
	/* [0xc] Rx Generic protocol detect Cam compare mask_1 (low) ... */
	uint32_t rx_gpd_cam_mask_1;
	/* [0x10] Rx Generic protocol detect Cam compare mask_1 (high ... */
	uint32_t rx_gpd_cam_mask_2;
	/* [0x14] Rx Generic protocol detect Cam compare control */
	uint32_t rx_gpd_cam_ctrl;
	/* [0x18] Generic protocol detect Parser result vector pointe ... */
	uint32_t gpd_p1;
	/* [0x1c] Generic protocol detect Parser result vector pointe ... */
	uint32_t gpd_p2;
	/* [0x20] Generic protocol detect Parser result vector pointe ... */
	uint32_t gpd_p3;
	/* [0x24] Generic protocol detect Parser result vector pointe ... */
	uint32_t gpd_p4;
	/* [0x28] Generic protocol detect Parser result vector pointe ... */
	uint32_t gpd_p5;
	/* [0x2c] Generic protocol detect Parser result vector pointe ... */
	uint32_t gpd_p6;
	/* [0x30] Generic protocol detect Parser result vector pointe ... */
	uint32_t gpd_p7;
	/* [0x34] Generic protocol detect Parser result vector pointe ... */
	uint32_t gpd_p8;
	/* [0x38] Rx Generic crc parameters legacy */
	uint32_t rx_gcp_legacy;
	/* [0x3c] Rx Generic crc prameters table address */
	uint32_t rx_gcp_table_addr;
	/* [0x40] Rx Generic crc prameters table general */
	uint32_t rx_gcp_table_gen;
	/* [0x44] Rx Generic crc parametrs tabel mask word 1 */
	uint32_t rx_gcp_table_mask_1;
	/* [0x48] Rx Generic crc parametrs tabel mask word 2 */
	uint32_t rx_gcp_table_mask_2;
	/* [0x4c] Rx Generic crc parametrs tabel mask word 3 */
	uint32_t rx_gcp_table_mask_3;
	/* [0x50] Rx Generic crc parametrs tabel mask word 4 */
	uint32_t rx_gcp_table_mask_4;
	/* [0x54] Rx Generic crc parametrs tabel mask word 5 */
	uint32_t rx_gcp_table_mask_5;
	/* [0x58] Rx Generic crc parametrs tabel mask word 6 */
	uint32_t rx_gcp_table_mask_6;
	/* [0x5c] Rx Generic crc parametrs tabel crc init */
	uint32_t rx_gcp_table_crc_init;
	/* [0x60] Rx Generic crc parametrs tabel result configuration ... */
	uint32_t rx_gcp_table_res;
	/* [0x64] Rx Generic crc  parameters table alu opcode */
	uint32_t rx_gcp_table_alu_opcode;
	/* [0x68] Rx Generic crc  parameters table alu opsel */
	uint32_t rx_gcp_table_alu_opsel;
	/* [0x6c] Rx Generic crc  parameters table alu constant value ... */
	uint32_t rx_gcp_table_alu_val;
	/* [0x70] Generic crc engin parameters alu Parser result vect ... */
	uint32_t rx_gcp_alu_p1;
	/* [0x74] Generic crc engine parameters alu Parser result vec ... */
	uint32_t rx_gcp_alu_p2;
	/* [0x78] Header split control table address */
	uint32_t hs_ctrl_table_addr;
	/* [0x7c] Header split control table */
	uint32_t hs_ctrl_table;
	/* [0x80] Header split control alu opcode */
	uint32_t hs_ctrl_table_alu_opcode;
	/* [0x84] Header split control alu opsel */
	uint32_t hs_ctrl_table_alu_opsel;
	/* [0x88] Header split control alu constant value */
	uint32_t hs_ctrl_table_alu_val;
	/* [0x8c] Header split control configuration */
	uint32_t hs_ctrl_cfg;
	/* [0x90] Header split control alu Parser result vector point ... */
	uint32_t hs_ctrl_alu_p1;
	/* [0x94] Header split control alu Parser result vector point ... */
	uint32_t hs_ctrl_alu_p2;
};
struct al_ec_crypto {
	/* [0x0] Tx inline crypto configuration */
	uint32_t tx_config;
	/* [0x4] Rx inline crypto configuration */
	uint32_t rx_config;
	/* [0x8] inline XTS alpha [31:0] */
	uint32_t xts_alpha_1;
	/* [0xc] inline XTS alpha [63:32] */
	uint32_t xts_alpha_2;
	/* [0x10] inline XTS alpha [95:64] */
	uint32_t xts_alpha_3;
	/* [0x14] inline XTS alpha [127:96] */
	uint32_t xts_alpha_4;
	/* [0x18] inline XTS sector ID increment [31:0] */
	uint32_t xts_sector_id_1;
	/* [0x1c] inline XTS sector ID increment [63:32] */
	uint32_t xts_sector_id_2;
	/* [0x20] inline XTS sector ID increment [95:64] */
	uint32_t xts_sector_id_3;
	/* [0x24] inline XTS sector ID increment [127:96] */
	uint32_t xts_sector_id_4;
};

struct al_ec_regs {
	uint32_t rsrvd_0[32];
	struct al_ec_gen gen;                                /* [0x80] */
	struct al_ec_mac mac;                                /* [0xc0] */
	struct al_ec_rxf rxf;                                /* [0x100] */
	struct al_ec_epe epe[2];                             /* [0x180] */
	struct al_ec_epe_res epe_res;                        /* [0x200] */
	struct al_ec_epe_h epe_h[32];                        /* [0x280] */
	struct al_ec_epe_p epe_p[32];                        /* [0x300] */
	struct al_ec_epe_a epe_a[32];                        /* [0x680] */
	struct al_ec_rfw rfw;                                /* [0x700] */
	struct al_ec_rfw_udma rfw_udma[4];                   /* [0x7f0] */
	struct al_ec_rfw_hash rfw_hash[10];                  /* [0x800] */
	struct al_ec_rfw_priority rfw_priority[8];           /* [0x828] */
	struct al_ec_rfw_default rfw_default[8];             /* [0x848] */
	struct al_ec_fwd_mac fwd_mac[32];                    /* [0x868] */
	struct al_ec_msw msw;                                /* [0xae8] */
	struct al_ec_tso tso;                                /* [0xb00] */
	struct al_ec_tso_sel tso_sel[8];                     /* [0xb60] */
	struct al_ec_tpe tpe;                                /* [0xb80] */
	struct al_ec_tpm_udma tpm_udma[4];                   /* [0xbc0] */
	struct al_ec_tpm_sel tpm_sel[4];                     /* [0xbf0] */
	struct al_ec_tfw tfw;                                /* [0xc00] */
	struct al_ec_tfw_udma tfw_udma[4];                   /* [0xc60] */
	struct al_ec_tmi tmi;                                /* [0xcc0] */
	struct al_ec_efc efc;                                /* [0xcd0] */
	struct al_ec_fc_udma fc_udma[4];                     /* [0xd00] */
	struct al_ec_tpg_rpa_res tpg_rpa_res;                /* [0xf00] */
	struct al_ec_eee eee;                                /* [0x1000] */
	struct al_ec_stat stat;                              /* [0x1100] */
	struct al_ec_stat_udma stat_udma[4];                 /* [0x1300] */
	struct al_ec_msp msp;                                /* [0x1700] */
	struct al_ec_msp_p msp_p[32];                        /* [0x1740] */
	struct al_ec_msp_c msp_c[32];                        /* [0x17c0] */
	struct al_ec_crce crce;                              /* [0x1b40] */
	struct al_ec_wol wol;                                /* [0x1b80] */
	uint32_t rsrvd_1[80];
	struct al_ec_pth pth;                                /* [0x1d00] */
	struct al_ec_pth_egress pth_egress[8];               /* [0x1d80] */
	struct al_ec_pth_db pth_db[16];                      /* [0x1e80] */
	struct al_ec_roce roce;                              /* [0x2000] */
	struct al_ec_roce_qp_db roce_qp_db[16];              /* [0x2100] */
	struct al_ec_gen_v3 gen_v3;                             /* [0x2680] */
	struct al_ec_tfw_v3 tfw_v3;                             /* [0x2690] */
	struct al_ec_rfw_v3 rfw_v3;                             /* [0x26f0] */
	uint32_t rsrvd_2[2];
	struct al_ec_crypto crypto;                             /* [0x2790] */
};

/*
* Registers Fields
*/

/* Selection between descriptor caching options (WORD selection) ... */
#define EC_GEN_EN_EXT_CACHE_WORD_SPLIT   (1 << 20)

/**** wr_fifo register ****/

/**** lb_fifo register ****/

/**** out_drop register ****/

/* Enable SIP/DIP swap if SIP<DIP */
#define EC_RFW_THASH_CFG_1_ENABLE_IP_SWAP (1 << 16)
/* Enable PORT swap if SPORT<DPORT */
#define EC_RFW_THASH_CFG_1_ENABLE_PORT_SWAP (1 << 17)

/* Selects how to calculate the L3 header length when L3 is IpPv ... */
#define EC_RFW_META_L3_LEN_CALC          (1 << 4)

/* Select the header that will be used for the checksum when a t ... */
#define EC_RFW_CHECKSUM_HDR_SEL          (1 << 1)

/* Default data selection 0 - Default value 1 - Table data out ... */
#define EC_RFW_CTRL_TABLE_DEF_SEL        (1 << 20)

/* Drop indication */
#define EC_FWD_MAC_CTRL_RX_VAL_DROP		(1 << 0)

/* UDMA selection */
#define EC_FWD_MAC_CTRL_RX_VAL_UDMA_MASK	0x000000078
#define EC_FWD_MAC_CTRL_RX_VAL_UDMA_SHIFT	3

/* queue number */
#define EC_FWD_MAC_CTRL_RX_VAL_QID_MASK		0x00000180
#define EC_FWD_MAC_CTRL_RX_VAL_QID_SHIFT	7

/* Entry is valid for Rx forwarding engine. */
#define EC_FWD_MAC_CTRL_RX_VALID         (1 << 15)
/* Control value for Tx forwarding engine */
#define EC_FWD_MAC_CTRL_TX_VAL_MASK      0x001F0000
#define EC_FWD_MAC_CTRL_TX_VAL_SHIFT     16
/* Entry is valid for Tx forwarding engine. */
#define EC_FWD_MAC_CTRL_TX_VALID         (1 << 31)

/**** cfg_add_0 register ****/
/* MSS selection option:0 - MSS value is selected using MSS_sel  ... */
#define EC_TSO_CFG_ADD_0_MSS_SEL         (1 << 0)

/**** cfg_tunnel register ****/
/* Enable TSO with tunnelling */
#define EC_TSO_CFG_TUNNEL_EN_TUNNEL_TSO  (1 << 0)
/* Enable outer UDP checksum update */
#define EC_TSO_CFG_TUNNEL_EN_UDP_CHKSUM  (1 << 8)
/* Enable outer UDP length update */
#define EC_TSO_CFG_TUNNEL_EN_UDP_LEN     (1 << 9)
/* Enable outer Ip6  length update */
#define EC_TSO_CFG_TUNNEL_EN_IPV6_PLEN   (1 << 10)
/* Enable outer IPv4 checksum update */
#define EC_TSO_CFG_TUNNEL_EN_IPV4_CHKSUM (1 << 11)
/* Enable outer IPv4 Identification update */
#define EC_TSO_CFG_TUNNEL_EN_IPV4_IDEN   (1 << 12)
/* Enable outer IPv4 length update */
#define EC_TSO_CFG_TUNNEL_EN_IPV4_TLEN   (1 << 13)

/**** tx_cfg register ****/
/* Swap output byte order */
#define EC_TMI_TX_CFG_SWAP_BYTES         (1 << 0)
/* Enable forwarding to the Rx data path. */
#define EC_TMI_TX_CFG_EN_FWD_TO_RX       (1 << 1)

#define EC_EFC_EC_XOFF_MASK_2_SHIFT      8

/**** xon register ****/
/* Mask 1 for generating XON pulse, masking XOFF [0] */
#define EC_EFC_XON_MASK_1                (1 << 0)
/* Mask 2 for generating XON pulse, masking Almost Full indicati ... */
#define EC_EFC_XON_MASK_2                (1 << 1)

#define EC_EFC_RX_FIFO_HYST_TH_HIGH_SHIFT 16

/* Use Ethernet controller Tx FIFO empty status for EEE control */
#define EC_EEE_CFG_E_USE_EC_TX_FIFO      (1 << 2)
/* Use Ethernet controller Rx FIFO empty status for EEE control */
#define EC_EEE_CFG_E_USE_EC_RX_FIFO      (1 << 3)
/* Enable Low power signalling. */
#define EC_EEE_CFG_E_ENABLE              (1 << 4)
/* Mask output to MAC.  */
#define EC_EEE_CFG_E_MASK_MAC_EEE        (1 << 8)
/* Mask output to stop MAC interface. */
#define EC_EEE_CFG_E_MASK_EC_TMI_STOP    (1 << 9)

/**** int_update_subseconds_lsb register ****/

/**** ext_update_subseconds_lsb register ****/

/**** trigger_subseconds_lsb register ****/

/**** pulse_width_subseconds_lsb register ****/

/**** qual register ****/

#endif /* __AL_HAL_EC_REG_H */

/** @} end of ... group */
