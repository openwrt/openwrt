/*
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * danube_cgu.c
 *
 *  Description:
 *    device driver of clock generation unit of Danube chip
 *  Author:
 *    Samuels Xu Liang
 *  Created:
 *    19 Jul 2005
 *  History & Modification Tag:
 *  ___________________________________________________________________________
 *  |  Tag   |                  Comments                   | Modifier & Time  |
 *  |--------+---------------------------------------------+------------------|
 *  |  S0.0  | First version of this driver and the tag is | Samuels Xu Liang |
 *  |        | implied.                                    |   19 Jul 2005    |
 *  ---------------------------------------------------------------------------
 *
 */


/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/irq.h>
#include <linux/errno.h>

/*
 *  Chip Specific Head File
 */
#include "ifx_cgu.h"


/*
 * ####################################
 *              Definition
 * ####################################
 */

#define DEBUG_ON_AMAZON                 1
#define DEBUG_PRINT_INFO                1

/*
 *  Frequency of Clock Direct Feed from The Analog Line Driver Chip
 */
#define BASIC_INPUT_CLOCK_FREQUENCY     35328000

/*
 *  Bits Operation
 */
#define GET_BITS(x, msb, lsb)           (((x) & ((1 << ((msb) + 1)) - 1)) >> (lsb))
#define SET_BITS(x, msb, lsb, value)    (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

/*
 *  CGU Register Mapping
 */
#define DANUBE_CGU                      (KSEG1 + 0x1F103000)
#define DANUBE_CGU_DIV                  ((volatile u32*)(DANUBE_CGU + 0x0000))
#define DANUBE_CGU_PLL_NMK0             ((volatile u32*)(DANUBE_CGU + 0x0004))
#define DANUBE_CGU_PLL_SR0              ((volatile u32*)(DANUBE_CGU + 0x0008))
#define DANUBE_CGU_PLL_NMK1             ((volatile u32*)(DANUBE_CGU + 0x000C))
#define DANUBE_CGU_PLL_SR1              ((volatile u32*)(DANUBE_CGU + 0x0010))
#define DANUBE_CGU_PLL_SR2              ((volatile u32*)(DANUBE_CGU + 0x0014))
#define DANUBE_CGU_IF_CLK               ((volatile u32*)(DANUBE_CGU + 0x0018))
#define DANUBE_CGU_OSC_CTRL             ((volatile u32*)(DANUBE_CGU + 0x001C))
#define DANUBE_CGU_SMD                  ((volatile u32*)(DANUBE_CGU + 0x0020))
#define DANUBE_CGU_CRD                  ((volatile u32*)(DANUBE_CGU + 0x0024))
#define DANUBE_CGU_CT1SR                ((volatile u32*)(DANUBE_CGU + 0x0028))
#define DANUBE_CGU_CT2SR                ((volatile u32*)(DANUBE_CGU + 0x002C))
#define DANUBE_CGU_PCMCR                ((volatile u32*)(DANUBE_CGU + 0x0030))
#define DANUBE_CGU_MUX                  ((volatile u32*)(DANUBE_CGU + 0x0034))

/*
 *  CGU Divider Register
 */
#define CGU_DIV_SFTR                    (*DANUBE_CGU_DIV & (1 << 31))
#define CGU_DIV_DIVE                    (*DANUBE_CGU_DIV & (1 << 16))
#define CGU_DIV_IOR                     GET_BITS(*DANUBE_CGU_DIV, 5, 4)
#define CGU_DIV_FKS                     GET_BITS(*DANUBE_CGU_DIV, 3, 2)
#define CGU_DIV_FBS                     GET_BITS(*DANUBE_CGU_DIV, 1, 0)

/*
 *  CGU PLL0 NMK Register
 */
#define CGU_PLL_NMK0_PLLN               ((*DANUBE_CGU_PLL_NMK0 & (0xFFFFFFFF ^ ((1 << 24) - 1))) >> 24)
#define CGU_PLL_NMK0_PLLM               GET_BITS(*DANUBE_CGU_PLL_NMK0, 23, 20)
#define CGU_PLL_NMK0_PLLK               GET_BITS(*DANUBE_CGU_PLL_NMK0, 19, 0)

/*
 *  CGU PLL0 Status Register
 */
#define CGU_PLL_SR0_PLLDIV              ((*DANUBE_CGU_PLL_SR0 & (0xFFFFFFFF ^ ((1 << 28) - 1))) >> 28)
#define CGU_PLL_SR0_PLLDEN              (*DANUBE_CGU_PLL_SR0 & (1 << 26))
#define CGU_PLL_SR0_PLLPSE              GET_BITS(*DANUBE_CGU_PLL_SR0, 5, 4)
#define CGU_PLL_SR0_PLLB                (*DANUBE_CGU_PLL_SR0 & (1 << 2))
#define CGU_PLL_SR0_PLLL                (*DANUBE_CGU_PLL_SR0 & (1 << 1))
#define CGU_PLL_SR0_PLLEN               (*DANUBE_CGU_PLL_SR0 & (1 << 0))

#define CGU_PLL_SR0_DSMSEL              1
#define CGU_PLL_SR0_PHASE_DIV_EN        1

/*
 *  CGU PLL1 NMK Register
 */
#define CGU_PLL_NMK1_PLLN               ((*DANUBE_CGU_PLL_NMK1 & (0xFFFFFFFF ^ ((1 << 24) - 1))) >> 24)
#define CGU_PLL_NMK1_PLLM               GET_BITS(*DANUBE_CGU_PLL_NMK1, 23, 20)
#define CGU_PLL_NMK1_PLLK               GET_BITS(*DANUBE_CGU_PLL_NMK1, 19, 0)

/*
 *  CGU PLL1 Status Register
 */
#define CGU_PLL_SR1_PLLDIV              ((*DANUBE_CGU_PLL_SR1 & (0xFFFFFFFF ^ ((1 << 28) - 1))) >> 28)
#define CGU_PLL_SR1_PLLDEN              (*DANUBE_CGU_PLL_SR1 & (1 << 26))
#define CGU_PLL_SR1_PLLPSE              GET_BITS(*DANUBE_CGU_PLL_SR1, 5, 4)
#define CGU_PLL_SR1_PLLB                (*DANUBE_CGU_PLL_SR1 & (1 << 2))
#define CGU_PLL_SR1_PLLL                (*DANUBE_CGU_PLL_SR1 & (1 << 1))
#define CGU_PLL_SR1_PLLEN               (*DANUBE_CGU_PLL_SR1 & (1 << 0))

#define CGU_PLL_SR1_DSMSEL              1
#define CGU_PLL_SR1_PHASE_DIV_EN        1

/*
 *  CGU PLL2 Status Register
 */
#define CGU_PLL_SR2_PLLDIV              ((*DANUBE_CGU_PLL_SR2 & (0xFFFFFFFF ^ ((1 << 28) - 1))) >> 28)
#define CGU_PLL_SR2_PLLDEN              (*DANUBE_CGU_PLL_SR2 & (1 << 27))
#define CGU_PLL_SR2_PLLN                GET_BITS(*DANUBE_CGU_PLL_SR2, 25, 20)
#define CGU_PLL_SR2_PLLM                GET_BITS(*DANUBE_CGU_PLL_SR2, 19, 16)
#define CGU_PLL_SR2_PLLPS               (*DANUBE_CGU_PLL_SR2 & (1 << 5))
#define CGU_PLL_SR2_PLLPE               (*DANUBE_CGU_PLL_SR2 & (1 << 4))
#define CGU_PLL_SR2_PLLB                (*DANUBE_CGU_PLL_SR2 & (1 << 2))
#define CGU_PLL_SR2_PLLL                (*DANUBE_CGU_PLL_SR2 & (1 << 1))
#define CGU_PLL_SR2_PLLEN               (*DANUBE_CGU_PLL_SR2 & (1 << 0))

/*
 *  CGU Interface Clock Register
 */
#define CGU_IF_CLK_CLKOD0               GET_BITS(*DANUBE_CGU_IF_CLK, 27, 26)
#define CGU_IF_CLK_CLKOD1               GET_BITS(*DANUBE_CGU_IF_CLK, 25, 24)
#define CGU_IF_CLK_CLKOD2               GET_BITS(*DANUBE_CGU_IF_CLK, 23, 22)
#define CGU_IF_CLK_CLKOD3               GET_BITS(*DANUBE_CGU_IF_CLK, 21, 20)
#define CGU_IF_CLK_PDA                  (*DANUBE_CGU_IF_CLK & (1 << 18))
#define CGU_IF_CLK_PCI_B                (*DANUBE_CGU_IF_CLK & (1 << 17))
#define CGU_IF_CLK_PCIBM                (*DANUBE_CGU_IF_CLK & (1 << 16))
#define CGU_IF_CLK_MIICS                (*DANUBE_CGU_IF_CLK & (1 << 3))
#define CGU_IF_CLK_USBCS                (*DANUBE_CGU_IF_CLK & (1 << 2))
#define CGU_IF_CLK_PCIF                 (*DANUBE_CGU_IF_CLK & (1 << 1))
#define CGU_IF_CLK_PCIS                 (*DANUBE_CGU_IF_CLK & (1 << 0))

/*
 *  CGU Oscillator Control Register
 */
#define CGU_OSC_CTRL                    GET_BITS(*DANUBE_CGU_OSC_CTRL, 1, 0)

/*
 *  CGU SDRAM Memory Delay Register
 */
#define CGU_SMD_CLKI                    (*DANUBE_CGU_SMD & (1 << 31))
#define CGU_SMD_MIDS                    GET_BITS(*DANUBE_CGU_SMD, 17, 12)
#define CGU_SMD_MODS                    GET_BITS(*DANUBE_CGU_SMD, 11, 6)
#define CGU_SMD_MDSEL                   GET_BITS(*DANUBE_CGU_SMD, 5, 0)

/*
 *  CGU CPU Clock Reduction Register
 */
#define CGU_CRD_SFTR                    (*DANUBE_CGU_CRD & (1 << 31))
#define CGU_CRD_DIVE                    (*DANUBE_CGU_CRD & (1 << 16))
#define CGU_CRD_CRD1                    GET_BITS(*DANUBE_CGU_CRD, 3, 2)
#define CGU_CRD_CRD                     GET_BITS(*DANUBE_CGU_CRD, 1, 0)

/*
 *  CGU CT Status Register 1
 */
#define CGU_CT1SR_PDOUT                 GET_BITS(*DANUBE_CGU_CT1SR, 13, 0)

/*
 *  CGU CT Status Register 2
 */
#define CGU_CT2SR_PLL1K                 GET_BITS(*DANUBE_CGU_CT2SR, 9, 0)

/*
 *  CGU PCM Control Register
 */
#define CGU_PCMCR_DCL1                  GET_BITS(*DANUBE_CGU_PCMCR, 27, 25)
#define CGU_PCMCR_MUXDCL                (*DANUBE_CGU_PCMCR & (1 << 22))
#define CGU_PCMCR_MUXFSC                (*DANUBE_CGU_PCMCR & (1 << 18))
#define CGU_PCMCR_PCM_SL                (*DANUBE_CGU_PCMCR & (1 << 13))
#define CGU_PCMCR_DNTR                  (*DANUBE_CGU_PCMCR & (1 << 12))

/*
 *  CGU Clock Mux Register
 */
#define CGU_MUX_MII_CLK                 (*DANUBE_CGU_MUX & (1 << 6))
#define CGU_MUX_SUB_SYS                 GET_BITS(*DANUBE_CGU_MUX, 5, 3)
#define CGU_MUX_PP32                    GET_BITS(*DANUBE_CGU_MUX, 1, 0)


/*
 * ####################################
 * Preparation of Debug on Amazon Chip
 * ####################################
 */

/*
 *  If try module on Amazon chip, prepare some tricks to prevent invalid memory write.
 */
#if defined(DEBUG_ON_AMAZON) && DEBUG_ON_AMAZON
    u32 g_pFakeRegisters[0x0100];

    #undef  DANUBE_CGU
    #define DANUBE_CGU                  ((u32)g_pFakeRegisters)
#endif  //  defined(DEBUG_ON_AMAZON) && DEBUG_ON_AMAZON


/*
 * ####################################
 *              Data Type
 * ####################################
 */


/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  Pre-declaration of File Operations
 */
static ssize_t cgu_read(struct file *, char *, size_t, loff_t *);
static ssize_t cgu_write(struct file *, const char *, size_t, loff_t *);
static int cgu_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static int cgu_open(struct inode *, struct file *);
static int cgu_release(struct inode *, struct file *);

/*
 *  Pre-declaration of 64-bit Unsigned Integer Operation
 */
static inline void uint64_multiply(unsigned int, unsigned int, unsigned int *);
static inline void uint64_divide(unsigned int *, unsigned int, unsigned int *, unsigned int *);

/*
 *  Calculate PLL Frequency
 */
static inline u32 cal_dsm(u32, u32);
static inline u32 mash_dsm(u32, u32, u32);
static inline u32 ssff_dsm_1(u32, u32, u32);
static inline u32 ssff_dsm_2(u32, u32, u32);
static inline u32 dsm(u32 M, u32, u32, int, int);
static inline u32 cgu_get_pll0_fosc(void);
static inline u32 cgu_get_pll0_fps(void);
static inline u32 cgu_get_pll0_fdiv(void);
static inline u32 cgu_get_pll1_fosc(void);
static inline u32 cgu_get_pll1_fps(void);
static inline u32 cgu_get_pll1_fdiv(void);
static inline u32 cgu_get_pll2_fosc(void);
static inline u32 cgu_get_pll2_fps(void);

/*
 *  Export Functions
 */
u32 cgu_get_mips_clock(int);
u32 cgu_get_cpu_clock(void);
u32 cgu_get_io_region_clock(void);
u32 cgu_get_fpi_bus_clock(int);
u32 cgu_get_pp32_clock(void);
u32 cgu_get_pci_clock(void);
u32 cgu_get_ethernet_clock(void);
u32 cgu_get_usb_clock(void);
u32 cgu_get_clockout(int);


/*
 * ####################################
 *            Local Variable
 * ####################################
 */

static struct file_operations cgu_fops = {
    owner:      THIS_MODULE,
    llseek:     no_llseek,
    read:       cgu_read,
    write:      cgu_write,
    ioctl:      cgu_ioctl,
    open:       cgu_open,
    release:    cgu_release
};

static struct miscdevice cgu_miscdev = {
    MISC_DYNAMIC_MINOR,
    "danube_cgu_dev",
    &cgu_fops
};


/*
 * ####################################
 *           Global Variable
 * ####################################
 */


/*
 * ####################################
 *            Local Function
 * ####################################
 */

static ssize_t cgu_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    return -EPERM;
}

static ssize_t cgu_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
    return -EPERM;
}

static int cgu_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    struct cgu_clock_rates rates;

    if ( _IOC_TYPE(cmd) != CGU_IOC_MAGIC
        || _IOC_NR(cmd) >= CGU_IOC_MAXNR )
        return -ENOTTY;

    if ( _IOC_DIR(cmd) & _IOC_READ )
        ret = !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd));
    else if ( _IOC_DIR(cmd) & _IOC_WRITE )
        ret = !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd));
    if ( ret )
        return -EFAULT;

    switch ( cmd )
    {
    case CGU_GET_CLOCK_RATES:
        /*  Calculate Clock Rates   */
        rates.mips0     = cgu_get_mips_clock(0);
        rates.mips1     = cgu_get_mips_clock(1);
        rates.cpu       = cgu_get_cpu_clock();
        rates.io_region = cgu_get_io_region_clock();
        rates.fpi_bus1  = cgu_get_fpi_bus_clock(1);
        rates.fpi_bus2  = cgu_get_fpi_bus_clock(2);
        rates.pp32      = cgu_get_pp32_clock();
        rates.pci       = cgu_get_pci_clock();
        rates.ethernet  = cgu_get_ethernet_clock();
        rates.usb       = cgu_get_usb_clock();
        rates.clockout0 = cgu_get_clockout(0);
        rates.clockout1 = cgu_get_clockout(1);
        rates.clockout2 = cgu_get_clockout(2);
        rates.clockout3 = cgu_get_clockout(3);
        /*  Copy to User Space      */
        copy_to_user((char*)arg, (char*)&rates, sizeof(rates));

        ret = 0;
        break;
    default:
        ret = -ENOTTY;
    }

    return ret;
}

static int cgu_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int cgu_release(struct inode *inode, struct file *file)
{
    return 0;
}

/*
 *  Description:
 *    calculate 64-bit multiplication result of two 32-bit unsigned integer
 *  Input:
 *    u32Multiplier1 --- u32 (32-bit), one of the multipliers
 *    u32Multiplier2 --- u32 (32-bit), the other multiplier
 *    u32Result      --- u32[2], array to retrieve the multiplication result,
 *                       index 0 is high word, index 1 is low word
 *  Output:
*    none
 */
static inline void uint64_multiply(u32 u32Multiplier1, u32 u32Multiplier2, u32 u32Result[2])
{
	u32 u32Multiplier1LowWord = u32Multiplier1 & 0xFFFF;
	u32 u32Multiplier1HighWord = u32Multiplier1 >> 16;
	u32 u32Multiplier2LowWord = u32Multiplier2 & 0xFFFF;
	u32 u32Multiplier2HighWord = u32Multiplier2 >> 16;
	u32 u32Combo1, u32Combo2, u32Combo3, u32Combo4;
	u32 u32Word1, u32Word2, u32Word3, u32Word4;

	u32Combo1 = u32Multiplier1LowWord * u32Multiplier2LowWord;
	u32Combo2 = u32Multiplier1HighWord * u32Multiplier2LowWord;
	u32Combo3 = u32Multiplier1LowWord * u32Multiplier2HighWord;
	u32Combo4 = u32Multiplier1HighWord * u32Multiplier2HighWord;

	u32Word1 = u32Combo1 & 0xFFFF;
	u32Word2 = (u32Combo1 >> 16) + (u32Combo2 & 0xFFFF) + (u32Combo3 & 0xFFFF);
	u32Word3 = (u32Combo2 >> 16) + (u32Combo3 >> 16) + (u32Combo4 & 0xFFFF) + (u32Word2 >> 16);
	u32Word4 = (u32Combo4 >> 16) + (u32Word3 >> 16);

	u32Result[0] = (u32Word4 << 16) | u32Word3;
	u32Result[1] = (u32Word2 << 16) | u32Word1;
}

/*
 *  Description:
 *    divide 64-bit unsigned integer with 32-bit unsigned integer
 *  Input:
 *    u32Numerator   --- u32[2], index 0 is high word of numerator, while
 *                       index 1 is low word of numerator
 *    u32Denominator --- u32 (32-bit), the denominator in division, this
 *                       parameter can not be zero, or lead to unpredictable
 *                       result
 *    pu32Quotient   --- u32 *, the pointer to retrieve 32-bit quotient, null
 *                       pointer means ignore quotient
 *    pu32Residue    --- u32 *, the pointer to retrieve 32-bit residue null
 *                       pointer means ignore residue
 *  Output:
 *    none
 */
static inline void uint64_divide(u32 u32Numerator[2], u32 u32Denominator, u32 *pu32Quotient, u32 *pu32Residue)
{
	u32 u32DWord1, u32DWord2, u32DWord3;
	u32 u32Quotient;
	int i;

	u32DWord3 = 0;
	u32DWord2 = u32Numerator[0];
	u32DWord1 = u32Numerator[1];

	u32Quotient = 0;

	for ( i = 0; i < 64; i++ )
	{
		u32DWord3 = (u32DWord3 << 1) | (u32DWord2 >> 31);
		u32DWord2 = (u32DWord2 << 1) | (u32DWord1 >> 31);
		u32DWord1 <<= 1;
		u32Quotient <<= 1;
		if ( u32DWord3 >= u32Denominator )
		{
			u32DWord3 -= u32Denominator;
			u32Quotient |= 1;
		}
	}
	if ( pu32Quotient )
	    *pu32Quotient = u32Quotient;
	if ( pu32Residue )
	    *pu32Residue = u32DWord3;
}

/*
 *  Description:
 *    common routine to calculate PLL frequency
 *  Input:
 *    num --- u32, numerator
 *    den --- u32, denominator
 *  Output:
 *    u32 --- frequency the PLL output
 */
static inline u32 cal_dsm(u32 num, u32 den)
{
    u32 ret;
    u32 temp[2];
    u32 residue;

    uint64_multiply(num, BASIC_INPUT_CLOCK_FREQUENCY, temp);
    uint64_divide(temp, den, &ret, &residue);
    if ( (residue << 1) >= den )
        ret++;

    return ret;
}

/*
 *  Description:
 *    calculate PLL frequency following MASH-DSM
 *  Input:
 *    M   --- u32, denominator coefficient
 *    N   --- u32, numerator integer coefficient
 *    K   --- u32, numerator fraction coefficient
 *  Output:
 *    u32 --- frequency the PLL output
 */
static inline u32 mash_dsm(u32 M, u32 N, u32 K)
{
    u32 num = ((N + 1) << 10) + K;
    u32 den = (M + 1) << 10;

    return cal_dsm(num, den);
}

/*
 *  Description:
 *    calculate PLL frequency following SSFF-DSM (0.25 < fraction < 0.75)
 *  Input:
 *    M   --- u32, denominator coefficient
 *    N   --- u32, numerator integer coefficient
 *    K   --- u32, numerator fraction coefficient
 *  Output:
 *    u32 --- frequency the PLL output
 */
static inline u32 ssff_dsm_1(u32 M, u32 N, u32 K)
{
    u32 num = ((N + 1) << 11) + K + 512;
    u32 den = (M + 1) << 11;

    return cal_dsm(num, den);
}

/*
 *  Description:
 *    calculate PLL frequency following SSFF-DSM
 *    (fraction < 0.125 || fraction > 0.875)
 *  Input:
 *    M   --- u32, denominator coefficient
 *    N   --- u32, numerator integer coefficient
 *    K   --- u32, numerator fraction coefficient
 *  Output:
 *    u32 --- frequency the PLL output
 */
static inline u32 ssff_dsm_2(u32 M, u32 N, u32 K)
{
    u32 num = K >= 512 ? ((N + 1) << 12) + K - 512 : ((N + 1) << 12) + K + 3584;
    u32 den = (M + 1) << 12;

    return cal_dsm(num, den);
}

/*
 *  Description:
 *    calculate PLL frequency
 *  Input:
 *    M            --- u32, denominator coefficient
 *    N            --- u32, numerator integer coefficient
 *    K            --- u32, numerator fraction coefficient
 *    dsmsel       --- int, 0: MASH-DSM, 1: SSFF-DSM
 *    phase_div_en --- int, 0: 0.25 < fraction < 0.75
 *                          1: fraction < 0.125 || fraction > 0.875
 *  Output:
 *    u32          --- frequency the PLL output
 */
static inline u32 dsm(u32 M, u32 N, u32 K, int dsmsel, int phase_div_en)
{
    if ( !dsmsel )
        return mash_dsm(M, N, K);
    else
        if ( !phase_div_en )
            return ssff_dsm_1(M, N, K);
        else
            return ssff_dsm_2(M, N, K);
}

/*
 *  Description:
 *    get oscillate frequency of PLL0
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL0 Fosc
 */
static inline u32 cgu_get_pll0_fosc(void)
{
    return CGU_PLL_SR0_PLLB ? BASIC_INPUT_CLOCK_FREQUENCY : dsm(CGU_PLL_NMK0_PLLM, CGU_PLL_NMK0_PLLN, CGU_PLL_NMK0_PLLK, CGU_PLL_SR0_DSMSEL, CGU_PLL_SR0_PHASE_DIV_EN);
}

/*
 *  Description:
 *    get output frequency of PLL0 phase shifter
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL0 Fps
 */
static inline u32 cgu_get_pll0_fps(void)
{
    register u32 fps = cgu_get_pll0_fosc();

    switch ( CGU_PLL_SR0_PLLPSE )
    {
    case 1:
        /*  1.5     */
        fps = ((fps << 1) + 1) / 3; break;
    case 2:
        /*  1.25    */
        fps = ((fps << 2) + 2) / 5; break;
    case 3:
        /*  3.5     */
        fps = ((fps << 1) + 3) / 7;
    }
    return fps;
}

/*
 *  Description:
 *    get output frequency of PLL0 output divider
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL0 Fdiv
 */
static inline u32 cgu_get_pll0_fdiv(void)
{
    register u32 fdiv = cgu_get_pll0_fosc();

    if ( CGU_PLL_SR0_PLLDEN )
        fdiv = (fdiv + (CGU_PLL_SR0_PLLDIV + 1) / 2) / (CGU_PLL_SR0_PLLDIV + 1);
    return fdiv;
}

/*
 *  Description:
 *    get oscillate frequency of PLL1
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL1 Fosc
 */
static inline u32 cgu_get_pll1_fosc(void)
{
    return CGU_PLL_SR1_PLLB ? BASIC_INPUT_CLOCK_FREQUENCY : dsm(CGU_PLL_NMK1_PLLM, CGU_PLL_NMK1_PLLN, CGU_PLL_NMK1_PLLK, CGU_PLL_SR1_DSMSEL, CGU_PLL_SR1_PHASE_DIV_EN);
}

/*
 *  Description:
 *    get output frequency of PLL1 phase shifter
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL1 Fps
 */
static inline u32 cgu_get_pll1_fps(void)
{
    register u32 fps = cgu_get_pll1_fosc();

    switch ( CGU_PLL_SR1_PLLPSE )
    {
    case 1:
        /*  1.5     */
        fps = ((fps << 1) + 1) / 3; break;
    case 2:
        /*  1.25    */
        fps = ((fps << 2) + 2) / 5; break;
    case 3:
        /*  3.5     */
        fps = ((fps << 1) + 3) / 7;
    }
    return fps;
}

/*
 *  Description:
 *    get output frequency of PLL1 output divider
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL1 Fdiv
 */
static inline u32 cgu_get_pll1_fdiv(void)
{
    register u32 fdiv = cgu_get_pll1_fosc();

    if ( CGU_PLL_SR1_PLLDEN )
        fdiv = (fdiv + (CGU_PLL_SR1_PLLDIV + 1) / 2) / (CGU_PLL_SR1_PLLDIV + 1);
    return fdiv;
}

/*
 *  Description:
 *    get oscillate frequency of PLL2
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL2 Fosc
 */
static inline u32 cgu_get_pll2_fosc(void)
{
    u32 ret;
    u32 temp[2];
    u32 residue;

    uint64_multiply((CGU_PLL_SR2_PLLN + 1) * 8, cgu_get_pll0_fdiv(), temp);
    uint64_divide(temp, CGU_PLL_SR2_PLLM + 1, &ret, &residue);
    if ( (residue << 1) >= CGU_PLL_SR2_PLLM )
        ret++;

    return ret;
}

/*
 *  Description:
 *    get output frequency of PLL2 phase shifter
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL2 Fps
 */
static inline u32 cgu_get_pll2_fps(void)
{
    register u32 fps = cgu_get_pll2_fosc();

    if ( CGU_PLL_SR2_PLLPE )
    {
        if ( CGU_PLL_SR2_PLLPS )
            /*  1.25    */
            fps = ((fps << 3) + 4) / 9;
        else
            /*  1.125   */
            fps = ((fps << 2) + 2) / 5;
    }

    return fps;
}


/*
 * ####################################
 *           Global Function
 * ####################################
 */

/*
 *  Description:
 *    get frequency of MIPS (0: core, 1: DSP)
 *  Input:
 *    cpu --- int, 0: core, 1: DSP
 *  Output:
 *    u32 --- frequency of MIPS coprocessor (0: core, 1: DSP)
 */
u32 cgu_get_mips_clock(int cpu)
{
    register u32 ret = cgu_get_pll0_fosc();

    if ( CGU_CRD_CRD )
        ret = (ret + (CGU_CRD_CRD >> 1)) / (CGU_CRD_CRD + 1);
    if ( cpu == 0 && CGU_CRD_CRD1 )
        ret >>= CGU_CRD_CRD1;
    return ret;
}

/*
 *  Description:
 *    get frequency of MIPS core
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of MIPS core
 */
u32 cgu_get_cpu_clock(void)
{
    return cgu_get_mips_clock(0);
}

/*
 *  Description:
 *    get frequency of sub-system and memory controller
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of sub-system and memory controller
 */
u32 cgu_get_io_region_clock(void)
{
    register u32 ret = (CGU_MUX_SUB_SYS > 4) ? cgu_get_pll0_fosc() : cgu_get_mips_clock(1);

    switch ( CGU_MUX_SUB_SYS )
    {
    case 0:
        break;
    case 1:
    default:
        ret = (ret + 1) >> 1; break;
    case 2:
        ret = (ret + 1) / 3; break;
    case 3:
        ret = (ret + 2) >> 2; break;
    case 5:
        ret = ((ret << 1) + 1) / 3; break;
    case 6:
        ret = ((ret << 1) + 2) / 5;
    }

    return ret;
}

/*
 *  Description:
 *    get frequency of FPI bus
 *  Input:
 *    fpi --- int, 1: FPI bus 1 (FBS1/Fast FPI Bus), 2: FPI bus 2 (FBS2)
 *  Output:
 *    u32 --- frequency of FPI bus
 */
u32 cgu_get_fpi_bus_clock(int fpi)
{
    register u32 ret = cgu_get_io_region_clock();

    if ( fpi == 2 )
        ret >>= 1;
    return ret;
}

/*
 *  Description:
 *    get frequency of PP32 processor
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PP32 processor
 */
u32 cgu_get_pp32_clock(void)
{
    register u32 ret;

    switch ( CGU_MUX_PP32 )
    {
    case 0:
    default:
        ret = ((cgu_get_pll2_fosc() << 2) + 2) / 5; break;
    case 1:
        ret = ((cgu_get_pll2_fosc() << 3) + 4) / 9; break;
    case 2:
        ret = cgu_get_fpi_bus_clock(1); break;
    case 3:
        ret = cgu_get_mips_clock(1);
    }

    return ret;
}

/*
 *  Description:
 *    get frequency of PCI bus
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PCI bus
 */
u32 cgu_get_pci_clock(void)
{
    register u32 ret = 0;

    if ( !CGU_IF_CLK_PCIS )
    {
        ret = cgu_get_pll2_fosc();
        if ( CGU_IF_CLK_PCIF )
            ret = (ret + 2) / 5;
        else
            ret = (ret + 4) / 9;
    }

    return ret;
}

/*
 *  Description:
 *    get frequency of ethernet module (MII)
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of ethernet module
 */
u32 cgu_get_ethernet_clock(void)
{
    register u32 ret = 0;

    if ( !CGU_IF_CLK_MIICS )
    {
        ret = cgu_get_pll2_fosc();
        if ( CGU_MUX_MII_CLK )
            ret = (ret + 3) / 6;
        else
            ret = (ret + 6) / 12;
    }

    return ret;
}

/*
 *  Description:
 *    get frequency of USB
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of USB
 */
u32 cgu_get_usb_clock(void)
{
    return CGU_IF_CLK_USBCS ? 12000000 : (cgu_get_pll2_fosc() + 12) / 25;
}

/*
 *  Description:
 *    get frequency of CLK_OUT pin
 *  Input:
 *    clkout --- int, clock out pin number
 *  Output:
 *    u32    --- frequency of CLK_OUT pin
 */
u32 cgu_get_clockout(int clkout)
{
    u32 fosc1 = cgu_get_pll1_fosc();
    u32 fosc2 = cgu_get_pll2_fosc();

    if ( clkout > 3 || clkout < 0 )
        return 0;

    switch ( ((u32)clkout << 2) | GET_BITS(*DANUBE_CGU_IF_CLK, 21 + clkout * 2, 20 + clkout * 2) )
    {
    case 0: /*  32.768KHz   */
    case 14:
        return (fosc1 + 6000) / 12000;
    case 1: /*  1.536MHz    */
        return (fosc1 + 128) / 256;
    case 2: /*  2.5MHz      */
        return (fosc2 + 60) / 120;
    case 3: /*  12MHz       */
    case 5:
    case 12:
        return (fosc2 + 12) / 25;
    case 4: /*  40MHz       */
        return (fosc2 * 2 + 7) / 15;
    case 6: /*  24MHz       */
        return (fosc2 * 2 + 12) / 25;
    case 7: /*  48MHz       */
        return (fosc2 * 4 + 12) / 25;
    case 8: /*  25MHz       */
    case 15:
        return (fosc2 + 6) / 12;
    case 9: /*  50MHz       */
    case 13:
        return (fosc2 + 3) / 6;
    case 10:/*  30MHz       */
        return (fosc2 + 5) / 10;
    case 11:/*  60MHz       */
        return (fosc2 + 2) / 5;
    }

    return 0;
}


/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

/*
 *  Description:
 *    register device
 *  Input:
 *    none
 *  Output:
 *    0    --- successful
 *    else --- failure, usually it is negative value of error code
 */
int __init danube_cgu_init(void)
{
    int ret;

    ret = misc_register(&cgu_miscdev);
    if ( ret )
    {
        printk(KERN_ERR "cgu: can't misc_register\n");
        return ret;
    }
    else
        printk(KERN_INFO "cgu: misc_register on minor = %d\n", cgu_miscdev.minor);

    /*
     *  initialize fake registers to do testing on Amazon
     */
#if defined(DEBUG_ON_AMAZON) && DEBUG_ON_AMAZON
    #ifdef  DEBUG_PRINT_INFO
    #undef  DEBUG_PRINT_INFO
    #endif
    #define DEBUG_PRINT_INFO    1

    *DANUBE_CGU_DIV         = 0x00010019;
    *DANUBE_CGU_PLL_NMK0    = 0x416002C3;
    *DANUBE_CGU_PLL_SR0     = 0x74000013;
    *DANUBE_CGU_PLL_NMK1    = 0x4C60009C;
    *DANUBE_CGU_PLL_SR1     = 0x54000013;
    *DANUBE_CGU_PLL_SR2     = 0x58890013;
    *DANUBE_CGU_IF_CLK      = 0x00000000;
    *DANUBE_CGU_OSC_CTRL    = 0x00000000;
    *DANUBE_CGU_SMD         = 0x00000000;
    *DANUBE_CGU_CRD         = 0x00010000;
    *DANUBE_CGU_CT1SR       = 0x00000000;
    *DANUBE_CGU_CT2SR       = CGU_PLL_NMK1_PLLK;
    *DANUBE_CGU_PCMCR       = 0x00000000;
    *DANUBE_CGU_MUX         = 0x00000008;
#endif  //  defined(DEBUG_ON_AMAZON) && DEBUG_ON_AMAZON

    /*
     *  for testing only
     */
#if defined(DEBUG_PRINT_INFO) && DEBUG_PRINT_INFO
    printk("pll0 N = %d, M = %d, K = %d, DIV = %d\n", CGU_PLL_NMK0_PLLN, CGU_PLL_NMK0_PLLM, CGU_PLL_NMK0_PLLK, CGU_PLL_SR0_PLLDIV);
    printk("pll1 N = %d, M = %d, K = %d, DIV = %d\n", CGU_PLL_NMK1_PLLN, CGU_PLL_NMK1_PLLM, CGU_PLL_NMK1_PLLK, CGU_PLL_SR1_PLLDIV);
    printk("pll2 N = %d, M = %d, DIV = %d\n", CGU_PLL_SR2_PLLN, CGU_PLL_SR2_PLLM, CGU_PLL_SR2_PLLDIV);
    printk("pll0_fosc    = %d\n", cgu_get_pll0_fosc());
    printk("pll0_fps     = %d\n", cgu_get_pll0_fps());
    printk("pll0_fdiv    = %d\n", cgu_get_pll0_fdiv());
    printk("pll1_fosc    = %d\n", cgu_get_pll1_fosc());
    printk("pll1_fps     = %d\n", cgu_get_pll1_fps());
    printk("pll1_fdiv    = %d\n", cgu_get_pll1_fdiv());
    printk("pll2_fosc    = %d\n", cgu_get_pll2_fosc());
    printk("pll2_fps     = %d\n", cgu_get_pll2_fps());
    printk("mips0 clock  = %d\n", cgu_get_mips_clock(0));
    printk("mips1 clock  = %d\n", cgu_get_mips_clock(1));
    printk("cpu clock    = %d\n", cgu_get_cpu_clock());
    printk("IO region    = %d\n", cgu_get_io_region_clock());
    printk("FPI bus 1    = %d\n", cgu_get_fpi_bus_clock(1));
    printk("FPI bus 2    = %d\n", cgu_get_fpi_bus_clock(2));
    printk("PP32 clock   = %d\n", cgu_get_pp32_clock());
    printk("PCI clock    = %d\n", cgu_get_pci_clock());
    printk("Ethernet     = %d\n", cgu_get_ethernet_clock());
    printk("USB clock    = %d\n", cgu_get_usb_clock());
    printk("Clockout0    = %d\n", cgu_get_clockout(0));
    printk("Clockout1    = %d\n", cgu_get_clockout(1));
    printk("Clockout2    = %d\n", cgu_get_clockout(2));
    printk("Clockout3    = %d\n", cgu_get_clockout(3));
#endif  //  defined(DEBUG_PRINT_INFO) && DEBUG_PRINT_INFO

    return 0;
}

/*
 *  Description:
 *    deregister device
 *  Input:
 *    none
 *  Output:
 *    none
 */
void __exit danube_cgu_exit(void)
{
    int ret;

    ret = misc_deregister(&cgu_miscdev);
    if ( ret )
        printk(KERN_ERR "cgu: can't misc_deregister, get error number %d\n", -ret);
    else
        printk(KERN_INFO "cgu: misc_deregister successfully\n");
}

module_init(danube_cgu_init);
module_exit(danube_cgu_exit);
