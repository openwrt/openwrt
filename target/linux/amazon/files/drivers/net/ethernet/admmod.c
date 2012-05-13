/******************************************************************************
     Copyright (c) 2004, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
 ******************************************************************************
   Module      : admmod.c
   Date        : 2004-09-01
   Description : JoeLin
   Remarks:

   Revision:
	MarsLin, add to support VLAN

 *****************************************************************************/
//000001.joelin 2005/06/02 add"ADM6996_MDC_MDIO_MODE" define, 
//		if define ADM6996_MDC_MDIO_MODE==> ADM6996LC and ADM6996I will be in MDIO/MDC(SMI)(16 bit) mode,
//		amazon should contrl ADM6996 by MDC/MDIO pin
//  		if undef ADM6996_MDC_MDIO_MODE==> ADM6996  will be in EEProm(32 bit) mode,
//		amazon should contrl ADM6996 by GPIO15,16,17,18  pin
/* 507281:linmars 2005/07/28 support MDIO/EEPROM config mode */
/* 509201:linmars remove driver testing codes */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <asm/atomic.h>
#include <asm-mips/amazon/amazon.h>
#include <asm-mips/amazon/adm6996.h>
//#include <linux/amazon/adm6996.h>


unsigned int ifx_sw_conf[ADM_SW_MAX_PORT_NUM+1] = \
	{ADM_SW_PORT0_CONF, ADM_SW_PORT1_CONF, ADM_SW_PORT2_CONF, \
	ADM_SW_PORT3_CONF, ADM_SW_PORT4_CONF, ADM_SW_PORT5_CONF};
unsigned int ifx_sw_bits[8] = \
	{0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff};
unsigned int ifx_sw_vlan_port[6] = {0, 2, 4, 6, 7, 8};
//050613:fchang 
/* 507281:linmars start */
#ifdef CONFIG_SWITCH_ADM6996_MDIO
#define ADM6996_MDC_MDIO_MODE 1 //000001.joelin
#else
#undef ADM6996_MDC_MDIO_MODE
#endif
/* 507281:linmars end */
#define adm6996i 0
#define adm6996lc 1
#define adm6996l  2
unsigned int adm6996_mode=adm6996i;
/*
  initialize GPIO pins.
  output mode, low
*/
void ifx_gpio_init(void)
{
 //GPIO16,17,18 direction:output
 //GPIO16,17,18 output 0
 
    AMAZON_SW_REG(AMAZON_GPIO_P1_DIR) |= (GPIO_MDIO|GPIO_MDCS|GPIO_MDC);
    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT) =AMAZON_SW_REG(AMAZON_GPIO_P1_IN)& ~(GPIO_MDIO|GPIO_MDCS|GPIO_MDC);

}

/* read one bit from mdio port */
int ifx_sw_mdio_readbit(void)
{
    //int val;

    //val = (AMAZON_SW_REG(GPIO_conf0_REG) & GPIO0_INPUT_MASK) >> 8;
    //return val;
    //GPIO16
    return AMAZON_SW_REG(AMAZON_GPIO_P1_IN)&1;
}

/*
  MDIO mode selection
  1 -> output
  0 -> input

  switch input/output mode of GPIO 0
*/
void ifx_mdio_mode(int mode)
{
//    AMAZON_SW_REG(GPIO_conf0_REG) = mode ? GPIO_ENABLEBITS :
//                             ((GPIO_ENABLEBITS | MDIO_INPUT) & ~MDIO_OUTPUT_EN);
    mode?(AMAZON_SW_REG(AMAZON_GPIO_P1_DIR)|=GPIO_MDIO):
         (AMAZON_SW_REG(AMAZON_GPIO_P1_DIR)&=~GPIO_MDIO);
    /*int r=AMAZON_SW_REG(AMAZON_GPIO_P1_DIR);
    mode?(r|=GPIO_MDIO):(r&=~GPIO_MDIO);
    AMAZON_SW_REG(AMAZON_GPIO_P1_DIR)=r;*/
}

void ifx_mdc_hi(void)
{
    //GPIO_SET_HI(GPIO_MDC);
    //AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)|=GPIO_MDC;
    /*int r=AMAZON_SW_REG(AMAZON_GPIO_P1_OUT);
    r|=GPIO_MDC;
    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=r;*/

    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=AMAZON_SW_REG(AMAZON_GPIO_P1_IN)|GPIO_MDC;
}

void ifx_mdio_hi(void)
{
    //GPIO_SET_HI(GPIO_MDIO);
    //AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)|=GPIO_MDIO;
    /*int r=AMAZON_SW_REG(AMAZON_GPIO_P1_OUT);
    r|=GPIO_MDIO;
    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=r;*/

    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=AMAZON_SW_REG(AMAZON_GPIO_P1_IN)|GPIO_MDIO;
}

void ifx_mdcs_hi(void)
{
    //GPIO_SET_HI(GPIO_MDCS);
    //AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)|=GPIO_MDCS;
    /*int r=AMAZON_SW_REG(AMAZON_GPIO_P1_OUT);
    r|=GPIO_MDCS;
    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=r;*/

    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=AMAZON_SW_REG(AMAZON_GPIO_P1_IN)|GPIO_MDCS;
}

void ifx_mdc_lo(void)
{
    //GPIO_SET_LOW(GPIO_MDC);
    //AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)&=~GPIO_MDC;
    /*int r=AMAZON_SW_REG(AMAZON_GPIO_P1_OUT);
    r&=~GPIO_MDC;
    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=r;*/

    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=AMAZON_SW_REG(AMAZON_GPIO_P1_IN)&(~GPIO_MDC);
}

void ifx_mdio_lo(void)
{
    //GPIO_SET_LOW(GPIO_MDIO);
    //AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)&=~GPIO_MDIO;
    /*int r=AMAZON_SW_REG(AMAZON_GPIO_P1_OUT);
    r&=~GPIO_MDIO;
    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=r;*/

    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=AMAZON_SW_REG(AMAZON_GPIO_P1_IN)&(~GPIO_MDIO);
}

void ifx_mdcs_lo(void)
{
    //GPIO_SET_LOW(GPIO_MDCS);
    //AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)&=~GPIO_MDCS;
    /*int r=AMAZON_SW_REG(AMAZON_GPIO_P1_OUT);
    r&=~GPIO_MDCS;
    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=r;*/
    
    AMAZON_SW_REG(AMAZON_GPIO_P1_OUT)=AMAZON_SW_REG(AMAZON_GPIO_P1_IN)&(~GPIO_MDCS);
}

/*
  mdc pulse
  0 -> 1 -> 0
*/
static void ifx_sw_mdc_pulse(void)
{
    ifx_mdc_lo();
    udelay(ADM_SW_MDC_DOWN_DELAY);
    ifx_mdc_hi();
    udelay(ADM_SW_MDC_UP_DELAY);
    ifx_mdc_lo();
}

/*
  mdc toggle
  1 -> 0
*/
static void ifx_sw_mdc_toggle(void)
{
    ifx_mdc_hi();
    udelay(ADM_SW_MDC_UP_DELAY);
    ifx_mdc_lo();
    udelay(ADM_SW_MDC_DOWN_DELAY);
}

/*
  enable eeprom write
  For ATC 93C66 type EEPROM; accessing ADM6996 internal EEPROM type registers
*/
static void ifx_sw_eeprom_write_enable(void)
{
    unsigned int op;

    ifx_mdcs_lo();
    ifx_mdc_lo();
    ifx_mdio_hi();
    udelay(ADM_SW_CS_DELAY);
    /* enable chip select */
    ifx_mdcs_hi();
    udelay(ADM_SW_CS_DELAY);
    /* start bit */
    ifx_mdio_hi();
    ifx_sw_mdc_pulse();

    /* eeprom write enable */
    op = ADM_SW_BIT_MASK_4;
    while (op)
    {
        if (op & ADM_SW_EEPROM_WRITE_ENABLE)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    op = ADM_SW_BIT_MASK_1 << (EEPROM_TYPE - 3);
    while (op)
    {
        ifx_mdio_lo();
        ifx_sw_mdc_pulse();
        op >>= 1;
    }
    /* disable chip select */
    ifx_mdcs_lo();
    udelay(ADM_SW_CS_DELAY);
    ifx_sw_mdc_pulse();
}

/*
  disable eeprom write
*/
static void ifx_sw_eeprom_write_disable(void)
{
    unsigned int op;

    ifx_mdcs_lo();
    ifx_mdc_lo();
    ifx_mdio_hi();
    udelay(ADM_SW_CS_DELAY);
    /* enable chip select */
    ifx_mdcs_hi();
    udelay(ADM_SW_CS_DELAY);

    /* start bit */
    ifx_mdio_hi();
    ifx_sw_mdc_pulse();
    /* eeprom write disable */
    op = ADM_SW_BIT_MASK_4;
    while (op)
    {
        if (op & ADM_SW_EEPROM_WRITE_DISABLE)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    op = ADM_SW_BIT_MASK_1 << (EEPROM_TYPE - 3);
    while (op)
    {
        ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }
    /* disable chip select */
    ifx_mdcs_lo();
    udelay(ADM_SW_CS_DELAY);
    ifx_sw_mdc_pulse();
}

/*
  read registers from ADM6996
  serial registers start at 0x200 (addr bit 9 = 1b)
  EEPROM registers -> 16bits; Serial registers -> 32bits
*/
#ifdef ADM6996_MDC_MDIO_MODE //smi mode//000001.joelin
static int ifx_sw_read_adm6996i_smi(unsigned int addr, unsigned int *dat)
{
   addr=(addr<<16)&0x3ff0000;
   AMAZON_SW_REG(AMAZON_SW_MDIO_ACC) =(0xC0000000|addr);
   while ((AMAZON_SW_REG(AMAZON_SW_MDIO_ACC))&0x80000000){};
   *dat=((AMAZON_SW_REG(AMAZON_SW_MDIO_ACC))&0x0FFFF);
    return 0;
}
#endif

static int ifx_sw_read_adm6996i(unsigned int addr, unsigned int *dat)
{
    unsigned int op;

    ifx_gpio_init();

    ifx_mdcs_hi();
    udelay(ADM_SW_CS_DELAY);

    ifx_mdcs_lo();
    ifx_mdc_lo();
    ifx_mdio_lo();

    udelay(ADM_SW_CS_DELAY);

    /* preamble, 32 bit 1 */
    ifx_mdio_hi();
    op = ADM_SW_BIT_MASK_32;
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* command start (01b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_START)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* read command (10b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_READ)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* send address A9 ~ A0 */
    op = ADM_SW_BIT_MASK_10;
    while (op)
    {
        if (op & addr)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* turnaround bits */
    op = ADM_SW_BIT_MASK_2;
    ifx_mdio_hi();
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    udelay(ADM_SW_MDC_DOWN_DELAY);

    /* set MDIO pin to input mode */
    ifx_mdio_mode(ADM_SW_MDIO_INPUT);

    /* start read data */
    *dat = 0;
//adm6996i    op = ADM_SW_BIT_MASK_32;
    op = ADM_SW_BIT_MASK_16;//adm6996i
    while (op)
    {
        *dat <<= 1;
        if (ifx_sw_mdio_readbit()) *dat |= 1;
        ifx_sw_mdc_toggle();

        op >>= 1;
    }

    /* set MDIO to output mode */
    ifx_mdio_mode(ADM_SW_MDIO_OUTPUT);

    /* dummy clock */
    op = ADM_SW_BIT_MASK_4;
    ifx_mdio_lo();
    while(op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    ifx_mdc_lo();
    ifx_mdio_lo();
    ifx_mdcs_hi();

    /* EEPROM registers */
//adm6996i    if (!(addr & 0x200))
//adm6996i    {
//adm6996i        if (addr % 2)
//adm6996i            *dat >>= 16;
//adm6996i        else
//adm6996i        *dat &= 0xffff;
//adm6996i    }

    return 0;
}
//adm6996
static int ifx_sw_read_adm6996l(unsigned int addr, unsigned int *dat)
{
    unsigned int op;

    ifx_gpio_init();

    ifx_mdcs_hi();
    udelay(ADM_SW_CS_DELAY);

    ifx_mdcs_lo();
    ifx_mdc_lo();
    ifx_mdio_lo();

    udelay(ADM_SW_CS_DELAY);

    /* preamble, 32 bit 1 */
    ifx_mdio_hi();
    op = ADM_SW_BIT_MASK_32;
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* command start (01b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_START)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* read command (10b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_READ)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* send address A9 ~ A0 */
    op = ADM_SW_BIT_MASK_10;
    while (op)
    {
        if (op & addr)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* turnaround bits */
    op = ADM_SW_BIT_MASK_2;
    ifx_mdio_hi();
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    udelay(ADM_SW_MDC_DOWN_DELAY);

    /* set MDIO pin to input mode */
    ifx_mdio_mode(ADM_SW_MDIO_INPUT);

    /* start read data */
    *dat = 0;
    op = ADM_SW_BIT_MASK_32;
    while (op)
    {
        *dat <<= 1;
        if (ifx_sw_mdio_readbit()) *dat |= 1;
        ifx_sw_mdc_toggle();

        op >>= 1;
    }

    /* set MDIO to output mode */
    ifx_mdio_mode(ADM_SW_MDIO_OUTPUT);

    /* dummy clock */
    op = ADM_SW_BIT_MASK_4;
    ifx_mdio_lo();
    while(op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    ifx_mdc_lo();
    ifx_mdio_lo();
    ifx_mdcs_hi();

    /* EEPROM registers */
    if (!(addr & 0x200))
    {
        if (addr % 2)
            *dat >>= 16;
        else
        *dat &= 0xffff;
    }

    return 0;
}

static int ifx_sw_read(unsigned int addr, unsigned int *dat)
{
#ifdef ADM6996_MDC_MDIO_MODE //smi mode ////000001.joelin
	ifx_sw_read_adm6996i_smi(addr,dat);
#else	
	if (adm6996_mode==adm6996i) ifx_sw_read_adm6996i(addr,dat);
		else ifx_sw_read_adm6996l(addr,dat);
#endif		
	return 0;
	
}

/*
  write register to ADM6996 eeprom registers
*/
//for adm6996i -start
#ifdef ADM6996_MDC_MDIO_MODE //smi mode //000001.joelin
static int ifx_sw_write_adm6996i_smi(unsigned int addr, unsigned int dat)
{
 
   AMAZON_SW_REG(AMAZON_SW_MDIO_ACC) = ((addr<<16)&0x3ff0000)|dat|0x80000000;
   while ((AMAZON_SW_REG(AMAZON_SW_MDIO_ACC))&0x80000000){};
  
    return 0;
 
}
#endif //ADM6996_MDC_MDIO_MODE //000001.joelin

static int ifx_sw_write_adm6996i(unsigned int addr, unsigned int dat)
{
    unsigned int op;

    ifx_gpio_init();

    ifx_mdcs_hi();
    udelay(ADM_SW_CS_DELAY);

    ifx_mdcs_lo();
    ifx_mdc_lo();
    ifx_mdio_lo();

    udelay(ADM_SW_CS_DELAY);

    /* preamble, 32 bit 1 */
    ifx_mdio_hi();
    op = ADM_SW_BIT_MASK_32;
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* command start (01b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_START)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* write command (01b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_WRITE)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* send address A9 ~ A0 */
    op = ADM_SW_BIT_MASK_10;
    while (op)
    {
        if (op & addr)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* turnaround bits */
    op = ADM_SW_BIT_MASK_2;
    ifx_mdio_hi();
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    udelay(ADM_SW_MDC_DOWN_DELAY);

    /* set MDIO pin to output mode */
    ifx_mdio_mode(ADM_SW_MDIO_OUTPUT);

  
    /* start write data */
    op = ADM_SW_BIT_MASK_16;
    while (op)
    {
        if (op & dat)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_toggle();
        op >>= 1;
    }

 //   /* set MDIO to output mode */
 //   ifx_mdio_mode(ADM_SW_MDIO_OUTPUT);

    /* dummy clock */
    op = ADM_SW_BIT_MASK_4;
    ifx_mdio_lo();
    while(op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    ifx_mdc_lo();
    ifx_mdio_lo();
    ifx_mdcs_hi();

    /* EEPROM registers */
//adm6996i    if (!(addr & 0x200))
//adm6996i    {
//adm6996i        if (addr % 2)
//adm6996i            *dat >>= 16;
//adm6996i        else
//adm6996i        *dat &= 0xffff;
//adm6996i    }

    return 0;
}
//for adm6996i-end
static int ifx_sw_write_adm6996l(unsigned int addr, unsigned int dat)
{
    unsigned int op;

    ifx_gpio_init();

    /* enable write */
    ifx_sw_eeprom_write_enable();

    /* chip select */
    ifx_mdcs_hi();
    udelay(ADM_SW_CS_DELAY);

    /* issue write command */
    /* start bit */
    ifx_mdio_hi();
    ifx_sw_mdc_pulse();

    /* EEPROM write command */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_EEPROM_WRITE)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* send address A7 ~ A0 */
    op = ADM_SW_BIT_MASK_1 << (EEPROM_TYPE - 1);

    while (op)
    {
        if (op & addr)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_toggle();
        op >>= 1;
    }

    /* start write data */
    op = ADM_SW_BIT_MASK_16;
    while (op)
    {
        if (op & dat)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_toggle();
        op >>= 1;
    }

    /* disable cs & wait 1 clock */
    ifx_mdcs_lo();
    udelay(ADM_SW_CS_DELAY);
    ifx_sw_mdc_toggle();

    ifx_sw_eeprom_write_disable();

    return 0;
}

static int ifx_sw_write(unsigned int addr, unsigned int dat)
{
#ifdef ADM6996_MDC_MDIO_MODE //smi mode ////000001.joelin
	ifx_sw_write_adm6996i_smi(addr,dat);
#else	//000001.joelin
	if (adm6996_mode==adm6996i) ifx_sw_write_adm6996i(addr,dat);
		else ifx_sw_write_adm6996l(addr,dat);
#endif	//000001.joelin
	return 0;
}

/*
  do switch PHY reset
*/
int ifx_sw_reset(void)
{
    /* reset PHY */
    ifx_sw_write(ADM_SW_PHY_RESET, 0);

    return 0;
}

/* 509201:linmars start */
#if 0
/*
  check port status
*/
int ifx_check_port_status(int port)
{
    unsigned int val;

    if ((port < 0) || (port > ADM_SW_MAX_PORT_NUM))
    {
        ifx_printf(("error on port number (%d)!!\n", port));
        return -1;
    }

    ifx_sw_read(ifx_sw_conf[port], &val);
    if (ifx_sw_conf[port]%2) val >>= 16;
    /* only 16bits are effective */
    val &= 0xFFFF;

    ifx_printf(("Port %d status (%.8x): \n", port, val));

    if (val & ADM_SW_PORT_FLOWCTL)
        ifx_printf(("\t802.3x flow control supported!\n"));
    else
        ifx_printf(("\t802.3x flow control not supported!\n"));

    if (val & ADM_SW_PORT_AN)
        ifx_printf(("\tAuto negotiation ON!\n"));
    else
        ifx_printf(("\tAuto negotiation OFF!\n"));

    if (val & ADM_SW_PORT_100M)
        ifx_printf(("\tLink at 100M!\n"));
    else
        ifx_printf(("\tLink at 10M!\n"));

    if (val & ADM_SW_PORT_FULL)
        ifx_printf(("\tFull duplex!\n"));
    else
        ifx_printf(("\tHalf duplex!\n"));

    if (val & ADM_SW_PORT_DISABLE)
        ifx_printf(("\tPort disabled!\n"));
    else
        ifx_printf(("\tPort enabled!\n"));

    if (val & ADM_SW_PORT_TOS)
        ifx_printf(("\tTOS enabled!\n"));
    else
        ifx_printf(("\tTOS disabled!\n"));

    if (val & ADM_SW_PORT_PPRI)
        ifx_printf(("\tPort priority first!\n"));
    else
        ifx_printf(("\tVLAN or TOS priority first!\n"));

    if (val & ADM_SW_PORT_MDIX)
        ifx_printf(("\tAuto MDIX!\n"));
    else
        ifx_printf(("\tNo auto MDIX\n"));

    ifx_printf(("\tPVID: %d\n", \
  	    ((val >> ADM_SW_PORT_PVID_SHIFT)&ifx_sw_bits[ADM_SW_PORT_PVID_BITS])));

    return 0;
}
/*
  initialize a VLAN
  clear all VLAN bits
*/
int ifx_sw_vlan_init(int vlanid)
{
    ifx_sw_write(ADM_SW_VLAN0_CONF + vlanid, 0);

    return 0;
}

/*
  add a port to certain vlan
*/
int ifx_sw_vlan_add(int port, int vlanid)
{
    int reg = 0;

    if ((port < 0) || (port > ADM_SW_MAX_PORT_NUM) || (vlanid < 0) ||
        (vlanid > ADM_SW_MAX_VLAN_NUM))
    {
        ifx_printf(("Port number or VLAN number ERROR!!\n"));
        return -1;
    }
    ifx_sw_read(ADM_SW_VLAN0_CONF + vlanid, &reg);
    reg |= (1 << ifx_sw_vlan_port[port]);
    ifx_sw_write(ADM_SW_VLAN0_CONF + vlanid, reg);

    return 0;
}

/*
  delete a given port from certain vlan
*/
int ifx_sw_vlan_del(int port, int vlanid)
{
    unsigned int reg = 0;

    if ((port < 0) || (port > ADM_SW_MAX_PORT_NUM) || (vlanid < 0) || (vlanid > ADM_SW_MAX_VLAN_NUM))
    {
        ifx_printf(("Port number or VLAN number ERROR!!\n"));
        return -1;
    }
    ifx_sw_read(ADM_SW_VLAN0_CONF + vlanid, &reg);
    reg &= ~(1 << ifx_sw_vlan_port[port]);
    ifx_sw_write(ADM_SW_VLAN0_CONF + vlanid, reg);

    return 0;
}

/*
  default VLAN setting

  port 0~3 as untag port and PVID = 1
  VLAN1: port 0~3 and port 5 (MII)
*/
static int ifx_sw_init(void)
{
    ifx_printf(("Setting default ADM6996 registers... \n"));

    /* MAC clone, 802.1q based VLAN */
    ifx_sw_write(ADM_SW_VLAN_MODE, 0xff30);
    /* auto MDIX, PVID=1, untag */
    ifx_sw_write(ADM_SW_PORT0_CONF, 0x840f);
    ifx_sw_write(ADM_SW_PORT1_CONF, 0x840f);
    ifx_sw_write(ADM_SW_PORT2_CONF, 0x840f);
    ifx_sw_write(ADM_SW_PORT3_CONF, 0x840f);
    /* auto MDIX, PVID=2, untag */
    ifx_sw_write(ADM_SW_PORT5_CONF, 0x880f);
    /* port 0~3 & 5 as VLAN1 */
    ifx_sw_write(ADM_SW_VLAN0_CONF+1, 0x0155);

    return 0;
}
#endif
/* 509201:linmars end */

int adm_open(struct inode *node, struct file *filp)
{
    return 0;
}

ssize_t adm_read(struct file *filep, char *buf, size_t count, loff_t *ppos)
{
    return count;
}

ssize_t adm_write(struct file *filep, const char *buf, size_t count, loff_t *ppos)
{
    return count;
}

/* close */
int adm_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/* IOCTL function */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
static long adm_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
#else
static int adm_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long args)
#endif
{
    PREGRW uREGRW;
    unsigned int rtval;
    unsigned int val;		//6996i
    unsigned int control[6] ;	//6996i
    unsigned int status[6] ;	//6996i
    
    PMACENTRY mMACENTRY;//adm6996i
    PPROTOCOLFILTER uPROTOCOLFILTER ;///adm6996i

    if (_IOC_TYPE(cmd) != ADM_MAGIC)
    {
        printk("adm_ioctl: IOC_TYPE(%x) != ADM_MAGIC(%x)! \n", _IOC_TYPE(cmd), ADM_MAGIC);
        return (-EINVAL);
    }

    if(_IOC_NR(cmd) >= KEY_IOCTL_MAX_KEY)
    {
        printk(KERN_WARNING "adm_ioctl: IOC_NR(%x) invalid! \n", _IOC_NR(cmd));
        return (-EINVAL);
    }

    switch (cmd)
    {
        case ADM_IOCTL_REGRW:
        {
            uREGRW = (PREGRW)kmalloc(sizeof(REGRW), GFP_KERNEL);
            rtval = copy_from_user(uREGRW, (PREGRW)args, sizeof(REGRW));
            if (rtval != 0)
            {
                printk("ADM_IOCTL_REGRW: copy from user FAILED!! \n");
                return (-EFAULT);
            }

            switch(uREGRW->mode)
            {
                case REG_READ:
                    uREGRW->value = 0x12345678;//inl(uREGRW->addr);
                    copy_to_user((PREGRW)args, uREGRW, sizeof(REGRW));
                    break;
                case REG_WRITE:
                    //outl(uREGRW->value, uREGRW->addr);
                    break;

                default:
                    printk("No such Register Read/Write function!! \n");
                    return (-EFAULT);
            }
            kfree(uREGRW);
            break;
        }

        case ADM_SW_IOCTL_REGRW:
        {
            unsigned int val = 0xff;

            uREGRW = (PREGRW)kmalloc(sizeof(REGRW), GFP_KERNEL);
            rtval = copy_from_user(uREGRW, (PREGRW)args, sizeof(REGRW));
            if (rtval != 0)
            {
                printk("ADM_IOCTL_REGRW: copy from user FAILED!! \n");
                return (-EFAULT);
            }

            switch(uREGRW->mode)
            {
                case REG_READ:
                    ifx_sw_read(uREGRW->addr, &val);
                    uREGRW->value = val;
                    copy_to_user((PREGRW)args, uREGRW, sizeof(REGRW));
                    break;

                case REG_WRITE:
                    ifx_sw_write(uREGRW->addr, uREGRW->value);
                    break;
                default:
                    printk("No such Register Read/Write function!! \n");
                    return (-EFAULT);
            }
            kfree(uREGRW);
            break;
        }
/* 509201:linmars start */
#if 0
        case ADM_SW_IOCTL_PORTSTS:
            for (rtval = 0; rtval < ADM_SW_MAX_PORT_NUM+1; rtval++)
                ifx_check_port_status(rtval);
            break;
        case ADM_SW_IOCTL_INIT:
            ifx_sw_init();
            break;
#endif
/* 509201:linmars end */
//adm6996i
        case ADM_SW_IOCTL_MACENTRY_ADD:
        case ADM_SW_IOCTL_MACENTRY_DEL:
        case ADM_SW_IOCTL_MACENTRY_GET_INIT:
        case ADM_SW_IOCTL_MACENTRY_GET_MORE:
                

           mMACENTRY = (PMACENTRY)kmalloc(sizeof(MACENTRY), GFP_KERNEL);
            rtval = copy_from_user(mMACENTRY, (PMACENTRY)args, sizeof(MACENTRY));
            if (rtval != 0)
            {
                printk("ADM_SW_IOCTL_MACENTRY: copy from user FAILED!! \n");
                return (-EFAULT);
            }
           control[0]=(mMACENTRY->mac_addr[1]<<8)+mMACENTRY->mac_addr[0]     ; 
           control[1]=(mMACENTRY->mac_addr[3]<<8)+mMACENTRY->mac_addr[2]      ;         
           control[2]=(mMACENTRY->mac_addr[5]<<8)+mMACENTRY->mac_addr[4]     ;
           control[3]=(mMACENTRY->fid&0xf)+((mMACENTRY->portmap&0x3f)<<4);
           if (((mMACENTRY->info_type)&0x01)) control[4]=(mMACENTRY->ctrl.info_ctrl)+0x1000; //static ,info control
           	else	control[4]=((mMACENTRY->ctrl.age_timer)&0xff);//not static ,agetimer
         	if (cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT) { 	
	           //initial  the pointer to the first address	
			           val=0x8000;//busy ,status5[15]
			           while(val&0x8000){		//check busy ?
			 	          ifx_sw_read(0x125, &val);
			        	}    
			           control[5]=0x030;//initial the first address	
			           ifx_sw_write(0x11f,control[5]);
			        	       	
			           	
			           val=0x8000;//busy ,status5[15]
			           while(val&0x8000){		//check busy ?
			 	          ifx_sw_read(0x125, &val);
			        	}           	
	           	
	           }	//if (cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT)								
           if (cmd==ADM_SW_IOCTL_MACENTRY_ADD) control[5]=0x07;//create a new address
           	else if (cmd==ADM_SW_IOCTL_MACENTRY_DEL) control[5]=0x01f;//erased an existed address
           	else if ((cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT)||(cmd==ADM_SW_IOCTL_MACENTRY_GET_MORE)) 
           		control[5]=0x02c;//search by the mac address field
           
           val=0x8000;//busy ,status5[15]
           while(val&0x8000){		//check busy ?
 	          ifx_sw_read(0x125, &val);
        	}
        	ifx_sw_write(0x11a,control[0]);	
        	ifx_sw_write(0x11b,control[1]);	
        	ifx_sw_write(0x11c,control[2]);	
        	ifx_sw_write(0x11d,control[3]);	
        	ifx_sw_write(0x11e,control[4]);	
        	ifx_sw_write(0x11f,control[5]);	
           val=0x8000;//busy ,status5[15]
           while(val&0x8000){		//check busy ?
 	          ifx_sw_read(0x125, &val);
        	}	
           val=((val&0x7000)>>12);//result ,status5[14:12]
           mMACENTRY->result=val;
   
           if (!val) {
        		printk(" Command OK!! \n");
        		if ((cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT)||(cmd==ADM_SW_IOCTL_MACENTRY_GET_MORE)) {
			           	ifx_sw_read(0x120,&(status[0]));	
			        	ifx_sw_read(0x121,&(status[1]));	
			        	ifx_sw_read(0x122,&(status[2]));	
			        	ifx_sw_read(0x123,&(status[3]));	
			        	ifx_sw_read(0x124,&(status[4]));	
			        	ifx_sw_read(0x125,&(status[5]));	
		
		           		
		        		mMACENTRY->mac_addr[0]=(status[0]&0x00ff)	;
		        		mMACENTRY->mac_addr[1]=(status[0]&0xff00)>>8    ;
		        		mMACENTRY->mac_addr[2]=(status[1]&0x00ff)    ;
		        		mMACENTRY->mac_addr[3]=(status[1]&0xff00)>>8 ;
		        		mMACENTRY->mac_addr[4]=(status[2]&0x00ff)    ;
		        		mMACENTRY->mac_addr[5]=(status[2]&0xff00)>>8 ;
		        		mMACENTRY->fid=(status[3]&0xf);
		        		mMACENTRY->portmap=((status[3]>>4)&0x3f);
		        		if (status[5]&0x2) {//static info_ctrl //status5[1]????
		        			mMACENTRY->ctrl.info_ctrl=(status[4]&0x00ff);
		  				mMACENTRY->info_type=1;
		        				}
		        		else {//not static age_timer
		        			mMACENTRY->ctrl.age_timer=(status[4]&0x00ff);
		  				mMACENTRY->info_type=0;
		        				}
//status5[13]????					mMACENTRY->occupy=(status[5]&0x02)>>1;//status5[1]
					mMACENTRY->occupy=(status[5]&0x02000)>>13;//status5[13] ???
					mMACENTRY->bad=(status[5]&0x04)>>2;//status5[2]
				}//if ((cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT)||(cmd==ADM_SW_IOCTL_MACENTRY_GET_MORE)) 
			
        	}
           else if (val==0x001)  
                printk(" All Entry Used!! \n");
            else if (val==0x002) 
                printk("  Entry Not Found!! \n");
            else if (val==0x003) 
                printk(" Try Next Entry!! \n");
            else if (val==0x005)  
                printk(" Command Error!! \n");   
            else   
                printk(" UnKnown Error!! \n");
                
            copy_to_user((PMACENTRY)args, mMACENTRY,sizeof(MACENTRY));    
                
 	    break;  
 
        case ADM_SW_IOCTL_FILTER_ADD:
        case ADM_SW_IOCTL_FILTER_DEL:
        case ADM_SW_IOCTL_FILTER_GET:

            uPROTOCOLFILTER = (PPROTOCOLFILTER)kmalloc(sizeof(PROTOCOLFILTER), GFP_KERNEL);
            rtval = copy_from_user(uPROTOCOLFILTER, (PPROTOCOLFILTER)args, sizeof(PROTOCOLFILTER));
            if (rtval != 0)
            {
                printk("ADM_SW_IOCTL_FILTER_ADD: copy from user FAILED!! \n");
                return (-EFAULT);
            }
            
        	if(cmd==ADM_SW_IOCTL_FILTER_DEL) {	//delete filter
			uPROTOCOLFILTER->ip_p=00;	//delet filter
			uPROTOCOLFILTER->action=00;	//delete filter
		}					//delete filter

            ifx_sw_read(((uPROTOCOLFILTER->protocol_filter_num/2)+0x68), &val);//rx68~rx6b,protocol filter0~7	

            	if (((uPROTOCOLFILTER->protocol_filter_num)%2)==00){	
            		if(cmd==ADM_SW_IOCTL_FILTER_GET) uPROTOCOLFILTER->ip_p= val&0x00ff;//get filter ip_p
            			else val=(val&0xff00)|(uPROTOCOLFILTER->ip_p);//set filter ip_p
        	}
        	else {
        		if(cmd==ADM_SW_IOCTL_FILTER_GET) uPROTOCOLFILTER->ip_p= (val>>8);//get filter ip_p
        			else val=(val&0x00ff)|((uPROTOCOLFILTER->ip_p)<<8);//set filter ip_p
        	}	
            if(cmd!=ADM_SW_IOCTL_FILTER_GET) ifx_sw_write(((uPROTOCOLFILTER->protocol_filter_num/2)+0x68), val);//write rx68~rx6b,protocol filter0~7	
            		
            ifx_sw_read(0x95, &val);	//protocol filter action
            if(cmd==ADM_SW_IOCTL_FILTER_GET) {
            		uPROTOCOLFILTER->action= ((val>>(uPROTOCOLFILTER->protocol_filter_num*2))&0x3);//get filter action
            		copy_to_user((PPROTOCOLFILTER)args, uPROTOCOLFILTER, sizeof(PROTOCOLFILTER));
            	
            	}
            	else {
            		val=(val&(~(0x03<<(uPROTOCOLFILTER->protocol_filter_num*2))))|(((uPROTOCOLFILTER->action)&0x03)<<(uPROTOCOLFILTER->protocol_filter_num*2));
  //          		printk("%d----\n",val);
            		ifx_sw_write(0x95, val);	//write protocol filter action		
            	}
            	
            break;
//adm6996i  

        /* others */
        default:
            return -EFAULT;
    }
    /* end of switch */
    return 0;
}

/* Santosh: handle IGMP protocol filter ADD/DEL/GET */
int adm_process_protocol_filter_request (unsigned int cmd, PPROTOCOLFILTER uPROTOCOLFILTER)
{
    unsigned int val;		//6996i

	if(cmd==ADM_SW_IOCTL_FILTER_DEL) {	//delete filter
	uPROTOCOLFILTER->ip_p=00;	//delet filter
	uPROTOCOLFILTER->action=00;	//delete filter
	}					//delete filter

    ifx_sw_read(((uPROTOCOLFILTER->protocol_filter_num/2)+0x68), &val);//rx68~rx6b,protocol filter0~7	

    if (((uPROTOCOLFILTER->protocol_filter_num)%2)==00){	
    	if(cmd==ADM_SW_IOCTL_FILTER_GET) uPROTOCOLFILTER->ip_p= val&0x00ff;//get filter ip_p
        else val=(val&0xff00)|(uPROTOCOLFILTER->ip_p);//set filter ip_p
    }
    else {
    	if(cmd==ADM_SW_IOCTL_FILTER_GET) uPROTOCOLFILTER->ip_p= (val>>8);//get filter ip_p
    	else val=(val&0x00ff)|((uPROTOCOLFILTER->ip_p)<<8);//set filter ip_p
    }	
    if(cmd!=ADM_SW_IOCTL_FILTER_GET) ifx_sw_write(((uPROTOCOLFILTER->protocol_filter_num/2)+0x68), val);//write rx68~rx6b,protocol filter0~7	
            		
    	ifx_sw_read(0x95, &val);	//protocol filter action
    if(cmd==ADM_SW_IOCTL_FILTER_GET) {
       	uPROTOCOLFILTER->action= ((val>>(uPROTOCOLFILTER->protocol_filter_num*2))&0x3);//get filter action
    }
    else {
    	val=(val&(~(0x03<<(uPROTOCOLFILTER->protocol_filter_num*2))))|(((uPROTOCOLFILTER->action)&0x03)<<(uPROTOCOLFILTER->protocol_filter_num*2));
        ifx_sw_write(0x95, val);	//write protocol filter action		
    }
            	
	return 0;
}


/* Santosh: function for MAC ENTRY ADD/DEL/GET */

int adm_process_mac_table_request (unsigned int cmd, PMACENTRY mMACENTRY)
{
    unsigned int val;		//6996i
    unsigned int control[6] ;	//6996i
    unsigned int status[6] ;	//6996i

	// printk ("adm_process_mac_table_request: enter\n");	

    control[0]=(mMACENTRY->mac_addr[1]<<8)+mMACENTRY->mac_addr[0]     ; 
    control[1]=(mMACENTRY->mac_addr[3]<<8)+mMACENTRY->mac_addr[2]      ;         
    control[2]=(mMACENTRY->mac_addr[5]<<8)+mMACENTRY->mac_addr[4]     ;
    control[3]=(mMACENTRY->fid&0xf)+((mMACENTRY->portmap&0x3f)<<4);

    if (((mMACENTRY->info_type)&0x01)) control[4]=(mMACENTRY->ctrl.info_ctrl)+0x1000; //static ,info control
   		else	control[4]=((mMACENTRY->ctrl.age_timer)&0xff);//not static ,agetimer
        	if (cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT) { 	
	          //initial  the pointer to the first address	
	           val=0x8000;//busy ,status5[15]
	           while(val&0x8000){		//check busy ?
	           ifx_sw_read(0x125, &val);
	       	}    
	        control[5]=0x030;//initial the first address	
	        ifx_sw_write(0x11f,control[5]);
			        	       	
			           	
			           val=0x8000;//busy ,status5[15]
			           while(val&0x8000){		//check busy ?
			 	          ifx_sw_read(0x125, &val);
			        	}           	
	           	
	           }	//if (cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT)								
           if (cmd==ADM_SW_IOCTL_MACENTRY_ADD) control[5]=0x07;//create a new address
           	else if (cmd==ADM_SW_IOCTL_MACENTRY_DEL) control[5]=0x01f;//erased an existed address
           	else if ((cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT)||(cmd==ADM_SW_IOCTL_MACENTRY_GET_MORE)) 
           		control[5]=0x02c;//search by the mac address field
           
           val=0x8000;//busy ,status5[15]
           while(val&0x8000){		//check busy ?
 	          ifx_sw_read(0x125, &val);
        	}
        	ifx_sw_write(0x11a,control[0]);	
        	ifx_sw_write(0x11b,control[1]);	
        	ifx_sw_write(0x11c,control[2]);	
        	ifx_sw_write(0x11d,control[3]);	
        	ifx_sw_write(0x11e,control[4]);	
        	ifx_sw_write(0x11f,control[5]);	
           val=0x8000;//busy ,status5[15]
           while(val&0x8000){		//check busy ?
 	          ifx_sw_read(0x125, &val);
        	}	
           val=((val&0x7000)>>12);//result ,status5[14:12]
           mMACENTRY->result=val;
   
           if (!val) {
        		printk(" Command OK!! \n");
        		if ((cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT)||(cmd==ADM_SW_IOCTL_MACENTRY_GET_MORE)) {
			           	ifx_sw_read(0x120,&(status[0]));	
			        	ifx_sw_read(0x121,&(status[1]));	
			        	ifx_sw_read(0x122,&(status[2]));	
			        	ifx_sw_read(0x123,&(status[3]));	
			        	ifx_sw_read(0x124,&(status[4]));	
			        	ifx_sw_read(0x125,&(status[5]));	
		
		           		
		        		mMACENTRY->mac_addr[0]=(status[0]&0x00ff)	;
		        		mMACENTRY->mac_addr[1]=(status[0]&0xff00)>>8    ;
		        		mMACENTRY->mac_addr[2]=(status[1]&0x00ff)    ;
		        		mMACENTRY->mac_addr[3]=(status[1]&0xff00)>>8 ;
		        		mMACENTRY->mac_addr[4]=(status[2]&0x00ff)    ;
		        		mMACENTRY->mac_addr[5]=(status[2]&0xff00)>>8 ;
		        		mMACENTRY->fid=(status[3]&0xf);
		        		mMACENTRY->portmap=((status[3]>>4)&0x3f);
		        		if (status[5]&0x2) {//static info_ctrl //status5[1]????
		        			mMACENTRY->ctrl.info_ctrl=(status[4]&0x00ff);
		  				mMACENTRY->info_type=1;
		        				}
		        		else {//not static age_timer
		        			mMACENTRY->ctrl.age_timer=(status[4]&0x00ff);
		  				mMACENTRY->info_type=0;
		        				}
//status5[13]????					mMACENTRY->occupy=(status[5]&0x02)>>1;//status5[1]
					mMACENTRY->occupy=(status[5]&0x02000)>>13;//status5[13] ???
					mMACENTRY->bad=(status[5]&0x04)>>2;//status5[2]
				}//if ((cmd==ADM_SW_IOCTL_MACENTRY_GET_INIT)||(cmd==ADM_SW_IOCTL_MACENTRY_GET_MORE)) 
			
        	}
           else if (val==0x001)  
                printk(" All Entry Used!! \n");
            else if (val==0x002) 
                printk("  Entry Not Found!! \n");
            else if (val==0x003) 
                printk(" Try Next Entry!! \n");
            else if (val==0x005)  
                printk(" Command Error!! \n");   
            else   
                printk(" UnKnown Error!! \n");

	// printk ("adm_process_mac_table_request: Exit\n");	
	return 0;
}

/* Santosh: End of function for MAC ENTRY ADD/DEL*/
struct file_operations adm_ops =
{
    read: adm_read,
    write: adm_write,
    open: adm_open,
    release: adm_release,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
    unlocked_ioctl: adm_ioctl
#else
    ioctl: adm_ioctl
#endif
};

int adm_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(buf+len, " ************ Registers ************ \n");
    *eof = 1;
    return len;
}

int __init init_adm6996_module(void)
{
    unsigned int val = 000;
    unsigned int val1 = 000;

    printk("Loading ADM6996 driver... \n");

    /* if running on adm5120 */
    /* set GPIO 0~2 as adm6996 control pins */
    //outl(0x003f3f00, 0x12000028);
    /* enable switch port 5 (MII) as RMII mode (5120MAC <-> 6996MAC) */
    //outl(0x18a, 0x12000030);
    /* group adm5120 port 1 ~ 5 as VLAN0, port 5 & 6(CPU) as VLAN1 */
    //outl(0x417e, 0x12000040);
    /* end adm5120 fixup */
#ifdef ADM6996_MDC_MDIO_MODE //smi mode //000001.joelin
    register_chrdev(69, "adm6996", &adm_ops);
    AMAZON_SW_REG(AMAZON_SW_MDIO_CFG) = 0x27be;
    AMAZON_SW_REG(AMAZON_SW_EPHY) = 0xfc;
    adm6996_mode=adm6996i;
    ifx_sw_read(0xa0, &val);
    ifx_sw_read(0xa1, &val1);
    val=((val1&0x0f)<<16)|val;
    printk ("\nADM6996 SMI Mode-");
    printk ("Chip ID:%5x \n ", val);
#else    //000001.joelin
 
    AMAZON_SW_REG(AMAZON_SW_MDIO_CFG) = 0x2c50;
    AMAZON_SW_REG(AMAZON_SW_EPHY) = 0xff;

    AMAZON_SW_REG(AMAZON_GPIO_P1_ALTSEL0) &= ~(GPIO_MDIO|GPIO_MDCS|GPIO_MDC);
    AMAZON_SW_REG(AMAZON_GPIO_P1_ALTSEL1) &= ~(GPIO_MDIO|GPIO_MDCS|GPIO_MDC);
    AMAZON_SW_REG(AMAZON_GPIO_P1_OD) |= (GPIO_MDIO|GPIO_MDCS|GPIO_MDC);
  
    ifx_gpio_init();
    register_chrdev(69, "adm6996", &adm_ops);
    mdelay(100);

    /* create proc entries */
    //  create_proc_read_entry("admide", 0, NULL, admide_proc, NULL);

//joelin adm6996i support start
    adm6996_mode=adm6996i;
    ifx_sw_read(0xa0, &val);
    adm6996_mode=adm6996l;
    ifx_sw_read(0x200, &val1);
//  printk ("\n %0x \n",val1);
    if ((val&0xfff0)==0x1020) {
        printk ("\n ADM6996I .. \n");
        adm6996_mode=adm6996i;	
    }
    else if ((val1&0xffffff00)==0x71000) {//71010 or 71020
        printk ("\n ADM6996LC .. \n");
        adm6996_mode=adm6996lc;	
    }
    else  {
        printk ("\n ADM6996L .. \n");
        adm6996_mode=adm6996l;	
    }
#endif //ADM6996_MDC_MDIO_MODE //smi mode //000001.joelin	

    if ((adm6996_mode==adm6996lc)||(adm6996_mode==adm6996i)){
#if 0	/* removed by MarsLin */
        ifx_sw_write(0x29,0xc000);
        ifx_sw_write(0x30,0x0985);
#else
        ifx_sw_read(0xa0, &val);
        if (val == 0x1021) // for both 6996LC and 6996I, only AB version need the patch
            ifx_sw_write(0x29, 0x9000);
        ifx_sw_write(0x30,0x0985);
#endif
    }
//joelin adm6996i support end
    return 0;
}

void __exit cleanup_adm6996_module(void)
{
    printk("Free ADM device driver... \n");

    unregister_chrdev(69, "adm6996");

    /* remove proc entries */
    //  remove_proc_entry("admide", NULL);
}

/* MarsLin, add start */
#if defined(CONFIG_IFX_NFEXT_AMAZON_SWITCH_PHYPORT) || defined(CONFIG_IFX_NFEXT_AMAZON_SWITCH_PHYPORT_MODULE)
    #define SET_BIT(reg, mask)		reg |= (mask)
    #define CLEAR_BIT(reg, mask)	reg &= (~mask)
    static int ifx_hw_reset(void)
    {
        CLEAR_BIT((*AMAZON_GPIO_P0_ALTSEL0),0x2000);
        CLEAR_BIT((*AMAZON_GPIO_P0_ALTSEL1),0x2000);
        SET_BIT((*AMAZON_GPIO_P0_OD),0x2000);
        SET_BIT((*AMAZON_GPIO_P0_DIR), 0x2000);
	CLEAR_BIT((*AMAZON_GPIO_P0_OUT), 0x2000);
	mdelay(500);
	SET_BIT((*AMAZON_GPIO_P0_OUT), 0x2000);
        cleanup_adm6996_module();
        return init_adm6996_module();
    }
    int (*adm6996_hw_reset)(void) = ifx_hw_reset;
    EXPORT_SYMBOL(adm6996_hw_reset);
    EXPORT_SYMBOL(adm6996_mode);
    int (*adm6996_sw_read)(unsigned int addr, unsigned int *data) = ifx_sw_read;
    EXPORT_SYMBOL(adm6996_sw_read);
    int (*adm6996_sw_write)(unsigned int addr, unsigned int data) = ifx_sw_write;
    EXPORT_SYMBOL(adm6996_sw_write);
#endif
/* MarsLin, add end */

/* Santosh: for IGMP proxy/snooping, Begin */
EXPORT_SYMBOL (adm_process_mac_table_request);
EXPORT_SYMBOL (adm_process_protocol_filter_request);
/* Santosh: for IGMP proxy/snooping, End */
	
MODULE_DESCRIPTION("ADMtek 6996 Driver");
MODULE_AUTHOR("Joe Lin <joe.lin@infineon.com>");
MODULE_LICENSE("GPL");

module_init(init_adm6996_module);
module_exit(cleanup_adm6996_module);

