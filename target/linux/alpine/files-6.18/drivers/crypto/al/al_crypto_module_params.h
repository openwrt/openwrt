/*
 * drivers/crypto/al/al_crypto_module_params.h
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

#ifndef __AL_CRYPTO_MODULE_PARAMS_H__
#define __AL_CRYPTO_MODULE_PARAMS_H__

bool al_crypto_get_use_virtual_function(void);

int al_crypto_get_crc_channels(void);

int al_crypto_get_max_channels(void);

int al_crypto_get_ring_alloc_order(void);

int al_crypto_get_tx_descs_order(void);

int al_crypto_get_rx_descs_order(void);

bool al_crypto_get_use_single_msix(void);

#endif /* __AL_CRYPTO_MODULE_PARAMS_H__ */
