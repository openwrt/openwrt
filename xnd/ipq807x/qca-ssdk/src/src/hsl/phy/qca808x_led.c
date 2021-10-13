/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "sw.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"
#include "qca808x_phy.h"
#include "qca808x_led.h"

static sw_error_t
_qca808x_phy_led_active_set(a_uint32_t dev_id, a_uint32_t phy_id,
	led_ctrl_pattern_t *pattern)
{
	sw_error_t rv = SW_OK;
	a_uint16_t phy_data = 0;

	phy_data= qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_PHY_MMD7_LED_POLARITY_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);
	if(pattern->map & BIT(LED_ACTIVE_HIGH))
	{
		phy_data |= QCA808X_PHY_MMD7_LED_POLARITY_MASK;
	}
	else
	{
		phy_data &= ~QCA808X_PHY_MMD7_LED_POLARITY_MASK;
	}
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
		QCA808X_PHY_MMD7_LED_POLARITY_CTRL, phy_data);

	return rv;
}

static sw_error_t
_qca808x_phy_led_active_get(a_uint32_t dev_id, a_uint32_t phy_id,
	led_ctrl_pattern_t *pattern)
{
	a_uint16_t phy_data = 0;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
		QCA808X_PHY_MMD7_LED_POLARITY_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);
	if(phy_data & QCA808X_PHY_MMD7_LED_POLARITY_MASK)
	{
		pattern->map |= BIT(LED_ACTIVE_HIGH);
	}

	return SW_OK;
}

static sw_error_t
_qca808x_phy_led_pattern_map_from_phy(a_uint32_t dev_id, a_uint32_t phy_id,
	led_ctrl_pattern_t *pattern, a_uint16_t *phy_data)
{
	if (qca808x_phy_2500caps(dev_id, phy_id) == A_TRUE)
	{
		if(*phy_data & QCA808X_PHY_LINK_2500M_LIGHT_EN)
		{
			pattern->map |= BIT(LINK_2500M_LIGHT_EN);
		}
	}
	if(*phy_data & QCA808X_PHY_LINK_1000M_LIGHT_EN)
	{
		pattern->map |= BIT(LINK_1000M_LIGHT_EN);
	}
	if(*phy_data & QCA808X_PHY_LINK_100M_LIGHT_EN)
	{
		pattern->map |= BIT(LINK_100M_LIGHT_EN);
	}
	if(*phy_data & QCA808X_PHY_LINK_10M_LIGHT_EN)
	{
		pattern->map |= BIT(LINK_10M_LIGHT_EN);
	}
	if (*phy_data & QCA808X_PHY_RX_TRAFFIC_BLINK_EN)
	{
		pattern->map |= BIT(RX_TRAFFIC_BLINK_EN);
	}
	if (*phy_data & QCA808X_PHY_TX_TRAFFIC_BLINK_EN)
	{
		pattern->map |= BIT(TX_TRAFFIC_BLINK_EN);
	}

	return SW_OK;
}

static sw_error_t
_qca808x_phy_led_pattern_map_to_phy(a_uint32_t dev_id, a_uint32_t phy_id,
	led_ctrl_pattern_t *pattern, a_uint32_t *led_map)
{
	if (qca808x_phy_2500caps(dev_id, phy_id) == A_TRUE)
	{
		if (pattern->map & BIT(LINK_2500M_LIGHT_EN))
		{
			*led_map |=  QCA808X_PHY_LINK_2500M_LIGHT_EN;
		}
	}
	if (pattern->map & BIT(LINK_1000M_LIGHT_EN))
	{
		*led_map |=  QCA808X_PHY_LINK_1000M_LIGHT_EN;
	}
	if (pattern->map & BIT(LINK_100M_LIGHT_EN))
	{
		*led_map |=  QCA808X_PHY_LINK_100M_LIGHT_EN;
	}
	if (pattern->map & BIT(LINK_10M_LIGHT_EN))
	{
		*led_map |=  QCA808X_PHY_LINK_10M_LIGHT_EN;
	}
	if (pattern->map & BIT(RX_TRAFFIC_BLINK_EN))
	{
		*led_map |=  QCA808X_PHY_RX_TRAFFIC_BLINK_EN;
	}
	if (pattern->map & BIT(TX_TRAFFIC_BLINK_EN))
	{
		*led_map |=  QCA808X_PHY_TX_TRAFFIC_BLINK_EN;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_led_ctrl_pattern_set
*
*/
sw_error_t
qca808x_phy_led_ctrl_pattern_set(a_uint32_t dev_id, a_uint32_t phy_id,
	led_ctrl_pattern_t *pattern)
{
	sw_error_t rv = SW_OK;
	a_uint32_t source_id = 0;

	if(LED_PATTERN_MAP_EN != pattern->mode)
	{
		SSDK_ERROR("led mode %d is not supported\n", pattern->mode);
		return SW_NOT_SUPPORTED;
	}
	for(source_id = QCA808X_PHY_LED_SOURCE0; source_id <= QCA808X_PHY_LED_SOURCE2;
		source_id++)
	{
		/*three source use the same pattern*/
		rv = qca808x_phy_led_ctrl_source_set (dev_id, phy_id, source_id, pattern);
		SW_RTN_ON_ERROR(rv);
	}

	return rv;
}

/******************************************************************************
*
* qca808x_phy_led_ctrl_pattern_get
*
*/
sw_error_t
qca808x_phy_led_ctrl_pattern_get(a_uint32_t dev_id, a_uint32_t phy_id,
	led_ctrl_pattern_t *pattern)
{
	sw_error_t rv = SW_OK;

	/*three source use the same pattern*/
	rv = qca808x_phy_led_ctrl_source_get(dev_id, phy_id, QCA808X_PHY_LED_SOURCE0,
		pattern);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_led_source_pattern_set
*
*/
sw_error_t
qca808x_phy_led_ctrl_source_set(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t source_id, led_ctrl_pattern_t *pattern)
{
	sw_error_t rv = SW_OK;
	a_uint32_t led_map = 0;
	a_uint16_t led_mmd_addr = 0;

	if(LED_PATTERN_MAP_EN != pattern->mode)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = _qca808x_phy_led_active_set(dev_id,  phy_id, pattern);
	SW_RTN_ON_ERROR(rv);
	rv = _qca808x_phy_led_pattern_map_to_phy(dev_id, phy_id, pattern, &led_map);
	SW_RTN_ON_ERROR(rv);
	switch(source_id)
	{
		case QCA808X_PHY_LED_SOURCE0:
			led_mmd_addr = QCA808X_PHY_MMD7_LED0_CTRL;
			break;
		case QCA808X_PHY_LED_SOURCE1:
			led_mmd_addr = QCA808X_PHY_MMD7_LED1_CTRL;
			break;
		case QCA808X_PHY_LED_SOURCE2:
			led_mmd_addr = QCA808X_PHY_MMD7_LED2_CTRL;
			break;
		default:
			SSDK_ERROR("source %d is not support\n", source_id);
			break;
	}
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
		led_mmd_addr, led_map);

	return rv;
}
/******************************************************************************
*
* qca808x_phy_led_source_pattern_get
*
*/
sw_error_t
qca808x_phy_led_ctrl_source_get(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t source_id, led_ctrl_pattern_t *pattern)
{
	sw_error_t rv = SW_OK;
	a_uint16_t phy_data = 0, led_mmd_addr = 0;

	pattern->map = 0;
	pattern->mode = LED_PATTERN_MAP_EN;
	rv = _qca808x_phy_led_active_get(dev_id, phy_id, pattern);
	SW_RTN_ON_ERROR(rv);
	switch(source_id)
	{
		case QCA808X_PHY_LED_SOURCE0:
			led_mmd_addr = QCA808X_PHY_MMD7_LED0_CTRL;
			break;
		case QCA808X_PHY_LED_SOURCE1:
			led_mmd_addr = QCA808X_PHY_MMD7_LED1_CTRL;
			break;
		case QCA808X_PHY_LED_SOURCE2:
			led_mmd_addr = QCA808X_PHY_MMD7_LED2_CTRL;
			break;
		default:
			SSDK_ERROR("source %d is not support\n", source_id);
			break;
	}
	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
		led_mmd_addr);
	PHY_RTN_ON_READ_ERROR(phy_data);
	rv = _qca808x_phy_led_pattern_map_from_phy(dev_id, phy_id, pattern,
		&phy_data);

	return rv;
}

void qca808x_phy_led_api_ops_init(hsl_phy_ops_t *qca808x_phy_led_api_ops)
{
	if (!qca808x_phy_led_api_ops) {
		return;
	}
	qca808x_phy_led_api_ops->phy_led_ctrl_pattern_get = qca808x_phy_led_ctrl_pattern_get;
	qca808x_phy_led_api_ops->phy_led_ctrl_pattern_set = qca808x_phy_led_ctrl_pattern_set;
	qca808x_phy_led_api_ops->phy_led_ctrl_source_set = qca808x_phy_led_ctrl_source_set;

	return;
}
