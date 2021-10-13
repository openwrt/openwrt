/* AQ_PhyInterface.c */

/************************************************************************************
* Copyright (c) 2015, Aquantia
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
*
* $Revision: #12 $
*
* $DateTime: 2015/02/25 15:34:49 $
*
* $Label: $
*
************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>

#include <linux/sockios.h>
#include <linux/mii.h>
#include <linux/ethtool.h>

#include "AQ_PhyInterface.h"
#include "AQ_PlatformRoutines.h"

#define MII_ADDR_C45 (0x8000)

extern int sock;
extern char devname[7];

static struct ifreq ifr;

/*! Provides generic synchronous PHY register write functionality. It is the
 * responsibility of the system designer to provide the specific MDIO address
 * pointer updates, etc. in order to accomplish this write operation.
 * It will be assumed that the write has been completed by the time this
 * function returns.*/
void AQ_API_MDIO_Write(
	/*! Uniquely identifies the port within the system. AQ_Port must be
	 * defined to a whatever data type is suitable for the platform.*/
	AQ_Port PHY_ID,
	/*! The address of the MMD within the target PHY. */
	unsigned int MMD,
	/*! The 16-bit address of the PHY register being written. */
	unsigned int address,
	/*! The 16-bits of data to write to the specified PHY register. */
	unsigned int data)
{
	struct mii_ioctl_data mii;

	/*
	 * Frame the control structures
	 * and send the ioctl to kernel.
	 */
	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, devname, sizeof(ifr.ifr_name));
	memset(&mii, 0, sizeof(mii));
	memcpy(&mii, &ifr.ifr_data, sizeof(mii));
	mii.phy_id = MII_ADDR_C45 | PHY_ID << 5 | MMD;
	mii.reg_num = address;
	mii.val_in = data;
	memcpy(&ifr.ifr_data, &mii, sizeof(mii));

	if (ioctl(sock, SIOCSMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name,
			strerror(errno));
	}

	return;
}

/*! Provides generic synchronous PHY register read functionality. It is the
 * responsibility of the system designer to provide the specific MDIO address
 * pointer updates, etc. in order to accomplish this read operation.*/
unsigned int AQ_API_MDIO_Read
(
	/*! Uniquely identifies the port within the system. AQ_Port must be
	 * defined to a whatever data type is suitable for the platform.*/
	AQ_Port PHY_ID,
	/*! The address of the MMD within the target PHY. */
	unsigned int MMD,
	/*! The 16-bit address of the PHY register being read. */
	unsigned int address)
{
	struct mii_ioctl_data mii;

	/*
	 * Frame the control structures
	 * and send the ioctl to kernel.
	 */
	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, devname, sizeof(ifr.ifr_name));
	memset(&mii, 0, sizeof(mii));
	memcpy(&mii, &ifr.ifr_data, sizeof(mii));
	mii.phy_id = MII_ADDR_C45 | PHY_ID << 5 | MMD;
	mii.reg_num = address;
	memcpy(&ifr.ifr_data, &mii, sizeof(mii));

	if (ioctl(sock, SIOCGMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name,
			strerror(errno));
		return -1;
	} else {
		memcpy(&mii, &ifr.ifr_data, sizeof(mii));
	}


	return mii.val_out;
}

/*! Returns after at least milliseconds have elapsed.  This must be implemented
 *  * in a platform-approriate way. AQ_API functions will call this function to
 *   * block for the specified period of time. If necessary, PHY register reads
 *    * may be performed on port to busy-wait. */
void AQ_API_Wait(
	uint32_t milliseconds,     /*!< The delay in milliseconds */
	AQ_API_Port* port          /*!< The PHY to use if delay reads are necessary*/ )
{
	unsigned long long  mirco = milliseconds *1000;
	usleep(mirco);
}
