/*
 * Copyright (c) 2012, 2015-2019, The Linux Foundation. All rights reserved.
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

/*qca808x_start*/
/**
 * @defgroup fal_port_ctrl FAL_PORT_CONTROL
 * @{
 */
#include "sw.h"
#include "fal_port_ctrl.h"
#include "hsl_api.h"
/*qca808x_end*/
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>
/*qca808x_start*/
static sw_error_t
_fal_port_duplex_set (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_duplex_t duplex)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_duplex_set != NULL) {
        rv = p_adpt_api->adpt_port_duplex_set(dev_id, port_id, duplex);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_duplex_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_duplex_set (dev_id, port_id, duplex);
  return rv;
}

static sw_error_t
_fal_port_speed_set (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_speed_t speed)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_speed_set != NULL) {
        rv = p_adpt_api->adpt_port_speed_set(dev_id, port_id, speed);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_speed_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_speed_set (dev_id, port_id, speed);
  return rv;
}
/*qca808x_end*/
static sw_error_t
_fal_port_flowctrl_set (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_flowctrl_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_flowctrl_set(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_flowctrl_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_flowctrl_set (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_flowctrl_forcemode_set (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
  adpt_api_t *p_adpt_api;

  if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
    if (NULL == p_adpt_api->adpt_port_flowctrl_forcemode_set)
      return SW_NOT_SUPPORTED;

  rv = p_adpt_api->adpt_port_flowctrl_forcemode_set(dev_id, port_id, enable);
  return rv;
  }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_flowctrl_forcemode_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_flowctrl_forcemode_set (dev_id, port_id, enable);
  return rv;
}
/*qca808x_start*/
static sw_error_t
_fal_port_speed_get (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_speed_t * pspeed)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_speed_get != NULL) {
        rv = p_adpt_api->adpt_port_speed_get(dev_id, port_id, pspeed);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_speed_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_speed_get (dev_id, port_id, pspeed);
  return rv;
}

static sw_error_t
_fal_port_duplex_get (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_duplex_t * pduplex)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_duplex_get != NULL) {
        rv = p_adpt_api->adpt_port_duplex_get(dev_id, port_id, pduplex);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_duplex_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_duplex_get (dev_id, port_id, pduplex);
  return rv;
}

static sw_error_t
_fal_port_autoneg_enable (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_autoneg_enable != NULL) {
        rv = p_adpt_api->adpt_port_autoneg_enable(dev_id, port_id);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_autoneg_enable)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_autoneg_enable (dev_id, port_id);
  return rv;
}

static sw_error_t
_fal_port_autoneg_restart (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_autoneg_restart != NULL) {
        rv = p_adpt_api->adpt_port_autoneg_restart(dev_id, port_id);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_autoneg_restart)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_autoneg_restart (dev_id, port_id);
  return rv;
}


static sw_error_t
_fal_port_autoneg_adv_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_uint32_t autoadv)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_autoneg_adv_set != NULL) {
        rv = p_adpt_api->adpt_port_autoneg_adv_set(dev_id, port_id, autoadv);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_autoneg_adv_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_autoneg_adv_set (dev_id, port_id, autoadv);
  return rv;
}

static sw_error_t
_fal_port_autoneg_status_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * status)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_autoneg_status_get != NULL) {
        rv = p_adpt_api->adpt_port_autoneg_status_get(dev_id, port_id, status);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_autoneg_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_autoneg_status_get (dev_id, port_id, status);
  return rv;
}

static sw_error_t
_fal_port_autoneg_adv_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_uint32_t * autoadv)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_autoneg_adv_get != NULL) {
        rv = p_adpt_api->adpt_port_autoneg_adv_get(dev_id, port_id, autoadv);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_autoneg_adv_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_autoneg_adv_get (dev_id, port_id, autoadv);
  return rv;
}
/*qca808x_end*/
#ifndef IN_PORTCONTROL_MINI
static sw_error_t
_fal_port_hdr_status_set (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_hdr_status_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_hdr_status_set (dev_id, port_id, enable);
  return rv;
}


static sw_error_t
_fal_port_hdr_status_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_hdr_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_hdr_status_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_flowctrl_get (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_flowctrl_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_flowctrl_get(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_flowctrl_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_flowctrl_get (dev_id, port_id, enable);
  return rv;
}



static sw_error_t
_fal_port_flowctrl_forcemode_get (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
  adpt_api_t *p_adpt_api;

  if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
    if (NULL == p_adpt_api->adpt_port_flowctrl_forcemode_get)
      return SW_NOT_SUPPORTED;

  rv = p_adpt_api->adpt_port_flowctrl_forcemode_get(dev_id, port_id, enable);
  return rv;
  }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_flowctrl_forcemode_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_flowctrl_forcemode_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_powersave_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_powersave_set != NULL) {
        rv = p_adpt_api->adpt_port_powersave_set(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_powersave_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_powersave_set (dev_id, port_id, enable);
  return rv;
}


static sw_error_t
_fal_port_powersave_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_powersave_get != NULL) {
        rv = p_adpt_api->adpt_port_powersave_get(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_powersave_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_powersave_get (dev_id, port_id, enable);
  return rv;
}
/*qca808x_start*/
static sw_error_t
_fal_port_hibernate_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_hibernate_set != NULL) {
        rv = p_adpt_api->adpt_port_hibernate_set(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_hibernate_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_hibernate_set (dev_id, port_id, enable);
  return rv;
}


static sw_error_t
_fal_port_hibernate_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_hibernate_get != NULL) {
        rv = p_adpt_api->adpt_port_hibernate_get(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_hibernate_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_hibernate_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_cdt (a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
	       a_uint32_t * cable_status, a_uint32_t * cable_len)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_cdt != NULL) {
        rv = p_adpt_api->adpt_port_cdt(dev_id, port_id, mdi_pair, cable_status, cable_len);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_cdt)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_cdt (dev_id, port_id, mdi_pair, cable_status, cable_len);
  return rv;
}
/*qca808x_end*/
static sw_error_t
_fal_port_rxhdr_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_header_mode_t * mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_rxhdr_mode_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_rxhdr_mode_get (dev_id, port_id, mode);
  return rv;
}
static sw_error_t
_fal_port_txhdr_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_header_mode_t * mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_txhdr_mode_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_txhdr_mode_get (dev_id, port_id, mode);
  return rv;
}
static sw_error_t
_fal_header_type_get (a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * type)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->header_type_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->header_type_get (dev_id, enable, type);
  return rv;
}
#endif
static sw_error_t
_fal_port_rxhdr_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_header_mode_t mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_rxhdr_mode_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_rxhdr_mode_set (dev_id, port_id, mode);
  return rv;
}



static sw_error_t
_fal_port_txhdr_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_header_mode_t mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_txhdr_mode_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_txhdr_mode_set (dev_id, port_id, mode);
  return rv;
}



static sw_error_t
_fal_header_type_set (a_uint32_t dev_id, a_bool_t enable, a_uint32_t type)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->header_type_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->header_type_set (dev_id, enable, type);
  return rv;
}



static sw_error_t
_fal_port_txmac_status_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_txmac_status_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_txmac_status_set(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_txmac_status_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_txmac_status_set (dev_id, port_id, enable);
  return rv;
}



static sw_error_t
_fal_port_rxmac_status_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_rxmac_status_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_rxmac_status_set(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_rxmac_status_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_rxmac_status_set (dev_id, port_id, enable);
  return rv;
}



static sw_error_t
_fal_port_txfc_status_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_txfc_status_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_txfc_status_set(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_txfc_status_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_txfc_status_set (dev_id, port_id, enable);
  return rv;
}



static sw_error_t
_fal_port_rxfc_status_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_rxfc_status_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_rxfc_status_set(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_rxfc_status_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_rxfc_status_set (dev_id, port_id, enable);
  return rv;
}
static sw_error_t
_fal_port_txfc_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_txfc_status_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_txfc_status_get(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_txfc_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_txfc_status_get (dev_id, port_id, enable);
  return rv;
}
static sw_error_t
_fal_port_rxfc_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_rxfc_status_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_rxfc_status_get(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_rxfc_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_rxfc_status_get (dev_id, port_id, enable);
  return rv;
}
/*qca808x_start*/
static sw_error_t
_fal_port_link_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * status)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_link_status_get != NULL) {
        rv = p_adpt_api->adpt_port_link_status_get(dev_id, port_id, status);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_link_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_link_status_get (dev_id, port_id, status);
  return rv;
}

static sw_error_t
_fal_port_power_off (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_power_off != NULL) {
        rv = p_adpt_api->adpt_port_power_off(dev_id, port_id);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_power_off)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_power_off (dev_id, port_id);
  return rv;
}

static sw_error_t
_fal_port_power_on (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_power_on != NULL) {
        rv = p_adpt_api->adpt_port_power_on(dev_id, port_id);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_power_on)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_power_on (dev_id, port_id);
  return rv;
}
/*qca808x_end*/
static sw_error_t
_fal_port_link_forcemode_set (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_link_forcemode_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_link_forcemode_set (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_link_forcemode_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_link_forcemode_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_link_forcemode_get (dev_id, port_id, enable);
  return rv;
}
#ifndef IN_PORTCONTROL_MINI
static sw_error_t
_fal_port_txmac_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_txmac_status_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_txmac_status_get(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_txmac_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_txmac_status_get (dev_id, port_id, enable);
  return rv;
}
static sw_error_t
_fal_port_rxmac_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_rxmac_status_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_rxmac_status_get(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_rxmac_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_rxmac_status_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_bp_status_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_bp_status_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_bp_status_set (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_bp_status_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_bp_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_bp_status_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_ports_link_status_get (a_uint32_t dev_id, a_uint32_t * status)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_ports_link_status_get != NULL) {
        rv = p_adpt_api->adpt_ports_link_status_get(dev_id, status);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->ports_link_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->ports_link_status_get (dev_id, status);
  return rv;
}

static sw_error_t
_fal_port_mac_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_mac_loopback_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_mac_loopback_set(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_mac_loopback_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_mac_loopback_set (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_mac_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_mac_loopback_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_mac_loopback_get(dev_id, port_id, enable);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_mac_loopback_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_mac_loopback_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_congestion_drop_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint32_t queue_id, a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_congestion_drop_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_congestion_drop_set (dev_id, port_id, queue_id, enable);
  return rv;
}

static sw_error_t
_fal_port_congestion_drop_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint32_t queue_id, a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_congestion_drop_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_congestion_drop_get (dev_id, port_id, queue_id, enable);
  return rv;
}

static sw_error_t
_fal_ring_flow_ctrl_thres_set (a_uint32_t dev_id, a_uint32_t ring_id,
			       a_uint8_t on_thres, a_uint8_t off_thres)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->ring_flow_ctrl_thres_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->ring_flow_ctrl_thres_set (dev_id, ring_id, on_thres, off_thres);
  return rv;
}

static sw_error_t
_fal_ring_flow_ctrl_thres_get (a_uint32_t dev_id, a_uint32_t ring_id,
			       a_uint8_t * on_thres, a_uint8_t * off_thres)
{
  sw_error_t rv;
  hsl_api_t *p_api;

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->ring_flow_ctrl_thres_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->ring_flow_ctrl_thres_get (dev_id, ring_id, on_thres, off_thres);
  return rv;
}
/*qca808x_start*/
static sw_error_t
_fal_port_8023az_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_8023az_set != NULL) {
        rv = p_adpt_api->adpt_port_8023az_set(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_8023az_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_8023az_set (dev_id, port_id, enable);
  return rv;
}


static sw_error_t
_fal_port_8023az_get (a_uint32_t dev_id, fal_port_t port_id,
		      a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_8023az_get != NULL) {
        rv = p_adpt_api->adpt_port_8023az_get(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_8023az_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_8023az_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_mdix_set (a_uint32_t dev_id, fal_port_t port_id,
		    fal_port_mdix_mode_t mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_mdix_set != NULL) {
        rv = p_adpt_api->adpt_port_mdix_set(dev_id, port_id, mode);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_mdix_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_mdix_set (dev_id, port_id, mode);
  return rv;
}

static sw_error_t
_fal_port_mdix_get (a_uint32_t dev_id, fal_port_t port_id,
		    fal_port_mdix_mode_t * mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_mdix_get != NULL) {
        rv = p_adpt_api->adpt_port_mdix_get(dev_id, port_id, mode);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_mdix_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_mdix_get (dev_id, port_id, mode);
  return rv;
}

static sw_error_t
_fal_port_mdix_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   fal_port_mdix_status_t * mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_mdix_status_get != NULL) {
        rv = p_adpt_api->adpt_port_mdix_status_get(dev_id, port_id, mode);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_mdix_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_mdix_status_get (dev_id, port_id, mode);
  return rv;
}
/*qca808x_end*/
static sw_error_t
_fal_port_combo_prefer_medium_set (a_uint32_t dev_id, fal_port_t port_id,
				   fal_port_medium_t medium)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_combo_prefer_medium_set != NULL) {
        rv = p_adpt_api->adpt_port_combo_prefer_medium_set(dev_id, port_id, medium);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_combo_prefer_medium_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_combo_prefer_medium_set (dev_id, port_id, medium);
  return rv;
}

static sw_error_t
_fal_port_combo_prefer_medium_get (a_uint32_t dev_id, fal_port_t port_id,
				   fal_port_medium_t * medium)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_combo_prefer_medium_get != NULL) {
        rv = p_adpt_api->adpt_port_combo_prefer_medium_get(dev_id, port_id, medium);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_combo_prefer_medium_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_combo_prefer_medium_get (dev_id, port_id, medium);
  return rv;
}

static sw_error_t
_fal_port_combo_medium_status_get (a_uint32_t dev_id, fal_port_t port_id,
				   fal_port_medium_t * medium)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_combo_medium_status_get != NULL) {
        rv = p_adpt_api->adpt_port_combo_medium_status_get(dev_id, port_id, medium);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_combo_medium_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_combo_medium_status_get (dev_id, port_id, medium);
  return rv;
}

static sw_error_t
_fal_port_combo_fiber_mode_set (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_fiber_mode_t mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_combo_fiber_mode_set != NULL) {
        rv = p_adpt_api->adpt_port_combo_fiber_mode_set(dev_id, port_id, mode);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_combo_fiber_mode_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_combo_fiber_mode_set (dev_id, port_id, mode);
  return rv;
}

static sw_error_t
_fal_port_combo_fiber_mode_get (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_fiber_mode_t * mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_combo_fiber_mode_get != NULL) {
        rv = p_adpt_api->adpt_port_combo_fiber_mode_get(dev_id, port_id, mode);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_combo_fiber_mode_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_combo_fiber_mode_get (dev_id, port_id, mode);
  return rv;
}
/*qca808x_start*/
static sw_error_t
_fal_port_local_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_local_loopback_set != NULL) {
        rv = p_adpt_api->adpt_port_local_loopback_set(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_local_loopback_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_local_loopback_set (dev_id, port_id, enable);
  return rv;
}


static sw_error_t
_fal_port_local_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_local_loopback_get != NULL) {
        rv = p_adpt_api->adpt_port_local_loopback_get(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_local_loopback_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_local_loopback_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_remote_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_remote_loopback_set != NULL) {
        rv = p_adpt_api->adpt_port_remote_loopback_set(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_remote_loopback_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_remote_loopback_set (dev_id, port_id, enable);
  return rv;
}


static sw_error_t
_fal_port_remote_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_remote_loopback_get!= NULL) {
        rv = p_adpt_api->adpt_port_remote_loopback_get(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_remote_loopback_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_remote_loopback_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_reset (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_reset != NULL) {
        rv = p_adpt_api->adpt_port_reset(dev_id, port_id);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_reset)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_reset (dev_id, port_id);
  return rv;
}


static sw_error_t
_fal_port_phy_id_get (a_uint32_t dev_id, fal_port_t port_id,a_uint16_t * org_id, a_uint16_t * rev_id)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_phy_id_get != NULL) {
        rv = p_adpt_api->adpt_port_phy_id_get(dev_id, port_id, org_id, rev_id);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_phy_id_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_phy_id_get (dev_id, port_id,org_id,rev_id);
  return rv;
}

static sw_error_t
_fal_port_wol_status_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_wol_status_set != NULL) {
        rv = p_adpt_api->adpt_port_wol_status_set(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_wol_status_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_wol_status_set (dev_id, port_id,enable);
  return rv;
}

static sw_error_t
_fal_port_wol_status_get (a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_wol_status_get != NULL) {
        rv = p_adpt_api->adpt_port_wol_status_get(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_wol_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_wol_status_get (dev_id, port_id,enable);
  return rv;
}

static sw_error_t
_fal_port_magic_frame_mac_set (a_uint32_t dev_id, fal_port_t port_id, fal_mac_addr_t * mac)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_magic_frame_mac_set != NULL) {
        rv = p_adpt_api->adpt_port_magic_frame_mac_set(dev_id, port_id, mac);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_magic_frame_mac_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_magic_frame_mac_set (dev_id, port_id, mac);
  return rv;
}

static sw_error_t
_fal_port_magic_frame_mac_get (a_uint32_t dev_id, fal_port_t port_id, fal_mac_addr_t * mac)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_magic_frame_mac_get != NULL) {
        rv = p_adpt_api->adpt_port_magic_frame_mac_get(dev_id, port_id, mac);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_magic_frame_mac_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_magic_frame_mac_get (dev_id, port_id, mac);
  return rv;
}
/*qca808x_end*/
static sw_error_t
_fal_port_interface_mode_set (a_uint32_t dev_id, fal_port_t port_id, fal_port_interface_mode_t  mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;
    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_interface_mode_set != NULL) {
        rv = p_adpt_api->adpt_port_interface_mode_set(dev_id, port_id, mode);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_interface_mode_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_interface_mode_set (dev_id, port_id, mode);
  return rv;
}

static sw_error_t
_fal_port_interface_mode_get (a_uint32_t dev_id, fal_port_t port_id, fal_port_interface_mode_t * mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_interface_mode_get != NULL) {
        rv = p_adpt_api->adpt_port_interface_mode_get(dev_id, port_id, mode);
        return rv;
    }

  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_interface_mode_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_interface_mode_get (dev_id, port_id, mode);
  return rv;
}
/*qca808x_start*/
static sw_error_t
_fal_port_interface_mode_status_get (a_uint32_t dev_id, fal_port_t port_id, fal_port_interface_mode_t * mode)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_interface_mode_status_get != NULL) {
        rv = p_adpt_api->adpt_port_interface_mode_status_get(dev_id, port_id, mode);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_interface_mode_status_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_interface_mode_status_get (dev_id, port_id, mode);
  return rv;
}

static sw_error_t
_fal_port_counter_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_counter_set != NULL) {
        rv = p_adpt_api->adpt_port_counter_set(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_counter_set)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_counter_set (dev_id, port_id, enable);
  return rv;
}


static sw_error_t
_fal_port_counter_get (a_uint32_t dev_id, fal_port_t port_id,
		      a_bool_t * enable)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_counter_get != NULL) {
        rv = p_adpt_api->adpt_port_counter_get(dev_id, port_id, enable);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_counter_get)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_counter_get (dev_id, port_id, enable);
  return rv;
}

static sw_error_t
_fal_port_counter_show (a_uint32_t dev_id, fal_port_t port_id, fal_port_counter_info_t * counter_info)
{
  sw_error_t rv;
  hsl_api_t *p_api;
/*qca808x_end*/
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_port_counter_show != NULL) {
        rv = p_adpt_api->adpt_port_counter_show(dev_id, port_id, counter_info);
        return rv;
    }
/*qca808x_start*/
  SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

  if (NULL == p_api->port_counter_show)
    return SW_NOT_SUPPORTED;

  rv = p_api->port_counter_show (dev_id, port_id, counter_info);
  return rv;
}
/*qca808x_end*/
#endif

sw_error_t
_fal_port_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t max_frame)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_max_frame_size_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_max_frame_size_set(dev_id, port_id, max_frame);
    return rv;
}

sw_error_t
_fal_port_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *max_frame)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_max_frame_size_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_max_frame_size_get(dev_id, port_id, max_frame);
	return rv;
}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
_fal_port_mru_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_mru_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_mru_set(dev_id, port_id, ctrl);
    return rv;
}
sw_error_t
_fal_port_mru_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_mru_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_mru_get(dev_id, port_id, ctrl);
    return rv;
}
sw_error_t
_fal_port_mtu_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_mtu_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_mtu_set(dev_id, port_id, ctrl);
    return rv;
}

sw_error_t
_fal_port_mtu_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_mtu_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_mtu_get(dev_id, port_id, ctrl);
    return rv;
}

sw_error_t
_fal_port_source_filter_get(a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t * enable)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_source_filter_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_source_filter_get(dev_id, port_id, enable);
    return rv;
}

sw_error_t
_fal_port_source_filter_set(a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t enable)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_source_filter_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_source_filter_set(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_port_source_filter_config_set(a_uint32_t dev_id,
	fal_port_t port_id, fal_src_filter_config_t *src_filter_config)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_source_filter_config_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_source_filter_config_set(dev_id, port_id,
			src_filter_config);
	return rv;
}

static sw_error_t
_fal_port_source_filter_config_get(a_uint32_t dev_id,
	fal_port_t port_id, fal_src_filter_config_t *src_filter_config)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_source_filter_config_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_source_filter_config_get(dev_id, port_id,
			src_filter_config);
	return rv;
}

static sw_error_t
_fal_port_interface_3az_status_set(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable)
{

	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_interface_3az_status_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_interface_3az_status_set(dev_id, port_id, enable);
	return rv;

}

static sw_error_t
_fal_port_interface_3az_status_get(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_interface_3az_status_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_interface_3az_status_get(dev_id, port_id, enable);
	return rv;

}

static sw_error_t
_fal_port_promisc_mode_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	adpt_api_t *p_adpt_api;

	if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
		if (NULL == p_adpt_api->adpt_port_promisc_mode_get) {
			return SW_NOT_SUPPORTED;
		}

		rv = p_adpt_api->adpt_port_promisc_mode_get(dev_id, port_id, enable);
		return rv;
	}

	return rv;
}
#endif

static sw_error_t
_fal_port_promisc_mode_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	adpt_api_t *p_adpt_api;

	if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
		if (NULL == p_adpt_api->adpt_port_promisc_mode_set) {
			return SW_NOT_SUPPORTED;
		}

		rv = p_adpt_api->adpt_port_promisc_mode_set(dev_id, port_id, enable);
		return rv;
	}

	return rv;
}

static sw_error_t
_fal_port_interface_eee_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg)
{

	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_interface_eee_cfg_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_interface_eee_cfg_set(dev_id, port_id, port_eee_cfg);
	return rv;

}

static sw_error_t
_fal_port_interface_eee_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_port_interface_eee_cfg_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_port_interface_eee_cfg_get(dev_id, port_id, port_eee_cfg);
	return rv;

}
static sw_error_t
_fal_switch_port_loopback_set(a_uint32_t dev_id, fal_port_t port_id,
	fal_loopback_config_t *loopback_cfg)
{

	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_switch_port_loopback_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_switch_port_loopback_set(dev_id, port_id, loopback_cfg);
	return rv;

}

static sw_error_t
_fal_switch_port_loopback_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_loopback_config_t *loopback_cfg)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));
	if (NULL == p_api->adpt_switch_port_loopback_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_switch_port_loopback_get(dev_id, port_id, loopback_cfg);
	return rv;

}

/*qca808x_start*/
/*insert flag for inner fal, don't remove it*/
/**
 * @brief Set duplex mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] duplex duplex mode
 * @return SW_OK or error code
 */
sw_error_t
fal_port_duplex_set (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_duplex_t duplex)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_duplex_set (dev_id, port_id, duplex);
  FAL_API_UNLOCK;
  return rv;
}



/**
 * @brief Set speed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] speed port speed
 * @return SW_OK or error code
 */
sw_error_t
fal_port_speed_set (a_uint32_t dev_id, fal_port_t port_id,
		    fal_port_speed_t speed)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_speed_set (dev_id, port_id, speed);
  FAL_API_UNLOCK;
  return rv;
}
/**
 * @brief Get duplex mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] duplex duplex mode
 * @return SW_OK or error code
 */
sw_error_t
fal_port_duplex_get (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_duplex_t * pduplex)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_duplex_get (dev_id, port_id, pduplex);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get speed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] speed port speed
 * @return SW_OK or error code
 */
sw_error_t
fal_port_speed_get (a_uint32_t dev_id, fal_port_t port_id,
		    fal_port_speed_t * pspeed)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_speed_get (dev_id, port_id, pspeed);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Enable auto negotiation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
sw_error_t
fal_port_autoneg_enable (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_autoneg_enable (dev_id, port_id);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Restart auto negotiation procedule on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
sw_error_t
fal_port_autoneg_restart (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_autoneg_restart (dev_id, port_id);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set auto negotiation advtisement ability on a particular port.
 *   @details  Comments:
 *   auto negotiation advtisement ability is defined by macro such as
 *   FAL_PHY_ADV_10T_HD, FAL_PHY_ADV_PAUSE...
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] autoadv auto negotiation advtisement ability bit map
 * @return SW_OK or error code
 */
sw_error_t
fal_port_autoneg_adv_set (a_uint32_t dev_id, fal_port_t port_id,
			  a_uint32_t autoadv)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_autoneg_adv_set (dev_id, port_id, autoadv);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get auto negotiation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] status A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_autoneg_status_get (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t * status)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_autoneg_status_get (dev_id, port_id, status);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get auto negotiation advtisement ability on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] autoadv auto negotiation advtisement ability bit map
 * @return SW_OK or error code
 */
sw_error_t
fal_port_autoneg_adv_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_uint32_t * autoadv)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_autoneg_adv_get (dev_id, port_id, autoadv);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_end*/
#ifndef IN_PORTCONTROL_MINI
/**
 * @brief Set status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_hdr_status_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_hdr_status_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_hdr_status_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_hdr_status_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}


/**
 * @brief Get flow control status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_flowctrl_get (a_uint32_t dev_id, fal_port_t port_id,
		       a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_flowctrl_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}



/**
 * @brief Get flow control force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_flowctrl_forcemode_get (a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_flowctrl_forcemode_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set powersaving status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_powersave_set (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_powersave_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get powersaving status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_powersave_get (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_powersave_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_start*/
/**
 * @brief Set hibernate status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_hibernate_set (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_hibernate_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get hibernate status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_hibernate_get (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_hibernate_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief cable diagnostic test.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mdi_pair mdi pair id
 * @param[out] cable_status cable status
 * @param[out] cable_len cable len
 * @return SW_OK or error code
 */
sw_error_t
fal_port_cdt (a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
	      a_uint32_t * cable_status, a_uint32_t * cable_len)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_cdt (dev_id, port_id, mdi_pair, cable_status, cable_len);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_end*/
/**
 * @brief Get status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_rxhdr_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			 fal_port_header_mode_t * mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_rxhdr_mode_get (dev_id, port_id, mode);
  FAL_API_UNLOCK;
  return rv;
}
/**
 * @brief Get status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_txhdr_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			 fal_port_header_mode_t * mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_txhdr_mode_get (dev_id, port_id, mode);
  FAL_API_UNLOCK;
  return rv;
}
/**
 * @brief Get status of Atheros header type value on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] type header type value
 * @return SW_OK or error code
 */
sw_error_t
fal_header_type_get (a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * type)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_header_type_get (dev_id, enable, type);
  FAL_API_UNLOCK;
  return rv;
}
/**
 * @brief Get status of txmac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_txmac_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_txmac_status_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}
/**
 * @brief Get status of rxmac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_rxmac_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_rxmac_status_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}
#endif
/**
 * @brief Set flow control status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_flowctrl_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_flowctrl_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set flow control force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_flowctrl_forcemode_set (a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_flowctrl_forcemode_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_rxhdr_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			 fal_port_header_mode_t mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_rxhdr_mode_set (dev_id, port_id, mode);
  FAL_API_UNLOCK;
  return rv;
}



/**
 * @brief Set status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_txhdr_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			 fal_port_header_mode_t mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_txhdr_mode_set (dev_id, port_id, mode);
  FAL_API_UNLOCK;
  return rv;
}



/**
 * @brief Set status of Atheros header type value on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] type header type value
 * @return SW_OK or error code
 */
sw_error_t
fal_header_type_set (a_uint32_t dev_id, a_bool_t enable, a_uint32_t type)
{
  sw_error_t rv;
  FAL_API_LOCK;
  rv = _fal_header_type_set (dev_id, enable, type);
  FAL_API_UNLOCK;
  return rv;
}



/**
 * @brief Set status of txmac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_txmac_status_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t enable)
{
  sw_error_t rv;
  FAL_API_LOCK;
  rv = _fal_port_txmac_status_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}



/**
 * @brief Set status of rxmac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_rxmac_status_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t enable)
{
  sw_error_t rv;
  FAL_API_LOCK;
  rv = _fal_port_rxmac_status_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}



/**
 * @brief Set status of tx flow control on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_txfc_status_set (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{
  sw_error_t rv;
  FAL_API_LOCK;
  rv = _fal_port_txfc_status_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}



/**
 * @brief Set status of rx flow control on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_rxfc_status_set (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_rxfc_status_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}
/**
 * @brief Set status of rx flow control on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_rxfc_status_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_rxfc_status_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}
/**
 * @brief Get status of tx flow control on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_txfc_status_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_txfc_status_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_start*/
/**
 * @brief Get link status on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] status link status up (A_TRUE) or down (A_FALSE)
 * @return SW_OK or error code
 */
sw_error_t
fal_port_link_status_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * status)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_link_status_get (dev_id, port_id, status);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief power off on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_power_off (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_power_off (dev_id, port_id);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief power on on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_power_on (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_power_on (dev_id, port_id);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_end*/
/**
 * @brief Set link force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_link_forcemode_set (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_link_forcemode_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get link force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_link_forcemode_get (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_link_forcemode_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}
#ifndef IN_PORTCONTROL_MINI
/**
 * @brief Set status of back pressure on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_bp_status_set (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_bp_status_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of back pressure on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_bp_status_get (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_bp_status_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get link status on all ports.
 * @param[in] dev_id device id
 * @param[out] status link status bitmap and bit 0 for port 0, bit 1 for port 1, ...etc.
 * @return SW_OK or error code
 */
sw_error_t
fal_ports_link_status_get (a_uint32_t dev_id, a_uint32_t * status)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_ports_link_status_get (dev_id, status);
  FAL_API_UNLOCK;
  return rv;
}


/**
 * @brief Set loopback on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_mac_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_mac_loopback_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get link force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_mac_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_mac_loopback_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set congestion drop on a particular port queue.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_congestion_drop_set (a_uint32_t dev_id, fal_port_t port_id,
			      a_uint32_t queue_id, a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_congestion_drop_set (dev_id, port_id, queue_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get congestion drop on a particular port queue.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue_id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_congestion_drop_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_uint32_t queue_id, a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_congestion_drop_get (dev_id, port_id, queue_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set flow control threshold on a DMA ring.
 * @param[in] dev_id device id
 * @param[in] ring_id ring_id
 * @param[in] on_thres on_thres
 * @param[in] off_thres on_thres
 * @return SW_OK or error code
 */
sw_error_t
fal_ring_flow_ctrl_thres_set (a_uint32_t dev_id, a_uint32_t ring_id,
			      a_uint8_t on_thres, a_uint8_t off_thres)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_ring_flow_ctrl_thres_set (dev_id, ring_id, on_thres, off_thres);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get flow control threshold on a DMA ring.
 * @param[in] dev_id device id
 * @param[in] ring_id ring_id
 * @param[out] on_thres on_thres
 * @param[out] off_thres on_thres
 * @return SW_OK or error code
 */
sw_error_t
fal_ring_flow_ctrl_thres_get (a_uint32_t dev_id, a_uint32_t ring_id,
			      a_uint8_t * on_thres, a_uint8_t * off_thres)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_ring_flow_ctrl_thres_get (dev_id, ring_id, on_thres, off_thres);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_start*/
/**
 * @brief Set 8023az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_8023az_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_8023az_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get 8023az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_8023az_get (a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_8023az_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set mdix mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] set mdix mode [mdx , mdix or auto]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_mdix_set (a_uint32_t dev_id, fal_port_t port_id,
		   fal_port_mdix_mode_t mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_mdix_set (dev_id, port_id, mode);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get mdix on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] set mdx ,mdix or auto 
 * @return SW_OK or error code
 */
sw_error_t
fal_port_mdix_get (a_uint32_t dev_id, fal_port_t port_id,
		   fal_port_mdix_mode_t * mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_mdix_get (dev_id, port_id, mode);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get mdix status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] set mdx ,mdix 
 * @return SW_OK or error code
 */
sw_error_t
fal_port_mdix_status_get (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_mdix_status_t * mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_mdix_status_get (dev_id, port_id, mode);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_end*/
/**
 * @brief Set combo prefer medium  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] set combo prefer medium [fiber for copper]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_combo_prefer_medium_set (a_uint32_t dev_id, a_uint32_t port_id,
				  fal_port_medium_t medium)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_combo_prefer_medium_set (dev_id, port_id, medium);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get combo prefer medium  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] set combo prefer medium [fiber for copper]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_combo_prefer_medium_get (a_uint32_t dev_id, a_uint32_t port_id,
				  fal_port_medium_t * medium)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_combo_prefer_medium_get (dev_id, port_id, medium);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get combo  medium  status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] get combo [fiber for copper]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_combo_medium_status_get (a_uint32_t dev_id, a_uint32_t port_id,
				  fal_port_medium_t * medium)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_combo_medium_status_get (dev_id, port_id, medium);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set combo fiber mode  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] get combo fiber mode [1000bx or 100fx]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_combo_fiber_mode_set (a_uint32_t dev_id, a_uint32_t port_id,
			       fal_port_fiber_mode_t mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_combo_fiber_mode_set (dev_id, port_id, mode);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get combo fiber mode  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] get combo fiber mode [1000bx or 100fx]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_combo_fiber_mode_get (a_uint32_t dev_id, a_uint32_t port_id,
			       fal_port_fiber_mode_t * mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_combo_fiber_mode_get (dev_id, port_id, mode);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_start*/
/**
 * @brief Set local loopback  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_local_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_local_loopback_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get local loopback status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_local_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_local_loopback_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set remote loopback  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_remote_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_remote_loopback_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get remote loopback status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_remote_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_remote_loopback_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief software reset on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_reset (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_reset (dev_id, port_id);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief phy id on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_phy_id_get (a_uint32_t dev_id, fal_port_t port_id, a_uint16_t * org_id, a_uint16_t * rev_id)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_phy_id_get (dev_id, port_id,org_id,rev_id);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief wol status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_wol_status_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_wol_status_set (dev_id, port_id,enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief wol status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_wol_status_get (a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_wol_status_get (dev_id, port_id,enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief magic frame mac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_magic_frame_mac_set (a_uint32_t dev_id, fal_port_t port_id, fal_mac_addr_t * mac)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_magic_frame_mac_set (dev_id, port_id,mac);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief magic frame mac  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_magic_frame_mac_get (a_uint32_t dev_id, fal_port_t port_id, fal_mac_addr_t * mac)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_magic_frame_mac_get (dev_id, port_id,mac);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_end*/
/**
 * @brief interface mode  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_interface_mode_set (a_uint32_t dev_id, fal_port_t port_id, fal_port_interface_mode_t  mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_interface_mode_set (dev_id, port_id,mode);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief interface mode  on a particular port.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
 static sw_error_t
_fal_port_interface_mode_apply (a_uint32_t dev_id)
{
  sw_error_t rv = SW_OK;
  adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_interface_mode_apply)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_interface_mode_apply(dev_id);
        return rv;
    }

  return rv;
}

sw_error_t
fal_port_interface_mode_apply (a_uint32_t dev_id)
{
  sw_error_t rv = SW_OK;

  FAL_API_LOCK;
  rv = _fal_port_interface_mode_apply (dev_id);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief interface mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_interface_mode_get (a_uint32_t dev_id, fal_port_t port_id, fal_port_interface_mode_t * mode)
{
  sw_error_t rv = SW_OK;

  FAL_API_LOCK;
  rv = _fal_port_interface_mode_get (dev_id, port_id,mode);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_start*/
/**
 * @brief interface mode status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out]
 * @return SW_OK or error code
 */
sw_error_t
fal_port_interface_mode_status_get (a_uint32_t dev_id, fal_port_t port_id, fal_port_interface_mode_t * mode)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_interface_mode_status_get (dev_id, port_id,mode);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set counter status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_debug_phycounter_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_counter_set (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get counter status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_debug_phycounter_get (a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_counter_get (dev_id, port_id, enable);
  FAL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get counter statistics on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] counter frame number
 * @return SW_OK or error code
 */
sw_error_t
fal_debug_phycounter_show (a_uint32_t dev_id, fal_port_t port_id, fal_port_counter_info_t* port_counter_info)
{
  sw_error_t rv;

  FAL_API_LOCK;
  rv = _fal_port_counter_show (dev_id, port_id, port_counter_info);
  FAL_API_UNLOCK;
  return rv;
}
/*qca808x_end*/
#endif

sw_error_t
fal_port_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t max_frame)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_max_frame_size_set(dev_id, port_id, max_frame);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_port_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *max_frame)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_max_frame_size_get(dev_id, port_id, max_frame);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
fal_port_mru_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_mru_set(dev_id, port_id, ctrl);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_mru_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_mru_get(dev_id, port_id, ctrl);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_mtu_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_mtu_set(dev_id, port_id, ctrl);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_mtu_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_mtu_get(dev_id, port_id, ctrl);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_port_source_filter_status_get(a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_source_filter_get(dev_id,
                    port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_port_source_filter_enable(a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_source_filter_set(dev_id,
                    port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_port_source_filter_config_set(a_uint32_t dev_id,
		fal_port_t port_id, fal_src_filter_config_t *src_filter_config)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_source_filter_config_set(dev_id, port_id, src_filter_config);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_port_source_filter_config_get(a_uint32_t dev_id,
		fal_port_t port_id, fal_src_filter_config_t *src_filter_config)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_source_filter_config_get(dev_id, port_id, src_filter_config);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_port_interface_3az_status_set(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_interface_3az_status_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_port_interface_3az_status_get(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_interface_3az_status_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_port_promisc_mode_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_promisc_mode_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}
#endif

sw_error_t
fal_port_promisc_mode_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_promisc_mode_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_port_interface_eee_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_interface_eee_cfg_set(dev_id, port_id, port_eee_cfg);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_port_interface_eee_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_port_interface_eee_cfg_get(dev_id, port_id, port_eee_cfg);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_switch_port_loopback_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_loopback_config_t *loopback_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_switch_port_loopback_set(dev_id, port_id, loopback_cfg);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_switch_port_loopback_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_loopback_config_t *loopback_cfg)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_switch_port_loopback_get(dev_id, port_id, loopback_cfg);
    FAL_API_UNLOCK;
    return rv;
}

/*insert flag for outter fal, don't remove it*/
/**
 * @}
 */
 #ifndef IN_PORTCONTROL_MINI
EXPORT_SYMBOL(fal_port_mtu_set);
EXPORT_SYMBOL(fal_port_mtu_get);
EXPORT_SYMBOL(fal_port_mru_set);
EXPORT_SYMBOL(fal_port_mru_get);
#endif
EXPORT_SYMBOL(fal_port_duplex_set);
EXPORT_SYMBOL(fal_port_duplex_get);
EXPORT_SYMBOL(fal_port_speed_set);
EXPORT_SYMBOL(fal_port_speed_get);
EXPORT_SYMBOL(fal_port_autoneg_status_get);
EXPORT_SYMBOL(fal_port_autoneg_enable);
EXPORT_SYMBOL(fal_port_autoneg_restart);
EXPORT_SYMBOL(fal_port_autoneg_adv_set);
EXPORT_SYMBOL(fal_port_autoneg_adv_get);
EXPORT_SYMBOL(fal_port_flowctrl_set);
#ifndef IN_PORTCONTROL_MINI
EXPORT_SYMBOL(fal_port_flowctrl_get);
EXPORT_SYMBOL(fal_port_powersave_set);
EXPORT_SYMBOL(fal_port_powersave_get);
EXPORT_SYMBOL(fal_port_hibernate_set);
EXPORT_SYMBOL(fal_port_hibernate_get);
EXPORT_SYMBOL(fal_port_cdt);
EXPORT_SYMBOL(fal_port_txmac_status_get);
#endif
EXPORT_SYMBOL(fal_port_txmac_status_set);
EXPORT_SYMBOL(fal_port_rxmac_status_set);
#ifndef IN_PORTCONTROL_MINI
EXPORT_SYMBOL(fal_port_rxmac_status_get);
#endif
EXPORT_SYMBOL(fal_port_txfc_status_set);
EXPORT_SYMBOL(fal_port_txfc_status_get);
EXPORT_SYMBOL(fal_port_rxfc_status_set);
EXPORT_SYMBOL(fal_port_rxfc_status_get);
#ifndef IN_PORTCONTROL_MINI
EXPORT_SYMBOL(fal_port_bp_status_set);
EXPORT_SYMBOL(fal_port_bp_status_get);
#endif
EXPORT_SYMBOL(fal_port_link_status_get);
#ifndef IN_PORTCONTROL_MINI
EXPORT_SYMBOL(fal_ports_link_status_get);
EXPORT_SYMBOL(fal_port_mac_loopback_set);
EXPORT_SYMBOL(fal_port_mac_loopback_get);
EXPORT_SYMBOL(fal_port_8023az_set);
EXPORT_SYMBOL(fal_port_8023az_get);
EXPORT_SYMBOL(fal_port_mdix_set);
EXPORT_SYMBOL(fal_port_mdix_get);
EXPORT_SYMBOL(fal_port_mdix_status_get);
EXPORT_SYMBOL(fal_port_combo_prefer_medium_set);
EXPORT_SYMBOL(fal_port_combo_prefer_medium_get);
EXPORT_SYMBOL(fal_port_combo_medium_status_get);
EXPORT_SYMBOL(fal_port_combo_fiber_mode_set);
EXPORT_SYMBOL(fal_port_combo_fiber_mode_get);
EXPORT_SYMBOL(fal_port_local_loopback_set);
EXPORT_SYMBOL(fal_port_local_loopback_get);
EXPORT_SYMBOL(fal_port_remote_loopback_set);
EXPORT_SYMBOL(fal_port_remote_loopback_get);
EXPORT_SYMBOL(fal_port_reset);
#endif
EXPORT_SYMBOL(fal_port_power_off);
EXPORT_SYMBOL(fal_port_power_on);
#ifndef IN_PORTCONTROL_MINI
EXPORT_SYMBOL(fal_port_magic_frame_mac_set );
EXPORT_SYMBOL(fal_port_magic_frame_mac_get );
EXPORT_SYMBOL(fal_port_phy_id_get );
EXPORT_SYMBOL(fal_port_wol_status_set );
EXPORT_SYMBOL(fal_port_wol_status_get );
EXPORT_SYMBOL(fal_port_interface_mode_set);
EXPORT_SYMBOL(fal_port_interface_mode_apply);

EXPORT_SYMBOL(fal_port_interface_mode_get );
EXPORT_SYMBOL(fal_port_interface_mode_status_get );
EXPORT_SYMBOL(fal_port_source_filter_enable);
EXPORT_SYMBOL(fal_port_source_filter_status_get);
EXPORT_SYMBOL(fal_port_source_filter_config_get);
EXPORT_SYMBOL(fal_port_source_filter_config_set);
#endif
EXPORT_SYMBOL(fal_port_max_frame_size_set);
EXPORT_SYMBOL(fal_port_max_frame_size_get);
#ifndef IN_PORTCONTROL_MINI
EXPORT_SYMBOL(fal_port_interface_3az_status_set);
EXPORT_SYMBOL(fal_port_interface_3az_status_get);
#endif
EXPORT_SYMBOL(fal_port_flowctrl_forcemode_set);
#ifndef IN_PORTCONTROL_MINI
EXPORT_SYMBOL(fal_port_flowctrl_forcemode_get);
EXPORT_SYMBOL(fal_port_promisc_mode_set);
EXPORT_SYMBOL(fal_port_promisc_mode_get);
#endif
EXPORT_SYMBOL(fal_port_interface_eee_cfg_set);
EXPORT_SYMBOL(fal_port_interface_eee_cfg_get);
EXPORT_SYMBOL(fal_switch_port_loopback_set);
EXPORT_SYMBOL(fal_switch_port_loopback_get);

