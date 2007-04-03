/*
<:copyright-gpl 

 Copyright 2003 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 

:>
*/
/**************************************************************************
 * File Name  : boardparms.h
 *
 * Description: This file contains definitions and function prototypes for
 *              the BCM63xx board parameter access functions.
 * 
 * Updates    : 07/14/2003  Created.
 ***************************************************************************/

#if !defined(_BOARDPARMS_H)
#define _BOARDPARMS_H

/* Return codes. */
#define BP_SUCCESS                              0
#define BP_BOARD_ID_NOT_FOUND                   1
#define BP_VALUE_NOT_DEFINED                    2
#define BP_BOARD_ID_NOT_SET                     3

/* Values for BpGetSdramSize. */
#define BP_MEMORY_8MB_1_CHIP                    0
#define BP_MEMORY_16MB_1_CHIP                   1
#define BP_MEMORY_32MB_1_CHIP                   2
#define BP_MEMORY_64MB_2_CHIP                   3
#define BP_MEMORY_32MB_2_CHIP                   4
#define BP_MEMORY_16MB_2_CHIP                   5

/* Values for EthernetMacInfo PhyType. */
#define BP_ENET_NO_PHY                          0
#define BP_ENET_INTERNAL_PHY                    1
#define BP_ENET_EXTERNAL_PHY                    2
#define BP_ENET_EXTERNAL_SWITCH                 3

/* Values for EthernetMacInfo Configuration type. */
#define BP_ENET_CONFIG_MDIO                     0       /* Internal PHY, External PHY, Switch+(no GPIO, no SPI, no MDIO Pseudo phy */
#define BP_ENET_CONFIG_GPIO                     1       /* Bcm96345GW board + Bcm5325M/E */
#define BP_ENET_CONFIG_MDIO_PSEUDO_PHY          2       /* Bcm96348GW board + Bcm5325E */
#define BP_ENET_CONFIG_SPI_SSB_0                3       /* Bcm96348GW board + Bcm5325M/E */
#define BP_ENET_CONFIG_SPI_SSB_1                4       /* Bcm96348GW board + Bcm5325M/E */
#define BP_ENET_CONFIG_SPI_SSB_2                5       /* Bcm96348GW board + Bcm5325M/E */
#define BP_ENET_CONFIG_SPI_SSB_3                6       /* Bcm96348GW board + Bcm5325M/E */

/* Values for EthernetMacInfo Reverse MII. */
#define BP_ENET_NO_REVERSE_MII                  0
#define BP_ENET_REVERSE_MII                     1

/* Values for VoIPDSPInfo DSPType. */
#define BP_VOIP_NO_DSP                          0
#define BP_VOIP_DSP                             1


/* Values for GPIO pin assignments (AH = Active High, AL = Active Low). */
#define BP_ACTIVE_MASK                          0x8000
#define BP_ACTIVE_HIGH                          0x0000
#define BP_ACTIVE_LOW                           0x8000
#define BP_GPIO_0_AH                            (0  | BP_ACTIVE_HIGH)
#define BP_GPIO_0_AL                            (0  | BP_ACTIVE_LOW)
#define BP_GPIO_1_AH                            (1  | BP_ACTIVE_HIGH)
#define BP_GPIO_1_AL                            (1  | BP_ACTIVE_LOW)
#define BP_GPIO_2_AH                            (2  | BP_ACTIVE_HIGH)
#define BP_GPIO_2_AL                            (2  | BP_ACTIVE_LOW)
#define BP_GPIO_3_AH                            (3  | BP_ACTIVE_HIGH)
#define BP_GPIO_3_AL                            (3  | BP_ACTIVE_LOW)
#define BP_GPIO_4_AH                            (4  | BP_ACTIVE_HIGH)
#define BP_GPIO_4_AL                            (4  | BP_ACTIVE_LOW)
#define BP_GPIO_5_AH                            (5  | BP_ACTIVE_HIGH)
#define BP_GPIO_5_AL                            (5  | BP_ACTIVE_LOW)
#define BP_GPIO_6_AH                            (6  | BP_ACTIVE_HIGH)
#define BP_GPIO_6_AL                            (6  | BP_ACTIVE_LOW)
#define BP_GPIO_7_AH                            (7  | BP_ACTIVE_HIGH)
#define BP_GPIO_7_AL                            (7  | BP_ACTIVE_LOW)
#define BP_GPIO_8_AH                            (8  | BP_ACTIVE_HIGH)
#define BP_GPIO_8_AL                            (8  | BP_ACTIVE_LOW)
#define BP_GPIO_9_AH                            (9  | BP_ACTIVE_HIGH)
#define BP_GPIO_9_AL                            (9  | BP_ACTIVE_LOW)
#define BP_GPIO_10_AH                           (10 | BP_ACTIVE_HIGH)
#define BP_GPIO_10_AL                           (10 | BP_ACTIVE_LOW)
#define BP_GPIO_11_AH                           (11 | BP_ACTIVE_HIGH)
#define BP_GPIO_11_AL                           (11 | BP_ACTIVE_LOW)
#define BP_GPIO_12_AH                           (12 | BP_ACTIVE_HIGH)
#define BP_GPIO_12_AL                           (12 | BP_ACTIVE_LOW)
#define BP_GPIO_13_AH                           (13 | BP_ACTIVE_HIGH)
#define BP_GPIO_13_AL                           (13 | BP_ACTIVE_LOW)
#define BP_GPIO_14_AH                           (14 | BP_ACTIVE_HIGH)
#define BP_GPIO_14_AL                           (14 | BP_ACTIVE_LOW)
#define BP_GPIO_15_AH                           (15 | BP_ACTIVE_HIGH)
#define BP_GPIO_15_AL                           (15 | BP_ACTIVE_LOW)
#define BP_GPIO_16_AH                           (16 | BP_ACTIVE_HIGH)
#define BP_GPIO_16_AL                           (16 | BP_ACTIVE_LOW)
#define BP_GPIO_17_AH                           (17 | BP_ACTIVE_HIGH)
#define BP_GPIO_17_AL                           (17 | BP_ACTIVE_LOW)
#define BP_GPIO_18_AH                           (18 | BP_ACTIVE_HIGH)
#define BP_GPIO_18_AL                           (18 | BP_ACTIVE_LOW)
#define BP_GPIO_19_AH                           (19 | BP_ACTIVE_HIGH)
#define BP_GPIO_19_AL                           (19 | BP_ACTIVE_LOW)
#define BP_GPIO_20_AH                           (20 | BP_ACTIVE_HIGH)
#define BP_GPIO_20_AL                           (20 | BP_ACTIVE_LOW)
#define BP_GPIO_21_AH                           (21 | BP_ACTIVE_HIGH)
#define BP_GPIO_21_AL                           (21 | BP_ACTIVE_LOW)
#define BP_GPIO_22_AH                           (22 | BP_ACTIVE_HIGH)
#define BP_GPIO_22_AL                           (22 | BP_ACTIVE_LOW)
#define BP_GPIO_23_AH                           (23 | BP_ACTIVE_HIGH)
#define BP_GPIO_23_AL                           (23 | BP_ACTIVE_LOW)
#define BP_GPIO_24_AH                           (24 | BP_ACTIVE_HIGH)
#define BP_GPIO_24_AL                           (24 | BP_ACTIVE_LOW)
#define BP_GPIO_25_AH                           (25 | BP_ACTIVE_HIGH)
#define BP_GPIO_25_AL                           (25 | BP_ACTIVE_LOW)
#define BP_GPIO_26_AH                           (26 | BP_ACTIVE_HIGH)
#define BP_GPIO_26_AL                           (26 | BP_ACTIVE_LOW)
#define BP_GPIO_27_AH                           (27 | BP_ACTIVE_HIGH)
#define BP_GPIO_27_AL                           (27 | BP_ACTIVE_LOW)
#define BP_GPIO_28_AH                           (28 | BP_ACTIVE_HIGH)
#define BP_GPIO_28_AL                           (28 | BP_ACTIVE_LOW)
#define BP_GPIO_29_AH                           (29 | BP_ACTIVE_HIGH)
#define BP_GPIO_29_AL                           (29 | BP_ACTIVE_LOW)
#define BP_GPIO_30_AH                           (30 | BP_ACTIVE_HIGH)
#define BP_GPIO_30_AL                           (30 | BP_ACTIVE_LOW)
#define BP_GPIO_31_AH                           (31 | BP_ACTIVE_HIGH)
#define BP_GPIO_31_AL                           (31 | BP_ACTIVE_LOW)
#define BP_GPIO_32_AH                           (32 | BP_ACTIVE_HIGH)
#define BP_GPIO_32_AL                           (32 | BP_ACTIVE_LOW)
#define BP_GPIO_33_AH                           (33 | BP_ACTIVE_HIGH)
#define BP_GPIO_33_AL                           (33 | BP_ACTIVE_LOW)
#define BP_GPIO_34_AH                           (34 | BP_ACTIVE_HIGH)
#define BP_GPIO_34_AL                           (34 | BP_ACTIVE_LOW)
#define BP_GPIO_35_AH                           (35 | BP_ACTIVE_HIGH)
#define BP_GPIO_35_AL                           (35 | BP_ACTIVE_LOW)
#define BP_GPIO_36_AH                           (36 | BP_ACTIVE_HIGH)
#define BP_GPIO_36_AL                           (36 | BP_ACTIVE_LOW)

/* Values for external interrupt assignments. */
#define BP_EXT_INTR_0                           0
#define BP_EXT_INTR_1                           1
#define BP_EXT_INTR_2                           2
#define BP_EXT_INTR_3                           3

/* Values for chip select assignments. */
#define BP_CS_0                                 0
#define BP_CS_1                                 1
#define BP_CS_2                                 2
#define BP_CS_3                                 3

/* Value for GPIO and external interrupt fields that are not used. */
#define BP_NOT_DEFINED                          0xffff
#define BP_HW_DEFINED                           0xfff0
#define BP_UNEQUIPPED                           0xfff1

/* Maximum size of the board id string. */
#define BP_BOARD_ID_LEN                         16

/* Maximum number of Ethernet MACs. */
#define BP_MAX_ENET_MACS                        2

/* Maximum number of VoIP DSPs. */
#define BP_MAX_VOIP_DSP                         2

/* Wireless Antenna Settings. */
#define BP_WLAN_ANT_MAIN                        0
#define BP_WLAN_ANT_AUX                         1
#define BP_WLAN_ANT_BOTH                        3

#if !defined(__ASSEMBLER__)

/* Information about an Ethernet MAC.  If ucPhyType is BP_ENET_NO_PHY,
 * then the other fields are not valid.
 */
typedef struct EthernetMacInfo
{
    unsigned char ucPhyType;                    /* BP_ENET_xxx             */
    unsigned char ucPhyAddress;                 /* 0 to 31                 */
    unsigned short usGpioPhySpiSck;             /* GPIO pin or not defined */
    unsigned short usGpioPhySpiSs;              /* GPIO pin or not defined */
    unsigned short usGpioPhySpiMosi;            /* GPIO pin or not defined */
    unsigned short usGpioPhySpiMiso;            /* GPIO pin or not defined */
    unsigned short usGpioPhyReset;              /* GPIO pin or not defined (96348LV) */
    unsigned short numSwitchPorts;              /* Number of PHY ports */
    unsigned short usConfigType;                /* Configuration type */
    unsigned short usReverseMii;                /* Reverse MII */
} ETHERNET_MAC_INFO, *PETHERNET_MAC_INFO;


/* Information about VoIP DSPs.  If ucDspType is BP_VOIP_NO_DSP,
 * then the other fields are not valid.
 */
typedef struct VoIPDspInfo
{
    unsigned char  ucDspType;
    unsigned char  ucDspAddress;
    unsigned short usExtIntrVoip;
    unsigned short usGpioVoipReset;
    unsigned short usGpioVoipIntr;
    unsigned short usGpioLedVoip;
    unsigned short usCsVoip;

} VOIP_DSP_INFO;


/**************************************************************************
 * Name       : BpSetBoardId
 *
 * Description: This function find the BOARD_PARAMETERS structure for the
 *              specified board id string and assigns it to a global, static
 *              variable.
 *
 * Parameters : [IN] pszBoardId - Board id string that is saved into NVRAM.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_FOUND - Error, board id input string does not
 *                  have a board parameters configuration record.
 ***************************************************************************/
int BpSetBoardId( char *pszBoardId );

/**************************************************************************
 * Name       : BpGetBoardIds
 *
 * Description: This function returns all of the supported board id strings.
 *
 * Parameters : [OUT] pszBoardIds - Address of a buffer that the board id
 *                  strings are returned in.  Each id starts at BP_BOARD_ID_LEN
 *                  boundary.
 *              [IN] nBoardIdsSize - Number of BP_BOARD_ID_LEN elements that
 *                  were allocated in pszBoardIds.
 *
 * Returns    : Number of board id strings returned.
 ***************************************************************************/
int BpGetBoardIds( char *pszBoardIds, int nBoardIdsSize );

/**************************************************************************
 * Name       : BpGetEthernetMacInfo
 *
 * Description: This function returns all of the supported board id strings.
 *
 * Parameters : [OUT] pEnetInfos - Address of an array of ETHERNET_MAC_INFO
 *                  buffers.
 *              [IN] nNumEnetInfos - Number of ETHERNET_MAC_INFO elements that
 *                  are pointed to by pEnetInfos.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 ***************************************************************************/
int BpGetEthernetMacInfo( PETHERNET_MAC_INFO pEnetInfos, int nNumEnetInfos );

/**************************************************************************
 * Name       : BpGetSdramSize
 *
 * Description: This function returns a constant that describees the board's
 *              SDRAM type and size.
 *
 * Parameters : [OUT] pulSdramSize - Address of short word that the SDRAM size
 *                  is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 ***************************************************************************/
int BpGetSdramSize( unsigned long *pulSdramSize );

/**************************************************************************
 * Name       : BpGetPsiSize
 *
 * Description: This function returns the persistent storage size in K bytes.
 *
 * Parameters : [OUT] pulPsiSize - Address of short word that the persistent
 *                  storage size is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 ***************************************************************************/
int BpGetPsiSize( unsigned long *pulPsiSize );

/**************************************************************************
 * Name       : BpGetRj11InnerOuterPairGpios
 *
 * Description: This function returns the GPIO pin assignments for changing
 *              between the RJ11 inner pair and RJ11 outer pair.
 *
 * Parameters : [OUT] pusInner - Address of short word that the RJ11 inner pair
 *                  GPIO pin is returned in.
 *              [OUT] pusOuter - Address of short word that the RJ11 outer pair
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, values are returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetRj11InnerOuterPairGpios( unsigned short *pusInner,
    unsigned short *pusOuter );

/**************************************************************************
 * Name       : BpGetPressAndHoldResetGpio
 *
 * Description: This function returns the GPIO pin assignment for the press
 *              and hold reset button.
 *
 * Parameters : [OUT] pusValue - Address of short word that the press and hold
 *                  reset button GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetPressAndHoldResetGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetVoipResetGpio
 *
 * Description: This function returns the GPIO pin assignment for the VOIP
 *              Reset operation.
 *
 * Parameters : [OUT] pusValue - Address of short word that the VOIP reset
 *                  GPIO pin is returned in.
 *              [IN] dspNum - Address of the DSP to query.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoipResetGpio( unsigned char dspNum, unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetVoipIntrGpio
 *
 * Description: This function returns the GPIO pin assignment for VoIP interrupt.
 *
 * Parameters : [OUT] pusValue - Address of short word that the VOIP interrupt
 *                  GPIO pin is returned in.
 *              [IN] dspNum - Address of the DSP to query.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoipIntrGpio( unsigned char dspNum, unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetPcmciaResetGpio
 *
 * Description: This function returns the GPIO pin assignment for the PCMCIA
 *              Reset operation.
 *
 * Parameters : [OUT] pusValue - Address of short word that the PCMCIA reset
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetPcmciaResetGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetUartRtsCtsGpios
 *
 * Description: This function returns the GPIO pin assignments for RTS and CTS
 *              UART signals.
 *
 * Parameters : [OUT] pusRts - Address of short word that the UART RTS GPIO
 *                  pin is returned in.
 *              [OUT] pusCts - Address of short word that the UART CTS GPIO
 *                  pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, values are returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetRtsCtsUartGpios( unsigned short *pusRts, unsigned short *pusCts );

/**************************************************************************
 * Name       : BpGetAdslLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the ADSL
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the ADSL LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetAdslLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetAdslFailLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the ADSL
 *              LED that is used when there is a DSL connection failure.
 *
 * Parameters : [OUT] pusValue - Address of short word that the ADSL LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetAdslFailLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetWirelessLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the Wireless
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the Wireless LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetWirelessAntInUse
 *
 * Description: This function returns the antennas in use for wireless
 *
 * Parameters : [OUT] pusValue - Address of short word that the Wireless Antenna
 *                  is in use.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessAntInUse( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetWirelessSesBtnGpio
 *
 * Description: This function returns the GPIO pin assignment for the Wireless
 *              Ses Button.
 *
 * Parameters : [OUT] pusValue - Address of short word that the Wireless Ses
 *                  Button GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessSesBtnGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetWirelessSesExtIntr
 *
 * Description: This function returns the external interrupt number for the 
 *              Wireless Ses Button.
 *
 * Parameters : [OUT] pusValue - Address of short word that the Wireless Ses
 *                  external interrup is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessSesExtIntr( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetWirelessSesLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the Wireless
 *              Ses Led.
 *
 * Parameters : [OUT] pusValue - Address of short word that the Wireless Ses
 *                  Led GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessSesLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetUsbLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the USB
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the USB LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetUsbLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetHpnaLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the HPNA
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the HPNA LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetHpnaLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetWanDataLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the WAN Data
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the WAN Data LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWanDataLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetPppLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the PPP
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the PPP LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetPppLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetPppFailLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the PPP
 *              LED that is used when there is a PPP connection failure.
 *
 * Parameters : [OUT] pusValue - Address of short word that the PPP LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetPppFailLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetVoipLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the VOIP
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the VOIP LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoipLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetBootloaderPowerOnLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the power
 *              on LED that is set by the bootloader.
 *
 * Parameters : [OUT] pusValue - Address of short word that the alarm LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetBootloaderPowerOnLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetBootloaderAlarmLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the alarm
 *              LED that is set by the bootloader.
 *
 * Parameters : [OUT] pusValue - Address of short word that the alarm LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetBootloaderAlarmLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetBootloaderResetCfgLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the reset
 *              configuration LED that is set by the bootloader.
 *
 * Parameters : [OUT] pusValue - Address of short word that the reset
 *                  configuration LED GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetBootloaderResetCfgLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetBootloaderStopLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the break
 *              into bootloader LED that is set by the bootloader.
 *
 * Parameters : [OUT] pusValue - Address of short word that the break into
 *                  bootloader LED GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetBootloaderStopLedGpio( unsigned short *pusValue );

/**************************************************************************
 * Name       : BpGetWirelessExtIntr
 *
 * Description: This function returns the Wireless external interrupt number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the wireless
 *                  external interrupt number is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessExtIntr( unsigned long *pulValue );

/**************************************************************************
 * Name       : BpGetAdslDyingGaspExtIntr
 *
 * Description: This function returns the ADSL Dying Gasp external interrupt
 *              number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the ADSL Dying Gasp
 *                  external interrupt number is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetAdslDyingGaspExtIntr( unsigned long *pulValue );

/**************************************************************************
 * Name       : BpGetVoipExtIntr
 *
 * Description: This function returns the VOIP external interrupt number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the VOIP
 *                  external interrupt number is returned in.
 *              [IN] dspNum - Address of the DSP to query.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoipExtIntr( unsigned char dspNum, unsigned long *pulValue );

/**************************************************************************
 * Name       : BpGetHpnaExtIntr
 *
 * Description: This function returns the HPNA external interrupt number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the HPNA
 *                  external interrupt number is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetHpnaExtIntr( unsigned long *pulValue );

/**************************************************************************
 * Name       : BpGetHpnaChipSelect
 *
 * Description: This function returns the HPNA chip select number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the HPNA
 *                  chip select number is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetHpnaChipSelect( unsigned long *pulValue );

/**************************************************************************
 * Name       : BpGetVoipChipSelect
 *
 * Description: This function returns the VOIP chip select number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the VOIP
 *                  chip select number is returned in.
 *              [IN] dspNum - Address of the DSP to query.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoipChipSelect( unsigned char dspNum, unsigned long *pulValue );

#endif /* __ASSEMBLER__ */

#endif /* _BOARDPARMS_H */

