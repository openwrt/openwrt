/*
 * Copyright (C) 2008 Stanley Pinchak <stanley_dot_pinchak_at_gmail_dot_com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef __AR7_TITAN_H__
#define __AR7_TITAN_H__

#ifndef __AR7_GPIO_H__
#include <asm/ar7/gpio.h>
#endif

typedef enum TITAN_GPIO_PIN_MODE_tag
{
    FUNCTIONAL_PIN = 0,
    GPIO_PIN = 1
} TITAN_GPIO_PIN_MODE_T;

typedef enum TITAN_GPIO_PIN_DIRECTION_tag
{
    GPIO_OUTPUT_PIN = 0,
    GPIO_INPUT_PIN = 1
} TITAN_GPIO_PIN_DIRECTION_T;

/**********************************************************************
 *  GPIO Control
 **********************************************************************/

typedef struct 
{
    int pinSelReg;
    int shift;
    int func;

} GPIO_CFG;

static GPIO_CFG gptable[]= {
		      /* PIN_SEL_REG, START_BIT, GPIO_CFG_MUX_VALUE */
	              {4,24,1},
		      {4,26,1},
		      {4,28,1},
		      {4,30,1},
		      {5,6,1},
		      {5,8,1},
		      {5,10,1},
		      {5,12,1},
		      {7,14,3},
		      {7,16,3},
		      {7,18,3},
		      {7,20,3},
		      {7,22,3},
		      {7,26,3},
		      {7,28,3},
		      {7,30,3},
                      {8,0,3},
		      {8,2,3},
		      {8,4,3},
		      {8,10,3},
		      {8,14,3},
		      {8,16,3},
		      {8,18,3},
		      {8,20,3},
		      {9,8,3},
		      {9,10,3},
		      {9,12,3},
		      {9,14,3},
		      {9,18,3},
		      {9,20,3},
		      {9,24,3},
		      {9,26,3},
		      {9,28,3},
		      {9,30,3},
		      {10,0,3},
		      {10,2,3},
		      {10,8,3},
		      {10,10,3},
		      {10,12,3},
		      {10,14,3},
		      {13,12,3},
		      {13,14,3},
		      {13,16,3},
		      {13,18,3},
		      {13,24,3},
		      {13,26,3},
		      {13,28,3},
		      {13,30,3},
		      {14,2,3},
		      {14,6,3},
		      {14,8,3},
		      {14,12,3}
};

typedef struct
{
    volatile unsigned int reg[21];
}
PIN_SEL_REG_ARRAY_T;

typedef struct
{
    unsigned int data_in [2];
    unsigned int data_out[2];
    unsigned int dir[2];
    unsigned int enable[2];

} TITAN_GPIO_CONTROL_T;

#define AVALANCHE_PIN_SEL_BASE        0xA861160C /*replace with KSEG1ADDR()*/

static inline int titan_gpio_ctrl(unsigned int gpio_pin, TITAN_GPIO_PIN_MODE_T pin_mode,
                        TITAN_GPIO_PIN_DIRECTION_T pin_direction)
{
    int reg_index = 0;
    int mux_status;
    GPIO_CFG  gpio_cfg;
    volatile PIN_SEL_REG_ARRAY_T *pin_sel_array = (PIN_SEL_REG_ARRAY_T*) AVALANCHE_PIN_SEL_BASE;
    volatile TITAN_GPIO_CONTROL_T   *gpio_cntl     = (TITAN_GPIO_CONTROL_T*) KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_INPUT_0);
	
    if (gpio_pin > 51 )
        return(-1);

    gpio_cfg = gptable[gpio_pin];
    mux_status = (pin_sel_array->reg[gpio_cfg.pinSelReg - 1] >> gpio_cfg.shift) & 0x3;
    if(!((mux_status == 0 /* tri-stated */ ) || (mux_status == gpio_cfg.func /*GPIO functionality*/)))
    {
        return(-1); /* Pin have been configured for non GPIO funcs. */
    }

    /* Set the pin to be used as GPIO. */
    pin_sel_array->reg[gpio_cfg.pinSelReg - 1] |= ((gpio_cfg.func & 0x3) << gpio_cfg.shift);

    /* Check whether gpio refers to the first GPIO reg or second. */
    if(gpio_pin > 31)
    {
	reg_index = 1;
	gpio_pin -= 32;
    }

    if(pin_mode)
        gpio_cntl->enable[reg_index] |=  (1 << gpio_pin); /* Enable */
    else
	gpio_cntl->enable[reg_index] &= ~(1 << gpio_pin);

    if(pin_direction)
        gpio_cntl->dir[reg_index] |=  (1 << gpio_pin); /* Input */
    else
	gpio_cntl->dir[reg_index] &= ~(1 << gpio_pin);

    return(0);

}/* end of function titan_gpio_ctrl */

static inline int titan_sysGpioInValue(unsigned int *in_val, unsigned int reg_index)
{
    volatile TITAN_GPIO_CONTROL_T   *gpio_cntl     = (TITAN_GPIO_CONTROL_T*) KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_INPUT_0);

    if(reg_index > 1)
       return (-1);

    *in_val = gpio_cntl->data_in[reg_index];

    return (0);
}


#endif
