#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/unaligned.h>
#include "dpns_common.h"
#include "se_mcast.h"

extern MCAST_t* g_mcast;
extern u32 mcsg_blk_id;
/**
 * debug interfaces
 */

static int tcam_mcsg_add_set(const char *buf, const struct kernel_param *kp)
{
        tcam_blk_idx_t idx;
        se_l3_mcsg_rule_t r = { };
        u32 dip, sip, iport, oport_bitmap, ovid, intf_idx;

        if (8 != sscanf(buf, "item=%u slice=%u dip=%x sip=%x iport=%u intf_idx=%u oport_bitmap=%x ovid=%u",
                        &idx.item, &idx.slice, &dip, &sip, &iport, &intf_idx, &oport_bitmap, &ovid)) {
                return -EINVAL;
        }

        r.intf_idx = intf_idx;
        r.dip = dip;
        r.sip = sip;
        r.iport_id = iport;
        r.oport_bitmap = oport_bitmap;
        r.ovid = ovid;

        return se_l3_mcsg_write(&r, &idx);
}

static const struct kernel_param_ops tcam_mcsg_add_param_ops = {
        .set = tcam_mcsg_add_set,
};

module_param_cb(tcam_mcsg_add, &tcam_mcsg_add_param_ops, NULL, 0600);

static int tcam_mcsg_del_set(const char *buf, const struct kernel_param *kp)
{
        tcam_blk_idx_t tcam_idx;

        if (2 != sscanf(buf, "item=%u slice=%u", &tcam_idx.item, &tcam_idx.slice))
                return -EINVAL;

        return se_l3_mcsg_clear(&tcam_idx);
}

static const struct kernel_param_ops tcam_mcsg_del_param_ops = {
        .set = tcam_mcsg_del_set,
};

module_param_cb(tcam_mcsg_clear, &tcam_mcsg_del_param_ops, NULL, 0600);

static tcam_blk_idx_t tcam_mcsg_read_idx;

static int tcam_mcsg_read_set(const char *buf, const struct kernel_param *kp)
{
        if (2 != sscanf(buf, "item=%u slice=%u", &tcam_mcsg_read_idx.item, &tcam_mcsg_read_idx.slice))
                return -EINVAL;

        return 0;
}

static void tcam_read_slice(u8 *p, u8 req_id, u8 req_addr)
{
        sf_writel(g_mcast, SE_TCAM_OPT_ADDR, FIELD_PREP(TCAM_OPT_ID, req_id) |
                                 FIELD_PREP(TCAM_OPT_REQ_ADDR, req_addr));
        g_mcast->cpriv->se_wait(g_mcast->cpriv, SE_TCAM_OPT_ADDR, TCAM_OPT_BUSY);

        put_unaligned(sf_readq(g_mcast, SE_TCAM_TB_RDDATA_LO), (u64 *)p);
        p[8] = sf_readb(g_mcast, SE_TCAM_TB_RDDATA_HI);
}

static int tcam_read(u8 *p, size_t sz, u8 blk_id, u8 req_id, u8 req_addr)
{
        u32 i /* slice */, cnt;

        cnt = (sz / 9);

        if (cnt > 8) {
                MCAST_DBG(ERR_LV, "tcam slice cannot hold such size\n");
                return -E2BIG;
        }

        if (sz % 9) {
                MCAST_DBG(ERR_LV, "size is not slice aligned\n");
                return -EINVAL;
        }

        for (i = 0; i < cnt; i++, p += 9)
                tcam_read_slice(p, TCAM_BLK_REQ_ID(blk_id, (req_id + i)), req_addr);

        return 0;
}

static int se_l3_mcsg_read(se_l3_mcsg_rule_t rule[2], tcam_blk_idx_t *idx)
{
        u32 req_addr = 2 * idx->item;
        u32 req_id = idx->slice;

        if (!is_valid_tcam_idx(idx))
                return -EINVAL;

        tcam_read((u8 *)&rule[0], sizeof(se_l3_mcsg_rule_t), mcsg_blk_id, req_id, req_addr);
        tcam_read((u8 *)&rule[1], sizeof(se_l3_mcsg_rule_t), mcsg_blk_id, req_id, req_addr + 1);

        return 0;
}

static int tcam_mcsg_read_get(char *buf, const struct kernel_param *kp)
{
        se_l3_mcsg_rule_t r[2] = { };
        int len = 0;
        int err;

        if ((err = se_l3_mcsg_read(r, &tcam_mcsg_read_idx)))
                return err;

        len += scnprintf(buf + len, PAGE_SIZE - len,
                         "rule: intf: %u oport_bitmap: 0x%08llx iport: %u dip: 0x%08x sip: 0x%08x ovid: %u\n",
                         r[0].intf_idx, (u64){r[0].oport_bitmap}, r[0].iport_id, r[0].dip, r[0].sip, r[0].ovid);
        len += scnprintf(buf + len, PAGE_SIZE - len,
                         "mask: intf: %u oport_bitmap: 0x%08llx iport: %u dip: 0x%08x sip: 0x%08x ovid: %u\n",
                         r[1].intf_idx, (u64){r[1].oport_bitmap}, r[1].iport_id, r[1].dip, r[1].sip, r[1].ovid);

        return len;
}

static const struct kernel_param_ops tcam_mcsg_read_param_ops = {
        .set = tcam_mcsg_read_set,
        .get = tcam_mcsg_read_get,
};

module_param_cb(tcam_mcsg_read, &tcam_mcsg_read_param_ops, NULL, 0600);

static int tcam_mcsg_dump_item = 0;

static int tcam_mcsg_dump_set(const char *buf, const struct kernel_param *kp)
{
        if (1 != sscanf(buf, "item=%u", &tcam_mcsg_dump_item))
                return -EINVAL;

        return 0;
}

static int tcam_mcsg_dump_get(char *buf, const struct kernel_param *kp)
{
        tcam_blk_idx_t idx = { .item = tcam_mcsg_dump_item };
        se_l3_mcsg_rule_t r[2];
        int len = 0;
        int s;

        len += scnprintf(buf + len, PAGE_SIZE - len, "item slice intf iport        sip        dip oport_bitmap ovid\n");

        for (s = 0; s < TCAM_SLICES_PER_ITEM; s += 2) {
                int err;

                idx.slice = s;

                if ((err = se_l3_mcsg_read(r, &idx))) {
                        pr_err("failed to read item: %u slice: %u\n", idx.item, idx.slice);
                        return err;
                }

                len += scnprintf(buf + len, PAGE_SIZE - len,
                                 "%4d %5d %4u %5u 0x%08x 0x%08x  0x%09llx %4u\n",
                                 idx.item, idx.slice, r[0].intf_idx, r[0].iport_id,
                                 r[0].sip, r[0].dip, (u64){r[0].oport_bitmap}, (u16){r[0].ovid});
                len += scnprintf(buf + len, PAGE_SIZE - len,
                                 "%4d %5d %4u %5u 0x%08x 0x%08x  0x%09llx %4u\n",
                                 idx.item, idx.slice, r[1].intf_idx, r[1].iport_id,
                                 r[1].sip, r[1].dip, (u64){r[1].oport_bitmap}, (u16){r[1].ovid});
        }

        if (len >= PAGE_SIZE)
                pr_err("buffer is full\n");

        return len;
}

static const struct kernel_param_ops tcam_mcsg_dump_param_ops = {
        .set = tcam_mcsg_dump_set,
        .get = tcam_mcsg_dump_get,
};

module_param_cb(tcam_mcsg_dump, &tcam_mcsg_dump_param_ops, NULL, 0600);
