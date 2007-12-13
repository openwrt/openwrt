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
 *   Copyright (C) 2006 infineon
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <linux/errno.h>
#include <asm/danube/danube.h>
#include <asm/danube/danube_gpio.h>
#include <asm/danube/danube_gptu.h>

#define LED_CONFIG                      0x01

#define CONFIG_OPERATION_UPDATE_SOURCE  0x0001
#define CONFIG_OPERATION_BLINK          0x0002
#define CONFIG_OPERATION_UPDATE_CLOCK   0x0004
#define CONFIG_OPERATION_STORE_MODE     0x0008
#define CONFIG_OPERATION_SHIFT_CLOCK    0x0010
#define CONFIG_OPERATION_DATA_OFFSET    0x0020
#define CONFIG_OPERATION_NUMBER_OF_LED  0x0040
#define CONFIG_OPERATION_DATA           0x0080
#define CONFIG_OPERATION_MIPS0_ACCESS   0x0100
#define CONFIG_DATA_CLOCK_EDGE          0x0200


/*
*  Data Type Used to Call ioctl
*/
struct led_config_param {
	unsigned long   operation_mask;         //  Select operations to be performed
	unsigned long   led;                    //  LED to change update source (LED or ADSL)
	unsigned long   source;                 //  Corresponding update source (LED or ADSL)
	unsigned long   blink_mask;             //  LEDs to set blink mode
	unsigned long   blink;                  //  Set to blink mode or normal mode
	unsigned long   update_clock;           //  Select the source of update clock
	unsigned long   fpid;                   //  If FPI is the source of update clock, set the divider
	//  else if GPT is the source, set the frequency
	unsigned long   store_mode;             //  Set clock mode or single pulse mode for store signal
	unsigned long   fpis;                   //  FPI is the source of shift clock, set the divider
	unsigned long   data_offset;            //  Set cycles to be inserted before data is transmitted
	unsigned long   number_of_enabled_led;  //  Total number of LED to be enabled
	unsigned long   data_mask;              //  LEDs to set value
	unsigned long   data;                   //  Corresponding value
	unsigned long   mips0_access_mask;      //  LEDs to set access right
	unsigned long   mips0_access;           //  1: the corresponding data is output from MIPS0, 0: MIPS1
	unsigned long   f_data_clock_on_rising; //  1: data clock on rising edge, 0: data clock on falling edge
};


extern int danube_led_set_blink(unsigned int, unsigned int);
extern int danube_led_set_data(unsigned int, unsigned int);
extern int danube_led_config(struct led_config_param *);

#define DATA_CLOCKING_EDGE              FALLING_EDGE
#define RISING_EDGE                     0
#define FALLING_EDGE                    1

#define LED_SH_PORT                     0
#define LED_SH_PIN                      4
#define LED_SH_DIR                      1
#define LED_SH_ALTSEL0                  1
#define LED_SH_ALTSEL1                  0
#define LED_SH_OPENDRAIN                1
#define LED_D_PORT                      0
#define LED_D_PIN                       5
#define LED_D_DIR                       1
#define LED_D_ALTSEL0                   1
#define LED_D_ALTSEL1                   0
#define LED_D_OPENDRAIN                 1
#define LED_ST_PORT                     0
#define LED_ST_PIN                      6
#define LED_ST_DIR                      1
#define LED_ST_ALTSEL0                  1
#define LED_ST_ALTSEL1                  0
#define LED_ST_OPENDRAIN                1

#define LED_ADSL0_PORT                  0
#define LED_ADSL0_PIN                   4
#define LED_ADSL0_DIR                   1
#define LED_ADSL0_ALTSEL0               0
#define LED_ADSL0_ALTSEL1               1
#define LED_ADSL0_OPENDRAIN             1
#define LED_ADSL1_PORT                  0
#define LED_ADSL1_PIN                   5
#define LED_ADSL1_DIR                   1
#define LED_ADSL1_ALTSEL0               1
#define LED_ADSL1_ALTSEL1               1
#define LED_ADSL1_OPENDRAIN             1

#if (LED_SH_PORT == LED_ADSL0_PORT && LED_SH_PIN == LED_ADSL0_PIN)      \
    || (LED_D_PORT == LED_ADSL0_PORT && LED_D_PIN == LED_ADSL0_PIN)     \
    || (LED_ST_PORT == LED_ADSL0_PORT && LED_ST_PIN == LED_ADSL0_PIN)   \
    || (LED_SH_PORT == LED_ADSL1_PORT && LED_SH_PIN == LED_ADSL1_PIN)   \
    || (LED_D_PORT == LED_ADSL1_PORT && LED_D_PIN == LED_ADSL1_PIN)     \
    || (LED_ST_PORT == LED_ADSL1_PORT && LED_ST_PIN == LED_ADSL1_PIN)
  #define ADSL_LED_IS_EXCLUSIVE         1
#else
  #define ADSL_LED_IS_EXCLUSIVE         0
#endif

#if LED_SH_DIR
  #define LED_SH_DIR_SETUP              danube_port_set_dir_out
#else
  #define LED_SH_DIR_SETUP              danube_port_clear_dir_out
#endif
#if LED_SH_ALTSEL0
  #define LED_SH_ALTSEL0_SETUP          danube_port_set_altsel0
#else
  #define LED_SH_ALTSEL0_SETUP          danube_port_clear_altsel0
#endif
#if LED_SH_ALTSEL1
  #define LED_SH_ALTSEL1_SETUP          danube_port_set_altsel1
#else
  #define LED_SH_ALTSEL1_SETUP          danube_port_clear_altsel1
#endif
#if LED_SH_OPENDRAIN
  #define LED_SH_OPENDRAIN_SETUP        danube_port_set_open_drain
#else
  #define LED_SH_OPENDRAIN_SETUP        danube_port_clear_open_drain
#endif

#if LED_D_DIR
  #define LED_D_DIR_SETUP               danube_port_set_dir_out
#else
  #define LED_D_DIR_SETUP               danube_port_clear_dir_out
#endif
#if LED_D_ALTSEL0
  #define LED_D_ALTSEL0_SETUP           danube_port_set_altsel0
#else
  #define LED_D_ALTSEL0_SETUP           danube_port_clear_altsel0
#endif
#if LED_D_ALTSEL1
  #define LED_D_ALTSEL1_SETUP           danube_port_set_altsel1
#else
  #define LED_D_ALTSEL1_SETUP           danube_port_clear_altsel1
#endif
#if LED_D_OPENDRAIN
  #define LED_D_OPENDRAIN_SETUP         danube_port_set_open_drain
#else
  #define LED_D_OPENDRAIN_SETUP         danube_port_clear_open_drain
#endif

#if LED_ST_DIR
  #define LED_ST_DIR_SETUP              danube_port_set_dir_out
#else
  #define LED_ST_DIR_SETUP              danube_port_clear_dir_out
#endif
#if LED_ST_ALTSEL0
  #define LED_ST_ALTSEL0_SETUP          danube_port_set_altsel0
#else
  #define LED_ST_ALTSEL0_SETUP          danube_port_clear_altsel0
#endif
#if LED_ST_ALTSEL1
  #define LED_ST_ALTSEL1_SETUP          danube_port_set_altsel1
#else
  #define LED_ST_ALTSEL1_SETUP          danube_port_clear_altsel1
#endif
#if LED_ST_OPENDRAIN
  #define LED_ST_OPENDRAIN_SETUP        danube_port_set_open_drain
#else
  #define LED_ST_OPENDRAIN_SETUP        danube_port_clear_open_drain
#endif

#if LED_ADSL0_DIR
  #define LED_ADSL0_DIR_SETUP           danube_port_set_dir_out
#else
  #define LED_ADSL0_DIR_SETUP           danube_port_clear_dir_out
#endif
#if LED_ADSL0_ALTSEL0
  #define LED_ADSL0_ALTSEL0_SETUP       danube_port_set_altsel0
#else
  #define LED_ADSL0_ALTSEL0_SETUP       danube_port_clear_altsel0
#endif
#if LED_ADSL0_ALTSEL1
  #define LED_ADSL0_ALTSEL1_SETUP       danube_port_set_altsel1
#else
  #define LED_ADSL0_ALTSEL1_SETUP       danube_port_clear_altsel1
#endif
#if LED_ADSL0_OPENDRAIN
  #define LED_ADSL0_OPENDRAIN_SETUP     danube_port_set_open_drain
#else
  #define LED_ADSL0_OPENDRAIN_SETUP     danube_port_clear_open_drain
#endif

#if LED_ADSL1_DIR
  #define LED_ADSL1_DIR_SETUP           danube_port_set_dir_out
#else
  #define LED_ADSL1_DIR_SETUP           danube_port_clear_dir_out
#endif
#if LED_ADSL1_ALTSEL0
  #define LED_ADSL1_ALTSEL0_SETUP       danube_port_set_altsel0
#else
  #define LED_ADSL1_ALTSEL0_SETUP       danube_port_clear_altsel0
#endif
#if LED_ADSL1_ALTSEL1
  #define LED_ADSL1_ALTSEL1_SETUP       danube_port_set_altsel1
#else
  #define LED_ADSL1_ALTSEL1_SETUP       danube_port_clear_altsel1
#endif
#if LED_ADSL1_OPENDRAIN
  #define LED_ADSL1_OPENDRAIN_SETUP     danube_port_set_open_drain
#else
  #define LED_ADSL1_OPENDRAIN_SETUP     danube_port_clear_open_drain
#endif

#define SET_BITS(x, msb, lsb, value)    (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

static struct semaphore led_sem;

static unsigned long gpt_on = 0;
static unsigned long gpt_freq = 0;

static unsigned long adsl_on = 0;
static unsigned long f_led_on = 0;

static inline int
update_led (void)
{
    int i, j;

    /*
     *  GPT2 or FPID is the clock to update LEDs automatically.
     */
    if (readl(DANUBE_LED_CON1) >> 30)
        return 0;

    /*
     *  Check the status to prevent conflict of two consecutive update
     */
    for ( i = 100000; i != 0; i -= j / 16 )
    {
        down(&led_sem);
        if (!(readl(DANUBE_LED_CON0) & LED_CON0_SWU))
        {
            *DANUBE_LED_CON0 |= 1 << 31;
            up(&led_sem);
            return 0;
        }
        else
            up(&led_sem);
        for ( j = 0; j < 1000 * 16; j++ );
    }

    return -EBUSY;
}

static inline unsigned int
set_update_source (unsigned int reg, unsigned long led, unsigned long source)
{
    return (reg & ~((led & 0x03) << 24)) | ((source & 0x03) << 24);
}

static inline unsigned int
set_blink_in_batch (unsigned int reg, unsigned long mask, unsigned long blink)
{
    return (reg & (~(mask & 0x00FFFFFF) & 0x87FFFFFF)) | (blink & 0x00FFFFFF);
}

static inline unsigned int
set_data_clock_edge (unsigned int reg, unsigned long f_on_rising_edge)
{
    return f_on_rising_edge ? (reg & ~(1 << 26)) : (reg | (1 << 26));
}

static inline unsigned int
set_update_clock (unsigned int reg, unsigned long clock, unsigned long fpid)
{
    switch ( clock )
    {
    case 0:
		reg &= ~0xC0000000;
		break;

    case 1:
		reg = (reg & ~0xC0000000) | 0x40000000;
		break;

    case 2:
		reg = (reg & ~0xCF800000) | 0x80000000 | ((fpid & 0x1F) << 23);
		break;
    }

	return reg;
}

static inline unsigned int
set_store_mode (unsigned int reg, unsigned long mode)
{
    return mode ? (reg | (1 << 28)) : (reg & ~(1 << 28));
}

static inline
unsigned int set_shift_clock (unsigned int reg, unsigned long fpis)
{
    return SET_BITS(reg, 21, 20, fpis);
}

static inline
unsigned int set_data_offset (unsigned int reg, unsigned long offset)
{
    return SET_BITS(reg, 19, 18, offset);
}

static inline
unsigned int set_number_of_enabled_led (unsigned int reg, unsigned long number)
{
    unsigned int bit_mask;

    bit_mask = number > 16 ? 0x07 : (number > 8 ? 0x03 : (number ? 0x01 : 0x00));
    return (reg & ~0x07) | bit_mask;
}

static inline unsigned int
set_data_in_batch (unsigned int reg, unsigned long mask, unsigned long data)
{
    return (reg & ~(mask & 0x00FFFFFF)) | (data & 0x00FFFFFF);
}

static inline unsigned int
set_access_right (unsigned int reg, unsigned long mask, unsigned long ar)
{
    return (reg & ~(mask & 0x00FFFFFF)) | (~ar & mask);
}

static inline void
enable_led (void)
{
    /*  Activate LED module in PMU. */
    int i = 1000000;

    writel(readl(DANUBE_PMU_PWDCR) & ~DANUBE_PMU_PWDCR_LED, DANUBE_PMU_PWDCR);
    while (--i && (readl(DANUBE_PMU_PWDSR) & DANUBE_PMU_PWDCR_LED)) {}

	if (!i)
        panic("Activating LED in PMU failed!");
}

static inline void
disable_led (void)
{
    writel(readl(DANUBE_PMU_PWDCR) | DANUBE_PMU_PWDCR_LED, DANUBE_PMU_PWDCR);
}

static inline void
release_gpio_port (unsigned long adsl)
{
    if ( adsl )
    {
        danube_port_free_pin(LED_ADSL0_PORT, LED_ADSL0_PIN);
        danube_port_free_pin(LED_ADSL1_PORT, LED_ADSL1_PIN);
    }
    else
    {
        danube_port_free_pin(LED_ST_PORT, LED_ST_PIN);
        danube_port_free_pin(LED_D_PORT, LED_D_PIN);
        danube_port_free_pin(LED_SH_PORT, LED_SH_PIN);
    }
}

static inline int
setup_gpio_port (unsigned long adsl)
{
    int ret = 0;

    /*
     *  Reserve all pins before config them.
     */
    if ( adsl )
    {
        ret |= danube_port_reserve_pin(LED_ADSL0_PORT, LED_ADSL0_PIN);
        ret |= danube_port_reserve_pin(LED_ADSL1_PORT, LED_ADSL1_PIN);
    }
    else
    {
        ret |= danube_port_reserve_pin(LED_ST_PORT, LED_ST_PIN);
        ret |= danube_port_reserve_pin(LED_D_PORT, LED_D_PIN);
        ret |= danube_port_reserve_pin(LED_SH_PORT, LED_SH_PIN);
    }
    if ( ret )
    {
        release_gpio_port(adsl);
        return ret; //  Should be -EBUSY
    }

    if ( adsl )
    {
        LED_ADSL0_ALTSEL0_SETUP(LED_ADSL0_PORT, LED_ADSL0_PIN);
        LED_ADSL0_ALTSEL1_SETUP(LED_ADSL0_PORT, LED_ADSL0_PIN);
        LED_ADSL0_DIR_SETUP(LED_ADSL0_PORT, LED_ADSL0_PIN);
        LED_ADSL0_OPENDRAIN_SETUP(LED_ADSL0_PORT, LED_ADSL0_PIN);

        LED_ADSL1_ALTSEL0_SETUP(LED_ADSL1_PORT, LED_ADSL1_PIN);
        LED_ADSL1_ALTSEL1_SETUP(LED_ADSL1_PORT, LED_ADSL1_PIN);
        LED_ADSL1_DIR_SETUP(LED_ADSL1_PORT, LED_ADSL1_PIN);
        LED_ADSL1_OPENDRAIN_SETUP(LED_ADSL1_PORT, LED_ADSL1_PIN);
    }
    else
    {
        /*
         *  Set LED_ST
         *    I don't check the return value, because I'm sure the value is valid
         *    and the pins are reserved already.
         */
        LED_ST_ALTSEL0_SETUP(LED_ST_PORT, LED_ST_PIN);
        LED_ST_ALTSEL1_SETUP(LED_ST_PORT, LED_ST_PIN);
        LED_ST_DIR_SETUP(LED_ST_PORT, LED_ST_PIN);
        LED_ST_OPENDRAIN_SETUP(LED_ST_PORT, LED_ST_PIN);

        /*
         *  Set LED_D
         */
        LED_D_ALTSEL0_SETUP(LED_D_PORT, LED_D_PIN);
        LED_D_ALTSEL1_SETUP(LED_D_PORT, LED_D_PIN);
        LED_D_DIR_SETUP(LED_D_PORT, LED_D_PIN);
        LED_D_OPENDRAIN_SETUP(LED_D_PORT, LED_D_PIN);

        /*
         *  Set LED_SH
         */
        LED_SH_ALTSEL0_SETUP(LED_SH_PORT, LED_SH_PIN);
        LED_SH_ALTSEL1_SETUP(LED_SH_PORT, LED_SH_PIN);
        LED_SH_DIR_SETUP(LED_SH_PORT, LED_SH_PIN);
        LED_SH_OPENDRAIN_SETUP(LED_SH_PORT, LED_SH_PIN);
    }

    return 0;
}

static inline int
setup_gpt (int timer, unsigned long freq)
{
    int ret;

    timer = TIMER(timer, 1);

    ret  = request_timer(timer,
                           TIMER_FLAG_SYNC
                         | TIMER_FLAG_16BIT
                         | TIMER_FLAG_INT_SRC
                         | TIMER_FLAG_CYCLIC | TIMER_FLAG_COUNTER | TIMER_FLAG_DOWN
                         | TIMER_FLAG_ANY_EDGE
                         | TIMER_FLAG_NO_HANDLE,
                         8000000 / freq,
                         0,
                         0);

    if ( !ret )
    {
        ret = start_timer(timer, 0);
        if ( ret )
            free_timer(timer);
    }

    return ret;
}

static inline void
release_gpt (int timer)
{
    timer = TIMER(timer, 1);
    stop_timer(timer);
    free_timer(timer);
}

static inline int
turn_on_led (unsigned long adsl)
{
    int ret;

    ret = setup_gpio_port(adsl);
    if ( ret )
        return ret;

    enable_led();

    return 0;
}

static inline void
turn_off_led (unsigned long adsl)
{
    release_gpio_port(adsl);
    disable_led();
}


int
danube_led_set_blink (unsigned int led, unsigned int blink)
{
    unsigned int bit_mask;

    if ( led > 23 )
        return -EINVAL;

    bit_mask = 1 << led;
    down(&led_sem);
    if ( blink )
        *DANUBE_LED_CON0 |= bit_mask;
    else
        *DANUBE_LED_CON0 &= ~bit_mask;
    up(&led_sem);

    return (led == 0 && (readl(DANUBE_LED_CON0) & LED_CON0_AD0)) || (led == 1 && (readl(DANUBE_LED_CON0) & LED_CON0_AD1)) ? -EINVAL : 0;
}

int
danube_led_set_data (unsigned int led, unsigned int data)
{
    unsigned long f_update;
    unsigned int bit_mask;

    if ( led > 23 )
        return -EINVAL;

    bit_mask = 1 << led;
    down(&led_sem);
    if ( data )
        *DANUBE_LED_CPU0 |= bit_mask;
    else
        *DANUBE_LED_CPU0 &= ~bit_mask;
    f_update = !(*DANUBE_LED_AR & bit_mask);
    up(&led_sem);

    return f_update ? update_led() : 0;
}

int
danube_led_config (struct led_config_param* param)
{
    int ret;
    unsigned int reg_con0, reg_con1, reg_cpu0, reg_ar;
    unsigned int clean_reg_con0, clean_reg_con1, clean_reg_cpu0, clean_reg_ar;
    unsigned int f_setup_gpt2;
    unsigned int f_software_update;
    unsigned int new_led_on, new_adsl_on;

    if ( !param )
        return -EINVAL;

    down(&led_sem);

    reg_con0 = *DANUBE_LED_CON0;
    reg_con1 = *DANUBE_LED_CON1;
    reg_cpu0 = *DANUBE_LED_CPU0;
    reg_ar   = *DANUBE_LED_AR;

    clean_reg_con0 = 1;
    clean_reg_con1 = 1;
    clean_reg_cpu0 = 1;
    clean_reg_ar   = 1;

    f_setup_gpt2 = 0;

    f_software_update = (readl(DANUBE_LED_CON0) & LED_CON0_SWU) ? 0 : 1;

    new_led_on = f_led_on;
    new_adsl_on = adsl_on;

    /*  ADSL or LED */
    if ( (param->operation_mask & CONFIG_OPERATION_UPDATE_SOURCE) )
    {
        if ( param->led > 0x03 || param->source > 0x03 )
            goto INVALID_PARAM;
        clean_reg_con0 = 0;
        reg_con0 = set_update_source(reg_con0, param->led, param->source);
#if 0   //  ADSL0,1 is source for bit 0, 1 in shift register
        new_adsl_on = param->source;
#endif
    }

    /*  Blink   */
    if ( (param->operation_mask & CONFIG_OPERATION_BLINK) )
    {
        if ( (param->blink_mask & 0xFF000000) || (param->blink & 0xFF000000) )
            goto INVALID_PARAM;
        clean_reg_con0 = 0;
        reg_con0 = set_blink_in_batch(reg_con0, param->blink_mask, param->blink);
    }

    /*  Edge    */
    if ( (param->operation_mask & CONFIG_DATA_CLOCK_EDGE) )
    {
        clean_reg_con0 = 0;
        reg_con0 = set_data_clock_edge(reg_con0, param->f_data_clock_on_rising);
    }

    /*  Update Clock    */
    if ( (param->operation_mask & CONFIG_OPERATION_UPDATE_CLOCK) )
    {
        if ( param->update_clock > 0x02 || (param->update_clock == 0x02 && param->fpid > 0x3) )
            goto INVALID_PARAM;
        clean_reg_con1 = 0;
        f_software_update = param->update_clock == 0 ? 1 : 0;
        if ( param->update_clock == 0x01 )
            f_setup_gpt2 = 1;
        reg_con1 = set_update_clock(reg_con1, param->update_clock, param->fpid);
    }

    /*  Store Mode  */
    if ( (param->operation_mask & CONFIG_OPERATION_STORE_MODE) )
    {
        clean_reg_con1 = 0;
        reg_con1 = set_store_mode(reg_con1, param->store_mode);
    }

    /*  Shift Clock */
    if ( (param->operation_mask & CONFIG_OPERATION_SHIFT_CLOCK) )
    {
        if ( param->fpis > 0x03 )
            goto INVALID_PARAM;
        clean_reg_con1 = 0;
        reg_con1 = set_shift_clock(reg_con1, param->fpis);
    }

    /*  Data Offset */
    if ( (param->operation_mask & CONFIG_OPERATION_DATA_OFFSET) )
    {
        if ( param->data_offset > 0x03 )
            goto INVALID_PARAM;
        clean_reg_con1 = 0;
        reg_con1 = set_data_offset(reg_con1, param->data_offset);
    }

    /*  Number of LED   */
    if ( (param->operation_mask & CONFIG_OPERATION_NUMBER_OF_LED) )
    {
        if ( param->number_of_enabled_led > 0x24 )
            goto INVALID_PARAM;

        /*
         *  If there is at lease one LED enabled, the GPIO pin must be setup.
         */
        new_led_on = param->number_of_enabled_led ? 1 : 0;

        clean_reg_con1 = 0;
        reg_con1 = set_number_of_enabled_led(reg_con1, param->number_of_enabled_led);
    }

    /*  LED Data    */
    if ( (param->operation_mask & CONFIG_OPERATION_DATA) )
    {
        if ( (param->data_mask & 0xFF000000) || (param->data & 0xFF000000) )
            goto INVALID_PARAM;
        clean_reg_cpu0 = 0;
        reg_cpu0 = set_data_in_batch(reg_cpu0, param->data_mask, param->data);
        if ( f_software_update )
        {
            clean_reg_con0 = 0;
            reg_con0 |= 0x80000000;
        }
    }

    /*  Access Right    */
    if ( (param->operation_mask & CONFIG_OPERATION_MIPS0_ACCESS) )
    {
        if ( (param->mips0_access_mask & 0xFF000000) || (param->mips0_access & 0xFF000000) )
            goto INVALID_PARAM;
        clean_reg_ar = 0;
        reg_ar = set_access_right(reg_ar, param->mips0_access_mask, param->mips0_access);
    }

    /*  Setup GPT   */
    if ( f_setup_gpt2 && !new_adsl_on )     //  If ADSL led is on, GPT is disabled.
    {
        ret = 0;

        if ( gpt_on )
        {
            if ( gpt_freq != param->fpid )
            {
                release_gpt(2);
                gpt_on = 0;
                ret = setup_gpt(2, param->fpid);
            }
        }
        else
            ret = setup_gpt(2, param->fpid);

        if ( ret )
        {
#if 1
            printk("Setup GPT error!\n");
#endif
            goto SETUP_GPT_ERROR;
        }
        else
        {
#if 0
            printk("Setup GPT successfully!\n");
#endif
            gpt_on = 1;
        }
    }
    else
        if ( gpt_on )
        {
            release_gpt(2);
            gpt_on = 0;
        }

    /*  Turn on LED */
    if ( new_adsl_on )
        new_led_on = 1;
    if ( !new_led_on || adsl_on != new_adsl_on )
    {
        turn_off_led(adsl_on);
        f_led_on = 0;
        adsl_on = 0;
    }
    if ( !f_led_on && new_led_on )
    {
        ret = turn_on_led(new_adsl_on);
        if ( ret )
        {
            printk("Setup GPIO error!\n");
            goto SETUP_GPIO_ERROR;
        }
        adsl_on = new_adsl_on;
        f_led_on = 1;
    }

    /*  Write Register  */
    if ( !f_led_on )
        enable_led();
    if ( !clean_reg_ar )
        *DANUBE_LED_AR   = reg_ar;
    if ( !clean_reg_cpu0 )
        *DANUBE_LED_CPU0 = reg_cpu0;
    if ( !clean_reg_con1 )
        *DANUBE_LED_CON1 = reg_con1;
    if ( !clean_reg_con0 )
        *DANUBE_LED_CON0 = reg_con0;
    if ( !f_led_on )
        disable_led();

    up(&led_sem);
    return 0;

SETUP_GPIO_ERROR:
    release_gpt(2);
    gpt_on = 0;
SETUP_GPT_ERROR:
    up(&led_sem);
    return ret;

INVALID_PARAM:
    up(&led_sem);
    return -EINVAL;
}

static int
led_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = -EINVAL;
    struct led_config_param param;

    switch ( cmd )
    {
    case LED_CONFIG:
        copy_from_user(&param, (char*)arg, sizeof(param));
        ret = danube_led_config(&param);
        break;
    }

    return ret;
}

static int
led_open (struct inode *inode, struct file *file)
{
    return 0;
}

static int
led_release (struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations led_fops = {
    owner:      THIS_MODULE,
    ioctl:      led_ioctl,
    open:       led_open,
    release:    led_release
};

static struct miscdevice led_miscdev = {
    151,
    "led",
    &led_fops,
    NULL,
    NULL,
    NULL
};

int __init
danube_led_init (void)
{
    int ret = 0;
    struct led_config_param param = {0};

    enable_led();

    writel(0, DANUBE_LED_AR);
    writel(0, DANUBE_LED_CPU0);
    writel(0, DANUBE_LED_CPU1);
    writel(0, DANUBE_LED_CON1);
    writel((0x80000000 | (DATA_CLOCKING_EDGE << 26)), DANUBE_LED_CON0);

    disable_led();

    sema_init(&led_sem, 0);

    ret = misc_register(&led_miscdev);
    if (ret == -EBUSY)
    {
        led_miscdev.minor = MISC_DYNAMIC_MINOR;
        ret = misc_register(&led_miscdev);
    }

	if (ret)
    {
        printk(KERN_ERR "led: can't misc_register\n");
        goto out;
    } else {
        printk(KERN_INFO "led: misc_register on minor = %d\n", led_miscdev.minor);
	}

    up(&led_sem);

    /*  Add to enable hardware relay    */
        /*  Map for LED on reference board
              WLAN_READ     LED11   OUT1    15
              WARNING       LED12   OUT2    14
              FXS1_LINK     LED13   OUT3    13
              FXS2_LINK     LED14   OUT4    12
              FXO_ACT       LED15   OUT5    11
              USB_LINK      LED16   OUT6    10
              ADSL2_LINK    LED19   OUT7    9
              BT_LINK       LED17   OUT8    8
              SD_LINK       LED20   OUT9    7
              ADSL2_TRAFFIC LED31   OUT16   0
            Map for hardware relay on reference board
              USB Power On          OUT11   5
              RELAY                 OUT12   4
        */
    param.operation_mask = CONFIG_OPERATION_NUMBER_OF_LED;
    param.number_of_enabled_led = 16;
    danube_led_config(&param);
    param.operation_mask = CONFIG_OPERATION_DATA;
    param.data_mask = 1 << 4;
    param.data = 1 << 4;
    danube_led_config(&param);

    //  by default, update by FSC clock (FPID)
    param.operation_mask = CONFIG_OPERATION_UPDATE_CLOCK;
    param.update_clock   = 2;   //  FPID
    param.fpid           = 3;   //  10Hz
    danube_led_config(&param);

    //  source of LED 0, 1 is ADSL
    param.operation_mask = CONFIG_OPERATION_UPDATE_SOURCE;
    param.led            = 3;   //  LED 0, 1
    param.source         = 3;   //  ADSL
    danube_led_config(&param);

    //  turn on USB
    param.operation_mask = CONFIG_OPERATION_DATA;
    param.data_mask = 1 << 5;
    param.data = 1 << 5;
    danube_led_config(&param);

out:
    return ret;
}

void __exit
danube_led_exit (void)
{
    int ret;

    ret = misc_deregister(&led_miscdev);
    if ( ret )
        printk(KERN_ERR "led: can't misc_deregister, get error number %d\n", -ret);
    else
        printk(KERN_INFO "led: misc_deregister successfully\n");
}

EXPORT_SYMBOL(danube_led_set_blink);
EXPORT_SYMBOL(danube_led_set_data);
EXPORT_SYMBOL(danube_led_config);

module_init(danube_led_init);
module_exit(danube_led_exit);

