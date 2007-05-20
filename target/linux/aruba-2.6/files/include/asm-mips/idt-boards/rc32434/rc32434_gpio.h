/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   GPIO register definition
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
 *         
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2004 rkt, neb.
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#ifndef __IDT_GPIO_H__
#define __IDT_GPIO_H__

enum
{
	GPIO0_PhysicalAddress	= 0x18050000,
	GPIO_PhysicalAddress	= GPIO0_PhysicalAddress,	// Default

	GPIO0_VirtualAddress	= 0xb8050000,
	GPIO_VirtualAddress	= GPIO0_VirtualAddress,		// Default
} ;

typedef struct
{
	u32   gpiofunc;   /* GPIO Function Register
			   * gpiofunc[x]==0 bit = gpio
			   * func[x]==1  bit = altfunc
			   */
	u32   gpiocfg;	  /* GPIO Configuration Register
			   * gpiocfg[x]==0 bit = input
			   * gpiocfg[x]==1 bit = output
			   */
	u32   gpiod;	  /* GPIO Data Register
			   * gpiod[x] read/write gpio pinX status
			   */
	u32   gpioilevel; /* GPIO Interrupt Status Register
			   * interrupt level (see gpioistat)
			   */
	u32   gpioistat;  /* Gpio Interrupt Status Register
			   * istat[x] = (gpiod[x] == level[x])
			   * cleared in ISR (STICKY bits)
			   */
	u32   gpionmien;  /* GPIO Non-maskable Interrupt Enable Register */
} volatile * GPIO_t ;

typedef enum
{
	GPIO_gpio_v		= 0,		// gpiofunc use pin as GPIO.
	GPIO_alt_v		= 1,		// gpiofunc use pin as alt.
	GPIO_input_v		= 0,		// gpiocfg use pin as input.
	GPIO_output_v		= 1,		// gpiocfg use pin as output.
	GPIO_pin0_b		= 0,
	GPIO_pin0_m		= 0x00000001,
	GPIO_pin1_b		= 1,
	GPIO_pin1_m		= 0x00000002,
	GPIO_pin2_b		= 2,
	GPIO_pin2_m		= 0x00000004,
	GPIO_pin3_b		= 3,
	GPIO_pin3_m		= 0x00000008,
	GPIO_pin4_b		= 4,
	GPIO_pin4_m		= 0x00000010,
	GPIO_pin5_b		= 5,
	GPIO_pin5_m		= 0x00000020,
	GPIO_pin6_b		= 6,
	GPIO_pin6_m		= 0x00000040,
	GPIO_pin7_b		= 7,
	GPIO_pin7_m		= 0x00000080,
	GPIO_pin8_b		= 8,
	GPIO_pin8_m		= 0x00000100,
	GPIO_pin9_b		= 9,
	GPIO_pin9_m		= 0x00000200,
	GPIO_pin10_b		= 10,
	GPIO_pin10_m		= 0x00000400,
	GPIO_pin11_b		= 11,
	GPIO_pin11_m		= 0x00000800,
	GPIO_pin12_b		= 12,
	GPIO_pin12_m		= 0x00001000,
	GPIO_pin13_b		= 13,
	GPIO_pin13_m		= 0x00002000,

// Alternate function pins.  Corrsponding gpiofunc bit set to GPIO_alt_v.

	GPIO_u0sout_b		= GPIO_pin0_b,		// UART 0 serial out.
	GPIO_u0sout_m		= GPIO_pin0_m,
		GPIO_u0sout_cfg_v	= GPIO_output_v,
	GPIO_u0sinp_b	= GPIO_pin1_b,			// UART 0 serial in.
	GPIO_u0sinp_m	= GPIO_pin1_m,
		GPIO_u0sinp_cfg_v	= GPIO_input_v,
	GPIO_u0rtsn_b	= GPIO_pin2_b,			// UART 0 req. to send.
	GPIO_u0rtsn_m	= GPIO_pin2_m,
		GPIO_u0rtsn_cfg_v	= GPIO_output_v,
	GPIO_u0ctsn_b	= GPIO_pin3_b,			// UART 0 clear to send.
	GPIO_u0ctsn_m	= GPIO_pin3_m,
		GPIO_u0ctsn_cfg_v	= GPIO_input_v,

	GPIO_maddr22_b		= GPIO_pin4_b, 	// M&P bus bit 22.
	GPIO_maddr22_m		= GPIO_pin4_m,
		GPIO_maddr22_cfg_v	= GPIO_output_v,

	GPIO_maddr23_b		= GPIO_pin5_b, 	// M&P bus bit 23.
	GPIO_maddr23_m		= GPIO_pin5_m,
		GPIO_maddr23_cfg_v	= GPIO_output_v,

	GPIO_maddr24_b		= GPIO_pin6_b, 	// M&P bus bit 24.
	GPIO_maddr24_m		= GPIO_pin6_m,
		GPIO_maddr24_cfg_v	= GPIO_output_v,

	GPIO_maddr25_b		= GPIO_pin7_b, 	// M&P bus bit 25.
	GPIO_maddr25_m		= GPIO_pin7_m,
		GPIO_maddr25_cfg_v	= GPIO_output_v,

	GPIO_cpudmadebug_b 	= GPIO_pin8_b, 	// CPU or DMA debug pin
	GPIO_cpudmadebug_m 	= GPIO_pin8_m,
		GPIO_cpudmadebug_cfg_v	= GPIO_output_v,

	GPIO_pcireq4_b 	= GPIO_pin9_b, 	// PCI Request 4
	GPIO_pcireq4_m 	= GPIO_pin9_m,
		GPIO_pcireq4_cfg_v	= GPIO_input_v,

	GPIO_pcigrant4_b 	= GPIO_pin10_b, 	// PCI Grant 4
	GPIO_pcigrant4_m 	= GPIO_pin10_m,
		GPIO_pcigrant4_cfg_v	= GPIO_output_v,

	GPIO_pcireq5_b 	= GPIO_pin11_b, 	// PCI Request 5
	GPIO_pcireq5_m 	= GPIO_pin11_m,
		GPIO_pcireq5_cfg_v	= GPIO_input_v,

	GPIO_pcigrant5_b 	= GPIO_pin12_b, 	// PCI Grant 5
	GPIO_pcigrant5_m 	= GPIO_pin12_m,
		GPIO_pcigrant5_cfg_v	= GPIO_output_v,

	GPIO_pcimuintn_b	= GPIO_pin13_b, 	// PCI messaging int.
	GPIO_pcimuintn_m	= GPIO_pin13_m,
		GPIO_pcimuintn_cfg_v	= GPIO_output_v,

} GPIO_DEFS_t;

#endif	// __IDT_GPIO_H__

