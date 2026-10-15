// SPDX-License-Identifier: GPL-2.0-only

#include <linux/bitmap.h>
#include <linux/cleanup.h>
#include <linux/mutex.h>
#include <linux/soc/realtek/otto_table.h>
#include <linux/string.h>
#include <net/dsa.h>

#include "pie.h"
#include "rtl-otto.h"
#include "stats.h"

#define MIB_ITEM(_reg, _offset, _size) \
		{.reg = _reg, .offset = _offset, .size = _size}

#define MIB_LIST_ITEM(_name, _item) \
		{.name = _name, .item = _item}

static const struct otto_stats_mib_list_item otto_stats_838x_mib_list[] = {
	MIB_LIST_ITEM("dot1dTpPortInDiscards", MIB_ITEM(MIB_REG_STD, 0xec, 1)),
	MIB_LIST_ITEM("ifOutDiscards", MIB_ITEM(MIB_REG_STD, 0xd0, 1)),
	MIB_LIST_ITEM("DropEvents", MIB_ITEM(MIB_REG_STD, 0xa8, 1)),
	MIB_LIST_ITEM("tx_BroadcastPkts", MIB_ITEM(MIB_REG_STD, 0xa4, 1)),
	MIB_LIST_ITEM("tx_MulticastPkts", MIB_ITEM(MIB_REG_STD, 0xa0, 1)),
	MIB_LIST_ITEM("tx_UndersizePkts", MIB_ITEM(MIB_REG_STD, 0x98, 1)),
	MIB_LIST_ITEM("rx_UndersizeDropPkts", MIB_ITEM(MIB_REG_STD, 0x90, 1)),
	MIB_LIST_ITEM("tx_OversizePkts", MIB_ITEM(MIB_REG_STD, 0x8c, 1)),
	MIB_LIST_ITEM("Collisions", MIB_ITEM(MIB_REG_STD, 0x7c, 1)),
	MIB_LIST_ITEM("rx_MacDiscards", MIB_ITEM(MIB_REG_STD, 0x40, 1))
};

const struct otto_stats_mib_desc otto_stats_838x_mib_desc = {
	.symbol_errors = MIB_ITEM(MIB_REG_STD, 0xb8, 1),

	.if_in_octets = MIB_ITEM(MIB_REG_STD, 0xf8, 2),
	.if_out_octets = MIB_ITEM(MIB_REG_STD, 0xf0, 2),
	.if_in_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xe8, 1),
	.if_in_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe4, 1),
	.if_in_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe0, 1),
	.if_out_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xdc, 1),
	.if_out_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xd8, 1),
	.if_out_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xd4, 1),
	.if_out_discards = MIB_ITEM(MIB_REG_STD, 0xd0, 1),
	.single_collisions = MIB_ITEM(MIB_REG_STD, 0xcc, 1),
	.multiple_collisions = MIB_ITEM(MIB_REG_STD, 0xc8, 1),
	.deferred_transmissions = MIB_ITEM(MIB_REG_STD, 0xc4, 1),
	.late_collisions = MIB_ITEM(MIB_REG_STD, 0xc0, 1),
	.excessive_collisions = MIB_ITEM(MIB_REG_STD, 0xbc, 1),
	.crc_align_errors = MIB_ITEM(MIB_REG_STD, 0x9c, 1),

	.unsupported_opcodes = MIB_ITEM(MIB_REG_STD, 0xb4, 1),

	.rx_undersize_pkts = MIB_ITEM(MIB_REG_STD, 0x94, 1),
	.rx_oversize_pkts = MIB_ITEM(MIB_REG_STD, 0x88, 1),
	.rx_fragments = MIB_ITEM(MIB_REG_STD, 0x84, 1),
	.rx_jabbers = MIB_ITEM(MIB_REG_STD, 0x80, 1),

	.tx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x78, 1),
		MIB_ITEM(MIB_REG_STD, 0x70, 1),
		MIB_ITEM(MIB_REG_STD, 0x68, 1),
		MIB_ITEM(MIB_REG_STD, 0x60, 1),
		MIB_ITEM(MIB_REG_STD, 0x58, 1),
		MIB_ITEM(MIB_REG_STD, 0x50, 1),
		MIB_ITEM(MIB_REG_STD, 0x48, 1)
	},
	.rx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x74, 1),
		MIB_ITEM(MIB_REG_STD, 0x6c, 1),
		MIB_ITEM(MIB_REG_STD, 0x64, 1),
		MIB_ITEM(MIB_REG_STD, 0x5c, 1),
		MIB_ITEM(MIB_REG_STD, 0x54, 1),
		MIB_ITEM(MIB_REG_STD, 0x4c, 1),
		MIB_ITEM(MIB_REG_STD, 0x44, 1)
	},
	.rmon_ranges = {
		{ 0, 64 },
		{ 65, 127 },
		{ 128, 255 },
		{ 256, 511 },
		{ 512, 1023 },
		{ 1024, 1518 },
		{ 1519, 10000 }
	},

	.drop_events = MIB_ITEM(MIB_REG_STD, 0xa8, 1),
	.collisions = MIB_ITEM(MIB_REG_STD, 0x7c, 1),

	.rx_pause_frames = MIB_ITEM(MIB_REG_STD, 0xb0, 1),
	.tx_pause_frames = MIB_ITEM(MIB_REG_STD, 0xac, 1),

	.list_count = ARRAY_SIZE(otto_stats_838x_mib_list),
	.list = otto_stats_838x_mib_list
};

static const struct otto_stats_mib_list_item otto_stats_839x_mib_list[] = {
	MIB_LIST_ITEM("ifOutDiscards", MIB_ITEM(MIB_REG_STD, 0xd4, 1)),
	MIB_LIST_ITEM("dot1dTpPortInDiscards", MIB_ITEM(MIB_REG_STD, 0xd0, 1)),
	MIB_LIST_ITEM("DropEvents", MIB_ITEM(MIB_REG_STD, 0xa8, 1)),
	MIB_LIST_ITEM("tx_BroadcastPkts", MIB_ITEM(MIB_REG_STD, 0xa4, 1)),
	MIB_LIST_ITEM("tx_MulticastPkts", MIB_ITEM(MIB_REG_STD, 0xa0, 1)),
	MIB_LIST_ITEM("tx_UndersizePkts", MIB_ITEM(MIB_REG_STD, 0x98, 1)),
	MIB_LIST_ITEM("rx_UndersizeDropPkts", MIB_ITEM(MIB_REG_STD, 0x90, 1)),
	MIB_LIST_ITEM("tx_OversizePkts", MIB_ITEM(MIB_REG_STD, 0x8c, 1)),
	MIB_LIST_ITEM("Collisions", MIB_ITEM(MIB_REG_STD, 0x7c, 1)),
	MIB_LIST_ITEM("rx_LengthFieldError", MIB_ITEM(MIB_REG_STD, 0x40, 1)),
	MIB_LIST_ITEM("rx_FalseCarrierTimes", MIB_ITEM(MIB_REG_STD, 0x3c, 1)),
	MIB_LIST_ITEM("rx_UnderSizeOctets", MIB_ITEM(MIB_REG_STD, 0x38, 1)),
	MIB_LIST_ITEM("tx_Fragments", MIB_ITEM(MIB_REG_STD, 0x34, 1)),
	MIB_LIST_ITEM("tx_Jabbers", MIB_ITEM(MIB_REG_STD, 0x30, 1)),
	MIB_LIST_ITEM("tx_CRCAlignErrors", MIB_ITEM(MIB_REG_STD, 0x2c, 1)),
	MIB_LIST_ITEM("rx_FramingErrors", MIB_ITEM(MIB_REG_STD, 0x28, 1)),
	MIB_LIST_ITEM("rx_MacDiscards", MIB_ITEM(MIB_REG_STD, 0x24, 1))
};

const struct otto_stats_mib_desc otto_stats_839x_mib_desc = {
	.symbol_errors = MIB_ITEM(MIB_REG_STD, 0xb8, 1),

	.if_in_octets = MIB_ITEM(MIB_REG_STD, 0xf8, 2),
	.if_out_octets = MIB_ITEM(MIB_REG_STD, 0xf0, 2),
	.if_in_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xec, 1),
	.if_in_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe8, 1),
	.if_in_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe4, 1),
	.if_out_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xe0, 1),
	.if_out_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xdc, 1),
	.if_out_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xd8, 1),
	.if_out_discards = MIB_ITEM(MIB_REG_STD, 0xd4, 1),
	.single_collisions = MIB_ITEM(MIB_REG_STD, 0xcc, 1),
	.multiple_collisions = MIB_ITEM(MIB_REG_STD, 0xc8, 1),
	.deferred_transmissions = MIB_ITEM(MIB_REG_STD, 0xc4, 1),
	.late_collisions = MIB_ITEM(MIB_REG_STD, 0xc0, 1),
	.excessive_collisions = MIB_ITEM(MIB_REG_STD, 0xbc, 1),
	.crc_align_errors = MIB_ITEM(MIB_REG_STD, 0x9c, 1),

	.unsupported_opcodes = MIB_ITEM(MIB_REG_STD, 0xb4, 1),

	.rx_undersize_pkts = MIB_ITEM(MIB_REG_STD, 0x94, 1),
	.rx_oversize_pkts = MIB_ITEM(MIB_REG_STD, 0x88, 1),
	.rx_fragments = MIB_ITEM(MIB_REG_STD, 0x84, 1),
	.rx_jabbers = MIB_ITEM(MIB_REG_STD, 0x80, 1),

	.tx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x78, 1),
		MIB_ITEM(MIB_REG_STD, 0x70, 1),
		MIB_ITEM(MIB_REG_STD, 0x68, 1),
		MIB_ITEM(MIB_REG_STD, 0x60, 1),
		MIB_ITEM(MIB_REG_STD, 0x58, 1),
		MIB_ITEM(MIB_REG_STD, 0x50, 1),
		MIB_ITEM(MIB_REG_STD, 0x48, 1)
	},
	.rx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x74, 1),
		MIB_ITEM(MIB_REG_STD, 0x6c, 1),
		MIB_ITEM(MIB_REG_STD, 0x64, 1),
		MIB_ITEM(MIB_REG_STD, 0x5c, 1),
		MIB_ITEM(MIB_REG_STD, 0x54, 1),
		MIB_ITEM(MIB_REG_STD, 0x4c, 1),
		MIB_ITEM(MIB_REG_STD, 0x44, 1)
	},
	.rmon_ranges = {
		{ 0, 64 },
		{ 65, 127 },
		{ 128, 255 },
		{ 256, 511 },
		{ 512, 1023 },
		{ 1024, 1518 },
		{ 1519, 12288 }
	},

	.drop_events = MIB_ITEM(MIB_REG_STD, 0xa8, 1),
	.collisions = MIB_ITEM(MIB_REG_STD, 0x7c, 1),

	.rx_pause_frames = MIB_ITEM(MIB_REG_STD, 0xb0, 1),
	.tx_pause_frames = MIB_ITEM(MIB_REG_STD, 0xac, 1),

	.list_count = ARRAY_SIZE(otto_stats_839x_mib_list),
	.list = otto_stats_839x_mib_list
};

static const struct otto_stats_mib_list_item otto_stats_930x_mib_list[] = {
	MIB_LIST_ITEM("ifOutDiscards", MIB_ITEM(MIB_REG_STD, 0xbc, 1)),
	MIB_LIST_ITEM("dot1dTpPortInDiscards", MIB_ITEM(MIB_REG_STD, 0xb8, 1)),
	MIB_LIST_ITEM("DropEvents", MIB_ITEM(MIB_REG_STD, 0x90, 1)),
	MIB_LIST_ITEM("tx_BroadcastPkts", MIB_ITEM(MIB_REG_STD, 0x8c, 1)),
	MIB_LIST_ITEM("tx_MulticastPkts", MIB_ITEM(MIB_REG_STD, 0x88, 1)),
	MIB_LIST_ITEM("tx_CRCAlignErrors", MIB_ITEM(MIB_REG_STD, 0x84, 1)),
	MIB_LIST_ITEM("tx_UndersizePkts", MIB_ITEM(MIB_REG_STD, 0x7c, 1)),
	MIB_LIST_ITEM("tx_OversizePkts", MIB_ITEM(MIB_REG_STD, 0x74, 1)),
	MIB_LIST_ITEM("tx_Fragments", MIB_ITEM(MIB_REG_STD, 0x6c, 1)),
	MIB_LIST_ITEM("tx_Jabbers", MIB_ITEM(MIB_REG_STD, 0x64, 1)),
	MIB_LIST_ITEM("tx_Collisions", MIB_ITEM(MIB_REG_STD, 0x5c, 1)),
	MIB_LIST_ITEM("rx_UndersizeDropPkts", MIB_ITEM(MIB_REG_PRV, 0x7c, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsSet1", MIB_ITEM(MIB_REG_PRV, 0x68, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsSet1", MIB_ITEM(MIB_REG_PRV, 0x64, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsCRCSet1", MIB_ITEM(MIB_REG_PRV, 0x60, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsCRCSet1", MIB_ITEM(MIB_REG_PRV, 0x5c, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsSet0", MIB_ITEM(MIB_REG_PRV, 0x58, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsSet0", MIB_ITEM(MIB_REG_PRV, 0x54, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsCRCSet0", MIB_ITEM(MIB_REG_PRV, 0x50, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsCRCSet0", MIB_ITEM(MIB_REG_PRV, 0x4c, 1)),
	MIB_LIST_ITEM("LengthFieldError", MIB_ITEM(MIB_REG_PRV, 0x48, 1)),
	MIB_LIST_ITEM("FalseCarrierTimes", MIB_ITEM(MIB_REG_PRV, 0x44, 1)),
	MIB_LIST_ITEM("UndersizeOctets", MIB_ITEM(MIB_REG_PRV, 0x40, 1)),
	MIB_LIST_ITEM("FramingErrors", MIB_ITEM(MIB_REG_PRV, 0x3c, 1)),
	MIB_LIST_ITEM("ParserErrors", MIB_ITEM(MIB_REG_PRV, 0x38, 1)),
	MIB_LIST_ITEM("rx_MacDiscards", MIB_ITEM(MIB_REG_PRV, 0x34, 1)),
	MIB_LIST_ITEM("rx_MacIPGShortDrop", MIB_ITEM(MIB_REG_PRV, 0x30, 1))
};

const struct otto_stats_mib_desc otto_stats_930x_mib_desc = {
	.symbol_errors = MIB_ITEM(MIB_REG_STD, 0xa0, 1),

	.if_in_octets = MIB_ITEM(MIB_REG_STD, 0xf8, 2),
	.if_out_octets = MIB_ITEM(MIB_REG_STD, 0xf0, 2),
	.if_in_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xe8, 2),
	.if_in_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe0, 2),
	.if_in_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xd8, 2),
	.if_out_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xd0, 2),
	.if_out_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xc8, 2),
	.if_out_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xc0, 2),
	.if_out_discards = MIB_ITEM(MIB_REG_STD, 0xbc, 1),
	.single_collisions = MIB_ITEM(MIB_REG_STD, 0xb4, 1),
	.multiple_collisions = MIB_ITEM(MIB_REG_STD, 0xb0, 1),
	.deferred_transmissions = MIB_ITEM(MIB_REG_STD, 0xac, 1),
	.late_collisions = MIB_ITEM(MIB_REG_STD, 0xa8, 1),
	.excessive_collisions = MIB_ITEM(MIB_REG_STD, 0xa4, 1),
	.crc_align_errors = MIB_ITEM(MIB_REG_STD, 0x80, 1),
	.rx_pkts_over_max_octets = MIB_ITEM(MIB_REG_PRV, 0x6c, 1),

	.unsupported_opcodes = MIB_ITEM(MIB_REG_STD, 0x9c, 1),

	.rx_undersize_pkts = MIB_ITEM(MIB_REG_STD, 0x78, 1),
	.rx_oversize_pkts = MIB_ITEM(MIB_REG_STD, 0x70, 1),
	.rx_fragments = MIB_ITEM(MIB_REG_STD, 0x68, 1),
	.rx_jabbers = MIB_ITEM(MIB_REG_STD, 0x60, 1),

	.tx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x58, 1),
		MIB_ITEM(MIB_REG_STD, 0x50, 1),
		MIB_ITEM(MIB_REG_STD, 0x48, 1),
		MIB_ITEM(MIB_REG_STD, 0x40, 1),
		MIB_ITEM(MIB_REG_STD, 0x38, 1),
		MIB_ITEM(MIB_REG_STD, 0x30, 1),
		MIB_ITEM(MIB_REG_PRV, 0x78, 1),
		MIB_ITEM(MIB_REG_PRV, 0x70, 1)
	},
	.rx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x54, 1),
		MIB_ITEM(MIB_REG_STD, 0x4c, 1),
		MIB_ITEM(MIB_REG_STD, 0x44, 1),
		MIB_ITEM(MIB_REG_STD, 0x3c, 1),
		MIB_ITEM(MIB_REG_STD, 0x34, 1),
		MIB_ITEM(MIB_REG_STD, 0x2c, 1),
		MIB_ITEM(MIB_REG_PRV, 0x74, 1),
		MIB_ITEM(MIB_REG_PRV, 0x6c, 1),
	},
	.rmon_ranges = {
		{ 0, 64 },
		{ 65, 127 },
		{ 128, 255 },
		{ 256, 511 },
		{ 512, 1023 },
		{ 1024, 1518 },
		{ 1519, 12288 },
		{ 12289, 65535 }
	},

	.drop_events = MIB_ITEM(MIB_REG_STD, 0x90, 1),
	.collisions = MIB_ITEM(MIB_REG_STD, 0x5c, 1),

	.rx_pause_frames = MIB_ITEM(MIB_REG_STD, 0x98, 1),
	.tx_pause_frames = MIB_ITEM(MIB_REG_STD, 0x94, 1),

	.list_count = ARRAY_SIZE(otto_stats_930x_mib_list),
	.list = otto_stats_930x_mib_list
};

static const struct otto_stats_mib_list_item otto_stats_931x_mib_list[] = {
	MIB_LIST_ITEM("ifOutDiscards", MIB_ITEM(MIB_TBL_STD, 36, 1)),
	MIB_LIST_ITEM("dot1dTpPortInDiscards", MIB_ITEM(MIB_TBL_STD, 35, 1)),
	MIB_LIST_ITEM("DropEvents", MIB_ITEM(MIB_TBL_STD, 25, 1)),
	MIB_LIST_ITEM("tx_BroadcastPkts", MIB_ITEM(MIB_TBL_STD, 24, 1)),
	MIB_LIST_ITEM("tx_MulticastPkts", MIB_ITEM(MIB_TBL_STD, 23, 1)),
	MIB_LIST_ITEM("tx_CRCAlignErrors", MIB_ITEM(MIB_TBL_STD, 22, 1)),
	MIB_LIST_ITEM("tx_UndersizePkts", MIB_ITEM(MIB_TBL_STD, 20, 1)),
	MIB_LIST_ITEM("tx_OversizePkts", MIB_ITEM(MIB_TBL_STD, 18, 1)),
	MIB_LIST_ITEM("tx_Fragments", MIB_ITEM(MIB_TBL_STD, 16, 1)),
	MIB_LIST_ITEM("tx_Jabbers", MIB_ITEM(MIB_TBL_STD, 14, 1)),
	MIB_LIST_ITEM("tx_Collisions", MIB_ITEM(MIB_TBL_STD, 12, 1)),

	MIB_LIST_ITEM("rx_UndersizeDropPkts", MIB_ITEM(MIB_TBL_PRV, 27, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsSet1", MIB_ITEM(MIB_TBL_PRV, 22, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsSet1", MIB_ITEM(MIB_TBL_PRV, 21, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsCRCSet1", MIB_ITEM(MIB_TBL_PRV, 20, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsCRCSet1", MIB_ITEM(MIB_TBL_PRV, 19, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsSet0", MIB_ITEM(MIB_TBL_PRV, 18, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsSet0", MIB_ITEM(MIB_TBL_PRV, 17, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsCRCSet0", MIB_ITEM(MIB_TBL_PRV, 16, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsCRCSet0", MIB_ITEM(MIB_TBL_PRV, 15, 1)),
	MIB_LIST_ITEM("LengthFieldError", MIB_ITEM(MIB_TBL_PRV, 14, 1)),
	MIB_LIST_ITEM("FalseCarrierTimes", MIB_ITEM(MIB_TBL_PRV, 13, 1)),
	MIB_LIST_ITEM("UndersizeOctets", MIB_ITEM(MIB_TBL_PRV, 12, 1)),
	MIB_LIST_ITEM("FramingErrors", MIB_ITEM(MIB_TBL_PRV, 11, 1)),
	MIB_LIST_ITEM("rx_MacDiscards", MIB_ITEM(MIB_TBL_PRV, 9, 1)),
	MIB_LIST_ITEM("rx_MacIPGShortDrop", MIB_ITEM(MIB_TBL_PRV, 8, 1))
};

const struct otto_stats_mib_desc otto_stats_931x_mib_desc = {
	.symbol_errors = MIB_ITEM(MIB_TBL_STD, 29, 1),

	.if_in_octets = MIB_ITEM(MIB_TBL_STD, 51, 2),
	.if_out_octets = MIB_ITEM(MIB_TBL_STD, 49, 2),
	.if_in_ucast_pkts = MIB_ITEM(MIB_TBL_STD, 47, 2),
	.if_in_mcast_pkts = MIB_ITEM(MIB_TBL_STD, 45, 2),
	.if_in_bcast_pkts = MIB_ITEM(MIB_TBL_STD, 43, 2),
	.if_out_ucast_pkts = MIB_ITEM(MIB_TBL_STD, 41, 2),
	.if_out_mcast_pkts = MIB_ITEM(MIB_TBL_STD, 39, 2),
	.if_out_bcast_pkts = MIB_ITEM(MIB_TBL_STD, 37, 2),
	.if_out_discards = MIB_ITEM(MIB_TBL_STD, 36, 1),
	.single_collisions = MIB_ITEM(MIB_TBL_STD, 34, 1),
	.multiple_collisions = MIB_ITEM(MIB_TBL_STD, 33, 1),
	.deferred_transmissions = MIB_ITEM(MIB_TBL_STD, 32, 1),
	.late_collisions = MIB_ITEM(MIB_TBL_STD, 31, 1),
	.excessive_collisions = MIB_ITEM(MIB_TBL_STD, 30, 1),
	.crc_align_errors = MIB_ITEM(MIB_TBL_STD, 21, 1),
	.rx_pkts_over_max_octets = MIB_ITEM(MIB_TBL_PRV, 23, 1),

	.unsupported_opcodes = MIB_ITEM(MIB_TBL_STD, 28, 1),

	.rx_undersize_pkts = MIB_ITEM(MIB_TBL_STD, 19, 1),
	.rx_oversize_pkts = MIB_ITEM(MIB_TBL_STD, 17, 1),
	.rx_fragments = MIB_ITEM(MIB_TBL_STD, 15, 1),
	.rx_jabbers = MIB_ITEM(MIB_TBL_STD, 13, 1),

	.tx_pkts = {
		MIB_ITEM(MIB_TBL_STD, 11, 1),
		MIB_ITEM(MIB_TBL_STD, 9, 1),
		MIB_ITEM(MIB_TBL_STD, 7, 1),
		MIB_ITEM(MIB_TBL_STD, 5, 1),
		MIB_ITEM(MIB_TBL_STD, 3, 1),
		MIB_ITEM(MIB_TBL_STD, 1, 1),
		MIB_ITEM(MIB_TBL_PRV, 26, 1),
		MIB_ITEM(MIB_TBL_PRV, 24, 1)
	},
	.rx_pkts = {
		MIB_ITEM(MIB_TBL_STD, 10, 1),
		MIB_ITEM(MIB_TBL_STD, 8, 1),
		MIB_ITEM(MIB_TBL_STD, 6, 1),
		MIB_ITEM(MIB_TBL_STD, 4, 1),
		MIB_ITEM(MIB_TBL_STD, 2, 1),
		MIB_ITEM(MIB_TBL_STD, 0, 1),
		MIB_ITEM(MIB_TBL_PRV, 25, 1),
		MIB_ITEM(MIB_TBL_PRV, 23, 1),
	},
	.rmon_ranges = {
		{ 0, 64 },
		{ 65, 127 },
		{ 128, 255 },
		{ 256, 511 },
		{ 512, 1023 },
		{ 1024, 1518 },
		{ 1519, 12288 },
		{ 12289, 65535 }
	},

	.drop_events = MIB_ITEM(MIB_TBL_STD, 25, 1),
	.collisions = MIB_ITEM(MIB_TBL_STD, 12, 1),

	.rx_pause_frames = MIB_ITEM(MIB_TBL_STD, 27, 1),
	.tx_pause_frames = MIB_ITEM(MIB_TBL_STD, 26, 1),

	.list_count = ARRAY_SIZE(otto_stats_931x_mib_list),
	.list = otto_stats_931x_mib_list
};

void otto_stats_838x_init(struct otto_dsa_priv *priv)
{
	/* Enable statistics module: all counters plus debug */
	sw_w32_mask(0, 3, RTL838X_STAT_CTRL);
}

u64 otto_stats_931x_port_table_read(int port, unsigned int mib_size,
				    unsigned int mib_offset, bool is_pvt)
{
	enum otto_table_id id;
	int field_offset;
	u32 val[2];

	if (is_pvt) {
		id = RTL9310_TBL_STAT_PORT_PRVTE_CNTR;
		field_offset = 27;
	} else {
		id = RTL9310_TBL_STAT_PORT_MIB_CNTR;
		field_offset = 52;
	}

	/* Counter fields are numbered down from the last data word of the
	 * entry, so the high half of a 64 bit counter sits at the lower word
	 * index.
	 */
	if (mib_size == 2) {
		otto_table_offset_read(id, port, &val, field_offset - (mib_offset + 1));

		return (u64)val[0] << 32 | val[1];
	}

	otto_table_offset_read(id, port, &val[0], field_offset - mib_offset);

	return val[0];
}

void otto_stats_init(struct otto_dsa_priv *priv)
{
	mutex_lock(&priv->reg_mutex);

	if (priv->r->stat_init)
		priv->r->stat_init(priv);

	/* Reset statistics counters */
	sw_w32_mask(0, 1, priv->r->stat_rst);

	mutex_unlock(&priv->reg_mutex);
}

static bool otto_stats_read_mib_item(struct otto_dsa_priv *priv, int port,
				     const struct otto_stats_mib_item *mib_item,
				     u64 *data)
{
	u32 high1, high2;
	int reg, reg_offset, addr_low;

	switch (mib_item->reg) {
	case MIB_REG_STD:
		reg = priv->r->stat_port_std_mib;
		reg_offset = 256;
		break;
	case MIB_REG_PRV:
		reg = priv->r->stat_port_prv_mib;
		reg_offset = 128;
		break;
	case MIB_TBL_STD:
	case MIB_TBL_PRV:
		if (!priv->r->stat_port_table_read)
			return false;

		*data = priv->r->stat_port_table_read(port, mib_item->size, mib_item->offset,
						      mib_item->reg == MIB_TBL_PRV);

		return true;
	default:
		return false;
	}

	addr_low = reg + (port + 1) * reg_offset - 4 - mib_item->offset;

	if (mib_item->size == 2) {
		high1 = sw_r32(addr_low - 4);
		*data = sw_r32(addr_low);
		high2 = sw_r32(addr_low - 4);
		if (high1 != high2) {
			/* Low must have wrapped and overflowed into high, read again */
			*data = sw_r32(addr_low);
		}
		*data |= (u64)high2 << 32;
	} else {
		*data = sw_r32(addr_low);
	}

	return true;
}

static void otto_stats_update_counter(struct otto_dsa_priv *priv, int port,
				      struct otto_stats_counter *counter,
				      const struct otto_stats_mib_item *mib_item)
{
	u64 val;
	u32 val32, diff;

	if (!otto_stats_read_mib_item(priv, port, mib_item, &val))
		return;

	if (mib_item->size == 2) {
		counter->val = val;
	} else {
		val32 = (u32)val;
		diff = val32 - counter->last;
		counter->val += diff;
		counter->last = val32;
	}
}

static void otto_stats_update_link(struct rtnl_link_stats64 *s,
				   const struct otto_stats_counter_state *counters)
{
	s->rx_packets = counters->if_in_ucast_pkts.val +
			counters->if_in_mcast_pkts.val +
			counters->if_in_bcast_pkts.val +
			counters->rx_pkts_over_max_octets.val;

	s->tx_packets = counters->if_out_ucast_pkts.val +
			counters->if_out_mcast_pkts.val +
			counters->if_out_bcast_pkts.val -
			counters->if_out_discards.val;

	/* Subtract FCS for each packet, and pause frames */
	s->rx_bytes = counters->if_in_octets.val -
		      4 * s->rx_packets -
		      64 * counters->rx_pause_frames.val;
	s->tx_bytes = counters->if_out_octets.val -
		      4 * s->tx_packets -
		      64 * counters->tx_pause_frames.val;

	s->multicast = counters->if_in_mcast_pkts.val;
	s->collisions = counters->collisions.val;

	s->rx_dropped = counters->drop_events.val;
	s->tx_dropped = counters->if_out_discards.val;

	s->rx_crc_errors = counters->crc_align_errors.val;
	s->rx_errors = s->rx_crc_errors;

	s->tx_aborted_errors = counters->excessive_collisions.val;
	s->tx_window_errors = counters->late_collisions.val;
	s->tx_errors = s->tx_aborted_errors + s->tx_window_errors;
}

static void otto_stats_update_port_counters(struct otto_dsa_priv *priv, int port)
{
	struct otto_stats_counter_state *counters = &priv->ports[port].counters;
	const struct otto_stats_mib_desc *mib_desc;
	ktime_t now;

	mib_desc = priv->r->mib_desc;

	/* Prevent unnecessary updates when the user accesses different stats quickly.
	 * This compensates a bit for always updating all stats, even when just a
	 * subset is actually requested.
	 */
	now = ktime_get();
	if (ktime_before(now, ktime_add_ms(counters->last_update, 100)))
		return;
	counters->last_update = now;

	otto_stats_update_counter(priv, port, &counters->symbol_errors,
				  &mib_desc->symbol_errors);

	otto_stats_update_counter(priv, port, &counters->if_in_octets,
				  &mib_desc->if_in_octets);
	otto_stats_update_counter(priv, port, &counters->if_out_octets,
				  &mib_desc->if_out_octets);
	otto_stats_update_counter(priv, port, &counters->if_in_ucast_pkts,
				  &mib_desc->if_in_ucast_pkts);
	otto_stats_update_counter(priv, port, &counters->if_in_mcast_pkts,
				  &mib_desc->if_in_mcast_pkts);
	otto_stats_update_counter(priv, port, &counters->if_in_bcast_pkts,
				  &mib_desc->if_in_bcast_pkts);
	otto_stats_update_counter(priv, port, &counters->if_out_ucast_pkts,
				  &mib_desc->if_out_ucast_pkts);
	otto_stats_update_counter(priv, port, &counters->if_out_mcast_pkts,
				  &mib_desc->if_out_mcast_pkts);
	otto_stats_update_counter(priv, port, &counters->if_out_bcast_pkts,
				  &mib_desc->if_out_bcast_pkts);
	otto_stats_update_counter(priv, port, &counters->if_out_discards,
				  &mib_desc->if_out_discards);
	otto_stats_update_counter(priv, port, &counters->single_collisions,
				  &mib_desc->single_collisions);
	otto_stats_update_counter(priv, port, &counters->multiple_collisions,
				  &mib_desc->multiple_collisions);
	otto_stats_update_counter(priv, port, &counters->deferred_transmissions,
				  &mib_desc->deferred_transmissions);
	otto_stats_update_counter(priv, port, &counters->late_collisions,
				  &mib_desc->late_collisions);
	otto_stats_update_counter(priv, port, &counters->excessive_collisions,
				  &mib_desc->excessive_collisions);
	otto_stats_update_counter(priv, port, &counters->crc_align_errors,
				  &mib_desc->crc_align_errors);
	otto_stats_update_counter(priv, port, &counters->rx_pkts_over_max_octets,
				  &mib_desc->rx_pkts_over_max_octets);

	otto_stats_update_counter(priv, port, &counters->unsupported_opcodes,
				  &mib_desc->unsupported_opcodes);

	otto_stats_update_counter(priv, port, &counters->rx_undersize_pkts,
				  &mib_desc->rx_undersize_pkts);
	otto_stats_update_counter(priv, port, &counters->rx_oversize_pkts,
				  &mib_desc->rx_oversize_pkts);
	otto_stats_update_counter(priv, port, &counters->rx_fragments,
				  &mib_desc->rx_fragments);
	otto_stats_update_counter(priv, port, &counters->rx_jabbers,
				  &mib_desc->rx_jabbers);

	for (int i = 0; i < ARRAY_SIZE(mib_desc->tx_pkts); i++) {
		if (mib_desc->tx_pkts[i].reg == MIB_REG_INVALID)
			break;

		otto_stats_update_counter(priv, port, &counters->tx_pkts[i],
					  &mib_desc->tx_pkts[i]);
	}
	for (int i = 0; i < ARRAY_SIZE(mib_desc->rx_pkts); i++) {
		if (mib_desc->rx_pkts[i].reg == MIB_REG_INVALID)
			break;

		otto_stats_update_counter(priv, port, &counters->rx_pkts[i],
					  &mib_desc->rx_pkts[i]);
	}

	otto_stats_update_counter(priv, port, &counters->drop_events,
				  &mib_desc->drop_events);
	otto_stats_update_counter(priv, port, &counters->collisions,
				  &mib_desc->collisions);

	otto_stats_update_counter(priv, port, &counters->rx_pause_frames,
				  &mib_desc->rx_pause_frames);
	otto_stats_update_counter(priv, port, &counters->tx_pause_frames,
				  &mib_desc->tx_pause_frames);

	/* prepare get_stats64 reply without requiring caller waiting for mutex */
	spin_lock(&counters->link_stat_lock);
	otto_stats_update_link(&counters->link_stat, counters);
	spin_unlock(&counters->link_stat_lock);
}

void otto_stats_counters_lock_register(struct otto_dsa_priv *priv, int port)
	__acquires(&priv->ports[port].counters.lock)
{
	spin_lock(&priv->ports[port].counters.lock);
}

void otto_stats_counters_unlock_register(struct otto_dsa_priv *priv, int port)
	__releases(&priv->ports[port].counters.lock)
{
	spin_unlock(&priv->ports[port].counters.lock);
}

void otto_stats_counters_lock_table(struct otto_dsa_priv *priv, int port __maybe_unused)
	__acquires(&priv->counters_lock)
{
	mutex_lock(&priv->counters_lock);
}

void otto_stats_counters_unlock_table(struct otto_dsa_priv *priv, int port __maybe_unused)
	__releases(&priv->ports[port].counters.lock)
{
	mutex_unlock(&priv->counters_lock);
}

static void otto_stats_counters_lock(struct otto_dsa_priv *priv, int port)
{
	priv->r->stat_counters_lock(priv, port);
}

static void otto_stats_counters_unlock(struct otto_dsa_priv *priv, int port)
{
	priv->r->stat_counters_unlock(priv, port);
}

static void otto_stats_poll_counters(struct work_struct *work)
{
	struct otto_dsa_priv *priv = container_of(to_delayed_work(work),
							struct otto_dsa_priv,
							counters_work);

	for (int port = 0; port < priv->r->cpu_port; port++) {
		if (!priv->ports[port].phy && !priv->ports[port].has_pcs)
			continue;

		otto_stats_counters_lock(priv, port);
		otto_stats_update_port_counters(priv, port);
		otto_stats_counters_unlock(priv, port);
	}

	queue_delayed_work(priv->wq, &priv->counters_work,
			   priv->r->stat_counter_poll_interval);
}

void otto_stats_init_counters(struct otto_dsa_priv *priv)
{
	struct otto_stats_counter_state *counters;

	for (int port = 0; port < priv->r->cpu_port; port++) {
		if (!priv->ports[port].phy && !priv->ports[port].has_pcs)
			continue;

		counters = &priv->ports[port].counters;

		memset(counters, 0, sizeof(*counters));
		spin_lock_init(&counters->lock);
		spin_lock_init(&counters->link_stat_lock);
	}

	INIT_DELAYED_WORK(&priv->counters_work, otto_stats_poll_counters);
	queue_delayed_work(priv->wq, &priv->counters_work,
			   priv->r->stat_counter_poll_interval);
}

void otto_stats_get_strings(struct dsa_switch *ds, int port, u32 stringset, u8 *data)
{
	struct otto_dsa_priv *priv = ds->priv;
	const struct otto_stats_mib_desc *mib_desc;

	if (stringset != ETH_SS_STATS)
		return;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	mib_desc = priv->r->mib_desc;

	for (int i = 0; i < mib_desc->list_count; i++)
		ethtool_puts(&data, mib_desc->list[i].name);
}

void otto_stats_get_ethtool(struct dsa_switch *ds, int port,
			    u64 *data)
{
	struct otto_dsa_priv *priv = ds->priv;
	const struct otto_stats_mib_desc *mib_desc;
	const struct otto_stats_mib_item *mib_item;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	mib_desc = priv->r->mib_desc;
	for (int i = 0; i < mib_desc->list_count; i++) {
		mib_item = &mib_desc->list[i].item;
		otto_stats_read_mib_item(priv, port, mib_item, &data[i]);
	}
}

int otto_stats_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	struct otto_dsa_priv *priv = ds->priv;

	if (sset != ETH_SS_STATS)
		return 0;

	if (port < 0 || port >= priv->r->cpu_port)
		return 0;

	return priv->r->mib_desc->list_count;
}

void otto_stats_get_eth_phy(struct dsa_switch *ds, int port,
			    struct ethtool_eth_phy_stats *phy_stats)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct otto_stats_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	otto_stats_counters_lock(priv, port);

	otto_stats_update_port_counters(priv, port);

	phy_stats->SymbolErrorDuringCarrier = counters->symbol_errors.val;

	otto_stats_counters_unlock(priv, port);
}

void otto_stats_get_eth_mac(struct dsa_switch *ds, int port,
			    struct ethtool_eth_mac_stats *mac_stats)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct otto_stats_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	otto_stats_counters_lock(priv, port);

	otto_stats_update_port_counters(priv, port);

	/* Frame and octet counters are calculated based on RFC3635, while also
	 * taking into account that the behaviour of the hardware counters differs
	 * in some places.
	 */

	mac_stats->FramesReceivedOK = counters->if_in_ucast_pkts.val +
				      counters->if_in_mcast_pkts.val +
				      counters->if_in_bcast_pkts.val +
				      counters->rx_pause_frames.val +
				      counters->rx_pkts_over_max_octets.val;

	mac_stats->FramesTransmittedOK = counters->if_out_ucast_pkts.val +
					 counters->if_out_mcast_pkts.val +
					 counters->if_out_bcast_pkts.val +
					 counters->tx_pause_frames.val -
					 counters->if_out_discards.val;

	mac_stats->OctetsReceivedOK = counters->if_in_octets.val -
				      18 * mac_stats->FramesReceivedOK;
	mac_stats->OctetsTransmittedOK = counters->if_out_octets.val -
					 18 * mac_stats->FramesTransmittedOK;

	mac_stats->MulticastFramesXmittedOK = counters->if_out_mcast_pkts.val;
	mac_stats->BroadcastFramesXmittedOK = counters->if_out_bcast_pkts.val;
	mac_stats->MulticastFramesReceivedOK = counters->if_in_mcast_pkts.val;
	mac_stats->BroadcastFramesReceivedOK = counters->if_in_bcast_pkts.val;

	mac_stats->SingleCollisionFrames = counters->single_collisions.val;
	mac_stats->MultipleCollisionFrames = counters->multiple_collisions.val;
	mac_stats->FramesWithDeferredXmissions = counters->deferred_transmissions.val;
	mac_stats->LateCollisions = counters->late_collisions.val;
	mac_stats->FramesAbortedDueToXSColls = counters->excessive_collisions.val;

	mac_stats->FrameCheckSequenceErrors = counters->crc_align_errors.val;

	otto_stats_counters_unlock(priv, port);
}

void otto_stats_get_eth_ctrl(struct dsa_switch *ds, int port,
			     struct ethtool_eth_ctrl_stats *ctrl_stats)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct otto_stats_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	otto_stats_counters_lock(priv, port);

	otto_stats_update_port_counters(priv, port);

	ctrl_stats->UnsupportedOpcodesReceived = counters->unsupported_opcodes.val;

	otto_stats_counters_unlock(priv, port);
}

void otto_stats_get_rmon(struct dsa_switch *ds, int port,
			 struct ethtool_rmon_stats *rmon_stats,
			 const struct ethtool_rmon_hist_range **ranges)
{
	struct otto_dsa_priv *priv = ds->priv;
	const struct otto_stats_mib_desc *mib_desc;
	struct otto_stats_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	mib_desc = priv->r->mib_desc;

	otto_stats_counters_lock(priv, port);

	otto_stats_update_port_counters(priv, port);

	rmon_stats->undersize_pkts = counters->rx_undersize_pkts.val;
	rmon_stats->oversize_pkts = counters->rx_oversize_pkts.val;
	rmon_stats->fragments = counters->rx_fragments.val;
	rmon_stats->jabbers = counters->rx_jabbers.val;

	for (int i = 0; i < ARRAY_SIZE(mib_desc->rx_pkts); i++) {
		if (mib_desc->rx_pkts[i].reg == MIB_REG_INVALID)
			break;

		rmon_stats->hist[i] = counters->rx_pkts[i].val;
	}

	for (int i = 0; i < ARRAY_SIZE(mib_desc->tx_pkts); i++) {
		if (mib_desc->tx_pkts[i].reg == MIB_REG_INVALID)
			break;

		rmon_stats->hist_tx[i] = counters->tx_pkts[i].val;
	}

	*ranges = mib_desc->rmon_ranges;

	otto_stats_counters_unlock(priv, port);
}

void otto_stats_update_counters_atomically(struct otto_dsa_priv *priv, int port)
{
	otto_stats_counters_lock(priv, port);
	otto_stats_update_port_counters(priv, port);
	otto_stats_counters_unlock(priv, port);
}

void otto_stats_get64(struct dsa_switch *ds, int port,
		      struct rtnl_link_stats64 *s)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct otto_stats_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	if (priv->r->stat_update_counters_atomically)
		priv->r->stat_update_counters_atomically(priv, port);

	/* retrieve prepared return data without potentially sleeping via mutex */
	spin_lock(&counters->link_stat_lock);
	memcpy(s, &counters->link_stat, sizeof(*s));
	spin_unlock(&counters->link_stat_lock);
}

void otto_stats_get_pause(struct dsa_switch *ds, int port,
			  struct ethtool_pause_stats *pause_stats)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct otto_stats_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	otto_stats_counters_lock(priv, port);

	otto_stats_update_port_counters(priv, port);

	pause_stats->tx_pause_frames = counters->tx_pause_frames.val;
	pause_stats->rx_pause_frames = counters->rx_pause_frames.val;

	otto_stats_counters_unlock(priv, port);
}

/*
 * Packet counters share hardware memory with octet counters (2 packet counters
 * per 1 octet block). Allocation relies on two complementary bitmaps:
 *
 *   octet_cntr_use_bm:  0 = free block, 1 = used (or split into packet counters)
 *   packet_cntr_use_bm: 1 = free standalone counter, 0 = unavailable
 *
 * Allocation strategy:
 * 1. Look for a free standalone counter from an already split block (bit = 1).
 * 2. If none are free, claim a new octet block 'j', use counter index (2 * j),
 *    and mark counter (2 * j + 1) as available for future allocations.
 */

/**
 * otto_stats_packet_cntr_alloc - Allocate a hardware packet counter.
 * @priv: Switch driver private structure.
 *
 * Return: Counter index (>= 0) on success, or -1 if full.
 */
int otto_stats_packet_cntr_alloc(struct otto_dsa_priv *priv)
{
	int idx, j, base = 0;

	/* On SoCs where a PIE rule implicitly logs into the LOG table entry
	 * with its own rule ID (RTL930x), the tc cls_flower offload consumes
	 * one LOG counter per PIE rule and does not pass through this
	 * allocator. Keep that range reserved so a route counter handed out
	 * here cannot alias a flow's LOG entry.
	 *
	 * The offload is ingress-only, so PIE rule IDs currently populate
	 * only the lower half of that range, and a route counter's LOG entry
	 * (idx / 2) never dips below the upper half either. If egress PIE
	 * rules are ever wired up, this reservation needs to grow so their
	 * LOG entries stay clear of route counters too.
	 */
	if (priv->r->pie_rule_id_is_log_counter)
		base = priv->r->n_pie_blocks * PIE_BLOCK_SIZE;

	scoped_guard(mutex, &priv->reg_mutex) {
		idx = find_next_bit(priv->packet_cntr_use_bm, priv->r->n_counters * 2, base);
		if (idx >= priv->r->n_counters * 2) {
			j = find_next_zero_bit(priv->octet_cntr_use_bm, priv->r->n_counters,
					       base / 2);
			if (j >= priv->r->n_counters)
				return -1;

			__set_bit(j, priv->octet_cntr_use_bm);
			idx = j * 2;
			__set_bit(j * 2 + 1, priv->packet_cntr_use_bm);
		} else {
			__clear_bit(idx, priv->packet_cntr_use_bm);
		}
	}

	return idx;
}

/**
 * otto_stats_packet_cntr_free - Release a packet counter from otto_stats_packet_cntr_alloc().
 * @priv: Switch driver private structure.
 * @idx: Packet counter index to free; a negative id is ignored.
 *
 * Marks the counter free again and, once both halves of its octet block are
 * free, returns the whole block to the octet counter pool.
 */
void otto_stats_packet_cntr_free(struct otto_dsa_priv *priv, int idx)
{
	int j;

	if (idx < 0 || idx >= priv->r->n_counters * 2)
		return;

	scoped_guard(mutex, &priv->reg_mutex) {
		/* already free - guard against a double release */
		if (test_bit(idx, priv->packet_cntr_use_bm))
			return;

		__set_bit(idx, priv->packet_cntr_use_bm);

		j = idx / 2;
		if (test_bit(j, priv->octet_cntr_use_bm) &&
		    test_bit(idx ^ 1, priv->packet_cntr_use_bm)) {
			__clear_bit(idx, priv->packet_cntr_use_bm);
			__clear_bit(idx ^ 1, priv->packet_cntr_use_bm);
			__clear_bit(j, priv->octet_cntr_use_bm);
		}
	}
}

u32 otto_stats_838x_packet_cntr_read(struct otto_dsa_priv *priv, int counter)
{
	u32 buf[2];
	u32 v;

	dev_dbg(priv->dev, "reading LOG packet counter %d\n", counter);
	otto_table_read(RTL8380_TBL_LOG, counter / 2, &buf);

	dev_dbg(priv->dev, "LOG entry: %08x %08x\n", buf[0], buf[1]);
	if (counter % 2)
		v = buf[0];
	else
		v = buf[1];

	return v;
}

void otto_stats_838x_packet_cntr_clear(struct otto_dsa_priv *priv, int counter)
{
	int tbl = otto_table_acquire(RTL8380_TBL_LOG);
	u32 buf[2];

	dev_dbg(priv->dev, "clearing LOG packet counter %d\n", counter);

	/*
	 * Two counters share one LOG table entry. Read the current entry
	 * first so clearing one half preserves the adjacent counter.
	 */
	__otto_table_read(tbl, counter / 2, &buf);

	if (counter % 2)
		buf[0] = 0;
	else
		buf[1] = 0;

	__otto_table_write(tbl, counter / 2, &buf);

	otto_table_release(tbl);
}

u32 otto_stats_839x_packet_cntr_read(struct otto_dsa_priv *priv, int counter)
{
	u32 buf[2];
	u32 v;

	dev_dbg(priv->dev, "reading LOG packet counter %d\n", counter);
	otto_table_read(RTL8390_TBL_LOG, counter / 2, &buf);

	if (counter % 2)
		v = buf[0];
	else
		v = buf[1];

	return v;
}

void otto_stats_839x_packet_cntr_clear(struct otto_dsa_priv *priv, int counter)
{
	u32 buf[2] = {};

	dev_dbg(priv->dev, "clearing LOG packet counter %d\n", counter);

	/* A LOG entry holds two counters and takes no data: a write to the
	 * clear address of one half zeroes that half and leaves the other.
	 */
	otto_table_write(counter % 2 ? RTL8390_TBL_LOG_CLRH : RTL8390_TBL_LOG_CLRL,
			 counter / 2, &buf);
}

/* A PIE rule logs its matched packets into the LOG table entry that carries
 * its own rule ID, in data word 1 - one entry per rule. Counters handed out
 * by otto_stats_packet_cntr_alloc() for L3 route statistics start above the PIE
 * rule ID range and pack two 32-bit counters into one 64-bit LOG entry.
 */
#define RTL930X_PIE_RULE_IDS	(rtldsa_930x_cfg.n_pie_blocks * PIE_BLOCK_SIZE)

u32 otto_stats_930x_packet_cntr_read(struct otto_dsa_priv *priv, int counter)
{
	u32 buf[2];
	u32 v;

	dev_dbg(priv->dev, "reading LOG packet counter %d\n", counter);

	if (counter < RTL930X_PIE_RULE_IDS) {
		otto_table_read(RTL9300_TBL_LOG, counter, &buf);
		v = buf[1];
	} else {
		/* Two counters share one entry, so the parity of counter picks
		 * which half of it to return.
		 */
		otto_table_read(RTL9300_TBL_LOG, counter / 2, &buf);
		v = counter % 2 ? buf[0] : buf[1];
	}

	dev_dbg(priv->dev, "LOG entry: %08x %08x\n", buf[0], buf[1]);

	return v;
}

void otto_stats_930x_packet_cntr_clear(struct otto_dsa_priv *priv, int counter)
{
	int tbl = otto_table_acquire(RTL9300_TBL_LOG);

	u32 v[2];

	dev_dbg(priv->dev, "clearing LOG packet counter %d\n", counter);

	/* Read-modify-write: for a route counter the other 32-bit counter in
	 * the same 64-bit entry must be preserved; for a PIE rule ID the entry
	 * is ours alone but the RMW is harmless.
	 */
	if (counter < RTL930X_PIE_RULE_IDS) {
		__otto_table_read(tbl, counter, &v);
		v[1] = 0;
		__otto_table_write(tbl, counter, &v);
	} else {
		__otto_table_read(tbl, counter / 2, &v);
		v[counter % 2 ? 0 : 1] = 0;
		__otto_table_write(tbl, counter / 2, &v);
	}

	otto_table_release(tbl);
}
