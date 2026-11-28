#include <linux/netlink.h>
#include <linux/platform_device.h>
#include <linux/genetlink.h>
#include <linux/unaligned.h>
#include <net/genetlink.h>

#include "dpns_common.h"
#include "sf_genl_msg.h"
#include "dpns_acl.h"

static struct acl_priv *priv;

static void acl_add_data(struct acl_data *data, struct acl_genl_msg_add *msg,
                                bool fill_both, u32 v4_mode, u32 v6_mode)
{
        if (msg->is_ipv4 || fill_both) {
                switch (v4_mode) {
                case NPU_ACL_MODE0:
                        priv->v4_w_addr = TBID_KMD_V4_MOD0;
                        data->sz4 = SZ_9B;
                        acl_data_mem_alloc(&data->key, &data->mask, data->sz4);
                        acl_add_data_mode0(data->key, &msg->key);
                        acl_add_data_mode0(data->mask, &msg->mask);
                        break;
                case NPU_ACL_MODE1:
                        priv->v4_w_addr = TBID_KMD_V4_MOD1;
                        data->sz4 = SZ_9B;
                        acl_data_mem_alloc(&data->key, &data->mask, data->sz4);
                        acl_add_data_v4_mode1(data->key, &msg->key);
                        acl_add_data_v4_mode1(data->mask, &msg->mask);
                        break;
                case NPU_ACL_MODE2:
                        priv->v4_w_addr = TBID_KMD_V4_MOD2;
                        data->sz4 = SZ_9B;
                        acl_data_mem_alloc(&data->key, &data->mask, data->sz4);
                        acl_add_data_v4_mode2(data->key, &msg->key);
                        acl_add_data_v4_mode2(data->mask, &msg->mask);
                        break;
                case NPU_ACL_MODE3:
                        priv->v4_w_addr = TBID_KMD_V4_MOD3;
                        data->sz4 = SZ_18B;
                        acl_data_mem_alloc(&data->key, &data->mask, data->sz4);
                        acl_add_data_v4_mode3(data->key, &msg->key);
                        acl_add_data_v4_mode3(data->mask, &msg->mask);
                        break;
                case NPU_ACL_MODE4:
                        priv->v4_w_addr = TBID_KMD_V4_MOD4;
                        data->sz4 = SZ_36B;
                        acl_data_mem_alloc(&data->key, &data->mask, data->sz4);
                        acl_add_data_v4_mode4_v6_mode1(data->key, &msg->key);
                        acl_add_data_v4_mode4_v6_mode1(data->mask, &msg->mask);
                        break;
                case NPU_ACL_MODE5:
                        priv->v4_w_addr = TBID_KMD_V4_MOD5;
                        data->sz4 = SZ_36B;
                        acl_data_mem_alloc(&data->key, &data->mask, data->sz4);
                        acl_add_data_v4_mode5(data->key, &msg->key);
                        acl_add_data_v4_mode5(data->mask, &msg->mask);
                        break;
		case NPU_ACL_MODE6:
                        priv->v4_w_addr = TBID_KMD_V4_MOD6;
                        data->sz4 = SZ_72B;
                        acl_data_mem_alloc(&data->key, &data->mask, data->sz4);
                        acl_add_data_v4_mode6(data->key, &msg->key);
                        acl_add_data_v4_mode6(data->mask, &msg->mask);
                        break;
                default :
                        if (v4_mode != NPU_ACL_MODE7)
                                pr_warn("v4 mode set invalid, use default mode 7\n");
                        priv->v4_w_addr = TBID_KMD_V4_MOD7;
                        data->sz4 = SZ_72B;
                        acl_data_mem_alloc(&data->key, &data->mask, data->sz4);
                        acl_add_data_v4_mode7(data->key, &msg->key);
                        acl_add_data_v4_mode7(data->mask, &msg->mask);
                        break;
                }
        }

        if (msg->is_ipv6 || fill_both) {
                switch (v6_mode) {
                case NPU_ACL_MODE0:
                        if (v4_mode == NPU_ACL_MODE0 && fill_both)
                                break;
                        priv->v6_w_addr = TBID_KMD_V6_MOD0;
                        data->sz6 = SZ_9B;
                        acl_data_mem_alloc(&data->key6, &data->mask6, data->sz6);
                        acl_add_data_mode0(data->key6, &msg->key);
                        acl_add_data_mode0(data->mask6, &msg->mask);
                        break;
                case NPU_ACL_MODE1:
                        if (v4_mode == NPU_ACL_MODE4 && fill_both)
                                break;
                        priv->v6_w_addr = V4_MODE4_V6_MODE1;
                        data->sz6 = SZ_36B;
                        acl_data_mem_alloc(&data->key6, &data->mask6, data->sz6);
                        acl_add_data_v4_mode4_v6_mode1(data->key6, &msg->key);
                        acl_add_data_v4_mode4_v6_mode1(data->mask6, &msg->mask);
                        break;
                case NPU_ACL_MODE2:
			priv->v6_w_addr = TBID_KMD_V6_MOD2;
                        data->sz6 = SZ_36B;
                        acl_data_mem_alloc(&data->key6, &data->mask6, data->sz6);
                        acl_add_data_v6_mode2(data->key6, &msg->key);
                        acl_add_data_v6_mode2(data->mask6, &msg->mask);
                        break;
                case NPU_ACL_MODE3:
                        priv->v6_w_addr = TBID_KMD_V6_MOD3;
                        data->sz6 = SZ_36B;
                        acl_data_mem_alloc(&data->key6, &data->mask6, data->sz6);
                        acl_add_data_v6_mode3(data->key6, &msg->key);
                        acl_add_data_v6_mode3(data->mask6, &msg->mask);
                        break;
                default :
                        if (v6_mode != NPU_ACL_MODE7)
                                pr_warn("v6 mode set invalid, use default mode 7\n");
			if (fill_both && (v4_mode == NPU_ACL_MODE7))
				break;
                        priv->v6_w_addr = TBID_KMD_V6_MOD7;
                        data->sz6 = SZ_72B;
                        acl_data_mem_alloc(&data->key6, &data->mask6, data->sz6);
                        acl_add_data_v6_mode7(data->key6, &msg->key);
                        acl_add_data_v6_mode7(data->mask6, &msg->mask);
                        break;
                }
        }
}

static void acl_set_host_offset(struct acl_genl_msg_add *msg)
{
        int i, j;
        for (i = 0; i < PKG_OFFSET_CFG_NUM; i++) {
                if (msg->offset[i] < PKT_OFFSET_MAX) {
                        j = i / 3;
                        switch (i % 3) {
                        case 0:
                                sf_update(priv->cpriv, ACL_PKT_OFFSET_CFG(j),
                                          PKT_OFFSET_CFG0, FIELD_PREP(PKT_OFFSET_CFG0,
                                          msg->offset[i]));
                                break;
                        case 1:
                                sf_update(priv->cpriv, ACL_PKT_OFFSET_CFG(j),
                                          PKT_OFFSET_CFG1, FIELD_PREP(PKT_OFFSET_CFG1,
                                          msg->offset[i]));
                                break;
                        case 2:
                                sf_update(priv->cpriv, ACL_PKT_OFFSET_CFG(j),
                                          PKT_OFFSET_CFG2, FIELD_PREP(PKT_OFFSET_CFG2,
                                          msg->offset[i]));
                                break;
                        }
                } else {
                        break;
                }
        }

        if (i > 0)
                sf_update(priv->cpriv, ACL_PKT_OFFSET_CFG0, 0, HOST_EXTRACT_DBYTE_EN);
}

/* Undo a failed acl_add(): drop the entry and rebuild the TCAM from the list,
 * which also removes the part of the entry that was already written.
 */
static int acl_add_undo(struct acl_data *data, bool is_eacl)
{
	list_del(&data->list);
	kfree(data->key);
	kfree(data->mask);
	kfree(data->key6);
	kfree(data->mask6);
	kfree(data);
	acl_rewrite(is_eacl);

	return -ENOSPC;
}

int acl_add(struct acl_genl_msg_add *msg)
{
        struct acl_data *data, *pos;
        struct list_head *head = &priv->iacl_list;
        u32 v4_mode, v6_mode, index;
        bool fill_both = false;
        int ret;

        /* without ip info, need to fill both v4 and v6 */
        if (!(msg->is_ipv6 || msg->is_ipv4))
                fill_both = true;

        if (msg->is_eacl) {
                head = &priv->eacl_list;
                if (priv->eacl_size >= 8)
                        goto err_table_full;

                v4_mode = priv->ev4_mode;
                v6_mode = priv->ev6_mode;
                index = priv->eacl_last_index;
        } else {
                if (priv->iacl_size >= 8)
                        goto err_table_full;

                v4_mode = priv->iv4_mode;
                v6_mode = priv->iv6_mode;
                index = priv->iacl_last_index;
        }

        /* the index goes into a TCAM command register field */
        if (msg->key.policy == SPL_POLICY && msg->spl < SPL_MAX &&
            msg->spl_index >= ACL_SPL_TB_SZ)
                return -EINVAL;

        /* set offset for host_extract_data when use spec_info */
        acl_set_host_offset(msg);

        data = kmalloc(sizeof(*data), GFP_KERNEL);
        if (!data)
                return -ENOMEM;
        memset(data, 0, sizeof(*data));

        data->is_eacl = msg->is_eacl;
        data->index = msg->index;

        if (msg->key.policy == SPL_POLICY && msg->spl < SPL_MAX) {
                data->spl = msg->spl;
                data->spl_index = msg->spl_index;
        } else {
                data->spl = -1;
        }
        acl_add_data(data, msg, fill_both, v4_mode, v6_mode);

        /* Check if we're inserting or appending.
         * If msg->index >= priv->size, it's an append (no rewrite required),
         * otherwise it's an insertion. */
        if (msg->index >= index) {
                index = msg->index;
                list_add_tail(&data->list, head);
                if (data->key) {
                        ret = acl_write(data, 0);
                        if (ret < 0) {
                                pr_err("%s table for ipv4 full!\n", msg->is_eacl ? "EACL" : "IACL");
                                return acl_add_undo(data, msg->is_eacl);
                        }
                }
                if (data->key6) {
                        ret = acl_write(data, 1);
                        if (ret < 0) {
                                pr_err("%s table for ipv6 full!\n", msg->is_eacl ? "EACL" : "IACL");
                                return acl_add_undo(data, msg->is_eacl);
                        }
                }
        } else {
                list_for_each_entry(pos, head, list) {
                        if (msg->index <= pos->index) {
                                list_add(&data->list, &pos->list);
                                list_swap(&data->list, &pos->list);
                                ret = acl_rewrite(msg->is_eacl);
                                if (ret < 0)
                                        return acl_add_undo(data, msg->is_eacl);
                                break;
                        }
                }
        }

        if (msg->is_eacl)
                priv->eacl_last_index = index;
        else
                priv->iacl_last_index = index;

        return 0;
err_table_full:
        pr_err("%s table full\n", msg->is_eacl ? "EACL" : "IACL");
        return -ENOSPC;
}
EXPORT_SYMBOL(acl_add);

int acl_del(struct acl_genl_msg *msg)
{
        struct acl_data *pos;
        struct list_head *head = &priv->iacl_list;

        if(msg->is_eacl)
                head = &priv->eacl_list;

        list_for_each_entry(pos, head, list) {
                if (msg->index == pos->index) {
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
                        return acl_rewrite(msg->is_eacl);
                }
       }

        pr_err("index %u not found\n", msg->index);
        return -ENOENT;
}
EXPORT_SYMBOL(acl_del);

void acl_set_mode(struct acl_genl_msg_add *msg)
{
        if (msg->is_eacl) {
                if (priv->ev4_mode != msg->v4_mode && msg->v4_mode <= DEFAULT_MODE_SET) {
                        priv->ev4_mode = msg->v4_mode;
                        acl_clear(DIR_EACL);
                        acl_mode_set(msg->is_eacl, priv->ev4_mode, IS_V4);
                }

                if (priv->ev6_mode != msg->v6_mode && msg->v6_mode <= DEFAULT_MODE_SET) {
                        priv->ev6_mode = msg->v6_mode;
                        acl_clear(DIR_EACL);
                        acl_mode_set(msg->is_eacl, priv->ev6_mode, IS_V6);
                }
        } else {
                if (priv->iv4_mode != msg->v4_mode && msg->v4_mode <= DEFAULT_MODE_SET) {
                        priv->iv4_mode = msg->v4_mode;
                        acl_clear(DIR_IACL);
                        acl_mode_set(msg->is_eacl, priv->iv4_mode, IS_V4);
                }

                if (priv->iv6_mode != msg->v6_mode && msg->v6_mode <= DEFAULT_MODE_SET) {
                        priv->iv6_mode = msg->v6_mode;
                        acl_clear(DIR_IACL);
                        acl_mode_set(msg->is_eacl, priv->iv6_mode, IS_V6);
                }
        }
}
EXPORT_SYMBOL(acl_set_mode);

static int acl_genl_msg_recv(struct genl_info *info, void *buf, size_t buflen)
{
        struct acl_genl_msg_add *msg = buf;
        int err = 0, i;

        if (WARN_ON_ONCE(!priv))
                return -EBUSY;

        switch (msg->method) {
        case ACL_ADD:
                err = acl_add(msg);
                sfgenl_msg_reply(info, &err, sizeof(err));
                break;
        case ACL_DEL:
                err = acl_del((struct acl_genl_msg *)msg);
                sfgenl_msg_reply(info, &err, sizeof(err));
                break;
        case ACL_CLEAR:
                /* reset pkt offset cfg */
                for (i = 0; i < PKG_OFFSET_CFG_CNT; i++)
                        sf_writel(priv->cpriv, ACL_PKT_OFFSET_CFG(i), 0);
                priv->cpriv->tcam_clean(priv->cpriv, TCAM_SPL);
                acl_clear((struct acl_genl_msg *)msg->is_eacl);
                sfgenl_msg_reply(info, &err, sizeof(err));
                break;
	case ACL_SET_MODE:
		acl_set_mode(msg);
		sfgenl_msg_reply(info, &err, sizeof(err));
		break;
	case ACL_DUMP:
		acl_dump_table(msg->is_eacl);
		sfgenl_msg_reply(info, &err, sizeof(err));
		break;
        case ACL_DUMP_LIST:
                acl_dump_list(msg->is_eacl);
                sfgenl_msg_reply(info, &err, sizeof(err));
                break;
        default:
                err = -EINVAL;
                sfgenl_msg_reply(info, &err, sizeof(err));
        }

        return err;
}


struct sfgenl_msg_ops acl_genl_msg_ops = {
        .msg_recv = acl_genl_msg_recv,
};

int acl_genl_init(struct acl_priv * apriv)
{
	priv = apriv;
        return sfgenl_ops_register(SF_GENL_COMP_ACL, &acl_genl_msg_ops);
}

int acl_genl_deinit(void)
{
        return sfgenl_msg_ops_unregister(SF_GENL_COMP_ACL);
}
