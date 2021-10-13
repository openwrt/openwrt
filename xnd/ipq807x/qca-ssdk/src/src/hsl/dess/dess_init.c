/*
 * Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
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
 * @defgroup dess_init DESS_INIT
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "dess_mib.h"
#include "dess_port_ctrl.h"
#include "dess_portvlan.h"
#include "dess_vlan.h"
#include "dess_fdb.h"
#include "dess_qos.h"
#include "dess_mirror.h"
#include "dess_stp.h"
#include "dess_rate.h"
#include "dess_misc.h"
#include "dess_leaky.h"
#include "dess_igmp.h"
#include "dess_acl.h"
#include "dess_led.h"
#include "dess_cosmap.h"
#include "dess_ip.h"
#include "dess_nat.h"
#if defined(IN_NAT_HELPER)
#include "dess_nat_helper.h"
#endif
#include "dess_sec.h"
#include "dess_trunk.h"
#include "dess_interface_ctrl.h"
#include "dess_reg_access.h"
#include "dess_reg.h"
#include "dess_init.h"
#include <malibu_phy.h>


static ssdk_init_cfg * dess_cfg[SW_MAX_NR_DEV] = { 0 };
a_uint32_t dess_nat_global_status = 0;

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
/* For isis there are five internal PHY devices and seven MAC devices.
   PORT0 always connect to external DMA device.
   MAC1..MAC4 connect to internal PHY0..PHY3.
*/


a_uint32_t dess_pbmp_5[PORT_WRAPPER_MAX] = {
	((1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5)), /*PORT_WRAPPER_PSGMII*/
	((1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5)), /*PORT_WRAPPER_PSGMII_RGMII5*/
	((1<<1) | (1<<5)),                            /*PORT_WRAPPER_SGMII0_RGMII5*/
	((1<<2) | (1<<5)),                            /*PORT_WRAPPER_SGMII1_RGMII5*/
	((1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5)), /*PORT_WRAPPER_PSGMII_RMII0*/
	((1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5)), /*PORT_WRAPPER_PSGMII_RMII1*/
	((1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5)), /*PORT_WRAPPER_PSGMII_RMII0_RMII1*/
	((1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5)), /*PORT_WRAPPER_SGMII_RGMII4*/
	((1<<1) | (1<<4)),                            /*PORT_WRAPPER_SGMII0_RGMII4*/
	((1<<2) | (1<<4)),                            /*PORT_WRAPPER_SGMII1_RGMII4*/
	((1<<5) | (1<<4)),                            /*PORT_WRAPPER_SGMII4_RGMII4*/
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	((1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5)), /*PORT_WRAPPER_PSGMII_FIBER*/
	};

a_uint32_t dess_pbmp_2[PORT_WRAPPER_MAX] = {
	((1<<4) | (1<<5)), 				/*PORT_WRAPPER_PSGMII*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	((1<<4) | (1<<5)), 				/*PORT_WRAPPER_PSGMII_FIBER*/
	};

a_uint32_t dess_get_port_phy_id(void)
{
	return dess_cfg[0]->phy_id;
}

enum ssdk_port_wrapper_cfg dess_get_port_config(void)
{
	return dess_cfg[0]->mac_mode;
}

a_bool_t dess_mac_port_valid_check(fal_port_t port_id)
{
	a_uint32_t bitmap = 0;
	enum ssdk_port_wrapper_cfg cfg;
	a_uint32_t phy_id;

	cfg = dess_get_port_config();
	phy_id = dess_get_port_phy_id();

	if (phy_id == MALIBU_1_1_2PORT)
		bitmap = dess_pbmp_2[cfg];
	else {
		bitmap = dess_pbmp_5[cfg];
	}
	return SW_IS_PBMP_MEMBER(bitmap, port_id);

}

static sw_error_t
dess_portproperty_init(a_uint32_t dev_id)
{
    hsl_port_prop_t p_type;
    hsl_dev_t *pdev = NULL;
    fal_port_t port_id;
    enum ssdk_port_wrapper_cfg cfg;
    a_uint32_t bitmap = 0;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

    cfg = dess_get_port_config();
    bitmap = dess_pbmp_5[cfg];

    /* for port property set, SSDK should not generate some limitations */
    for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id++)
    {
        if((!SW_IS_PBMP_MEMBER(bitmap, port_id)) && (port_id != pdev->cpu_port_nr))
            continue;

        hsl_port_prop_portmap_set(dev_id, port_id);

        for (p_type = HSL_PP_PHY; p_type < HSL_PP_BUTT; p_type++)
        {

            switch (p_type)
            {
                case HSL_PP_PHY:
                    /* Only port0 without PHY device */
                    if (port_id != pdev->cpu_port_nr)
                    {
						if(SW_IS_PBMP_MEMBER(bitmap, port_id))
                        	SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                    }
                    break;

                case HSL_PP_INCL_CPU:
                    /* include cpu port but exclude wan port in some cases */
                    /* but which port is wan port, we are no meaning */
					if (port_id == pdev->cpu_port_nr)
						SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
					if(SW_IS_PBMP_MEMBER(bitmap, port_id))
                    	SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                    break;

                case HSL_PP_EXCL_CPU:
                    /* exclude cpu port and wan port in some cases */
                    /* which port is wan port, we are no meaning but port0 is
                       always CPU port */
                    if (port_id != pdev->cpu_port_nr)
                    {
						if(SW_IS_PBMP_MEMBER(bitmap, port_id))
                        	SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                    }
                    break;

                default:
                    break;
            }
        }

	if (port_id != pdev->cpu_port_nr)
	{
		SW_RTN_ON_ERROR(hsl_port_prop_set_phyid
			(dev_id, port_id, port_id - 1));
	}
    }

    return SW_OK;
}
#endif

static sw_error_t
dess_dev_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    a_uint32_t entry = 0;
    sw_error_t rv;
    hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

    HSL_REG_FIELD_GET(rv, dev_id, MASK_CTL, 0, DEVICE_ID,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (DESS_DEVICE_ID == entry)
    {
	a_uint32_t i = 0, port_nr = 0, tmp = 0;
	tmp = cfg->port_cfg.lan_bmp | cfg->port_cfg.wan_bmp;
	for(i = 0; i < SW_MAX_NR_PORT; i++) {
		if(tmp & (1 << i)) {
			port_nr++;
		}
	}
	pdev->nr_phy = port_nr;
	for(i = 0; i < SW_MAX_NR_PORT; i++) {
		if(cfg->port_cfg.cpu_bmp & (1 << i)) {
			port_nr++;
			pdev->cpu_port_nr = i;
			break;
		}
	}
	if(i >= SW_MAX_NR_PORT)
		return SW_BAD_VALUE;
	pdev->nr_ports = port_nr;
	pdev->nr_vlans = 4096;
	pdev->hw_vlan_query = A_TRUE;
	pdev->nr_queue = port_nr;
	pdev->cpu_mode = cfg->cpu_mode;
	pdev->wan_bmp = cfg->port_cfg.wan_bmp;
    }

    return SW_OK;
}

sw_error_t
dess_cleanup(a_uint32_t dev_id)
{
    sw_error_t rv;

    if (dess_cfg[dev_id])
    {
#if defined(IN_NAT_HELPER)
        if(dess_nat_global_status)
            DESS_NAT_HELPER_CLEANUP(rv, dev_id);
#endif

        DESS_ACL_CLEANUP(rv, dev_id);

        SW_RTN_ON_ERROR(hsl_port_prop_cleanup_by_dev(dev_id));

        aos_mem_free(dess_cfg[dev_id]);
        dess_cfg[dev_id] = NULL;
    }

    return SW_OK;
}

/**
 * @brief Init hsl layer.
 * @details Comments:
 *   This operation will init hsl layer and hsl layer
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t
dess_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (NULL == dess_cfg[dev_id])
    {
        dess_cfg[dev_id] = aos_mem_alloc(sizeof (ssdk_init_cfg));
    }

    if (NULL == dess_cfg[dev_id])
    {
        return SW_OUT_OF_MEM;
    }

    aos_mem_copy(dess_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));

    SW_RTN_ON_ERROR(dess_reg_access_init(dev_id, cfg->reg_mode));

    SW_RTN_ON_ERROR(dess_dev_init(dev_id, cfg));

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    {
        sw_error_t rv;

        SW_RTN_ON_ERROR(hsl_port_prop_init(dev_id));
        SW_RTN_ON_ERROR(hsl_port_prop_init_by_dev(dev_id));
        SW_RTN_ON_ERROR(dess_portproperty_init(dev_id));

        DESS_MIB_INIT(rv, dev_id);
        DESS_PORT_CTRL_INIT(rv, dev_id);
        DESS_PORTVLAN_INIT(rv, dev_id);
        DESS_VLAN_INIT(rv, dev_id);
        DESS_FDB_INIT(rv, dev_id);
        DESS_QOS_INIT(rv, dev_id);
        DESS_STP_INIT(rv, dev_id);
        DESS_MIRR_INIT(rv, dev_id);
        DESS_RATE_INIT(rv, dev_id);
        DESS_MISC_INIT(rv, dev_id);
        DESS_LEAKY_INIT(rv, dev_id);
        DESS_IGMP_INIT(rv, dev_id);
        DESS_ACL_INIT(rv, dev_id);
        DESS_LED_INIT(rv, dev_id);
        DESS_COSMAP_INIT(rv, dev_id);
        DESS_IP_INIT(rv, dev_id);
        DESS_NAT_INIT(rv, dev_id);
        DESS_TRUNK_INIT(rv, dev_id);
        DESS_SEC_INIT(rv, dev_id);
        DESS_INTERFACE_CTRL_INIT(rv, dev_id);

        {
            hsl_api_t *p_api;

            SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
            p_api->dev_clean   = dess_cleanup;
        }
#if 0
#if defined(IN_NAT_HELPER)
		if(!dess_nat_global_status) {
        	DESS_NAT_HELPER_INIT(rv, dev_id);
			dess_nat_global_status = 1;
		}
#endif
#endif
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

