/*

  Broadcom BCM43xx wireless driver

  Copyright (c) 2005 Martin Langer <martin-langer@gmx.de>,
                     Stefano Brivio <st3@riseup.net>
                     Michael Buesch <mb@bu3sch.de>
                     Danny van Dyk <kugelfang@gentoo.org>
                     Andreas Jaggi <andreas.jaggi@waterwave.ch>

  Some parts of the code in this file are derived from the ipw2200
  driver  Copyright(c) 2003 - 2004 Intel Corporation.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
  Boston, MA 02110-1301, USA.

*/

#include <linux/delay.h>

#include "bcm43xx.h"
#include "bcm43xx_power.h"
#include "bcm43xx_main.h"


//TODO Kill this file.

/* Set the PowerSavingControlBits.
 * Bitvalues:
 *   0  => unset the bit
 *   1  => set the bit
 *   -1 => calculate the bit
 */
void bcm43xx_power_saving_ctl_bits(struct bcm43xx_wldev *dev,
				   int bit25, int bit26)
{
	int i;
	u32 status;

//FIXME: Force 25 to off and 26 to on for now:
bit25 = 0;
bit26 = 1;

	if (bit25 == -1) {
		//TODO: If powersave is not off and FIXME is not set and we are not in adhoc
		//	and thus is not an AP and we are associated, set bit 25
	}
	if (bit26 == -1) {
		//TODO: If the device is awake or this is an AP, or we are scanning, or FIXME,
		//	or we are associated, or FIXME, or the latest PS-Poll packet sent was
		//	successful, set bit26
	}
	status = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	if (bit25)
		status |= BCM43xx_SBF_PS1;
	else
		status &= ~BCM43xx_SBF_PS1;
	if (bit26)
		status |= BCM43xx_SBF_PS2;
	else
		status &= ~BCM43xx_SBF_PS2;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, status);
	if (bit26 && dev->dev->id.revision >= 5) {
		for (i = 0; i < 100; i++) {
			if (bcm43xx_shm_read32(dev, BCM43xx_SHM_SHARED, 0x0040) != 4)
				break;
			udelay(10);
		}
	}
}
