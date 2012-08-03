/******************************************************************************

                               Copyright (c) 2007
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL
  PROPERTY INFRINGEMENT.

  EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
 *******************************************************************************/
#ifndef _SVIP_NAT_IO_H_
#define _SVIP_NAT_IO_H_

#include <asm/ioctl.h>

#define SVIP_NAT_DEVICE_NAME		"svip_nat"
#define PATH_SVIP_NAT_DEVICE_NAME	"/dev/"SVIP_NAT_DEVICE_NAME

#define MAJOR_NUM_SVIP_NAT		10
#define MINOR_NUM_SVIP_NAT		120

/** maximum SVIP devices supported on a Line card system */
#define SVIP_SYS_NUM			12

/** maximum voice packet channels possible per SVIP device */
#define SVIP_CODEC_NUM			16

/** start UDP port number of the SVIP Linecard System */
#define SVIP_UDP_FROM			50000

/** @defgroup SVIP_NATAPI  SVIP Custom NAT ioctl interface.
  An ioctl interface is provided to add a rule into the SVIP NAT table and
  to respectively remove the rule form it. The ioctl interface is accessible
  using the fd issued upon opening the special device node /dev/svip_nat.
  @{  */

/** Used to add a new rule to the SVIP Custom NAT table. If a rule already
  exists for the target UDP port, that rule shall be overwritten.

  \param SVIP_NAT_IO_Rule_t* The parameter points to a
  \ref SVIP_NAT_IO_Rule_t structure.
  */
#define FIO_SVIP_NAT_RULE_ADD \
	_IOW(MAJOR_NUM_SVIP_NAT, 1, SVIP_NAT_IO_Rule_t)

/** Used to remove a rule from the SVIP Custom NAT table. No check is
  performed whether the rule already exists or not. The remove operation is
  performed as long as the target UDP port is within the defined port range.

  \param SVIP_NAT_IO_Rule_t* The parameter points to a
  \ref SVIP_NAT_IO_Rule_t structure.
  */
#define FIO_SVIP_NAT_RULE_REMOVE \
	_IOW(MAJOR_NUM_SVIP_NAT, 2, SVIP_NAT_IO_Rule_t)

/** Used to list all rules in the SVIP Custom NAT table.

  \param <none>
  */
#define FIO_SVIP_NAT_RULE_LIST \
	_IO(MAJOR_NUM_SVIP_NAT, 3)

/** IP address in network-byte order */
typedef u32 SVIP_IP_ADDR_t;
/** UDP port in network-byte order */
typedef u16 SVIP_UDP_PORT_t;

#ifndef ETH_ALEN
#define ETH_ALEN			6 /* Octets in one ethernet address */
#endif

/** NAT parameters part of the NAT table.
  These paramters are configurable through the NAT API. */
typedef struct SVIP_NAT_IO_Rule
{
	/** Remote peer, IP address */
	SVIP_IP_ADDR_t remIP;
	/** Remote peer, MAC address */
	u8 remMAC[ETH_ALEN];
	/** Target SVIP, IP address (local peer) */
	SVIP_IP_ADDR_t locIP;
	/** Target SVIP, MAC address */
	u8 locMAC[ETH_ALEN];
	/** Target SVIP, UDP port number */
	SVIP_UDP_PORT_t locUDP;
} SVIP_NAT_IO_Rule_t;

/** @} */
#endif
