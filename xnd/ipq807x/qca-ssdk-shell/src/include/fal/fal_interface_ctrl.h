/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_interface_ctrl FAL_INTERFACE_CONTROL
 * @{
 */
#ifndef _FAL_INTERFACECTRL_H_
#define _FAL_INTERFACECTRL_H_

#ifdef __cplusplus
extern "c" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"

    typedef enum {
        FAL_MAC_MODE_RGMII = 0,
        FAL_MAC_MODE_GMII,
        FAL_MAC_MODE_MII,
        FAL_MAC_MODE_SGMII,
        FAL_MAC_MODE_FIBER,
        FAL_MAC_MODE_RMII,
        FAL_MAC_MODE_DEFAULT
    }
    fal_interface_mac_mode_t;

    typedef enum
    {
        FAL_INTERFACE_CLOCK_MAC_MODE = 0,
        FAL_INTERFACE_CLOCK_PHY_MODE = 1,
    } fal_interface_clock_mode_t;

    typedef struct
    {
        a_bool_t   txclk_delay_cmd;
        a_bool_t   rxclk_delay_cmd;
        a_uint32_t txclk_delay_sel;
        a_uint32_t rxclk_delay_sel;
    } fal_mac_rgmii_config_t;

    typedef struct
    {
        a_bool_t   master_mode;
        a_bool_t   slave_mode;
        a_bool_t   clock_inverse;
        a_bool_t   pipe_rxclk_sel;
    } fal_mac_rmii_config_t;

    typedef struct
    {
        fal_interface_clock_mode_t   clock_mode;
        a_uint32_t                   txclk_select;
        a_uint32_t                   rxclk_select;
    } fal_mac_gmii_config_t;

    typedef struct
    {
        fal_interface_clock_mode_t   clock_mode;
        a_uint32_t                   txclk_select;
        a_uint32_t                   rxclk_select;
    } fal_mac_mii_config_t;

    typedef struct
    {
        fal_interface_clock_mode_t  clock_mode;
        a_bool_t                    auto_neg;
        a_bool_t                    force_speed;
        a_bool_t                    prbs_enable;
        a_bool_t                    rem_phy_lpbk;
    } fal_mac_sgmii_config_t;

    typedef struct
    {
        a_bool_t                    auto_neg;
        a_bool_t                    fx100_enable;
    } fal_mac_fiber_config_t;

    typedef struct
    {
        fal_interface_mac_mode_t      mac_mode;
        union
        {
            fal_mac_rgmii_config_t    rgmii;
            fal_mac_gmii_config_t     gmii;
            fal_mac_mii_config_t      mii;
            fal_mac_sgmii_config_t    sgmii;
            fal_mac_rmii_config_t    rmii;
            fal_mac_fiber_config_t    fiber;
        } config;
    } fal_mac_config_t;

    typedef struct
    {
        fal_interface_mac_mode_t mac_mode;
        a_bool_t                 txclk_delay_cmd;
        a_bool_t                 rxclk_delay_cmd;
        a_uint32_t               txclk_delay_sel;
        a_uint32_t               rxclk_delay_sel;
    } fal_phy_config_t;

    typedef enum
    {
	Fx100BASE_MODE = 2,
	Fx100BASE_BUTT = 0xffff,
    } fx100_ctrl_link_mode_t;

    typedef enum
    {
	FX100_SERDS_MODE = 1,
	Fx100_SERDS_BUTT = 0xffff,
    } sgmii_fiber_mode_t;

#define    FX100_HALF_DUPLEX  0
#define    FX100_FULL_DUPLEX  1

    typedef struct
    {
        fx100_ctrl_link_mode_t   link_mode;
        a_bool_t                 overshoot;
        a_bool_t                 loopback;
        a_bool_t                 fd_mode;
        a_bool_t                 col_test;
        sgmii_fiber_mode_t       sgmii_fiber_mode;
        a_bool_t                 crs_ctrl;
        a_bool_t                 loopback_ctrl;
        a_bool_t                 crs_col_100_ctrl;
        a_bool_t                 loop_en;
    } fal_fx100_ctrl_config_t;

    sw_error_t
    fal_port_3az_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

    sw_error_t
    fal_port_3az_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);

    sw_error_t
    fal_interface_mac_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config);

    sw_error_t
    fal_interface_mac_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config);

    sw_error_t
    fal_interface_phy_mode_set(a_uint32_t dev_id, a_uint32_t phy_id, fal_phy_config_t * config);

    sw_error_t
    fal_interface_phy_mode_get(a_uint32_t dev_id, a_uint32_t phy_id, fal_phy_config_t * config);

    sw_error_t
    fal_interface_fx100_ctrl_set(a_uint32_t dev_id, fal_fx100_ctrl_config_t * config);

    sw_error_t
    fal_interface_fx100_ctrl_get(a_uint32_t dev_id, fal_fx100_ctrl_config_t * config);

    sw_error_t
    fal_interface_fx100_status_get(a_uint32_t dev_id, a_uint32_t* status);

    sw_error_t
    fal_interface_mac06_exch_set(a_uint32_t dev_id,  a_bool_t enable);

    sw_error_t
    fal_interface_mac06_exch_get(a_uint32_t dev_id,  a_bool_t* enable);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_INTERFACECTRL_H_ */
/**
 * @}
 */
