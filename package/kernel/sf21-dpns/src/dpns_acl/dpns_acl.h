/* to fill data struct */
#ifndef __DPNS_ACL_H
#define __DPNS_ACL_H
#include "dpns_acl_msg.h"
#include "dpns_common.h"

#define fill_data(x) do { data->x = msg_data->x; } while (0)

#define TCAM_BLK_MODE_ID(BLK_ID) ((BLK_ID)*9+8)
#define TCAM_BLK_RAM_ID(BLK_ID, RAM_INDEX) ((BLK_ID)*9+(RAM_INDEX))

#define DEFAULT_MODE_SET	7
#define SPL_POLICY		7
#define PKG_OFFSET_CFG_CNT	3
#define PKG_OFFSET_CFG_NUM	8

#define EACL_REQ_ID(slice_id)	(TCAM_EACL * 9 + (slice_id))
#define IACL_REQ_ID(slice_id)	(TCAM_IACL * 9 + (slice_id))
#define ACL_TBID(n)		((n) * 9 + 8)
#define TCAM_INDEX_SIZE		72
#define TCAM_LINE		16
#define ACL_SPL_RAM_ID		72


#define TCAM_CONFIG3_RGT		0x190014
#define ACL_SPL_MODE			BIT(1)
#define ACL_SPL_ZERO_LIMIT		BIT(0)

#define NPU_IACL_MPP_CFG0		0x2C000			// NPU iacl
#define NPU_IACL_MPP_CFG1		0x2c004
#define NPU_IACL_MPP_CFG2		0x2c008

#define NPU_IACL_MPP_CFG0_IPV6_EN	BIT(26)
#define NPU_IACL_MPP_CFG0_IPV4_EN	BIT(25)
#define ACL_MPP_CFG0_IACL_KEY0_V6_MODE	GENMASK(24, 22)
#define ACL_MPP_CFG0_IACL_KEY0_V4_MODE	GENMASK(21, 19)
#define NPU_IACL_MPP_CFG0_NEW_IP_P_EN	GENMASK(18, 17)
#define NPU_IACL_MPP_CFG0_NEW_DIP_DP_EN	BIT(18)
#define NPU_IACL_MPP_CFG0_NEW_SIP_SP_EN	BIT(17)
#define NPU_IACL_MPP_CFG1_IVPORT_BITMAP GENMASK(26,0)
#define NPU_IACL_MPP_CFG2_SPL_DROP_EN	BIT(10)

#define NPU_EACL_MPP_CFG0		0x30000
#define NPU_EACL_MPP_CFG1		0x30004
#define NPU_ECAL_MPP_CFG2		0x30008

#define ACL_MPP_CFG0_EACL_IPV6_EN	BIT(24)
#define ACL_MPP_CFG0_EACL_IPV4_EN	BIT(23)
#define ACL_MPP_CFG0_EACL_KEY0_V6_MODE	GENMASK(22, 20)
#define ACL_MPP_CFG0_EACL_KEY0_V4_MODE	GENMASK(19, 17)
#define ACL_MPP_CFG0_BYPASS		BIT(16)
#define ACL_EACL_MPP_CFG1_SPL_DROP_EN	BIT(31)
#define ACL_MPP_CFG1_IVPORT_BITMAP	GENMASK(26, 0)
#define ACL_MPP_CFG2_OVPORT_BITMAP	GENMASK(26, 0)

#define ACL_PKT_OFFSET_CFG0		0x80044
#define HOST_EXTRACT_DBYTE_EN		BIT(31)
#define ACL_PKT_OFFSET_CFG(n)		(ACL_PKT_OFFSET_CFG0 + 4 * (n))
#define	PKT_OFFSET_CFG2			GENMASK(29, 20)
#define PKT_OFFSET_CFG1			GENMASK(19, 10)
#define PKT_OFFSET_CFG0			GENMASK(9, 0)

struct acl_data {
	bool is_eacl;
	u8 req_id;
	u8 req_addr;
	u16 sz4;
	u16 sz6;
	u32 index;
	u32 spl;
	u32 spl_index;
	void *key;
	void *mask;
	void *key6;
	void *mask6;
	struct list_head list;
};


/** NPU ACL MODE SET
 * V4 mode: 0~7
 * V6 mode: 0~3, 7
 * **/
enum acl_mode {
	  NPU_ACL_MODE0 = 0,
	  NPU_ACL_MODE1,
	  NPU_ACL_MODE2,
	  NPU_ACL_MODE3,
	  NPU_ACL_MODE4,
	  NPU_ACL_MODE5,
	  NPU_ACL_MODE6,
	  NPU_ACL_MODE7,
};

enum mode_set {
	  IS_V4,
	  IS_V6,
};

void acl_add_data_mode0(struct acl_key_mode0 *data, const struct acl_data_t *msg_data);
void acl_add_data_v4_mode1(struct acl_key_v4_mode1 *data, const struct acl_data_t *msg_data);
void acl_add_data_v4_mode2(struct acl_key_v4_mode2 *data, const struct acl_data_t *msg_data);
void acl_add_data_v4_mode3(struct acl_key_v4_mode3 *data, const struct acl_data_t *msg_data);
void acl_add_data_v4_mode4_v6_mode1(struct acl_key_v4_mode4_v6_mode1 *data, const struct acl_data_t *msg_data);
void acl_add_data_v4_mode5(struct acl_key_v4_mode5 *data, const struct acl_data_t *msg_data);
void acl_add_data_v4_mode6(struct acl_key_v4_mode6 *data, const struct acl_data_t *msg_data);
void acl_add_data_v4_mode7(struct acl_key_v4_mode7 *data, const struct acl_data_t *msg_data);
void acl_add_data_v6_mode2(struct acl_key_v6_mode2 *data, const struct acl_data_t *msg_data);
void acl_add_data_v6_mode3(struct acl_key_v6_mode3 *data, const struct acl_data_t *msg_data);
void acl_add_data_v6_mode7(struct acl_key_v6_mode7 *data, const struct acl_data_t *msg_data);

void acl_dump_data_mode0(struct acl_key_mode0 *data);
void acl_dump_data_v4_mode1(struct acl_key_v4_mode1 *data);
void acl_dump_data_v4_mode2(struct acl_key_v4_mode2 *data);
void acl_dump_data_v4_mode3(struct acl_key_v4_mode3 *data);
void acl_dump_data_v4_mode4_v6_mode1(struct acl_key_v4_mode4_v6_mode1 *data);
void acl_dump_data_v4_mode5(struct acl_key_v4_mode5 *data);
void acl_dump_data_v4_mode6(struct acl_key_v4_mode6 *data);
void acl_dump_data_v4_mode7(struct acl_key_v4_mode7 *data);
void acl_dump_data_v6_mode2(struct acl_key_v6_mode2 *data);
void acl_dump_data_v6_mode3(struct acl_key_v6_mode3 *data);
void acl_dump_data_v6_mode7(struct acl_key_v6_mode7 *data);

int acl_genl_init(struct acl_priv * apriv);
int acl_genl_deinit(void);

void acl_data_mem_alloc(void **key, void **mask, u16 size);
void acl_clear(bool is_eacl);
void acl_mode_set(bool is_eacl, u32 mode, bool is_ipv6);
int acl_write(struct acl_data *data, bool is_ipv6);
int acl_rewrite(bool is_eacl);
void acl_dump_table(bool is_eacl);
void acl_dump_list(bool is_eacl);
#endif
