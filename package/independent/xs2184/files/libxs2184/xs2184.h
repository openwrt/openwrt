#ifndef _XS2184_H_
#define _XS2184_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <linux/types.h>
#include "./i2cbusses.h"
#include "./i2c-dev.h"


#define  BUS_NUM    0
#define  XS2184_1_IIC_ADDR   0x20
#define  XS2184_2_IIC_ADDR   0x28

#define  POWER_STA_REG		0x10     //供电状态寄存器	  	R,	PGOOD4-1||PWR_EN4-1
#define  PSE_ID_REG             0x1b     //芯片ID寄存器			R,	1101-0000
#define  DETECT_CLASS_EN_REG    0x14     //检测分级使能寄存器	 R/W,CLASS_EN4-1||DET_EN4
#define  POWER_EN_REG           0x19     //端口供电使能寄存器	 W，	PWR_OFF4-1||PWR_ON4-1

#define  XS2184_ID_VAL      	0xd0

#define  PORT_CRT_LSB_COMMON 0x30
#define  PORT_CRT_MSB_COMMON 0x31
#define  PORT_VLT_LSB_COMMON 0x32
#define  PORT_VLT_MSB_COMMON 0x33

//Address 0x14 检测/分级使能
// #define CLASS_EN4           ((u8)0x80)
// #define CLASS_EN3           ((u8)0x40)
// #define CLASS_EN2           ((u8)0x20)
// #define CLASS_EN1           ((u8)0x10)
// #define DET_EN4             ((u8)0x08)
// #define DET_EN3             ((u8)0x04)
// #define DET_EN2             ((u8)0x02)
// #define DET_EN1             ((u8)0x01)
//Address 0x19 电源使能按钮
// #define PWR_OFF4            ((u8)0x80)
// #define PWR_OFF3            ((u8)0x40)
// #define PWR_OFF2            ((u8)0x20)
// #define PWR_OFF1            ((u8)0x10)
// #define PWR_ON4             ((u8)0x08)
// #define PWR_ON3             ((u8)0x04)
// #define PWR_ON2             ((u8)0x02)
// #define PWR_ON1             ((u8)0x01)

/**
 * 	p	1	2	3	4	5	6	7	8
 *	b 	3	2	1	0	3	2	1	0
 */

#define PORT_TO_F(pn)		((pn + 3) % 4)
#define CLASS_EN(vp)		((u8)0x1 << (PORT_TO_F(vp) + 4))
#define DET_EN(vp)		((u8)0x1 << (PORT_TO_F(vp)))
#define PWR_OFF(vp)		((u8)0x1 << (PORT_TO_F(vp) + 4))
#define PWR_ON(vp)		((u8)0x1 << (PORT_TO_F(vp)))
#define PORT_MASK(pn)		((u8)0x1 << (pn - 1))

#define VOLT_MSB(vp)		(PORT_VLT_MSB_COMMON + 4 * ((vp - 1) % 4))
#define VOLT_LSB(vp)		(PORT_VLT_LSB_COMMON + 4 * ((vp - 1) % 4))
#define CURT_MSB(vp)		(PORT_CRT_MSB_COMMON + 4 * ((vp - 1) % 4))
#define CURT_LSB(vp)		(PORT_CRT_LSB_COMMON + 4 * ((vp - 1) % 4))

#define  CURRENT_PARA     122070	//uA
#define  VOLTAGE_PARA     5835		//mV

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

typedef unsigned char   u8;

int open_chip(u8 file, u8 chip_addr);
int chip_found( u8 chip_addr);
int enable_port(char port_num);
int disable_port(char port_num);

#endif /* _XS2184_H_ */
