/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __AL_FABRIC_H__
#define __AL_FABRIC_H__

/**
 * Check if Hardware Cache-Coherency is enabled or not
 * @return   0 if Hardware Cache-Coherency is not enabled and a positive number
 *           otherwise
 */
int al_fabric_hwcc_enabled(void);

int al_fabric_init(void);

#endif /* __AL_FABRIC_H__ */
