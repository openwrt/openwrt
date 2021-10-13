/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isis_port_ctrl.h"
#include "isis_mib.h"
#include "isis_misc.h"
#include "isis_reg.h"
#include "f1_phy.h"

#include <linux/autoconf.h>
#include <linux/kthread.h>
#include <linux/in.h>
#include <linux/if.h>
#include <linux/inet.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/rculist_nulls.h>

static struct task_struct *mac_scan_task;
static a_bool_t qca_phy_info[7] = {A_FALSE};
static a_bool_t qca_portvlan_mem_info[7] = {A_FALSE};
static struct net_device *master_dev = NULL;
static fal_port_t uplink_portid =5;

static void qca_cpu_pkt_xmit(struct net_device *dev)
{
    if (dev)
    {
        arp_send(ARPOP_RREQUEST, ETH_P_RARP, 0, dev, 0, NULL,
                 dev->dev_addr, dev->dev_addr);
    }
}

static a_bool_t
_isis_port_phy_connected(a_uint32_t dev_id, fal_port_t port_id)
{
    if ((0 == port_id) || (6 == port_id))
    {
        return A_FALSE;
    }
    else
    {
        return A_TRUE;
    }
}



static sw_error_t qca_isis_rec_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
        fal_port_speed_t speed)
{
    sw_error_t rv;
    a_uint32_t reg_val;

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));

    if (FAL_SPEED_10 == speed)
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 0, reg_val);
        SW_SET_REG_BY_FIELD(PORT_STATUS, DUPLEX_MODE, 1, reg_val);
    }
    else if (FAL_SPEED_100 == speed)
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 1, reg_val);
        SW_SET_REG_BY_FIELD(PORT_STATUS, DUPLEX_MODE, 1, reg_val);
    }
    else
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 2, reg_val);
        SW_SET_REG_BY_FIELD(PORT_STATUS, DUPLEX_MODE, 1, reg_val);
    }

    HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));
}


static sw_error_t qca_isis_nor_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
        fal_port_speed_t speed, fal_port_duplex_t duplex)
{
    sw_error_t rv;
    a_uint32_t reg_val;

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));

    if (FAL_SPEED_10 == speed)
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 0, reg_val);
    }
    else if (FAL_SPEED_100 == speed)
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 1, reg_val);
    }
    else
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 2, reg_val);
    }

    if (duplex == FAL_FULL_DUPLEX)
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, DUPLEX_MODE, 1, reg_val);
    }
    else
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, DUPLEX_MODE, 0, reg_val);
    }

    HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));
}


static void qca_mac_ctrl_init(void)
{
    hsl_dev_t *pdev = NULL;
    fal_port_t port_id;
    a_uint32_t dev_id = 0, reg_val = 0, reg_save = 0;
    sw_error_t rv;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return;

    /* for port property set, SSDK should not generate some limitations */
    for (port_id = 0; port_id < pdev->nr_ports; port_id++)
    {
        if (A_FALSE == _isis_port_phy_connected(dev_id, port_id))
            continue;

        HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                          (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));

        SW_SET_REG_BY_FIELD(PORT_STATUS, LINK_EN, 0, reg_val);

        reg_save = reg_val;

        HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, port_id,
                          (a_uint8_t *) (&reg_save), sizeof (a_uint32_t));

    }

    isis_mib_status_set(dev_id, A_TRUE);
}

static void qca_mac_phy_poll(void)
{
    hsl_dev_t *pdev = NULL;
    fal_port_t port_id;
    a_uint32_t dev_id = 0, phy_id = 0;
    a_bool_t status, lastStatus;
    fal_mib_info_t counter;
    fal_port_speed_t speed;
    fal_port_duplex_t duplex;
    a_uint32_t txok = 0;
    sw_error_t rv;
    a_uint32_t reg_val;
    a_uint32_t index;
    fal_pbmp_t uplink_portvlanmem = 0;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return;


    /* for port property set, SSDK should not generate some limitations */
    for (port_id = 0; port_id < pdev->nr_ports; port_id++)
    {
        if (A_FALSE == _isis_port_phy_connected(dev_id, port_id))
            continue;

        rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
        if (rv != SW_OK)
            continue;

        status = f1_phy_get_link_status(dev_id, phy_id);

        lastStatus = qca_phy_info[phy_id];

        if (lastStatus)
        {
            /*phy from up to down, disable mac rx/tx*/
            if (!status)
            {

                /*make sure UNI port doesn't forward these types of pkts*/
                isis_port_bc_filter_set(dev_id, port_id, A_TRUE);
                isis_port_unk_mc_filter_set(dev_id, port_id, A_TRUE);
                isis_port_unk_uc_filter_set(dev_id, port_id, A_TRUE);

                /*make sure traffic from uplink port doesn't go to UNI port*/
                isis_portvlan_member_get(0, uplink_portid, &uplink_portvlanmem);
                if ((0x1UL<<port_id) & uplink_portvlanmem)
                {
                    isis_portvlan_member_del(dev_id, uplink_portid, port_id);
                    qca_portvlan_mem_info[port_id] =  A_TRUE;
                    printk("%s: del UNI port %d from uplink portID %d\n", __FUNCTION__, port_id, uplink_portid);
                }
                isis_port_force_portvlan_set(dev_id, uplink_portid, A_TRUE);

                /*make sure traffic from cpu port doesn't go to UNI port*/
                isis_port_force_portvlan_set(dev_id, 0, A_TRUE);
                isis_portvlan_member_del(dev_id, 0, port_id);

                index = 0;
                /*loop used to make sure that the packets in the buffer are sent out*/
                do
                {
                    index ++;
                }
                while (index < 100);

                /*disable UNI port rx/tx*/
                isis_port_txmac_status_set(dev_id, port_id, A_FALSE);
                isis_port_rxmac_status_set(dev_id, port_id, A_FALSE);
                qca_phy_info[phy_id] =  status;


                isis_get_mib_info(dev_id, port_id, &counter);
                txok = counter.Tx64Byte +
                       counter.Tx128Byte +
                       counter.Tx256Byte +
                       counter.Tx512Byte +
                       counter.Tx1024Byte +
                       counter.Tx1518Byte +
                       counter.TxMaxByte;
                printk("%s: port %d down,  counter %d\n", __FUNCTION__, port_id,
                       txok);
            }
        }
        else
        {
            /*phy from down to up, disable mac rx/tx*/
            if (status)
            {
                HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                                  (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));

                if (SW_OK == f1_phy_get_speed(dev_id, phy_id, &speed))
                    qca_isis_rec_port_speed_set(dev_id, port_id, speed);

                isis_port_txmac_status_set(dev_id, port_id, A_TRUE);

                /*make sure cpu port can send pkts to UNI port*/
                isis_portvlan_member_add(0, 0, port_id);
                isis_port_force_portvlan_set(0, 0, A_FALSE);

                /*detct port tx counts*/
                isis_get_mib_info(dev_id, port_id, &counter);
                txok = counter.Tx64Byte +
                       counter.Tx128Byte +
                       counter.Tx256Byte +
                       counter.Tx512Byte +
                       counter.Tx1024Byte +
                       counter.Tx1518Byte +
                       counter.TxMaxByte;
                printk("%s: counter before sending pkts %d for port %d\n", __FUNCTION__,
                       txok, port_id);

                /*make sure cpu port can send pkts to UNI port*/
                isis_port_bc_filter_set(dev_id, port_id, A_FALSE);
                isis_port_unk_mc_filter_set(dev_id, port_id, A_FALSE);
                isis_port_unk_uc_filter_set(dev_id, port_id, A_FALSE);

                do
                {
                    qca_cpu_pkt_xmit(master_dev);
                    memset(&counter, 0, sizeof(fal_mib_info_t));
                    isis_get_mib_info(dev_id, port_id, &counter);
                    txok = counter.Tx64Byte +
                           counter.Tx128Byte +
                           counter.Tx256Byte +
                           counter.Tx512Byte +
                           counter.Tx1024Byte +
                           counter.Tx1518Byte +
                           counter.TxMaxByte;
                    printk("%s: cpu send pkts for port_id %d\n", __FUNCTION__, port_id);
                }
                while(txok == 0);

                f1_phy_get_speed(dev_id, phy_id, &speed);
                f1_phy_get_duplex(dev_id, phy_id, &duplex);
                isis_port_txmac_status_set(dev_id, port_id, A_FALSE);
                isis_port_rxmac_status_set(dev_id, port_id, A_FALSE);
                qca_isis_nor_port_speed_set(dev_id, port_id, speed, duplex);
                isis_port_txmac_status_set(dev_id, port_id, A_TRUE);
                isis_port_rxmac_status_set(dev_id, port_id, A_TRUE);

                /*restore uplink port-base vlan state*/
                if (qca_portvlan_mem_info[port_id])
                {
                    isis_portvlan_member_add(0, uplink_portid, port_id);
                    qca_portvlan_mem_info[port_id] =  A_FALSE;
                    printk("%s: add UNI port %d to uplink portID %d\n", __FUNCTION__, port_id, uplink_portid);
                }
                isis_port_force_portvlan_set(0, uplink_portid, A_FALSE);

                qca_phy_info[phy_id] =  status;

            }

        }

    }
}

static a_int32_t
qca_mac_scan_thread(void *param)
{
#define QCA_MAC_SCAN_POLLING_MSEC         500

    qca_mac_ctrl_init();

    while(1)
    {

        qca_mac_phy_poll();

        msleep_interruptible(QCA_MAC_SCAN_POLLING_MSEC);
    }


    return 0;
}

void
qca_mac_scan_helper_init(void)
{
    const char mac_thread_name[] = "mac_scan";

    mac_scan_task = kthread_create(qca_mac_scan_thread, NULL, mac_thread_name);


    if(IS_ERR(mac_scan_task))
    {
        aos_printk("thread: %s create fail\n", mac_thread_name);
        return;
    }

    wake_up_process(mac_scan_task);

    printk("thread: %s create success pid:%d\n",
           mac_thread_name, mac_scan_task->pid);
}


void
qca_mac_scan_helper_exit(void)
{
    if(mac_scan_task)
    {
        kthread_stop(mac_scan_task);
    }
}

void qca_set_master_dev(struct net_device *dev)
{
    master_dev = dev;

}



