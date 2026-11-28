#include <linux/kernel.h>
#include <net/genetlink.h>

#include "sf_genl_msg.h"
#include "dpns_router.h"
#include "dpns_router_genl.h"


static struct dpns_router_priv *g_priv;

static int
router_genl_msg_recv(struct genl_info *info, void *buf, size_t buflen)
{
	struct dpns_router_priv *priv = g_priv;
	struct router_genl_msg *msg = buf;
        struct router_tbl_entry entry;
	int i, err = 0;
	u32 ipaddr;
        u8 temp;
	u8 req_addr;

	if(WARN_ON_ONCE(!priv))
		return -EBUSY;

	switch (msg->method) {
                case ROUTER_DUMP:
                        dump_dpns_router_tbl(priv->cpriv);
                        break;

                case ROUTER_TABLE_ADD:
                        entry.prefix_len = msg->prefix_len;
                        entry.next_hop_ptr = msg->next_hop_ptr;
                        entry.intf_index = msg->intf_index;
                        entry.ovport = msg->ovport;
                        entry.ovid = msg->ovid;
                        entry.req_id = msg->req_id;
                        req_addr = msg->req_addr;
                        entry.addr_len = V4_ADDR_LEN;
                        entry.req_addr = req_addr/2;
                        ipaddr = msg->ipaddr;
                        ipaddr = ntohl(ipaddr);
                        memcpy(entry.addr, (u8*)&ipaddr, sizeof(ipaddr));
                        ipaddr = ntohl(ipaddr);
                        L3_DBG(INFO_LV, "add router entry ipv4_dip:%pI4 dst_len:%u next_hop_ptr:%u "
                                        "intf_index:%u ovport:%u ovid:%u req_id:%u "
                                        "req_addr:%u\n", &ipaddr, entry.prefix_len,
                                        entry.next_hop_ptr, entry.intf_index,
                                        entry.ovport, entry.ovid, entry.req_id,
                                        req_addr);
                        dpns_router_table_add(priv->cpriv, &entry);
                        break;

                case ROUTER_TABLE_ADD_V6:
                        entry.prefix_len = msg->prefix_len;
                        entry.next_hop_ptr = msg->next_hop_ptr;
                        entry.intf_index = msg->intf_index;
                        entry.ovport = msg->ovport;
                        entry.ovid = msg->ovid;
                        entry.req_id = msg->req_id;
                        req_addr = msg->req_addr;
                        entry.addr_len = V6_ADDR_LEN;
                        entry.req_addr = req_addr/2;

                        for (i = 0; i < 8; i ++) {
                                temp = msg->ipaddr6[i];
                                msg->ipaddr6[i] = msg->ipaddr6[15 - i];
                                msg->ipaddr6[15 - i] = temp;
                        }

                        memcpy(entry.addr, (u8*) msg->ipaddr6, sizeof(uint8_t) * 16);

                        for (i = 0; i < 8; i ++) {
                                temp = msg->ipaddr6[i];
                                msg->ipaddr6[i] = msg->ipaddr6[15 - i];
                                msg->ipaddr6[15 - i] = temp;
                        }

                        L3_DBG(INFO_LV, "add router entry ipv6_dip: %pI6 dst_len:%u next_hop_ptr:%u "
                                        "intf_index:%u ovport:%u ovid:%u req_id:%u "
                                        "req_addr:%u\n", msg->ipaddr6, entry.prefix_len,
                                        entry.next_hop_ptr, entry.intf_index,
                                        entry.ovport, entry.ovid, entry.req_id,
                                        req_addr);
                        dpns_router_table_add(priv->cpriv, &entry);
                        break;

                case ROUTER_TABLE_DEL:
                        entry.req_id = msg->req_id;
                        req_addr = msg->req_addr;
                        entry.req_addr = req_addr/2;
                        entry.addr_len = V4_ADDR_LEN;
                        L3_DBG(INFO_LV, "del router entry req_id:%u req_addr:%u\n",
                                        entry.req_id, req_addr);
                        dpns_router_table_del(priv->cpriv, &entry);
                        break;

                case ROUTER_TABLE_DEL_V6:
                        entry.req_id = msg->req_id;
                        req_addr = msg->req_addr;
                        entry.req_addr = req_addr/2;
                        entry.addr_len = V6_ADDR_LEN;
                        L3_DBG(INFO_LV, "del router entry req_id:%u req_addr:%u\n",
                                        entry.req_id, req_addr);
                        dpns_router_table_del(priv->cpriv, &entry);
                        break;
	}

	sfgenl_msg_reply(info, &err, sizeof(err));

	return err;
}


static struct sfgenl_msg_ops router_genl_msg_ops = {
	.msg_recv = router_genl_msg_recv,
};

int dpns_router_genl_init(struct dpns_router_priv *priv)
{
	g_priv = priv;
	return sfgenl_ops_register(SF_GENL_COMP_ROUTER, &router_genl_msg_ops);
}

int dpns_router_genl_exit(void)
{
	return sfgenl_msg_ops_unregister(SF_GENL_COMP_ROUTER);
}
