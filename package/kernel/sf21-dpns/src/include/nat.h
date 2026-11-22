#ifndef DPNS_NAT_H
#define DPNS_NAT_H

#include <net/flow_offload.h>
#include "hw.h"

#define	SA_CML		0x2
#define	DA_CML		0x2
#define NAT_ILKP_SZ	(4 * SZ_1K)
#define ELKP_SUB_TB(_n)	(1 << _n)
#define ELKP_OFFSET(size, sub_tb)	(size / 64 / sub_tb)
#define NPU_NAT_SUB_TB		8
#define NPU_HNAT_SIZE		65536	/* limited by visit table size */
#define NPU_HNAT_ILKP_SIZE	8192

#define NPU_HNAT_VISIT_SIZE 32768
#define NPU_HNAT_INAPT_MAXID 8192

#define NPU_NAT_MPP_CFG			0x0
#define NAT_MPP_CFG_BYPASS		BIT(24)
#define NAT_MPP_CFG0_RESP_FIFO_AFULL_THRESH	GENMASK(20, 16)
#define NAT_MPP_CFG0_VOQ_1_FIFO_AFULL_THRESH	GENMASK(14, 10)
#define NAT_MPP_CFG0_VOQ_0_FIFO_AFULL_THRESH	GENMASK(9, 5)
#define NAT_MPP_CFG0_FP_FIFO_AFULL_THRESH	GENMASK(4, 0)

#define NPU_NAT_PKT_SEARCH_MASK		0x4

#define NPU_NAT_PRIVATE_IPV4_INFO0	0x8
#define NPU_NAT_PRIVATE_IPV4_INFO(_n)	\
	(NPU_NAT_PRIVATE_IPV4_INFO0 + (_n) * 4)

/* hint: use readq to get all 8 lengths */
#define NPU_NAT_IPV4_MASK_LEN0123	0x28
#define NPU_NAT_IPV4_MASK_LEN4567	0x2c

#define NPU_NAT_IPV4_MASK_LEN_EN	BIT(6)

#define NPU_NAT_PRIVATE_IPV6_INFO0	0x30
#define NPU_NAT_PRIVATE_IPV6_INFO(_n)	\
	(NPU_NAT_PRIVATE_IPV6_INFO0 + (_n) * 0x10)

#define NPU_NAT_PRIVATE_IPV6_INFO1_BASE	0x34
#define NPU_NAT_PRIVATE_IPV6_INFO1(_n)	\
	(NPU_NAT_PRIVATE_IPV6_INFO1_BASE + (_n) * 0x10)
#define NPU_NAT_PRIVATE_IPV6_INFO1_VALID BIT(31)
#define NPU_NAT_RELAY_MAC_47_32 	GENMASK(15, 0)

#define NPU_NAT_IPV6_MASK_LEN01		0xb0
#define NPU_NAT_IPV6_MASK_LEN23		0xb4
#define NPU_NAT_IPV6_MASK_LEN45		0xb8

#define NPU_NAT_IPV6_MASK_LEN_EN	BIT(8)

/* use __raw_readw/writew */
#define NPU_NAT_IPV6_MASK_LEN(_n)	\
	(NPU_NAT_IPV6_MASK_LEN01 + (_n) * 2)

#define NPU_NAT_IPV6_MASK_LEN67_HOST_MODE	0xbc
#define NPU_V6LF_MODE_SET		BIT(29)
#define NPU_V4LF_MODE_SET		BIT(28)
#define NPU_SNAT_DNAT_MODE_SEL          BIT(27)
#define NPU_TCP_HNAT_MODE               GENMASK(26, 24)
#define NPU_UDP_HNAT_MODE		GENMASK(21, 19)
#define NPU_NAT_HOST_MODE_TTL1_TO_CPU	BIT(18)
#define NAT_IPV6_MASK_LEN7		GENMASK(17, 9)
#define NAT_IPV6_MASK_LEN6		GENMASK(8, 0)

enum npu_hnat_mode {
	NPU_HNAT_MODE_BASIC,
	NPU_HNAT_MODE_SYMMETRIC,
	NPU_HNAT_MODE_FULLCONE,
	NPU_HNAT_MODE_HOST_RESTRICTED,
	NPU_HNAT_MODE_PORT_RESTRICTED,
};

#define NPU_EVLAN_LKP_CFG	0x14000
#define EVLAN_TPID_SEL		BIT(18)

#define NPU_L3_MPP_CFG		0x20000
#define L3_CARE_NAT_RESULT	BIT(19)
#define L3_MPP_CFG_BYPASS	BIT(16)

#define SE_TB_CLR		0x180004

#define SE_CONFIG0		0x180008
#define CONFIG0_PORTBV_EN	BIT(9)
#define CONFIG0_IPORT_EN	BIT(8)

#define SE_CONFIG2		0x180010
#define CONFIG2_EVLAN_ACT_TABLE_EN	BIT(31)
#define CONFIG2_EVLAN_OTPID_TABLE_EN	BIT(15)
#define CONFIG2_INTF_TABLE_EN	BIT(7)
#define CONFIG2_MAC_TABLE_EN	BIT(6)

#define WAN_TB_DATA(_n)		(0x1800c0 + (_n) * 4)
#define WAN0_VLD		BIT(17)
#define WAN0_VLAN_ID		GENMASK(16, 5)
#define WAN0_IPORT		GENMASK(4, 0)

#define SE_NAT_CONFIG0		0x188004
#define NAT_CONFIG0_DNAT_SUB_TB_VALID	GENMASK(31, 24)
#define NAT_CONFIG0_SNAT_SUB_TB_VALID	GENMASK(23, 16)
#define NAT_CONFIG0_ELKP_V6_ACS_TIMES	GENMASK(14, 13)
#define NAT_CONFIG0_ELKP_V4_ACS_TIMES	GENMASK(12, 11)
#define NAT_CONFIG0_ELKP_TABLE_SIZE	GENMASK(10, 9)
#define NAT_CONFIG0_ELKP_VST_CLR_AFTER_VST	BIT(8)
#define NAT_CONFIG0_DIS_DDR_LKP	BIT(6)
#define NAT_CONFIG0_UDP_HNAT_MODE		GENMASK(2, 0)

#define SE_NAT_CONFIG1		0x188008
#define NAT_CONFIG1_V6LF_EN		BIT(31)
#define NAT_CONFIG1_V4LF_EN		BIT(30)
#define NAT_CONFIG1_V4LF_MODE		GENMASK(29, 27)
#define NAT_CONFIG1_TCP_HNAT_MODE	GENMASK(26, 24)
#define NAT_HASH_POLY_SEL7		GENMASK(23, 21)
#define NAT_HASH_POLY_SEL6		GENMASK(20, 18)
#define NAT_HASH_POLY_SEL5		GENMASK(17, 15)
#define NAT_HASH_POLY_SEL4		GENMASK(14, 12)
#define NAT_HASH_POLY_SEL3		GENMASK(11, 9)
#define NAT_HASH_POLY_SEL2		GENMASK(8, 6)
#define NAT_HASH_POLY_SEL1		GENMASK(5, 3)
#define NAT_HASH_POLY_SEL0		GENMASK(2, 0)

#define SE_NAT_CONFIG2		0x18800c
#define SE_NAT_DNAT_BASE_ADDR	0x188010
#define SE_NAT_SNAT_BASE_ADDR	0x188014

#define SE_NAT_CONFIG5		0x188018
#define NAT_CONFIG5_RELAY_MODE_EN	BIT(28)
#define NAT_SPL_CMPT_LEN		GENMASK(26, 21)
#define NAT_CONFIG5_V6_L2OFFLOAD_MODE	GENMASK(20, 18)
#define NAT_CONFIG5_V4_L2OFFLOAD_MODE	GENMASK(17, 15)
#define NAT_CONFIG5_V6LF_MODE	GENMASK(14, 12)
#define NAT_CONFIG5_MIB_MODE	GENMASK(11, 8)
#define NAT_CONFIG5_SPL_ZERO_LIMIT	BIT(5)
#define NAT_CONFIG5_SPL_CNT_MODE	GENMASK(4,3)
#define NAT_CONFIG5_SPL_MODE	BIT(2)
#define NAT_CONFIG5_SPL_SOURCE	GENMASK(1,0)
#define NPU_CFG_MODE_0 0x3400c
#define CFG_RELAY_MODE BIT(0)

#define SE_NAT_CONFIG6		0x18801c

enum nat_spl_cnt_mode {
	SPL_BYTE,
	SPL_PKT,
};

enum nat_spl_source {
	SEPARATE_SNAT_DNAT,
	ONLY_DNAT,
	ONLY_SNAT,
	UNIFORM_SNAT_DNAT,
};

#define SE_NAT_CLR		0x188024

#define SE_NAT_KEY_RAM_DATA0	0x188400

enum elkp_table_size {
	ELKP_TABLE_SIZE_512K,
	ELKP_TABLE_SIZE_1M,
	ELKP_TABLE_SIZE_2M,
	ELKP_TABLE_SIZE_4M,
};

#define SE_NAT_LKP_REQ		0x188038

#define SE_NAT_TB_OP		0x18803c
#define NAT_TB_OP_BUSY		BIT(31)
#define NAT_TB_OP_WR		BIT(24)
#define NAT_TB_OP_REQ_ID	GENMASK(19, 16)
#define NAT_TB_OP_REQ_ADDR	GENMASK(12, 0)

enum se_nat_tb_op_req_id {
	DNAT_HASH0_TABLE,
	DNAT_HASH1_TABLE,
	SNAT_HASH0_TABLE,
	SNAT_HASH1_TABLE,
	NAPT0_TABLE,
	NAPT1_TABLE,
	NAPT01_TABLE,
	SPEEDLIMIT_TABLE,
	NAT_RT_IP_RAM0,
	NAT_RT_IP_RAM1,
};

#define SE_NAT_TB_WRDATA0	0x188040
#define SE_NAT_TB_WRDATA_SIZE	0x4
#define SE_NAT_TB_WRDATA(n)	\
	(SE_NAT_TB_WRDATA0 + (n) * SE_NAT_TB_WRDATA_SIZE)
#define SE_NAT_TB_RDDATA0	0x188080
#define SE_NAT_TB_RDDATA(n)	\
	(SE_NAT_TB_RDDATA0 + (n) * SE_NAT_TB_WRDATA_SIZE)
#define SE_NAT_TB_DATA_SIZE	0x40

#define SE_NAT_KEY_RAM_DATA0	0x188400
#define SE_NAT_KEY_RAM_DATA_SIZE	0x4
#define SE_NAT_KEY_RAM_DATA(n)	(SE_NAT_KEY_RAM_DATA0 + (n) * SE_NAT_KEY_RAM_DATA_SIZE)
#define DNAT_FLAG		BIT(3)
#define V6_FLAG			BIT(4)
#define OFFLOAD_FLAG		BIT(5)

#define SE_NAT_RESULT_RAM_DATA0		0x188800
#define SE_NAT_RESULT_RAM_DATA_SIZE	0x4
#define SE_NAT_RESULT_RAM_DATA(n)	\
	(SE_NAT_RESULT_RAM_DATA0 + (n) * SE_NAT_RESULT_RAM_DATA_SIZE)

#define SE_NAT_RESULT6_DDR_OFFS_10_0	GENMASK(31, 21)
#define SE_NAT_RESULT6_NAT_ID		GENMASK(20, 5)
#define SE_NAT_RESULT6_SPL_EN		BIT(4)
#define SE_NAT_RESULT6_SPL_IDX_7_4	GENMASK(3, 0)

#define SE_NAT_RESULT7_HIT_ENAPT	BIT(17)
#define SE_NAT_RESULT7_HIT		BIT(16)
#define SE_NAT_RESULT7_OPORT_ID		GENMASK(15, 11)
#define SE_NAT_RESULT7_DDR_OFFS_21_11	GENMASK(10, 0)

#define SE_NAT_RESULT_RAM_DATA7	0x18881c
#define RESULT_NAT_ID		GENMASK(20, 5)
#define HIT_LOCATION		BIT(17)	/* 0: ILKP, 1: ELKP */
#define RESULT_HIT		BIT(16)

#define SE_NAT_VISIT(n)		(0x189000 + (n) * 4)

#define AXI_RD_TIMING_DELAY(n)		(0x38040 + (n) * 4)
#define AXI_RD_MAX_DELAY		GENMASK(31, 16)
#define AXI_RD_MIN_DELAY		GENMASK(15, 0)

#define AXI_RD_TIMING_RCD_RESULT(n)	(0x38048 + (n) * 4)
#define AXI_TIMING_RCD_CTRL		(0x34010)
#define AXI_WR_MAX_DELAY		GENMASK(31, 15)
#define AXI_DELAY_FBDIV			GENMASK(14, 8)
#define AXI_RD_TIMING_MEASURE_EN	BIT(0)

#define AXI_TIMING_RCD_RANGE_CTRL	(0x34014)
#define TIMING_INTERVAL_UNIT		GENMASK(15, 0)
#define MIN_TIMING_THRESHOLD		GENMASK(29, 16)

typedef __uint128_t	u128;
struct dpns_nat_priv;

enum hnat_dir {
	DIR_SNAT,
	DIR_DNAT,
};
typedef struct {
	u32 ip;
	u32 refcnt;
} ip_address;

typedef struct {
	u32 ip[4];
	u32 refcnt;
} ip6_address;

struct nat_ipv4_data {
		u32 public_ip;
		u16 public_port;
		u32 private_ip;
		u16 private_port;
		u32 router_ip;
		u16 router_port;
		u16 crc16_poly[NPU_NAT_SUB_TB];
		u16 srtmac_index:6;
		u16 drtmac_index:6;
		u16 primac_index:11;
		u16 pubmac_index:11;
		u16 router_ip_index:4;
		u16 l4_type:1;
		u16 soport_id:5;
		u16 doport_id:5;
} __packed;

typedef union {
	struct {
		u32 public_ip;
		u16 public_port;
		u32 private_ip;
		u16 private_port;
		u16 router_ip_index:4;
		u16 router_port:16;
		u16 l4_type:1;	/* 0: TCP, 1: UDP */
		u16 valid:1;
		u16 srtmac_index:6;
		u16 drtmac_index:6;
		u16 primac_index:11;
		u16 pubmac_index:11;
		u16 repl_pri_en:1;
		u16 repl_pri:3;
		u16 stat_en:1;
		u16 stat_index:9;
		u16 spl_en:1;
		u16 spl_index:8;
		u16 soport_id:5;
		u16 doport_id:5;
		u16 dummy:3;
		u16 v6_flag:1;
	} __packed;
	struct {
		u64 data[3];
	};
} nat_ipv4_table;

typedef union {
	struct {
		u32 public_ip;
		u16 public_port;
		u32 private_ip;
		u16 private_port;
		u32 router_ip;
		u16 router_port;
		u32 l4_type:1;
		u32 valid:1;
		u32 rtmac_index:6;
		u32 primac_index:11;
		u32 pubmac_index:11;
		u32 repl_pri_en:1;
		u32 repl_pri:3;
		u32 stat_en:1;
		u32 stat_index:9;
		u32 spl_en:1;
		u32 spl_index:8;
		u32 nat_id:16;
		u32 oport_id:5;
		u32 offset:22;
		u32 dummy1:10;
		u32 v6_flag:1;
		u32 dummy:5;
	} __packed;
	struct {
		u64 data[4];
	};
} nat_ipv4_ext_table;

struct nat_ipv6_data {
		u32 public_ip[4];
		u32 router_ip[4];
		u32 private_ip[4];
		u16 public_port;
		u16 private_port;
		u16 router_port;
		u16 crc16_poly[NPU_NAT_SUB_TB];
		u16 router_ip_index:4;
		u16 l4_type:1;
		u16 srtmac_index:6;
		u16 drtmac_index:6;
		u16 primac_index:11;
		u16 pubmac_index:11;
		u16 soport_id:5;
		u16 doport_id:5;
} __packed;

typedef union {
	struct {
		u32 public_ip[4]; // data[0-1]
		u16 public_port; // data[1] 0:15
		u32 private_ip[4]; // data[1] 16: data[2] data[3] :47
		u16 private_port; // data[3] 48:63
		u16 router_ip_index:4; // data[4] 0:3
		u16 router_port:16; // data[4] 4:19
		u16 l4_type:1; // data[4] 20
		u16 valid:1; // data[4] 21
		u16 srtmac_index:6;
		u16 drtmac_index:6;
		u16 primac_index:11;
		u16 pubmac_index:11;
		u16 repl_pri_en:1;
		u16 repl_pri:3;
		u16 stat_en:1;
		u16 stat_index:9;
		u16 spl_en:1;
		u16 spl_index:8;
		u16 soport_id:5;
		u16 doport_id:5;
		u16 flag:1;
	} __packed;
	struct {
		u64 data[7];
	};
} nat_ipv6_table;

typedef union {
	struct {
		u32 public_ip[4];
		u16 public_port;
		u32 private_ip[4];
		u16 private_port;
		u32 router_ip[4];
		u16 router_port:16;
		u16 l4_type:1;
		u16 valid:1;
		u16 rtmac_index:6;
		u16 primac_index:11;
		u16 pubmac_index:11;
		u16 repl_pri_en:1;
		u16 repl_pri:3;
		u16 stat_en:1;
		u16 stat_index:9;
		u16 spl_en:1;
		u16 spl_index:8;
		u16 nat_id:16;
		u16 oport_id:5;
		u16 flag:1;
		u16 dummy:5;
	} __packed;
	struct {
		u64 data[8];
	};
} nat_ipv6_ext_table;

union nat_table_u {
	nat_ipv4_ext_table v4[2];
	nat_ipv6_ext_table v6;
};

/* big endian, used for crc16 */
struct nat_hash_tuple {
	u8 l4_type;
	__be16 sport;
	union {
		struct in6_addr sipv6;
		struct {
			__be32 sipv4_pad[3];
			__be32 sipv4;
		};
	};
	__be16 dport;
	union {
		struct in6_addr dipv6;
		struct {
			__be32 dipv4_pad[3];
			__be32 dipv4;
		};
	};
} __packed;

struct dpns_nat_entry {
	struct rhash_head node;
	struct rcu_head rcu;
	unsigned long cookie;
	u16 nat_id;
	u16 crc16_poly[NPU_NAT_SUB_TB];
	union {
		u16 index; /* Only for ENAPT. */
		u16 hash_index; /* Only for INAPT. */
	};
	union {
		u16 sintf_index; /* Only for SNAT. */
		u16 dintf_index; /* Only for DNAT. */
	};
	bool is_dnat : 1;
	bool second_slot : 1;
	bool v6_flag : 1;
	/* flowtable block (struct dpns_nat_block) the entry was offloaded from */
	void *owner;
	struct list_head flush_node;
};

void dpns_nat_wait_rw(struct dpns_nat_priv *priv);
void dpns_nat_wait_lkp(struct dpns_nat_priv *priv);
int dpns_nat_genl_init(struct dpns_nat_priv *priv);
int dpns_nat_genl_exit(void);
int dpns_nat_show(struct dpns_nat_priv *priv);
void dpns_nat_hw_lookup4(struct dpns_nat_priv *priv, bool is_dnat, struct nat_ipv4_data *tb, bool is_offload);
void dpns_nat_hw_lookup6(struct dpns_nat_priv *priv, bool is_dnat, struct nat_ipv6_data *tb, bool is_offload);
u16 crc16_custom(const u8 *buf, size_t len, u8 poly_sel);
int dpns_nat_count(struct dpns_nat_priv *priv);
void dpns_nat_proc_init(struct dpns_nat_priv *priv);
void dpns_nat_proc_exit(void);
void dpns_nat_wan_sync(void);
void dpns_nat_wan_clear(int index);

/*
 * Not __builtin_bswap128(): GCC >= 11 lowers it to __bswapdi2 libcalls on
 * rv64gc without Zbb, and the kernel does not provide that helper.
 */
static __always_inline u128 swab128(u128 val)
{
	u64 lo = (u64)val;
	u64 hi = (u64)(val >> 64);
	u64 tmp;

	tmp = swab64(lo);
	lo = swab64(hi);
	hi = tmp;

	return lo | (u128)hi << 64;
}

#endif /* DPNS_NAT_H */
