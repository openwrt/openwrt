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
   Module      : ifx_swdrv.h
   Date        : 2004-09-01
   Description : JoeLin
   Remarks:

 *****************************************************************************/

#ifndef _ADM_6996_MODULE_H_
#define _ADM_6996_MODULE_H_

#include <asm/amazon/amazon.h>

#define ifx_printf(x)       printk x

/* command codes */
#define ADM_SW_SMI_READ     0x02
#define ADM_SW_SMI_WRITE    0x01
#define ADM_SW_SMI_START    0x01

#define ADM_SW_EEPROM_WRITE     0x01
#define ADM_SW_EEPROM_WRITE_ENABLE  0x03
#define ADM_SW_EEPROM_WRITE_DISABLE 0x00
#define EEPROM_TYPE     8   /* for 93C66 */

/* bit masks */
#define ADM_SW_BIT_MASK_1   0x00000001
#define ADM_SW_BIT_MASK_2   0x00000002
#define ADM_SW_BIT_MASK_4   0x00000008
#define ADM_SW_BIT_MASK_10  0x00000200
#define ADM_SW_BIT_MASK_16  0x00008000
#define ADM_SW_BIT_MASK_32  0x80000000

/* delay timers */
#define ADM_SW_MDC_DOWN_DELAY   5
#define ADM_SW_MDC_UP_DELAY 5
#define ADM_SW_CS_DELAY     5

/* MDIO modes */
#define ADM_SW_MDIO_OUTPUT  1
#define ADM_SW_MDIO_INPUT   0

#define ADM_SW_MAX_PORT_NUM 5
#define ADM_SW_MAX_VLAN_NUM 15

/* registers */
#define ADM_SW_PORT0_CONF   0x1
#define ADM_SW_PORT1_CONF   0x3
#define ADM_SW_PORT2_CONF   0x5
#define ADM_SW_PORT3_CONF   0x7
#define ADM_SW_PORT4_CONF   0x8
#define ADM_SW_PORT5_CONF   0x9
#define ADM_SW_VLAN_MODE    0x11
#define ADM_SW_MAC_LOCK     0x12
#define ADM_SW_VLAN0_CONF   0x13
#define ADM_SW_PORT0_PVID   0x28
#define ADM_SW_PORT1_PVID   0x29
#define ADM_SW_PORT2_PVID   0x2a
#define ADM_SW_PORT34_PVID  0x2b
#define ADM_SW_PORT5_PVID   0x2c
#define ADM_SW_PHY_RESET    0x2f
#define ADM_SW_MISC_CONF    0x30
#define ADM_SW_BNDWDH_CTL0  0x31
#define ADM_SW_BNDWDH_CTL1  0x32
#define ADM_SW_BNDWDH_CTL_ENA   0x33

/* port modes */
#define ADM_SW_PORT_FLOWCTL 0x1 /* 802.3x flow control */
#define ADM_SW_PORT_AN      0x2 /* auto negotiation */
#define ADM_SW_PORT_100M    0x4 /* 100M */
#define ADM_SW_PORT_FULL    0x8 /* full duplex */
#define ADM_SW_PORT_TAG     0x10    /* output tag on */
#define ADM_SW_PORT_DISABLE 0x20    /* disable port */
#define ADM_SW_PORT_TOS     0x40    /* TOS first */
#define ADM_SW_PORT_PPRI    0x80    /* port based priority first */
#define ADM_SW_PORT_MDIX    0x8000  /* auto MDIX on */
#define ADM_SW_PORT_PVID_SHIFT  10
#define ADM_SW_PORT_PVID_BITS   4

/* VLAN */
#define ADM_SW_VLAN_PORT0   0x1
#define ADM_SW_VLAN_PORT1   0x2
#define ADM_SW_VLAN_PORT2   0x10
#define ADM_SW_VLAN_PORT3   0x40
#define ADM_SW_VLAN_PORT4   0x80
#define ADM_SW_VLAN_PORT5   0x100


/* GPIO 012 enabled, output mode */
#define GPIO_ENABLEBITS 0x000700f8

/*
  define AMAZON GPIO port to ADM6996 EEPROM interface
  MDIO ->   EEDI        GPIO 16, AMAZON GPIO P1.0, bi-direction
  MDC ->    EESK        GPIO 17, AMAZON GPIO P1.1, output only
  MDCS ->   EECS        GPIO 18, AMAZON GPIO P1.2, output only
            EEDO        GPIO 15, AMAZON GPIO P0.15, do not need this one! */

#define GPIO_MDIO   1		//P1.0
#define GPIO_MDC    2		//P1.1
#define GPIO_MDCS   4		//P1.2

//joelin #define GPIO_MDIO   0
//joelin #define GPIO_MDC    5   /* PORT 0 GPIO5 */
//joelin #define GPIO_MDCS   6   /* PORT 0 GPIO6 */


#define MDIO_INPUT  0x00000001
#define MDIO_OUTPUT_EN  0x00010000


/* type definitions */
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;

typedef struct _REGRW_
{
  unsigned int addr;
  unsigned int value;
  unsigned int mode;
}REGRW, *PREGRW;

//joelin adm6996i
typedef struct _MACENTRY_
{
	unsigned char mac_addr[6];
	unsigned long fid:4;
            unsigned long portmap:6;
	union  {
		unsigned long age_timer:9;
		unsigned long info_ctrl:9;
	} ctrl;
	unsigned long occupy:1;
	unsigned long info_type:1;
	unsigned long bad:1;
	unsigned long result:3;//000:command ok ,001:all entry used,010:Entry Not found  ,011:try next entry ,101:command error
	
 }MACENTRY, *PMACENTRY;
typedef struct _PROTOCOLFILTER_
{
	int protocol_filter_num;//[0~7]
	int ip_p; //Value Compared with Protocol in IP Heade[7:0]
	char action:2;//Action for protocol Filter .
//00 = Protocol Portmap is Default Output Ports.
//01 = Protocol Portmap is 6'b0.
//10 = Protocol Portmap is the CPU port if the incoming port 
//is not the CPU port. But if the incoming port is the CPU port, then Type Portmap contains Default Output Ports, excluding the CPU port.
 }PROTOCOLFILTER, *PPROTOCOLFILTER;

//joelin adm6996i

/* Santosh: for IGMP proxy/snooping */

//050614:fchang int adm_process_mac_table_request (unsigned int cmd, struct _MACENTRY_ *mac);
//050614:fchang int adm_process_protocol_filter_request (unsigned int cmd, struct _PROTOCOLFILTER_ *filter);


/* IOCTL keys */
#define KEY_IOCTL_ADM_REGRW     	0x01
#define KEY_IOCTL_ADM_SW_REGRW      	0x02
#define KEY_IOCTL_ADM_SW_PORTSTS    	0x03
#define KEY_IOCTL_ADM_SW_INIT       	0x04
//for adm6996i-start
#define	KEY_IOCTL_ADM_SW_IOCTL_MACENTRY_ADD		0x05
#define	KEY_IOCTL_ADM_SW_IOCTL_MACENTRY_DEL		0x06
#define	KEY_IOCTL_ADM_SW_IOCTL_MACENTRY_GET_INIT	0x07
#define	KEY_IOCTL_ADM_SW_IOCTL_MACENTRY_GET_MORE	0x08
#define	KEY_IOCTL_ADM_SW_IOCTL_FILTER_ADD		0x09
#define	KEY_IOCTL_ADM_SW_IOCTL_FILTER_DEL		0x0a
#define	KEY_IOCTL_ADM_SW_IOCTL_FILTER_GET		0x0b
 
//adm6996i #define KEY_IOCTL_MAX_KEY       0x05
#define KEY_IOCTL_MAX_KEY       0x0c
//for adm6996i-end
/* IOCTL MAGIC */
#define ADM_MAGIC  ('a'|'d'|'m'|'t'|'e'|'k')

/* IOCTL parameters */
#define ADM_IOCTL_REGRW         _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_REGRW, REGRW)
#define ADM_SW_IOCTL_REGRW      _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_REGRW, REGRW)
#define ADM_SW_IOCTL_PORTSTS    _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_PORTSTS, NULL)
#define ADM_SW_IOCTL_INIT       _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_INIT, NULL)


//6996i-stat
#define ADM_SW_IOCTL_MACENTRY_ADD	    _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_IOCTL_MACENTRY_ADD,MACENTRY)
#define ADM_SW_IOCTL_MACENTRY_DEL	    _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_IOCTL_MACENTRY_DEL,MACENTRY)
#define ADM_SW_IOCTL_MACENTRY_GET_INIT      _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_IOCTL_MACENTRY_GET_INIT,MACENTRY)
#define ADM_SW_IOCTL_MACENTRY_GET_MORE      _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_IOCTL_MACENTRY_GET_MORE,MACENTRY)
#define ADM_SW_IOCTL_FILTER_ADD	    	    _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_IOCTL_FILTER_ADD,PROTOCOLFILTER)	
#define ADM_SW_IOCTL_FILTER_DEL	            _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_IOCTL_FILTER_DEL,PROTOCOLFILTER)
#define ADM_SW_IOCTL_FILTER_GET	            _IOWR(ADM_MAGIC, KEY_IOCTL_ADM_SW_IOCTL_FILTER_GET,PROTOCOLFILTER)	
 
//6996i-end


#define REG_READ    0x0
#define REG_WRITE   0x1

/* undefine symbol */
#define AMAZON_SW_REG(reg)  *((volatile U32*)(reg))
//#define GPIO0_INPUT_MASK    0
//#define GPIO_conf0_REG      0x12345678
//#define GPIO_SET_HI
//#define GPIO_SET_LOW

#endif
/* _ADM_6996_MODULE_H_ */
