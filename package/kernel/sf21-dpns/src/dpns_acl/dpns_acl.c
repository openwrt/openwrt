#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__

#include <linux/netlink.h>
#include <linux/platform_device.h>
#include <linux/genetlink.h>
#include <linux/unaligned.h>
#include <net/genetlink.h>
#include <linux/proc_fs.h>

#include "dpns_common.h"
#include "sf_genl_msg.h"
#include "dpns_acl.h"

static struct acl_priv *priv;
extern const struct proc_ops acl_ctrl;

static void acl_reset_data(bool is_eacl)
{
	if (!is_eacl) {
		priv->iacl_size = 0;
		priv->iacl_v4_line = -1;
		priv->iacl_v6_line = -1;
		priv->iacl_v4_cnt = 0;
		priv->iacl_v6_cnt = 0;
		priv->iacl_last_index = 0;
		bitmap_clear(priv->iacl_bitmap, 0, TCAM_LINE);
	} else {
		priv->eacl_size = 0;
		priv->eacl_size = 0;
		priv->eacl_v4_line = -1;
		priv->eacl_v6_line = -1;
		priv->eacl_v4_cnt = 0;
		priv->eacl_v6_cnt = 0;
		priv->eacl_last_index = 0;
		bitmap_clear(priv->eacl_bitmap, 0, TCAM_LINE);
	}
}

void acl_clear(bool is_eacl)
{
	struct acl_data *pos, *n;
	struct list_head *head = &priv->iacl_list;
	u32 tcam_id = TCAM_IACL;

	if (is_eacl) {
		tcam_id = TCAM_EACL;
		head = &priv->eacl_list;
	}

	priv->cpriv->tcam_clean(priv->cpriv, tcam_id);

	list_for_each_entry_safe(pos, n, head, list) {
		list_del(&pos->list);
		if (pos->key) {
			kfree(pos->key);
			kfree(pos->mask);
		}
		if (pos->key6) {
			kfree(pos->key6);
			kfree(pos->mask6);
		}
		kfree(pos);
	}
	acl_reset_data(is_eacl);
}

void acl_dump_list(bool is_eacl)
{
	u32 v4mode, v6mode;
	struct acl_data *pos;
	struct list_head *head = &priv->iacl_list;

	if (is_eacl)
		head = &priv->eacl_list;

	if (is_eacl) {
		v4mode = priv->ev4_mode;
		v6mode = priv->ev6_mode;
	} else {
		v4mode = priv->iv4_mode;
		v6mode = priv->iv6_mode;
	}

	list_for_each_entry(pos, head, list) {
		printk("index: %d\n", pos->index);
		if (pos->key) {
			printk("v4mode: %d\n", v4mode);
			if (pos->spl != -1)
				printk("spl_id: %d, spl: %d \n", pos->spl_index, pos->spl);

			if (v4mode == NPU_ACL_MODE0) {
				printk(KERN_CONT "data:");
				acl_dump_data_mode0((struct acl_key_mode0 *)pos->key);
				printk(KERN_CONT "mask:");
				acl_dump_data_mode0((struct acl_key_mode0 *)pos->mask);
			}
			else if (v4mode == NPU_ACL_MODE1) {
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode1((struct acl_key_v4_mode1 *)pos->key);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode1((struct acl_key_v4_mode1 *)pos->mask);
			}
			else if (v4mode == NPU_ACL_MODE2) {
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode2((struct acl_key_v4_mode2 *)pos->key);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode2((struct acl_key_v4_mode2 *)pos->mask);
			}
			else if (v4mode == NPU_ACL_MODE3) {
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode3((struct acl_key_v4_mode3 *)pos->key);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode3((struct acl_key_v4_mode3 *)pos->mask);
			}
			else if (v4mode == NPU_ACL_MODE4) {
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode4_v6_mode1((struct acl_key_v4_mode4_v6_mode1 *)pos->key);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode4_v6_mode1((struct acl_key_v4_mode4_v6_mode1 *)pos->mask);
			}
			else if (v4mode == NPU_ACL_MODE5) {
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode5((struct acl_key_v4_mode5 *)pos->key);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode5((struct acl_key_v4_mode5 *)pos->mask);
			}
			else if (v4mode == NPU_ACL_MODE6) {
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode6((struct acl_key_v4_mode6 *)pos->key);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode6((struct acl_key_v4_mode6 *)pos->mask);
			}
			else if (v4mode == NPU_ACL_MODE7) {
				printk(KERN_CONT "data:");
				acl_dump_data_v6_mode7((struct acl_key_v6_mode7 *)pos->key);
				printk(KERN_CONT "mask:");
				acl_dump_data_v6_mode7((struct acl_key_v6_mode7 *)pos->mask);
			}
		}

		printk("\n");

		if (pos->key6) {
			printk("v6mode: %d\n", v6mode);
			if (v6mode == NPU_ACL_MODE0) {
				printk(KERN_CONT "data:");
				acl_dump_data_mode0((struct acl_key_mode0 *)pos->key6);
				printk(KERN_CONT "mask:");
				acl_dump_data_mode0((struct acl_key_mode0 *)pos->mask6);
			}
			else if (v6mode == NPU_ACL_MODE1) {
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode1((struct acl_key_v4_mode1 *)pos->key6);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode1((struct acl_key_v4_mode1 *)pos->mask6);
			}
			else if (v6mode == NPU_ACL_MODE2) {
				printk(KERN_CONT "data:");
				acl_dump_data_v6_mode2((struct acl_key_v6_mode2 *)pos->key6);
				printk(KERN_CONT "mask:");
				acl_dump_data_v6_mode2((struct acl_key_v6_mode2 *)pos->mask6);
			}
			else if (v6mode == NPU_ACL_MODE3) {
				printk(KERN_CONT "data:");
				acl_dump_data_v6_mode3((struct acl_key_v6_mode3 *)pos->key6);
				printk(KERN_CONT "mask:");
				acl_dump_data_v6_mode3((struct acl_key_v6_mode3 *)pos->mask6);
			} else if (v6mode == NPU_ACL_MODE7) {
				printk(KERN_CONT "data:");
				acl_dump_data_v6_mode7((struct acl_key_v6_mode7 *)pos->key6);
				printk(KERN_CONT "mask:");
				acl_dump_data_v6_mode7((struct acl_key_v6_mode7 *)pos->mask6);
			}
		}
	}
}

void acl_dump_table(bool is_eacl)
{
	int mod_req_id, i, j,  tbid_kmd;
	int size, acl_req_id;
	u8 data[72] = {0}, mask[72] = {0};

	mod_req_id = is_eacl ? TCAM_BLK_MODE_ID(TCAM_EACL) : TCAM_BLK_MODE_ID(TCAM_IACL);
	acl_req_id = is_eacl ? TCAM_BLK_RAM_ID(TCAM_EACL, 0) : TCAM_BLK_RAM_ID(TCAM_IACL, 0);

	for(i = 0; i < 16; i += 2)
	{
		u32 access = FIELD_PREP(TCAM_OPT_ID, mod_req_id) |
				FIELD_PREP(TCAM_OPT_REQ_ADDR, i/2);
		sf_writel(priv->cpriv, SE_TCAM_OPT_ADDR, access);
		put_unaligned(sf_readl(priv->cpriv, TCAM_R_ADDR(0)), (u32*)&tbid_kmd);

		if((tbid_kmd == TBID_KMD_V4_MOD0) || (tbid_kmd == TBID_KMD_V4_MOD1) ||
					(tbid_kmd == TBID_KMD_V4_MOD2))
			size = 9;
		else if (tbid_kmd == TBID_KMD_V4_MOD3)
			size = 18;
		else if ((tbid_kmd == TBID_KMD_V4_MOD4) || (tbid_kmd == TBID_KMD_V4_MOD5) ||
					(tbid_kmd == TBID_KMD_V6_MOD2) || (tbid_kmd == TBID_KMD_V6_MOD3))
			size = 36;
		else if ((tbid_kmd == TBID_KMD_V4_MOD6) || (tbid_kmd == TBID_KMD_V4_MOD7))
			size = 72;


		for (j = 0; j < TCAM_INDEX_SIZE/size; j ++)
		{
			int req_id = acl_req_id + j * 8 / (TCAM_INDEX_SIZE/size);
			priv->cpriv->tcam_access(priv->cpriv, SE_OPT_R, req_id, i, data, size);
			priv->cpriv->tcam_access(priv->cpriv, SE_OPT_R, req_id, i + 1, mask, size);
			if (tbid_kmd == TBID_KMD_V4_MOD0) {
				if(((struct acl_key_mode0 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v4_mode0 or v6_mode0 req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_mode0((struct acl_key_mode0 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_mode0(((struct acl_key_mode0 *)mask));
			}
			else if (tbid_kmd == TBID_KMD_V4_MOD1) {
				if(((struct acl_key_v4_mode1 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v4_mode1 req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode1((struct acl_key_v4_mode1 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode1((struct acl_key_v4_mode1 *)mask);
			}
			else if (tbid_kmd == TBID_KMD_V4_MOD2) {
				if(((struct acl_key_v4_mode2 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v4_mode2 req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode1((struct acl_key_v4_mode1 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode1((struct acl_key_v4_mode1 *)mask);
			}
			else if (tbid_kmd == TBID_KMD_V4_MOD3) {
				if(((struct acl_key_v4_mode3 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v4_mode3 req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode3((struct acl_key_v4_mode3 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode3((struct acl_key_v4_mode3 *)mask);
			}
			else if (tbid_kmd == TBID_KMD_V4_MOD4) {
				if(((struct acl_key_v4_mode4_v6_mode1 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v4_mode4 or v6_mode1 req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode4_v6_mode1((struct acl_key_v4_mode4_v6_mode1 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode4_v6_mode1((struct acl_key_v4_mode4_v6_mode1 *)mask);
			}
			else if (tbid_kmd == TBID_KMD_V4_MOD5) {
				if(((struct acl_key_v4_mode5 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v4_mode5 req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode5((struct acl_key_v4_mode5 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode5((struct acl_key_v4_mode5 *)mask);
			}
			else if (tbid_kmd == TBID_KMD_V4_MOD6) {
				if(((struct acl_key_v4_mode6 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v4_mode6 req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_v4_mode6((struct acl_key_v4_mode6 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_v4_mode6((struct acl_key_v4_mode6 *)mask);
			}
			else if (tbid_kmd == TBID_KMD_V4_MOD7) {
				if(((struct acl_key_v4_mode7 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v4_mode7 or v6_mode7: req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_v6_mode7((struct acl_key_v6_mode7 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_v6_mode7((struct acl_key_v6_mode7 *)mask);
			}
			else if (tbid_kmd == TBID_KMD_V6_MOD2) {
				if(((struct acl_key_v6_mode2 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v6_mode2 req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_v6_mode2((struct acl_key_v6_mode2 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_v6_mode2((struct acl_key_v6_mode2 *)mask);
			}
			else if (tbid_kmd == TBID_KMD_V6_MOD3) {
				if(((struct acl_key_v6_mode3 *)mask)->policy == 0) {
					j = TCAM_INDEX_SIZE/size;
					break;
				}
				printk("v6_mode3 req_id: %d req_addr: %d :\n", req_id, i);
				printk(KERN_CONT "data:");
				acl_dump_data_v6_mode3((struct acl_key_v6_mode3 *)data);
				printk(KERN_CONT "mask:");
				acl_dump_data_v6_mode3((struct acl_key_v6_mode3 *)mask);
			}
		}
	}
}

static void acl_write_data(void *key, void *mask, u32 line, u32 offset, u8 loop_cnt,
				bool is_eacl, u8 acl_w_addr, u32 spl, u32 spl_index)
{
	u8 acl_req_id = 0, tcam_blk_id = TCAM_IACL;
	int i;

	if (is_eacl)
		tcam_blk_id = TCAM_EACL;

	for (i = 0; i < loop_cnt; i++, key += TCAM_SLICE_SIZE, mask += TCAM_SLICE_SIZE) {
		if (is_eacl)
			acl_req_id = EACL_REQ_ID(i) + offset;
		else
			acl_req_id = IACL_REQ_ID(i) + offset;
		priv->cpriv->tcam_access(priv->cpriv, SE_OPT_W, acl_req_id,
				line * 2, key, TCAM_SLICE_SIZE);
		priv->cpriv->tcam_access(priv->cpriv, SE_OPT_W, acl_req_id,
				line * 2 + 1, mask, TCAM_SLICE_SIZE);
	}

	sf_writel(priv->cpriv, TCAM_W_ADDR(0), acl_w_addr);
	sf_writel(priv->cpriv, SE_TCAM_OPT_ADDR, TCAM_OPT_WR |
						FIELD_PREP(TCAM_OPT_ID, ACL_TBID(tcam_blk_id)) |
						FIELD_PREP(TCAM_OPT_REQ_ADDR, line));
	priv->cpriv->se_wait(priv->cpriv, SE_TCAM_OPT_ADDR, TCAM_OPT_BUSY);

	if (spl < SPL_MAX) {
		sf_writel(priv->cpriv, TCAM_W_ADDR(0), spl);
		sf_writel(priv->cpriv, SE_TCAM_OPT_ADDR, TCAM_OPT_WR |
							FIELD_PREP(TCAM_OPT_ID, ACL_SPL_RAM_ID) |
							FIELD_PREP(TCAM_OPT_REQ_ADDR, spl_index));
		priv->cpriv->se_wait(priv->cpriv, SE_TCAM_OPT_ADDR, TCAM_OPT_BUSY);
	}
}

int acl_write(struct acl_data *data, bool is_ipv6)
{
	u32 line, offset;
	u8 loop_cnt = data->sz4 / TCAM_SLICE_SIZE, acl_w_addr = priv->v4_w_addr;
	unsigned long *bitmap = priv->iacl_bitmap;

	if (data->is_eacl)
		bitmap = priv->eacl_bitmap;

	if (!is_ipv6) {
		if (data->is_eacl) {
			line = priv->eacl_v4_line;
			offset = priv->eacl_v4_cnt * loop_cnt;
		} else {
			line = priv->iacl_v4_line;
			offset = priv->iacl_v4_cnt * loop_cnt;
		}
		if (line > TCAM_LINE) {
			line = find_first_zero_bit(bitmap, TCAM_LINE);
			if (line >= TCAM_LINE)
				return -ENOSPC;
			set_bit(line, bitmap);
		}
		acl_write_data(data->key, data->mask, line, offset, loop_cnt,
				data->is_eacl, acl_w_addr, data->spl, data->spl_index);

		if (data->is_eacl) {
			priv->eacl_v4_line = line;
			priv->eacl_v4_cnt++;
			if (priv->eacl_v4_cnt == TCAM_INDEX_SIZE / data->sz4) {
				priv->eacl_v4_cnt = 0;
				priv->eacl_v4_line = -1;
				priv->eacl_size++;
			}
		} else {
			priv->iacl_v4_line = line;
			priv->iacl_v4_cnt++;
			if (priv->iacl_v4_cnt == TCAM_INDEX_SIZE / data->sz4) {
				priv->iacl_v4_cnt = 0;
				priv->iacl_v4_line = -1;
				priv->iacl_size++;
			}
		}
	} else {
		acl_w_addr = priv->v6_w_addr;
		loop_cnt = data->sz6 / TCAM_SLICE_SIZE;
		if (data->is_eacl) {
			line = priv->eacl_v6_line;
			offset = priv->eacl_v6_cnt * loop_cnt;
		} else {
			line = priv->iacl_v6_line;
			offset = priv->iacl_v6_cnt * loop_cnt;
		}
		if (line > TCAM_LINE) {
			line = find_first_zero_bit(bitmap, TCAM_LINE);
			if (line >= TCAM_LINE)
				return -ENOSPC;
			set_bit(line, bitmap);
		}
		acl_write_data(data->key6, data->mask6, line, offset, loop_cnt,
				data->is_eacl, acl_w_addr, data->spl, data->spl_index);

		if (data->is_eacl) {
			priv->eacl_v6_line = line;
			priv->eacl_v6_cnt++;
			if (priv->eacl_v6_cnt == TCAM_INDEX_SIZE / data->sz6) {
				priv->eacl_v6_cnt = 0;
				priv->eacl_v6_line = -1;
				priv->eacl_size++;
			}
		} else {
			priv->iacl_v6_line = line;
			priv->iacl_v6_cnt++;
			if (priv->iacl_v6_cnt == TCAM_INDEX_SIZE / data->sz6) {
				priv->iacl_v6_cnt = 0;
				priv->iacl_v6_line = -1;
				priv->iacl_size++;
			}
		}
	}
	return 0;
}

int acl_rewrite(bool is_eacl)
{
	struct acl_data *pos;
	struct list_head *head = &priv->iacl_list;
	int tcam_id = TCAM_IACL, ret;

	if (is_eacl) {
		tcam_id = TCAM_EACL;
		head = &priv->eacl_list;
	}

	priv->cpriv->tcam_clean(priv->cpriv, tcam_id);
	acl_reset_data(is_eacl);

	list_for_each_entry(pos, head, list) {
		if (is_eacl)
			priv->eacl_last_index = pos->index;
		else
			priv->iacl_last_index = pos->index;

		if (pos->key) {
			ret = acl_write(pos, 0);
			if (ret < 0) {
				pr_err("%s table for ipv4 full!\n", is_eacl ? "EACL" : "IACL");
				return -ENOSPC;
			}
		}
		if (pos->key6) {
			ret = acl_write(pos, 1);
			if (ret < 0) {
				pr_err("%s table for ipv6 full!\n", is_eacl ? "EACL" : "IACL");
				return -ENOSPC;
			}
		}
	}
	return 0;
}

void acl_data_mem_alloc(void **key, void **mask, u16 size)
{
	*key = kmalloc(size, GFP_KERNEL);
	*mask = kmalloc(size, GFP_KERNEL);
	memset(*key, 0, size);
	memset(*mask, 0xff, size);
}

void acl_mode_set(bool is_eacl, u32 mode, bool is_ipv6)
{
	if (is_eacl) {
		if (is_ipv6)
			sf_update(priv->cpriv, NPU_EACL_MPP_CFG0,
				  ACL_MPP_CFG0_EACL_KEY0_V6_MODE,
				  FIELD_PREP(ACL_MPP_CFG0_EACL_KEY0_V6_MODE, mode));
		else
			sf_update(priv->cpriv, NPU_EACL_MPP_CFG0,
				  ACL_MPP_CFG0_EACL_KEY0_V4_MODE,
				  FIELD_PREP(ACL_MPP_CFG0_EACL_KEY0_V4_MODE, mode));

	} else {
		if (is_ipv6)
			sf_update(priv->cpriv, NPU_IACL_MPP_CFG0,
				  ACL_MPP_CFG0_IACL_KEY0_V6_MODE,
				  FIELD_PREP(ACL_MPP_CFG0_IACL_KEY0_V6_MODE, mode));
		else
			sf_update(priv->cpriv, NPU_IACL_MPP_CFG0,
				  ACL_MPP_CFG0_IACL_KEY0_V4_MODE,
				  FIELD_PREP(ACL_MPP_CFG0_IACL_KEY0_V4_MODE, mode));
	}
}

int dpns_acl_probe(struct platform_device *pdev)
{
	COMMON_t *cpriv = platform_get_drvdata(pdev);

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	INIT_LIST_HEAD(&priv->iacl_list);
	INIT_LIST_HEAD(&priv->eacl_list);

	priv->cpriv = cpriv;
	priv->iv4_mode = DEFAULT_MODE_SET;
	priv->iv6_mode = DEFAULT_MODE_SET;
	priv->ev4_mode = DEFAULT_MODE_SET;
	priv->ev6_mode = DEFAULT_MODE_SET;
	priv->iacl_v4_line = -1;
	priv->eacl_v4_line = -1;
	priv->iacl_v6_line = -1;
	priv->eacl_v6_line = -1;

	priv->cpriv->tcam_clean(cpriv, TCAM_IACL);
	priv->cpriv->tcam_clean(cpriv, TCAM_EACL);
	priv->cpriv->tcam_clean(cpriv, TCAM_SPL);

	sf_update(cpriv, NPU_IACL_MPP_CFG0, 0,
		NPU_IACL_MPP_CFG0_IPV6_EN | NPU_IACL_MPP_CFG0_IPV4_EN |
		ACL_MPP_CFG0_IACL_KEY0_V6_MODE | ACL_MPP_CFG0_IACL_KEY0_V4_MODE);
	sf_update(cpriv, NPU_IACL_MPP_CFG1, 0,
			NPU_IACL_MPP_CFG1_IVPORT_BITMAP);
	sf_writel(cpriv, NPU_IACL_MPP_CFG2, NPU_IACL_MPP_CFG2_SPL_DROP_EN);

	/* enable EACL v4 v6 mode7 */
	sf_update(cpriv, NPU_EACL_MPP_CFG0, 0,
		ACL_MPP_CFG0_EACL_IPV6_EN | ACL_MPP_CFG0_EACL_IPV4_EN |
		ACL_MPP_CFG0_EACL_KEY0_V6_MODE | ACL_MPP_CFG0_EACL_KEY0_V4_MODE);
	sf_writel(cpriv, NPU_EACL_MPP_CFG1,
		ACL_EACL_MPP_CFG1_SPL_DROP_EN | ACL_MPP_CFG1_IVPORT_BITMAP);
	sf_writel(cpriv, NPU_ECAL_MPP_CFG2, ACL_MPP_CFG2_OVPORT_BITMAP);

	/* allow speed limit to zero */
	sf_update(cpriv, TCAM_CONFIG3_RGT, 0, ACL_SPL_ZERO_LIMIT);

	acl_genl_init(priv);
	proc_create("dpns_acl", 0222, NULL, &acl_ctrl);

	printk("End %s\n", __func__);
	return 0;
}
EXPORT_SYMBOL(dpns_acl_probe);

void dpns_acl_remove(struct platform_device *pdev)
{
	int i;
	for (i = 0; i < PKG_OFFSET_CFG_CNT; i++)
		sf_writel(priv->cpriv, ACL_PKT_OFFSET_CFG(i), 0);
	priv->cpriv->tcam_clean(priv->cpriv, TCAM_SPL);
	acl_clear(DIR_IACL);
	acl_clear(DIR_EACL);
	acl_genl_deinit();
	remove_proc_entry("dpns_acl", NULL);
	printk("End %s\n", __func__);
}
EXPORT_SYMBOL(dpns_acl_remove);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Youjia Min <youjia.min@siflower.com.cn>");
MODULE_DESCRIPTION("DPNS ACL Interface");
