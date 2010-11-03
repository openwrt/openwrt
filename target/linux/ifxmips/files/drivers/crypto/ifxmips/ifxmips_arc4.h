#ifndef IFXMIPS_ARC4_H
#define IFXMIPS_ARC4_H

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2010 Ralph Hempel <ralph.hempel@lantiq.com>
 *   Copyright (C) 2009 Mohammad Firdaus
 */

/*!
 \defgroup IFX_DEU IFX_DEU_DRIVERS
 \ingroup API
 \brief ifx deu driver file
*/

/*!
  \defgroup IFX_DEU_DEFINITIONS IFX_DEU_DRIVERS
  \ingroup  IFX_DEU
  \brief ifx deu definitions
*/

/*!
  \file		ifxmips_arc4.h
  \brief 	ARC4 DEU header driver file
*/



#define ARC4_START   IFX_ARC4_CON

#ifdef CONFIG_CRYPTO_DEV_ARC4_AR9
#include "ifxmips_deu_structs_ar9.h"
#endif
#ifdef CONFIG_CRYPTO_DEV_DMA_AR9
#include "ifxmips_deu_structs_ar9.h"
#endif

#ifdef CONFIG_CRYPTO_DEV_ARC4_VR9
#include "ifxmips_deu_structs_vr9.h"
#endif
#ifdef CONFIG_CRYPTO_DEV_DMA_VR9
#include "ifxmips_deu_structs_vr9.h"
#include <asm/ifx/irq.h>
#endif

#endif  /* IFXMIPS_ARC4_H */
