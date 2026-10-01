// SPDX-License-Identifier: GPL-2.0-only

#include <net/dsa.h>

#include "rtl-otto.h"
#include "stats.h"

#define MIB_ITEM(_reg, _offset, _size) \
		{.reg = _reg, .offset = _offset, .size = _size}

#define MIB_LIST_ITEM(_name, _item) \
		{.name = _name, .item = _item}

static const struct rtldsa_mib_list_item rtldsa_838x_mib_list[] = {
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

const struct rtldsa_mib_desc rtldsa_838x_mib_desc = {
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

	.list_count = ARRAY_SIZE(rtldsa_838x_mib_list),
	.list = rtldsa_838x_mib_list
};

static const struct rtldsa_mib_list_item rtldsa_839x_mib_list[] = {
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

const struct rtldsa_mib_desc rtldsa_839x_mib_desc = {
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

	.list_count = ARRAY_SIZE(rtldsa_839x_mib_list),
	.list = rtldsa_839x_mib_list
};

static const struct rtldsa_mib_list_item rtldsa_930x_mib_list[] = {
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

const struct rtldsa_mib_desc rtldsa_930x_mib_desc = {
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

	.list_count = ARRAY_SIZE(rtldsa_930x_mib_list),
	.list = rtldsa_930x_mib_list
};

static const struct rtldsa_mib_list_item rtldsa_931x_mib_list[] = {
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

const struct rtldsa_mib_desc rtldsa_931x_mib_desc = {
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

	.list_count = ARRAY_SIZE(rtldsa_931x_mib_list),
	.list = rtldsa_931x_mib_list
};

void rtldsa_stats_init(struct rtl838x_switch_priv *priv)
{
	mutex_lock(&priv->reg_mutex);

	if (priv->r->stat_init)
		priv->r->stat_init(priv);

	/* Reset statistics counters */
	sw_w32_mask(0, 1, priv->r->stat_rst);

	mutex_unlock(&priv->reg_mutex);
}

static bool rtldsa_read_mib_item(struct rtl838x_switch_priv *priv, int port,
				 const struct rtldsa_mib_item *mib_item,
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

static void rtldsa_update_counter(struct rtl838x_switch_priv *priv, int port,
				  struct rtldsa_counter *counter,
				  const struct rtldsa_mib_item *mib_item)
{
	u64 val;
	u32 val32, diff;

	if (!rtldsa_read_mib_item(priv, port, mib_item, &val))
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

static void rtldsa_update_link_stat(struct rtnl_link_stats64 *s,
				    const struct rtldsa_counter_state *counters)
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

static void rtldsa_update_port_counters(struct rtl838x_switch_priv *priv, int port)
{
	struct rtldsa_counter_state *counters = &priv->ports[port].counters;
	const struct rtldsa_mib_desc *mib_desc;
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

	rtldsa_update_counter(priv, port, &counters->symbol_errors,
			      &mib_desc->symbol_errors);

	rtldsa_update_counter(priv, port, &counters->if_in_octets,
			      &mib_desc->if_in_octets);
	rtldsa_update_counter(priv, port, &counters->if_out_octets,
			      &mib_desc->if_out_octets);
	rtldsa_update_counter(priv, port, &counters->if_in_ucast_pkts,
			      &mib_desc->if_in_ucast_pkts);
	rtldsa_update_counter(priv, port, &counters->if_in_mcast_pkts,
			      &mib_desc->if_in_mcast_pkts);
	rtldsa_update_counter(priv, port, &counters->if_in_bcast_pkts,
			      &mib_desc->if_in_bcast_pkts);
	rtldsa_update_counter(priv, port, &counters->if_out_ucast_pkts,
			      &mib_desc->if_out_ucast_pkts);
	rtldsa_update_counter(priv, port, &counters->if_out_mcast_pkts,
			      &mib_desc->if_out_mcast_pkts);
	rtldsa_update_counter(priv, port, &counters->if_out_bcast_pkts,
			      &mib_desc->if_out_bcast_pkts);
	rtldsa_update_counter(priv, port, &counters->if_out_discards,
			      &mib_desc->if_out_discards);
	rtldsa_update_counter(priv, port, &counters->single_collisions,
			      &mib_desc->single_collisions);
	rtldsa_update_counter(priv, port, &counters->multiple_collisions,
			      &mib_desc->multiple_collisions);
	rtldsa_update_counter(priv, port, &counters->deferred_transmissions,
			      &mib_desc->deferred_transmissions);
	rtldsa_update_counter(priv, port, &counters->late_collisions,
			      &mib_desc->late_collisions);
	rtldsa_update_counter(priv, port, &counters->excessive_collisions,
			      &mib_desc->excessive_collisions);
	rtldsa_update_counter(priv, port, &counters->crc_align_errors,
			      &mib_desc->crc_align_errors);
	rtldsa_update_counter(priv, port, &counters->rx_pkts_over_max_octets,
			      &mib_desc->rx_pkts_over_max_octets);

	rtldsa_update_counter(priv, port, &counters->unsupported_opcodes,
			      &mib_desc->unsupported_opcodes);

	rtldsa_update_counter(priv, port, &counters->rx_undersize_pkts,
			      &mib_desc->rx_undersize_pkts);
	rtldsa_update_counter(priv, port, &counters->rx_oversize_pkts,
			      &mib_desc->rx_oversize_pkts);
	rtldsa_update_counter(priv, port, &counters->rx_fragments,
			      &mib_desc->rx_fragments);
	rtldsa_update_counter(priv, port, &counters->rx_jabbers,
			      &mib_desc->rx_jabbers);

	for (int i = 0; i < ARRAY_SIZE(mib_desc->tx_pkts); i++) {
		if (mib_desc->tx_pkts[i].reg == MIB_REG_INVALID)
			break;

		rtldsa_update_counter(priv, port, &counters->tx_pkts[i],
				      &mib_desc->tx_pkts[i]);
	}
	for (int i = 0; i < ARRAY_SIZE(mib_desc->rx_pkts); i++) {
		if (mib_desc->rx_pkts[i].reg == MIB_REG_INVALID)
			break;

		rtldsa_update_counter(priv, port, &counters->rx_pkts[i],
				      &mib_desc->rx_pkts[i]);
	}

	rtldsa_update_counter(priv, port, &counters->drop_events,
			      &mib_desc->drop_events);
	rtldsa_update_counter(priv, port, &counters->collisions,
			      &mib_desc->collisions);

	rtldsa_update_counter(priv, port, &counters->rx_pause_frames,
			      &mib_desc->rx_pause_frames);
	rtldsa_update_counter(priv, port, &counters->tx_pause_frames,
			      &mib_desc->tx_pause_frames);

	/* prepare get_stats64 reply without requiring caller waiting for mutex */
	spin_lock(&counters->link_stat_lock);
	rtldsa_update_link_stat(&counters->link_stat, counters);
	spin_unlock(&counters->link_stat_lock);
}

void rtldsa_counters_lock_register(struct rtl838x_switch_priv *priv, int port)
	__acquires(&priv->ports[port].counters.lock)
{
	spin_lock(&priv->ports[port].counters.lock);
}

void rtldsa_counters_unlock_register(struct rtl838x_switch_priv *priv, int port)
	__releases(&priv->ports[port].counters.lock)
{
	spin_unlock(&priv->ports[port].counters.lock);
}

void rtldsa_counters_lock_table(struct rtl838x_switch_priv *priv, int port __maybe_unused)
	__acquires(&priv->counters_lock)
{
	mutex_lock(&priv->counters_lock);
}

void rtldsa_counters_unlock_table(struct rtl838x_switch_priv *priv, int port __maybe_unused)
	__releases(&priv->ports[port].counters.lock)
{
	mutex_unlock(&priv->counters_lock);
}

static void rtldsa_counters_lock(struct rtl838x_switch_priv *priv, int port)
{
	priv->r->stat_counters_lock(priv, port);
}

static void rtldsa_counters_unlock(struct rtl838x_switch_priv *priv, int port)
{
	priv->r->stat_counters_unlock(priv, port);
}

static void rtldsa_poll_counters(struct work_struct *work)
{
	struct rtl838x_switch_priv *priv = container_of(to_delayed_work(work),
							struct rtl838x_switch_priv,
							counters_work);

	for (int port = 0; port < priv->r->cpu_port; port++) {
		if (!priv->ports[port].phy && !priv->ports[port].has_pcs)
			continue;

		rtldsa_counters_lock(priv, port);
		rtldsa_update_port_counters(priv, port);
		rtldsa_counters_unlock(priv, port);
	}

	queue_delayed_work(priv->wq, &priv->counters_work,
			   priv->r->stat_counter_poll_interval);
}

void rtldsa_stats_init_counters(struct rtl838x_switch_priv *priv)
{
	struct rtldsa_counter_state *counters;

	for (int port = 0; port < priv->r->cpu_port; port++) {
		if (!priv->ports[port].phy && !priv->ports[port].has_pcs)
			continue;

		counters = &priv->ports[port].counters;

		memset(counters, 0, sizeof(*counters));
		spin_lock_init(&counters->lock);
		spin_lock_init(&counters->link_stat_lock);
	}

	INIT_DELAYED_WORK(&priv->counters_work, rtldsa_poll_counters);
	queue_delayed_work(priv->wq, &priv->counters_work,
			   priv->r->stat_counter_poll_interval);
}

void rtldsa_get_strings(struct dsa_switch *ds, int port, u32 stringset, u8 *data)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	const struct rtldsa_mib_desc *mib_desc;

	if (stringset != ETH_SS_STATS)
		return;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	mib_desc = priv->r->mib_desc;

	for (int i = 0; i < mib_desc->list_count; i++)
		ethtool_puts(&data, mib_desc->list[i].name);
}

void rtldsa_get_ethtool_stats(struct dsa_switch *ds, int port,
				     u64 *data)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	const struct rtldsa_mib_desc *mib_desc;
	const struct rtldsa_mib_item *mib_item;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	mib_desc = priv->r->mib_desc;
	for (int i = 0; i < mib_desc->list_count; i++) {
		mib_item = &mib_desc->list[i].item;
		rtldsa_read_mib_item(priv, port, mib_item, &data[i]);
	}
}

int rtldsa_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	if (sset != ETH_SS_STATS)
		return 0;

	if (port < 0 || port >= priv->r->cpu_port)
		return 0;

	return priv->r->mib_desc->list_count;
}

void rtldsa_get_eth_phy_stats(struct dsa_switch *ds, int port,
				     struct ethtool_eth_phy_stats *phy_stats)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	struct rtldsa_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	rtldsa_counters_lock(priv, port);

	rtldsa_update_port_counters(priv, port);

	phy_stats->SymbolErrorDuringCarrier = counters->symbol_errors.val;

	rtldsa_counters_unlock(priv, port);
}

void rtldsa_get_eth_mac_stats(struct dsa_switch *ds, int port,
				     struct ethtool_eth_mac_stats *mac_stats)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	struct rtldsa_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	rtldsa_counters_lock(priv, port);

	rtldsa_update_port_counters(priv, port);

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

	rtldsa_counters_unlock(priv, port);
}

void rtldsa_get_eth_ctrl_stats(struct dsa_switch *ds, int port,
				      struct ethtool_eth_ctrl_stats *ctrl_stats)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	struct rtldsa_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	rtldsa_counters_lock(priv, port);

	rtldsa_update_port_counters(priv, port);

	ctrl_stats->UnsupportedOpcodesReceived = counters->unsupported_opcodes.val;

	rtldsa_counters_unlock(priv, port);
}

void rtldsa_get_rmon_stats(struct dsa_switch *ds, int port,
				  struct ethtool_rmon_stats *rmon_stats,
				  const struct ethtool_rmon_hist_range **ranges)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	const struct rtldsa_mib_desc *mib_desc;
	struct rtldsa_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	mib_desc = priv->r->mib_desc;

	rtldsa_counters_lock(priv, port);

	rtldsa_update_port_counters(priv, port);

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

	rtldsa_counters_unlock(priv, port);
}

void rtldsa_update_counters_atomically(struct rtl838x_switch_priv *priv, int port)
{
	rtldsa_counters_lock(priv, port);
	rtldsa_update_port_counters(priv, port);
	rtldsa_counters_unlock(priv, port);
}

void rtldsa_get_stats64(struct dsa_switch *ds, int port,
			       struct rtnl_link_stats64 *s)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	struct rtldsa_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	if (priv->r->stat_update_counters_atomically)
		priv->r->stat_update_counters_atomically(priv, port);

	/* retrieve prepared return data without potentially sleeping via mutex */
	spin_lock(&counters->link_stat_lock);
	memcpy(s, &counters->link_stat, sizeof(*s));
	spin_unlock(&counters->link_stat_lock);
}

void rtldsa_get_pause_stats(struct dsa_switch *ds, int port,
				   struct ethtool_pause_stats *pause_stats)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	struct rtldsa_counter_state *counters = &priv->ports[port].counters;

	if (port < 0 || port >= priv->r->cpu_port)
		return;

	rtldsa_counters_lock(priv, port);

	rtldsa_update_port_counters(priv, port);

	pause_stats->tx_pause_frames = counters->tx_pause_frames.val;
	pause_stats->rx_pause_frames = counters->rx_pause_frames.val;

	rtldsa_counters_unlock(priv, port);
}
