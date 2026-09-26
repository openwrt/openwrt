/*
 * al_eth_sysfs.h: AnnapurnaLabs Unified 1GbE and 10GbE ethernet driver.
 *
 * Copyright (C) 2014 Annapurna Labs Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
#ifndef __AL_ETH_SYSFS_H__
#define __AL_ETH_SYSFS_H__

int al_eth_sysfs_init(struct device *dev);

void al_eth_sysfs_terminate(struct device *dev);

#endif /* __AL_ETH_SYSFS_H__ */

