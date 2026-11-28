#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <linux/if_bridge.h>
#include <net/neighbour.h>
#include <net/arp.h>
#include <net/genetlink.h>

#include "sf_genl_msg.h"

#include "se_mcast.h"
#include "se_mcast_cfg.h"
#include "sf_genl_mcast.h"
#include "dpns_common.h"

extern MCAST_t *g_mcast;

static struct mcast_genl_msg *l3_mcast_genl_msg_make(int cmd, void *buf, size_t buflen)
{
        struct mcast_genl_msg *msg;

        if (cmd >= NUM_MC_MSG_CMDS)
                return NULL;

        msg = kvmalloc(mcast_genl_newsz(buf ? buflen : 0), GFP_KERNEL);
        if (!msg)
                return NULL;

        msg->cmd = cmd;
        msg->mc_type = MCAST_L3;

        if (buf) {
                msg->buflen = buflen;
                memcpy(msg->buf, buf, buflen);
        }

        return msg;
}

static int l3_mcast_add(se_l3_mcast_cfg_t *cfg)
{
        // TODO
        if (cfg->is_ipv6)
                return -ENOTSUPP;

        if (cfg->sip.ip4.d == 0)
                cfg->is_mcsg = 0;
        else
                cfg->is_mcsg = 1;

        return se_l3_mcast_add(cfg);
}

static int l3_mcast_del(se_l3_mcast_cfg_t *cfg)
{
        return se_l3_mcast_del(cfg);
}

static int l3_mcast_del_marked(char *mark)
{
        return se_l3_mcast_del_marked(mark);
}

static int l3_mcast_list(struct genl_info *info)
{
        struct mcast_genl_msg *msg = NULL;
        se_l3_mcast_cfg_t *list = NULL;
        size_t list_sz = 0;
        int err;

        if ((err = se_l3_mcast_list(&list, &list_sz)))
                return err;

        msg = l3_mcast_genl_msg_make(MC_MSG_CMD_LIST, list, list_sz);
        if (!msg) {
                err = -ENOMEM;
                goto free_list;
        }

        err = sfgenl_msg_reply(info, msg, mcast_genl_msglen(msg));
        if (err)
                MCAST_DBG(ERR_LV, "sfgenl_msg_reply() failed: %d\n", err);

        kvfree(msg);

free_list:
        if (list)
                kvfree(list);

        return err;
}

static int is_invalid_l3_mcast_msg(struct mcast_genl_msg *msg, se_l3_mcast_cfg_t *cfg)
{

        if (msg->buflen != se_l3_mcast_cfg_sz()) {
                MCAST_DBG(ERR_LV, "invalid message length\n");
                return 1;
        }

        if (cfg->oif_cnt > ARRAY_SIZE(cfg->oif)) {
                MCAST_DBG(ERR_LV, "invalid oif_cnt\n");
                return 1;
        }

        return 0;
}

static int arp_hwaddr_get(struct net_device *dev, __be32 ip, u8 *mac)
{
        struct neighbour *neigh;

        if (!dev || !mac)
                return -EINVAL;

        neigh = neigh_lookup(&arp_tbl, &ip, dev);
        if (!neigh) {
                MCAST_DBG(DBG_LV, "arp entry is not found for %pI4 dev %s\n", &ip, dev->name);
                return -ENOENT;
        }

        read_lock(&neigh->lock);

        // sizeof(neigh->ha) = 32 (MAX_ADDR_LEN)
        memcpy(mac, neigh->ha, ETH_ALEN);

        read_unlock(&neigh->lock);

        return 0;
}

static struct net_device *bridge_port_get_by_arp(struct net_device *br, __be32 ip, u16 vid)
{
        struct net_device *port = NULL;
        u8 mac[ETH_ALEN] = { };

        if (arp_hwaddr_get(br, ip, mac))
                return NULL;

        rtnl_lock();
        port = br_fdb_find_port(br, mac, vid);
        rtnl_unlock();

        return port;
}

static int l3_mcast_iif_update(se_l3_mcast_cfg_t *cfg)
{
        struct net_device *iif;

        iif = dev_get_by_index(&init_net, cfg->iif);
        if (!iif) {
               MCAST_DBG(INFO_LV, "iif %u is not found\n", cfg->iif);
                return -ENODEV;
        }

        if (netif_is_bridge_master(iif)) {
                // FIXME: vid hardcoded
                struct net_device *br_port = bridge_port_get_by_arp(iif, htonl(cfg->sip.ip4.d), 0);

                if (!br_port) {
                        MCAST_DBG(INFO_LV,"failed to lookup bridge port by arp\n");
                        return -ENOENT;
                }

                MCAST_DBG(INFO_LV,"change iif %s to %s\n", iif->name, br_port->name);

                cfg->iif = br_port->ifindex;
        }

        return 0;
}

void dpns_mcast_ubus_handler(struct work_struct *work)
{
        struct mcast_ubus_work *ubus_work = container_of(work, struct mcast_ubus_work, work);
        struct mcast_genl_msg *msg = ubus_work->msg;
        se_l3_mcast_cfg_t cfg = ubus_work->cfg;
        int err = 0;
        char mark[SE_MCAST_MARK_SZ];

        if (msg->cmd == MC_MSG_CMD_ADD || msg->cmd == MC_MSG_CMD_DEL) {
                if (is_invalid_l3_mcast_msg(msg, &cfg)) {
                        err = -EINVAL;
                        MCAST_DBG(ERR_LV, "ubus call dpns.l3_mcast add/del err: %pe\n", ERR_PTR(err));
                        goto err;
                }

                if ((err = l3_mcast_iif_update(&cfg))) {
                        MCAST_DBG(ERR_LV, "ubus call dpns.l3_mcast add/del err: %pe\n", ERR_PTR(err));
                        goto err;
                }

        }

        switch (msg->cmd) {
                case MC_MSG_CMD_DEL_MARKED:
                        if (msg->buflen != 0){
                                memset(mark, 0, sizeof(mark));
                                strncpy(mark, msg->buf, msg->buflen);
                                err = l3_mcast_del_marked(mark);
                                if (err < 0)
                                        MCAST_DBG(ERR_LV, "ubus call dpns.l3_mcast del_marked err: %pe\n", ERR_PTR(err));
                        }
                        break;
                case MC_MSG_CMD_ADD:
                        err = l3_mcast_add(&cfg);
                        if (err < 0)
                                MCAST_DBG(ERR_LV, "ubus call dpns.l3_mcast add err: %pe\n", ERR_PTR(err));
                        break;
                case MC_MSG_CMD_DEL:
                        err = l3_mcast_del(&cfg);
                        if (err < 0)
                                MCAST_DBG(ERR_LV, "ubus call dpns.l3_mcast del err: %pe\n", ERR_PTR(err));
                        break;
        }

err:

        mcast_kfree(ubus_work->msg);
        mcast_kfree(ubus_work);
}

static int l3_mcast_msg_recv(struct genl_info *info, struct mcast_genl_msg *msg)
{
        struct mcast_ubus_work *ubus_work;
        int err = 0;

        if (msg->cmd == MC_MSG_CMD_LIST) {
                if ((err = l3_mcast_list(info))) {
                        if (err < 0)
                                MCAST_DBG(ERR_LV, "ubus call dpns.l3_mcast list err: %pe\n", ERR_PTR(err));
                        goto err_mcast_list;
                }
        }

        ubus_work = mcast_kzalloc(sizeof(*ubus_work), GFP_ATOMIC);
        if (!ubus_work)
                return -ENOMEM;

        ubus_work->msg = mcast_kzalloc(sizeof(struct mcast_genl_msg), GFP_ATOMIC);
        if (!ubus_work->msg)
                goto err_msg_alloc;

        if (msg->cmd == MC_MSG_CMD_ADD || msg->cmd == MC_MSG_CMD_DEL) {
                memcpy(&ubus_work->cfg, msg->buf, sizeof(se_l3_mcast_cfg_t));
        }

        INIT_WORK(&ubus_work->work, dpns_mcast_ubus_handler);

        ubus_work->priv = g_mcast;
        *(ubus_work->msg) = *msg;

        queue_work(g_mcast->ubus_wq, &ubus_work->work);

        sfgenl_msg_reply(info, &err, sizeof(err));

        return err;

err_msg_alloc:
        mcast_kfree(ubus_work);
        err = -ENOMEM;
err_mcast_list:
        sfgenl_msg_reply(info, &err, sizeof(err));
        return err;
}

static int mcast_msg_recv(struct genl_info *info, void *buf, size_t buflen)
{
        struct mcast_genl_msg *msg = buf;
        int err = 0;

        if (is_invalid_mcast_msg(msg, buflen)) {
                MCAST_DBG(ERR_LV, "invalid message length\n");
                return -EINVAL;
        }

        switch (msg->mc_type) {
        case MCAST_L2:
                break;

        case MCAST_L3:
                err = l3_mcast_msg_recv(info, msg);
                break;

        default:
                break;
        }

        return err;
}

static struct sfgenl_msg_ops mcast_msg_ops = {
        .msg_recv = mcast_msg_recv,
};

int se_mcast_genl_init(void)
{
        return sfgenl_ops_register(SF_GENL_COMP_MCAST, &mcast_msg_ops);
}

int se_mcast_genl_exit(void)
{
        return sfgenl_msg_ops_unregister(SF_GENL_COMP_MCAST);
}
