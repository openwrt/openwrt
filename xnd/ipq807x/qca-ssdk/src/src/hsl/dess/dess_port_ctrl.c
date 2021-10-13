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
 * @defgroup dess_port_ctrl DESS_PORT_CONTROL
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "dess_port_ctrl.h"
#include "dess_reg.h"
#include "hsl_phy.h"

#define DMA_MAX_VIRT_RING 8
extern a_bool_t dess_mac_port_valid_check (fal_port_t port_id);

/*
PORT0 egress 6 queues
PORT1~4 egress 4 queues
PORT5 egress 6 queues
*/
static a_uint32_t port_queue[6] = { 6, 4, 4, 4, 4, 6 };


static a_bool_t
_dess_port_phy_connected (a_uint32_t dev_id, fal_port_t port_id)
{
  if (0 == port_id)
    {
      return A_FALSE;
    }
  else
    {

      return dess_mac_port_valid_check (port_id);
    }
}

static sw_error_t
_dess_port_duplex_set (a_uint32_t dev_id, fal_port_t port_id,
		       fal_port_duplex_t duplex)
{
  sw_error_t rv;
  a_uint32_t phy_id;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_duplex_set)
    return SW_NOT_SUPPORTED;

  if (FAL_DUPLEX_BUTT <= duplex)
    {
      return SW_BAD_PARAM;
    }
  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
      SW_RTN_ON_ERROR (rv);

      rv = phy_drv->phy_duplex_set (dev_id, phy_id, duplex);
      SW_RTN_ON_ERROR (rv);
#if 0
  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));

  /* for those ports without PHY device we set MAC register */
  if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
    {
      SW_SET_REG_BY_FIELD (PORT_STATUS, LINK_EN, 0, reg_val);
      if (FAL_HALF_DUPLEX == duplex)
	{
	  SW_SET_REG_BY_FIELD (PORT_STATUS, DUPLEX_MODE, 0, reg_val);
	}
      else
	{
	  SW_SET_REG_BY_FIELD (PORT_STATUS, DUPLEX_MODE, 1, reg_val);
	}
      reg_save = reg_val;
    }
  else
    {
      /* hardware requirement: set mac be config by sw and turn off RX/TX MAC */
      reg_save = reg_val;
      SW_SET_REG_BY_FIELD (PORT_STATUS, LINK_EN, 0, reg_val);
      SW_SET_REG_BY_FIELD (PORT_STATUS, RXMAC_EN, 0, reg_val);
      SW_SET_REG_BY_FIELD (PORT_STATUS, TXMAC_EN, 0, reg_val);

      HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
			 (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));

      rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
      SW_RTN_ON_ERROR (rv);

      rv = phy_drv->phy_duplex_set (dev_id, phy_id, duplex);
      SW_RTN_ON_ERROR (rv);

      /* If MAC not in sync with PHY mode, the behavior is undefine.
         You must be careful... */
      SW_GET_FIELD_BY_REG (PORT_STATUS, LINK_EN, force, reg_save);
      if (!force)
	{
	  if (FAL_HALF_DUPLEX == duplex)
	    {
	      SW_SET_REG_BY_FIELD (PORT_STATUS, DUPLEX_MODE, 0, reg_save);
	    }
	  else
	    {
	      SW_SET_REG_BY_FIELD (PORT_STATUS, DUPLEX_MODE, 1, reg_save);
	    }
	}
    }

  HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg_save), sizeof (a_uint32_t));
  #endif
  return rv;
}

static sw_error_t
_dess_port_duplex_get (a_uint32_t dev_id, fal_port_t port_id,
		       fal_port_duplex_t * pduplex)
{
	sw_error_t rv = SW_OK;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
	{
		return SW_BAD_PARAM;
	}

	/* for those ports without PHY device supposed always full duplex */
	if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
	{
		*pduplex = FAL_FULL_DUPLEX;
	}
	else
	{
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
		if (NULL == phy_drv->phy_duplex_get)
			return SW_NOT_SUPPORTED;

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);

		rv = phy_drv->phy_duplex_get (dev_id, phy_id, pduplex);
		SW_RTN_ON_ERROR (rv);
	}
#if 0
  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_GET_FIELD_BY_REG (PORT_STATUS, DUPLEX_MODE, field, reg);
  if (field)
    {
      *pduplex = FAL_FULL_DUPLEX;
    }
  else
    {
      *pduplex = FAL_HALF_DUPLEX;
    }

#endif

	return rv;
}

static sw_error_t
_dess_port_speed_set (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_speed_t speed)
{
  sw_error_t rv;
  a_uint32_t phy_id;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_speed_set)
    return SW_NOT_SUPPORTED;

  if (FAL_SPEED_1000 < speed)
    {
      return SW_BAD_PARAM;
    }

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
      SW_RTN_ON_ERROR (rv);

      rv = phy_drv->phy_speed_set (dev_id, phy_id, speed);
      SW_RTN_ON_ERROR (rv);
#if 0
  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));

  /* for those ports without PHY device we set MAC register */
  if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
    {
      SW_SET_REG_BY_FIELD (PORT_STATUS, LINK_EN, 0, reg_val);
      if (FAL_SPEED_10 == speed)
	{
	  SW_SET_REG_BY_FIELD (PORT_STATUS, SPEED_MODE, 0, reg_val);
	}
      else if (FAL_SPEED_100 == speed)
	{
	  SW_SET_REG_BY_FIELD (PORT_STATUS, SPEED_MODE, 1, reg_val);
	}
      else
	{
	  SW_SET_REG_BY_FIELD (PORT_STATUS, SPEED_MODE, 2, reg_val);
	}
      reg_save = reg_val;

    }
  else
    {
      /* hardware requirement: set mac be config by sw and turn off RX/TX MAC */
      reg_save = reg_val;
      SW_SET_REG_BY_FIELD (PORT_STATUS, LINK_EN, 0, reg_val);
      SW_SET_REG_BY_FIELD (PORT_STATUS, RXMAC_EN, 0, reg_val);
      SW_SET_REG_BY_FIELD (PORT_STATUS, TXMAC_EN, 0, reg_val);

      HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
			 (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));

      rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
      SW_RTN_ON_ERROR (rv);

      rv = phy_drv->phy_speed_set (dev_id, phy_id, speed);
      SW_RTN_ON_ERROR (rv);

      /* If MAC not in sync with PHY mode, the behavior is undefine.
         You must be careful... */
      SW_GET_FIELD_BY_REG (PORT_STATUS, LINK_EN, force, reg_save);
      if (!force)
	{
	  if (FAL_SPEED_10 == speed)
	    {
	      SW_SET_REG_BY_FIELD (PORT_STATUS, SPEED_MODE, 0, reg_save);
	    }
	  else if (FAL_SPEED_100 == speed)
	    {
	      SW_SET_REG_BY_FIELD (PORT_STATUS, SPEED_MODE, 1, reg_save);
	    }
	  else
	    {
	      SW_SET_REG_BY_FIELD (PORT_STATUS, SPEED_MODE, 2, reg_save);
	    }
	}
    }

  HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg_save), sizeof (a_uint32_t));
  #endif
  return rv;
}

static sw_error_t
_dess_port_speed_get (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_speed_t * pspeed)
{
	sw_error_t rv = SW_OK;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
	{
		return SW_BAD_PARAM;
	}

	/* for those ports without PHY device supposed always 1000Mbps */
	if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
	{
		*pspeed = FAL_SPEED_1000;
	}
	else
	{
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
		if (NULL == phy_drv->phy_speed_get)
			return SW_NOT_SUPPORTED;

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);
		rv = phy_drv->phy_speed_get (dev_id, phy_id, pspeed);
		SW_RTN_ON_ERROR (rv);
	}
#if 0
  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  SW_GET_FIELD_BY_REG (PORT_STATUS, SPEED_MODE, field, reg);
  if (0 == field)
    {
      *pspeed = FAL_SPEED_10;
    }
  else if (1 == field)
    {
      *pspeed = FAL_SPEED_100;
    }
  else if (2 == field)
    {
      *pspeed = FAL_SPEED_1000;
    }
  else
    {
      *pspeed = FAL_SPEED_BUTT;
      rv = SW_READ_ERROR;
    }
#endif

	return rv;
}

static sw_error_t
_dess_port_autoneg_status_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * status)
{
  a_uint32_t phy_id;
  sw_error_t rv;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_autoneg_status_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  *status = phy_drv->phy_autoneg_status_get (dev_id, phy_id);

  return SW_OK;
}

static sw_error_t
_dess_port_autoneg_enable (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  a_uint32_t phy_id;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_autoneg_enable_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_autoneg_enable_set (dev_id, phy_id);
  return rv;
}

static sw_error_t
_dess_port_autoneg_restart (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  a_uint32_t phy_id;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_restart_autoneg)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_restart_autoneg (dev_id, phy_id);
  return rv;
}

static sw_error_t
_dess_port_autoneg_adv_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_uint32_t autoadv)
{
  sw_error_t rv;
  a_uint32_t phy_id;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_autoneg_adv_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_autoneg_adv_set (dev_id, phy_id, autoadv);
  SW_RTN_ON_ERROR (rv);

  return SW_OK;
}

static sw_error_t
_dess_port_autoneg_adv_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_uint32_t * autoadv)
{
  sw_error_t rv;
  a_uint32_t phy_id;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_autoneg_adv_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  *autoadv = 0;
  rv = phy_drv->phy_autoneg_adv_get (dev_id, phy_id, autoadv);
  SW_RTN_ON_ERROR (rv);

  return SW_OK;
}

static sw_error_t
_dess_port_flowctrl_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t val, force, reg = 0, tmp;

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  if (A_TRUE == enable)
    {
      val = 1;
    }
  else if (A_FALSE == enable)
    {
      val = 0;
    }
  else
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  SW_GET_FIELD_BY_REG (PORT_STATUS, FLOW_LINK_EN, force, reg);
  if (force)
    {
      /* flow control isn't in force mode so can't set */
      return SW_DISABLE;
    }
	tmp = reg;

  SW_SET_REG_BY_FIELD (PORT_STATUS, RX_FLOW_EN, val, reg);
  SW_SET_REG_BY_FIELD (PORT_STATUS, TX_FLOW_EN, val, reg);
  SW_SET_REG_BY_FIELD (PORT_STATUS, TX_HALF_FLOW_EN, val, reg);
	if (reg == tmp)
		return SW_OK;

  HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_flowctrl_thresh_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_uint8_t on, a_uint8_t off)
{
	sw_error_t rv;
	a_uint32_t reg = 0;

	reg = (on << 16) | off;
	HSL_REG_ENTRY_SET (rv, dev_id, PORT_FLOC_CTRL_THRESH, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
	return rv;
}

static sw_error_t
_dess_port_flowctrl_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t rx, reg = 0;

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  SW_GET_FIELD_BY_REG (PORT_STATUS, RX_FLOW_EN, rx, reg);

  if (1 == rx)
    {
      *enable = A_TRUE;
    }
  else
    {
      *enable = A_FALSE;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_flowctrl_forcemode_set (a_uint32_t dev_id, fal_port_t port_id,
				   a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t reg = 0, tmp;

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);
	SW_GET_FIELD_BY_REG(PORT_STATUS, FLOW_LINK_EN, tmp, reg);

  if (A_TRUE == enable)
    {
		if (tmp== 0)
			return SW_OK;
      SW_SET_REG_BY_FIELD (PORT_STATUS, FLOW_LINK_EN, 0, reg);
    }
  else if (A_FALSE == enable)
    {
      /* for those ports without PHY, it can't sync flow control status */
      if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
	{
	  return SW_DISABLE;
	}
	  if (tmp == 1)
	  	return SW_OK;
      SW_SET_REG_BY_FIELD (PORT_STATUS, FLOW_LINK_EN, 1, reg);
    }
  else
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_flowctrl_forcemode_get (a_uint32_t dev_id, fal_port_t port_id,
				   a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t force, reg;

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  SW_GET_FIELD_BY_REG (PORT_STATUS, FLOW_LINK_EN, force, reg);
  if (0 == force)
    {
      *enable = A_TRUE;
    }
  else
    {
      *enable = A_FALSE;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_powersave_set (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_powersave_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_powersave_set (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_powersave_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_powersave_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_powersave_get (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_hibernate_set (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_hibernation_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_hibernation_set (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_hibernate_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_hibernation_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_hibernation_get (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_cdt (a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
		fal_cable_status_t * cable_status, a_uint32_t * cable_len)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_cdt)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_cdt (dev_id, phy_id, mdi_pair, cable_status, cable_len);

  return rv;
}

static sw_error_t
_dess_port_rxhdr_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			   fal_port_header_mode_t mode)
{
  sw_error_t rv;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  if (FAL_NO_HEADER_EN == mode)
    {
      val = 0;
    }
  else if (FAL_ONLY_MANAGE_FRAME_EN == mode && port_id != 0)
    {
      val = 1;
    }
  else if (FAL_ALL_TYPE_FRAME_EN == mode)
    {
      val = 2;
    }
  else
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_SET (rv, dev_id, PORT_HDR_CTL, port_id, RXHDR_MODE,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_rxhdr_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			   fal_port_header_mode_t * mode)
{
  sw_error_t rv;
  a_uint32_t val;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_GET (rv, dev_id, PORT_HDR_CTL, port_id, RXHDR_MODE,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (1 == val)
    {
      *mode = FAL_ONLY_MANAGE_FRAME_EN;
    }
  else if (2 == val)
    {
      *mode = FAL_ALL_TYPE_FRAME_EN;
    }
  else
    {
      *mode = FAL_NO_HEADER_EN;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_txhdr_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			   fal_port_header_mode_t mode)
{
  sw_error_t rv;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_EXCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  if (FAL_NO_HEADER_EN == mode)
    {
      val = 0;
    }
  else if (FAL_ONLY_MANAGE_FRAME_EN == mode)
    {
      val = 1;
    }
  else if (FAL_ALL_TYPE_FRAME_EN == mode)
    {
      val = 2;
    }
  else
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_SET (rv, dev_id, PORT_HDR_CTL, port_id, TXHDR_MODE,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_txhdr_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			   fal_port_header_mode_t * mode)
{
  sw_error_t rv;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_EXCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_GET (rv, dev_id, PORT_HDR_CTL, port_id, TXHDR_MODE,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (1 == val)
    {
      *mode = FAL_ONLY_MANAGE_FRAME_EN;
    }
  else if (2 == val)
    {
      *mode = FAL_ALL_TYPE_FRAME_EN;
    }
  else
    {
      *mode = FAL_NO_HEADER_EN;
    }

  return SW_OK;
}

static sw_error_t
_dess_header_type_set (a_uint32_t dev_id, a_bool_t enable, a_uint32_t type)
{
  a_uint32_t reg = 0;
  sw_error_t rv;

  HSL_DEV_ID_CHECK (dev_id);

  HSL_REG_ENTRY_GET (rv, dev_id, HEADER_CTL, 0,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (A_TRUE == enable)
    {
      if (0xffff < type)
	{
	  return SW_BAD_PARAM;
	}
      SW_SET_REG_BY_FIELD (HEADER_CTL, TYPE_LEN, 1, reg);
      SW_SET_REG_BY_FIELD (HEADER_CTL, TYPE_VAL, type, reg);
    }
  else if (A_FALSE == enable)
    {
      SW_SET_REG_BY_FIELD (HEADER_CTL, TYPE_LEN, 0, reg);
      SW_SET_REG_BY_FIELD (HEADER_CTL, TYPE_VAL, 0, reg);
    }
  else
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_SET (rv, dev_id, HEADER_CTL, 0,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_header_type_get (a_uint32_t dev_id, a_bool_t * enable,
		       a_uint32_t * type)
{
  a_uint32_t data, reg = 0;
  sw_error_t rv;

  HSL_DEV_ID_CHECK (dev_id);

  HSL_REG_ENTRY_GET (rv, dev_id, HEADER_CTL, 0,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  SW_GET_FIELD_BY_REG (HEADER_CTL, TYPE_LEN, data, reg);
  if (data)
    {
      SW_GET_FIELD_BY_REG (HEADER_CTL, TYPE_VAL, data, reg);
      *enable = A_TRUE;
      *type = data;
    }
  else
    {
      *enable = A_FALSE;
      *type = 0;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_txmac_status_set (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t reg, force, val = 0, tmp;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (A_TRUE == enable)
    {
      val = 1;
    }
  else if (A_FALSE == enable)
    {
      val = 0;
    }
  else
    {
      return SW_BAD_PARAM;
    }
	tmp = reg;

  /* for those ports without PHY device we set MAC register */
  if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
    {
      SW_SET_REG_BY_FIELD (PORT_STATUS, LINK_EN, 0, reg);
      SW_SET_REG_BY_FIELD (PORT_STATUS, TXMAC_EN, val, reg);
    }
  else
    {
      SW_GET_FIELD_BY_REG (PORT_STATUS, LINK_EN, force, reg);
      if (force)
	{
	  /* link isn't in force mode so can't set */
	  return SW_DISABLE;
	}
      else
	{
	  SW_SET_REG_BY_FIELD (PORT_STATUS, TXMAC_EN, val, reg);
	}
    }
	if (tmp == reg)
		return SW_OK;
  HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_txmac_status_get (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_GET (rv, dev_id, PORT_STATUS, port_id, TXMAC_EN,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (val)
    {
      *enable = A_TRUE;
    }
  else
    {
      *enable = A_FALSE;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_rxmac_status_set (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t reg = 0, force, val = 0, tmp = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (A_TRUE == enable)
    {
      val = 1;
    }
  else if (A_FALSE == enable)
    {
      val = 0;
    }
  else
    {
      return SW_BAD_PARAM;
    }
	tmp = reg;

  /* for those ports without PHY device we set MAC register */
  if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
    {
      SW_SET_REG_BY_FIELD (PORT_STATUS, LINK_EN, 0, reg);
      SW_SET_REG_BY_FIELD (PORT_STATUS, RXMAC_EN, val, reg);
    }
  else
    {
      SW_GET_FIELD_BY_REG (PORT_STATUS, LINK_EN, force, reg);
      if (force)
	{
	  /* link isn't in force mode so can't set */
	  return SW_DISABLE;
	}
      else
	{
	  SW_SET_REG_BY_FIELD (PORT_STATUS, RXMAC_EN, val, reg);
	}
    }
	if (tmp == reg)
		return SW_OK;
  HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_rxmac_status_get (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_GET (rv, dev_id, PORT_STATUS, port_id, RXMAC_EN,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (val)
    {
      *enable = A_TRUE;
    }
  else
    {
      *enable = A_FALSE;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_txfc_status_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t val = 0, reg = 0, force, tmp;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  if (A_TRUE == enable)
    {
      val = 1;
    }
  else if (A_FALSE == enable)
    {
      val = 0;
    }
  else
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);
	tmp = reg;

  /* for those ports without PHY device we set MAC register */
  if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
    {
      SW_SET_REG_BY_FIELD (PORT_STATUS, FLOW_LINK_EN, 0, reg);
      SW_SET_REG_BY_FIELD (PORT_STATUS, TX_FLOW_EN, val, reg);
    }
  else
    {
      SW_GET_FIELD_BY_REG (PORT_STATUS, FLOW_LINK_EN, force, reg);
      if (force)
	{
	  /* flow control isn't in force mode so can't set */
	  return SW_DISABLE;
	}
      else
	{
	  SW_SET_REG_BY_FIELD (PORT_STATUS, TX_FLOW_EN, val, reg);
	}
    }
	if (tmp == reg)
		return SW_OK;
  HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_txfc_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t val;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_GET (rv, dev_id, PORT_STATUS, port_id, TX_FLOW_EN,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (val)
    {
      *enable = A_TRUE;
    }
  else
    {
      *enable = A_FALSE;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_rxfc_status_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t val = 0, reg, force, tmp;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  if (A_TRUE == enable)
    {
      val = 1;
    }
  else if (A_FALSE == enable)
    {
      val = 0;
    }
  else
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);
	tmp = reg;

  /* for those ports without PHY device we set MAC register */
  if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
    {
      SW_SET_REG_BY_FIELD (PORT_STATUS, FLOW_LINK_EN, 0, reg);
      SW_SET_REG_BY_FIELD (PORT_STATUS, RX_FLOW_EN, val, reg);
    }
  else
    {
      SW_GET_FIELD_BY_REG (PORT_STATUS, FLOW_LINK_EN, force, reg);
      if (force)
	{
	  /* flow control isn't in force mode so can't set */
	  return SW_DISABLE;
	}
      else
	{
	  SW_SET_REG_BY_FIELD (PORT_STATUS, RX_FLOW_EN, val, reg);
	}
    }
	if (tmp == reg)
		return SW_OK;
  HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_rxfc_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t val;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_GET (rv, dev_id, PORT_STATUS, port_id, RX_FLOW_EN,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (val)
    {
      *enable = A_TRUE;
    }
  else
    {
      *enable = A_FALSE;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_bp_status_set (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t val = 0, tmp = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  if (A_TRUE == enable)
    {
      val = 1;
    }
  else if (A_FALSE == enable)
    {
      val = 0;
    }
  else
    {
      return SW_BAD_PARAM;
    }
	HSL_REG_FIELD_GET (rv, dev_id, PORT_STATUS, port_id, TX_HALF_FLOW_EN,
		     (a_uint8_t *) (&tmp), sizeof (a_uint32_t));
	if (tmp == val)
		return SW_OK;

  HSL_REG_FIELD_SET (rv, dev_id, PORT_STATUS, port_id, TX_HALF_FLOW_EN,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_bp_status_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_GET (rv, dev_id, PORT_STATUS, port_id, TX_HALF_FLOW_EN,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (val)
    {
      *enable = A_TRUE;
    }
  else
    {
      *enable = A_FALSE;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_link_forcemode_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t reg = 0, tmp = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_EXCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);
	SW_GET_FIELD_BY_REG(PORT_STATUS, LINK_EN, tmp, reg);

  if (A_TRUE == enable)
    {
		if(tmp == 0)
			return SW_OK;
      SW_SET_REG_BY_FIELD (PORT_STATUS, LINK_EN, 0, reg);
    }
  else if (A_FALSE == enable)
    {
		if(tmp == 1)
			return SW_OK;
      /* for those ports without PHY, it can't sync link status */
      if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
	{
	  return SW_DISABLE;
	}
      SW_SET_REG_BY_FIELD (PORT_STATUS, LINK_EN, 1, reg);
    }
  else
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_SET (rv, dev_id, PORT_STATUS, port_id,
		     (a_uint8_t *) (&reg), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_link_forcemode_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_EXCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_GET (rv, dev_id, PORT_STATUS, port_id, LINK_EN,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (0 == val)
    {
      *enable = A_TRUE;
    }
  else
    {
      *enable = A_FALSE;
    }

  return SW_OK;
}

static sw_error_t
_dess_port_link_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * status)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
	{
		return SW_BAD_PARAM;
	}

		/* for those ports without PHY device supposed always link up */
	if (A_FALSE == _dess_port_phy_connected (dev_id, port_id))
	{
		*status = A_TRUE;
	}
	else
	{
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
		if (NULL == phy_drv->phy_link_status_get)
			return SW_NOT_SUPPORTED;

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);

		if (A_TRUE == phy_drv->phy_link_status_get (dev_id, phy_id))
		{
			*status = A_TRUE;
		}
		else
		{
			*status = A_FALSE;
		}
	}

	return SW_OK;
}

static sw_error_t
_dess_ports_link_status_get(a_uint32_t dev_id, a_uint32_t * status)
{
	sw_error_t rv;
	a_uint32_t port_id;
	a_uint32_t phy_id;
	hsl_dev_t *pdev = NULL;
	hsl_phy_ops_t *phy_drv;
	a_uint32_t port_bmp[SW_MAX_NR_DEV] = {0};

	HSL_DEV_ID_CHECK(dev_id);

	pdev = hsl_dev_ptr_get(dev_id);
	if (pdev == NULL)
		return SW_NOT_INITIALIZED;

	port_bmp[dev_id] = qca_ssdk_phy_type_port_bmp_get(dev_id, MALIBU_PHY_CHIP);

	*status = 0x0;
	for (port_id = 0; port_id < pdev->nr_ports; port_id++)
	{
		if (port_id >= SW_MAX_NR_PORT)
			break;
		/* for those ports without PHY device supposed always link up */
		if (A_FALSE == _dess_port_phy_connected(dev_id, port_id))
		{
			*status |= (0x1 << port_id);
		}
		else
		{
			if(port_bmp[dev_id] & (0x1 << port_id))
			{
				SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
				if (NULL == phy_drv->phy_link_status_get)
					return SW_NOT_SUPPORTED;
				rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
				SW_RTN_ON_ERROR(rv);

				if (A_TRUE == phy_drv->phy_link_status_get (dev_id, phy_id))
				{
					*status |= (0x1 << port_id);
				}
				else
				{
					*status &= ~(0x1 << port_id);
				}
			}
		}
	}
	return SW_OK;
}

static sw_error_t
_dess_port_mac_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t enable)
{
  sw_error_t rv = SW_OK;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  if (A_TRUE == enable)
    {
      val = 1;
    }
  else if (A_FALSE == enable)
    {
      val = 0;
    }
  else
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_SET (rv, dev_id, PORT_HDR_CTL, port_id, LOOPBACK_EN,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  return rv;
}

static sw_error_t
_dess_port_mac_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_FIELD_GET (rv, dev_id, PORT_HDR_CTL, port_id, LOOPBACK_EN,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  SW_RTN_ON_ERROR (rv);

  if (0 == val)
    {
      *enable = A_FALSE;
    }
  else
    {
      *enable = A_TRUE;
    }

  return SW_OK;
}


static sw_error_t
_dess_port_congestion_drop_set (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t queue_id, a_bool_t enable)
{
  sw_error_t rv = SW_OK;
  a_uint32_t val = 0, offset = 0, field = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  if (queue_id >= port_queue[port_id])
    {
      return SW_BAD_PARAM;
    }

  if (port_id != 0)
    offset = port_id * 4 + 2;
  offset += queue_id;

  if (A_TRUE == enable)
    {
      field = 1 << offset;
    }
  else if (A_FALSE == enable)
    {
      field = ~(1 << offset);
    }
  else
    {
      return SW_BAD_PARAM;
    }



  HSL_REG_ENTRY_GET (rv, dev_id, FLOW_CONGE_DROP_CTRL0, 0,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  if (A_TRUE == enable)
    {
      val = val | field;
    }
  else
    {
      val = val & field;
    }

  HSL_REG_ENTRY_SET (rv, dev_id, FLOW_CONGE_DROP_CTRL0, 0,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  return rv;
}


static sw_error_t
_dess_port_congestion_drop_get (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t queue_id, a_bool_t * enable)
{
  sw_error_t rv = SW_OK;
  a_uint32_t val, offset = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
    {
      return SW_BAD_PARAM;
    }

  if (queue_id >= port_queue[port_id])
    {
      return SW_BAD_PARAM;
    }

  if (port_id != 0)
    offset = port_id * 4 + 2;
  offset += queue_id;

  HSL_REG_ENTRY_GET (rv, dev_id, FLOW_CONGE_DROP_CTRL0, 0,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));
  val = (val >> offset) & 0x1;
  if (val == 0)
    {
      *enable = A_FALSE;
    }
  else if (val == 1)
    {
      *enable = A_TRUE;
    }
  return rv;
}

static sw_error_t
_dess_ring_flow_ctrl_thres_set (a_uint32_t dev_id, a_uint32_t ring_id,
				a_uint8_t on_thres, a_uint8_t off_thres)
{
  sw_error_t rv;
  a_uint32_t val = 0;

  HSL_DEV_ID_CHECK (dev_id);

  if (ring_id >= DMA_MAX_VIRT_RING)
    {
      return SW_BAD_PARAM;
    }

  if (on_thres > off_thres || on_thres == 0)
    {
      return SW_BAD_PARAM;
    }

  SW_SET_REG_BY_FIELD (RING_FLOW_CTRL_THRES, XON, on_thres, val);
  SW_SET_REG_BY_FIELD (RING_FLOW_CTRL_THRES, XOFF, off_thres, val);
  HSL_REG_ENTRY_SET (rv, dev_id, RING_FLOW_CTRL_THRES, ring_id,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));


  return rv;
}


static sw_error_t
_dess_ring_flow_ctrl_thres_get (a_uint32_t dev_id, a_uint32_t ring_id,
				a_uint8_t * on_thres, a_uint8_t * off_thres)
{

  sw_error_t rv;
  a_uint32_t val = 0;
  a_uint8_t hthres, lthres;

  HSL_DEV_ID_CHECK (dev_id);

  if (ring_id >= DMA_MAX_VIRT_RING)
    {
      return SW_BAD_PARAM;
    }

  HSL_REG_ENTRY_GET (rv, dev_id, RING_FLOW_CTRL_THRES, ring_id,
		     (a_uint8_t *) (&val), sizeof (a_uint32_t));

  SW_GET_FIELD_BY_REG (RING_FLOW_CTRL_THRES, XON, hthres, val);
  SW_GET_FIELD_BY_REG (RING_FLOW_CTRL_THRES, XOFF, lthres, val);

  *on_thres = hthres;
  *off_thres = lthres;

  return rv;
}

static sw_error_t
_dess_port_8023az_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_8023az_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_8023az_set (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_8023az_get (a_uint32_t dev_id, fal_port_t port_id,
		       a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_8023az_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_8023az_get (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_mdix_set (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_mdix_mode_t mode)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_mdix_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_mdix_set (dev_id, phy_id, mode);

  return rv;
}

static sw_error_t
_dess_port_mdix_get (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_mdix_mode_t * mode)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_mdix_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_mdix_get (dev_id, phy_id, mode);

  return rv;
}

static sw_error_t
_dess_port_mdix_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    fal_port_mdix_status_t * mode)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_mdix_status_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_mdix_status_get (dev_id, phy_id, mode);

  return rv;
}

static sw_error_t
_dess_port_combo_prefer_medium_set (a_uint32_t dev_id, fal_port_t port_id,
				    fal_port_medium_t phy_medium)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_combo_prefer_medium_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_combo_prefer_medium_set (dev_id, phy_id, phy_medium);

  return rv;
}

static sw_error_t
_dess_port_combo_prefer_medium_get (a_uint32_t dev_id, fal_port_t port_id,
				    fal_port_medium_t * phy_medium)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_combo_prefer_medium_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_combo_prefer_medium_get (dev_id, phy_id, phy_medium);

  return rv;
}

static sw_error_t
_dess_port_combo_medium_status_get (a_uint32_t dev_id, fal_port_t port_id,
				    fal_port_medium_t * phy_medium)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_combo_medium_status_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_combo_medium_status_get (dev_id, phy_id, phy_medium);

  return rv;
}

static sw_error_t
_dess_port_combo_fiber_mode_set (a_uint32_t dev_id, fal_port_t port_id,
				 fal_port_fiber_mode_t fiber_mode)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_combo_fiber_mode_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_combo_fiber_mode_set (dev_id, phy_id, fiber_mode);

  return rv;
}

static sw_error_t
_dess_port_combo_fiber_mode_get (a_uint32_t dev_id, fal_port_t port_id,
				 fal_port_fiber_mode_t * fiber_mode)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_combo_fiber_mode_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_combo_fiber_mode_get (dev_id, phy_id, fiber_mode);

  return rv;
}

static sw_error_t
_dess_port_local_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_local_loopback_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_local_loopback_set (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_local_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_local_loopback_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_local_loopback_get (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_remote_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_remote_loopback_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_remote_loopback_set (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_remote_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_remote_loopback_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_remote_loopback_get (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_reset (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_reset)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_reset(dev_id, phy_id);

  return rv;
}

static sw_error_t
_dess_port_power_off (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_power_off)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_power_off(dev_id, phy_id);

  return rv;
}

static sw_error_t
_dess_port_power_on (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_power_on)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_power_on(dev_id, phy_id);

  return rv;
}

static sw_error_t
_dess_port_wol_status_set (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_wol_status_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_wol_status_set (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_wol_status_get (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_wol_status_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_wol_status_get (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_phy_id_get (a_uint32_t dev_id, fal_port_t port_id,
				a_uint16_t * org_id, a_uint16_t * rev_id)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;
  a_uint32_t phy_data;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_id_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_id_get (dev_id, phy_id, &phy_data);
  SW_RTN_ON_ERROR (rv);

  *org_id = (phy_data >> 16) & 0xffff;
  *rev_id = phy_data & 0xffff;

  return rv;
}

static sw_error_t
_dess_port_magic_frame_mac_set (a_uint32_t dev_id, fal_port_t port_id,
				fal_mac_addr_t * mac)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_magic_frame_mac_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_magic_frame_mac_set (dev_id, phy_id,mac);

  return rv;
}

static sw_error_t
_dess_port_magic_frame_mac_get (a_uint32_t dev_id, fal_port_t port_id,
				fal_mac_addr_t * mac)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_magic_frame_mac_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_magic_frame_mac_get (dev_id, phy_id,mac);

  return rv;
}

static sw_error_t
_dess_port_interface_mode_set (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_interface_mode_t  mode)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_interface_mode_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_interface_mode_set (dev_id, phy_id,mode);

  return rv;
}

static sw_error_t
_dess_port_interface_mode_get (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_interface_mode_t * mode)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_interface_mode_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_interface_mode_get (dev_id, phy_id,mode);

  return rv;
}

static sw_error_t
_dess_port_interface_mode_status_get (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_interface_mode_t * mode)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_interface_mode_status_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_interface_mode_status_get (dev_id, phy_id,mode);

  return rv;
}

static sw_error_t
_dess_port_counter_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);
  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_counter_set)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_counter_set (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_counter_get (a_uint32_t dev_id, fal_port_t port_id,
		       a_bool_t * enable)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_counter_get)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_counter_get (dev_id, phy_id, enable);

  return rv;
}

static sw_error_t
_dess_port_counter_show (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_counter_info_t * counter_info)
{
  sw_error_t rv;
  a_uint32_t phy_id = 0;
  hsl_phy_ops_t *phy_drv;

  HSL_DEV_ID_CHECK (dev_id);

  if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
    {
      return SW_BAD_PARAM;
    }

  SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
  if (NULL == phy_drv->phy_counter_show)
    return SW_NOT_SUPPORTED;

  rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
  SW_RTN_ON_ERROR (rv);

  rv = phy_drv->phy_counter_show (dev_id, phy_id,counter_info);

  return rv;
}

/**
 * @brief Set duplex mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] duplex duplex mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_duplex_set (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_duplex_t duplex)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_duplex_set (dev_id, port_id, duplex);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get duplex mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] duplex duplex mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_duplex_get (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_duplex_t * pduplex)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_duplex_get (dev_id, port_id, pduplex);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set speed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] speed port speed
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_speed_set (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_speed_t speed)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_speed_set (dev_id, port_id, speed);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get speed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] speed port speed
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_speed_get (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_speed_t * pspeed)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_speed_get (dev_id, port_id, pspeed);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get auto negotiation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] status A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_autoneg_status_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * status)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_autoneg_status_get (dev_id, port_id, status);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Enable auto negotiation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_autoneg_enable (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_autoneg_enable (dev_id, port_id);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Restart auto negotiation procedule on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_autoneg_restart (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_autoneg_restart (dev_id, port_id);
  HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
dess_port_autoneg_adv_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_uint32_t autoadv)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_autoneg_adv_set (dev_id, port_id, autoadv);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get auto negotiation advtisement ability on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] autoadv auto negotiation advtisement ability bit map
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_autoneg_adv_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_uint32_t * autoadv)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_autoneg_adv_get (dev_id, port_id, autoadv);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set flow control(rx/tx/bp) status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_flowctrl_set (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_flowctrl_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set flow control(rx/tx/bp) threshold on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] on on threshold
 * @param[in] off off threshold
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_flowctrl_thresh_set (a_uint32_t dev_id, fal_port_t port_id,
			a_uint8_t on, a_uint8_t off)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _dess_port_flowctrl_thresh_set (dev_id, port_id, on, off);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get flow control status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_flowctrl_get (a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_flowctrl_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set flow control force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_flowctrl_forcemode_set (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_flowctrl_forcemode_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get flow control force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_flowctrl_forcemode_get (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_flowctrl_forcemode_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set powersaving status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_powersave_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_powersave_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get powersaving status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_powersave_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_powersave_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set hibernate status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_hibernate_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_hibernate_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get hibernate status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_hibernate_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_hibernate_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Run cable diagnostic test on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mdi_pair mdi pair id
 * @param[out] cable_status cable status
 * @param[out] cable_len cable len
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_cdt (a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
	       fal_cable_status_t * cable_status, a_uint32_t * cable_len)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_cdt (dev_id, port_id, mdi_pair, cable_status, cable_len);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_rxhdr_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_header_mode_t mode)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_rxhdr_mode_set (dev_id, port_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_rxhdr_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_header_mode_t * mode)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_rxhdr_mode_get (dev_id, port_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_txhdr_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_header_mode_t mode)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_txhdr_mode_set (dev_id, port_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_txhdr_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_header_mode_t * mode)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_txhdr_mode_get (dev_id, port_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of Atheros header type value on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] type header type value
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_header_type_set (a_uint32_t dev_id, a_bool_t enable, a_uint32_t type)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_header_type_set (dev_id, enable, type);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get status of Atheros header type value on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] type header type value
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_header_type_get (a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * type)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_header_type_get (dev_id, enable, type);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of txmac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_txmac_status_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_txmac_status_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get status of txmac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_txmac_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_txmac_status_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of rxmac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_rxmac_status_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_rxmac_status_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get status of rxmac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_rxmac_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_rxmac_status_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of tx flow control on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_txfc_status_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_txfc_status_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get status of tx flow control on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_txfc_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_txfc_status_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of rx flow control on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_rxfc_status_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_rxfc_status_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of rx flow control on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_rxfc_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_rxfc_status_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of back pressure on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_bp_status_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_bp_status_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set status of back pressure on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_bp_status_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_bp_status_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set link force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_link_forcemode_set (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_link_forcemode_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get link force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_link_forcemode_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_link_forcemode_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get link status on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] status link status up (A_TRUE) or down (A_FALSE)
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_link_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * status)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_link_status_get (dev_id, port_id, status);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get link status on all ports.
 * @param[in] dev_id device id
 * @param[out] status link status bitmap and bit 0 for port 0, bi 1 for port 1, ..., etc.
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ports_link_status_get(a_uint32_t dev_id, a_uint32_t * status)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ports_link_status_get(dev_id, status);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set mac loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_mac_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_mac_loopback_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get mac loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_mac_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_mac_loopback_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set flow congestion drop on a particular port queue.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_congestion_drop_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint32_t queue_id, a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_congestion_drop_set (dev_id, port_id, queue_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set flow congestion drop on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_congestion_drop_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint32_t queue_id, a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_congestion_drop_get (dev_id, port_id, queue_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set flow ctrl thres on a particular DMA ring.
 * @param[in] dev_id device id
 * @param[in] ring_id ring id
 * @param[in] on_thres on_thres
 * @param[in] off_thres off_thres
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ring_flow_ctrl_thres_set (a_uint32_t dev_id, a_uint32_t ring_id,
			       a_uint8_t on_thres, a_uint8_t off_thres)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_ring_flow_ctrl_thres_set (dev_id, ring_id, on_thres, off_thres);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set flow ctrl thres on a particular DMA ring.
 * @param[in] dev_id device id
 * @param[in] ring_id ring id
 * @param[out] on_thres on_thres
 * @param[out] off_thres off_thres
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ring_flow_ctrl_thres_get (a_uint32_t dev_id, a_uint32_t ring_id,
			       a_uint8_t * on_thres, a_uint8_t * off_thres)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_ring_flow_ctrl_thres_get (dev_id, ring_id, on_thres, off_thres);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set 802.3az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_8023az_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_8023az_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get 8023az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_8023az_get (a_uint32_t dev_id, fal_port_t port_id,
		      a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_8023az_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set mdix on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_mdix_set (a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t mode)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_mdix_set (dev_id, phy_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get mdix configuration on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode 
 * @return SW_OK or error code
 */

HSL_LOCAL sw_error_t
dess_port_mdix_get (a_uint32_t dev_id, fal_port_t port_id,
		    fal_port_mdix_mode_t * mode)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_mdix_get (dev_id, port_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get mdix status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode 
 * @return SW_OK or error code
 */

HSL_LOCAL sw_error_t
dess_port_mdix_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   fal_port_mdix_status_t * mode)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_mdix_status_get (dev_id, port_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set combo prefer medium on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] phy_medium [fiber or copper]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_combo_prefer_medium_set (a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t phy_medium)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_combo_prefer_medium_set (dev_id, phy_id, phy_medium);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get combo prefer medium on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] phy_medium [fiber or copper]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_combo_prefer_medium_get (a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t * phy_medium)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_combo_prefer_medium_get (dev_id, phy_id, phy_medium);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get current combo medium status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] phy_medium [fiber or copper]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_combo_medium_status_get (a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t * phy_medium)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_combo_medium_status_get (dev_id, phy_id, phy_medium);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set fiber mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] fiber mode [1000bx or 100fx]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_combo_fiber_mode_set (a_uint32_t dev_id, a_uint32_t phy_id,
				fal_port_fiber_mode_t fiber_mode)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_combo_fiber_mode_set (dev_id, phy_id, fiber_mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get fiber mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] fiber mode [1000bx or 100fx]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_combo_fiber_mode_get (a_uint32_t dev_id, a_uint32_t phy_id,
				fal_port_fiber_mode_t * fiber_mode)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_combo_fiber_mode_get (dev_id, phy_id, fiber_mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set phy local loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_local_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_local_loopback_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get phy local loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_local_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_local_loopback_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set phy remote loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_remote_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_remote_loopback_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get phy remote loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_remote_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_remote_loopback_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief software reset on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_reset (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_reset (dev_id, port_id);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief phy power off on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_power_off (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_power_off (dev_id, port_id);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief phy power on on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_power_on (a_uint32_t dev_id, fal_port_t port_id)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_power_on (dev_id, port_id);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set phy wol enable on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_wol_status_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_wol_status_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get phy wol status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_wol_status_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_wol_status_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get phy id on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] org_id and rev_id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_phy_id_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint16_t * org_id, a_uint16_t * rev_id)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_phy_id_get (dev_id, port_id, org_id,rev_id);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set phy magic frame mac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mac address
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_magic_frame_mac_set (a_uint32_t dev_id, fal_port_t port_id,
			      fal_mac_addr_t * mac)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_magic_frame_mac_set (dev_id, port_id, mac);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get phy magic frame mac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mac address
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_magic_frame_mac_get (a_uint32_t dev_id, fal_port_t port_id,
			      fal_mac_addr_t * mac)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_magic_frame_mac_get (dev_id, port_id, mac);
  HSL_API_UNLOCK;
  return rv;
}


/**
 * @brief Set phy interface mode.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] interface mode..
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_interface_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t  mode)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_interface_mode_set (dev_id, port_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set phy interface mode.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] interface mode..
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_interface_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t  * mode)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_interface_mode_get (dev_id, port_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set phy interface mode status.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] interface mode..
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_interface_mode_status_get (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t  * mode)
{
  sw_error_t rv;
  HSL_API_LOCK;
  rv = _dess_port_interface_mode_status_get (dev_id, port_id, mode);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Set counter status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_counter_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_counter_set (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get counter status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_counter_get (a_uint32_t dev_id, fal_port_t port_id,
		      a_bool_t * enable)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_counter_get (dev_id, port_id, enable);
  HSL_API_UNLOCK;
  return rv;
}

/**
 * @brief Get counter statistics  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] frame counter statistics
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_counter_show(a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_counter_info_t * counter_info)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _dess_port_counter_show (dev_id, port_id, counter_info);
  HSL_API_UNLOCK;
  return rv;
}

sw_error_t
dess_port_ctrl_init (a_uint32_t dev_id)
{
  HSL_DEV_ID_CHECK (dev_id);

#ifndef HSL_STANDALONG
  {
    hsl_api_t *p_api;

    SW_RTN_ON_NULL (p_api = hsl_api_ptr_get (dev_id));

    p_api->port_duplex_get = dess_port_duplex_get;
    p_api->port_duplex_set = dess_port_duplex_set;
    p_api->port_speed_get = dess_port_speed_get;
    p_api->port_speed_set = dess_port_speed_set;
    p_api->port_autoneg_status_get = dess_port_autoneg_status_get;
    p_api->port_autoneg_enable = dess_port_autoneg_enable;
    p_api->port_autoneg_restart = dess_port_autoneg_restart;
    p_api->port_autoneg_adv_get = dess_port_autoneg_adv_get;
    p_api->port_autoneg_adv_set = dess_port_autoneg_adv_set;
    p_api->port_flowctrl_set = dess_port_flowctrl_set;
    p_api->port_flowctrl_get = dess_port_flowctrl_get;
	p_api->port_flowctrl_thresh_set = dess_port_flowctrl_thresh_set;
    p_api->port_flowctrl_forcemode_set = dess_port_flowctrl_forcemode_set;
    p_api->port_flowctrl_forcemode_get = dess_port_flowctrl_forcemode_get;
    p_api->port_powersave_set = dess_port_powersave_set;
    p_api->port_powersave_get = dess_port_powersave_get;
    p_api->port_hibernate_set = dess_port_hibernate_set;
    p_api->port_hibernate_get = dess_port_hibernate_get;
    p_api->port_cdt = dess_port_cdt;
    p_api->port_rxhdr_mode_set = dess_port_rxhdr_mode_set;
    p_api->port_rxhdr_mode_get = dess_port_rxhdr_mode_get;
    p_api->port_txhdr_mode_set = dess_port_txhdr_mode_set;
    p_api->port_txhdr_mode_get = dess_port_txhdr_mode_get;
    p_api->header_type_set = dess_header_type_set;
    p_api->header_type_get = dess_header_type_get;
    p_api->port_txmac_status_set = dess_port_txmac_status_set;
    p_api->port_txmac_status_get = dess_port_txmac_status_get;
    p_api->port_rxmac_status_set = dess_port_rxmac_status_set;
    p_api->port_rxmac_status_get = dess_port_rxmac_status_get;
    p_api->port_txfc_status_set = dess_port_txfc_status_set;
    p_api->port_txfc_status_get = dess_port_txfc_status_get;
    p_api->port_rxfc_status_set = dess_port_rxfc_status_set;
    p_api->port_rxfc_status_get = dess_port_rxfc_status_get;
    p_api->port_bp_status_set = dess_port_bp_status_set;
    p_api->port_bp_status_get = dess_port_bp_status_get;
    p_api->port_link_forcemode_set = dess_port_link_forcemode_set;
    p_api->port_link_forcemode_get = dess_port_link_forcemode_get;
    p_api->port_link_status_get = dess_port_link_status_get;
    p_api->ports_link_status_get = dess_ports_link_status_get;
    p_api->port_mac_loopback_set = dess_port_mac_loopback_set;
    p_api->port_mac_loopback_get = dess_port_mac_loopback_get;
    p_api->port_congestion_drop_set = dess_port_congestion_drop_set;
    p_api->port_congestion_drop_get = dess_port_congestion_drop_get;
    p_api->ring_flow_ctrl_thres_set = dess_ring_flow_ctrl_thres_set;
    p_api->ring_flow_ctrl_thres_get = dess_ring_flow_ctrl_thres_get;
    p_api->port_8023az_set = dess_port_8023az_set;
    p_api->port_8023az_get = dess_port_8023az_get;
    p_api->port_mdix_set = dess_port_mdix_set;
    p_api->port_mdix_get = dess_port_mdix_get;
    p_api->port_mdix_status_get = dess_port_mdix_status_get;
    p_api->port_combo_prefer_medium_set = dess_port_combo_prefer_medium_set;
    p_api->port_combo_prefer_medium_get = dess_port_combo_prefer_medium_get;
    p_api->port_combo_medium_status_get = dess_port_combo_medium_status_get;
    p_api->port_combo_fiber_mode_set = dess_port_combo_fiber_mode_set;
    p_api->port_combo_fiber_mode_get = dess_port_combo_fiber_mode_get;
    p_api->port_local_loopback_set = dess_port_local_loopback_set;
    p_api->port_local_loopback_get = dess_port_local_loopback_get;
    p_api->port_remote_loopback_set = dess_port_remote_loopback_set;
    p_api->port_remote_loopback_get = dess_port_remote_loopback_get;
    p_api->port_reset = dess_port_reset;
    p_api->port_power_off = dess_port_power_off;
    p_api->port_power_on = dess_port_power_on;
    p_api->port_phy_id_get = dess_port_phy_id_get;
    p_api->port_wol_status_set = dess_port_wol_status_set;
    p_api->port_wol_status_get = dess_port_wol_status_get;
    p_api->port_magic_frame_mac_set = dess_port_magic_frame_mac_set;
    p_api->port_magic_frame_mac_get = dess_port_magic_frame_mac_get;
    p_api->port_interface_mode_set = dess_port_interface_mode_set;
    p_api->port_interface_mode_get = dess_port_interface_mode_get;
    p_api->port_interface_mode_status_get = dess_port_interface_mode_status_get;
    p_api->port_counter_set = dess_port_counter_set;
    p_api->port_counter_get = dess_port_counter_get;
    p_api->port_counter_show = dess_port_counter_show;
  }
#endif

  return SW_OK;
}

/**
 * @}
 */
