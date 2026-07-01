/*
 * drivers/crypto/al/al_crypto_module_params.c
 *
 * Annapurna Labs Crypto driver - module params
 *
 * Copyright (C) 2013 Annapurna Labs Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/module.h>

#include "al_crypto.h"
#include "al_crypto_module_params.h"

static bool use_virtual_function = true;
module_param(use_virtual_function, bool, 0644);
MODULE_PARM_DESC(
	use_virtual_function,
	"use the SR-IOV capability of the crypto engine (default: true)"
	"If we use the VF we will have 4 crc channels and 4 crypto channels");

static int crc_channels = 1;
module_param(crc_channels, int, 0644);
MODULE_PARM_DESC(
	crc_channels,
	"number of  crc channels (queues) to enable (default: 1)");

static int max_channels = AL_CRYPTO_DMA_MAX_CHANNELS;
module_param(max_channels, int, 0644);
MODULE_PARM_DESC(
	max_channels,
	"maximum number of channels (queues) to enable (default: 4)");

static int ring_alloc_order = 10;
module_param(ring_alloc_order, int, 0644);
MODULE_PARM_DESC(
	ring_alloc_order,
	 "allocate 2^n descriptors per channel"
	 " (default: 10 max: 16)");

static int tx_descs_order = 14;
module_param(tx_descs_order, int, 0644);
MODULE_PARM_DESC(
	tx_descs_order,
	"allocate 2^n of descriptors in Tx queue (default: 14)");

static int rx_descs_order = 14;
module_param(rx_descs_order, int, 0644);
MODULE_PARM_DESC(
	rx_descs_order,
	"allocate 2^n of descriptors in Rx queue (default: 14)");

static bool use_single_msix;
module_param(use_single_msix, bool, 0644);
MODULE_PARM_DESC(
	use_single_msix,
	"Use single msix (one msi-x per group and not one per queue)");

bool al_crypto_get_use_virtual_function(void)
{
	return use_virtual_function;
}

bool al_crypto_get_use_single_msix(void)
{
	return use_single_msix;
}

int al_crypto_get_crc_channels(void)
{
	return crc_channels;
}

int al_crypto_get_max_channels(void)
{
	return max_channels;
}

int al_crypto_get_ring_alloc_order(void)
{
	return ring_alloc_order;
}

int al_crypto_get_tx_descs_order(void)
{
	return tx_descs_order;
}

int al_crypto_get_rx_descs_order(void)
{
	return rx_descs_order;
}
