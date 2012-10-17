/******************************************************************************
**
** FILE NAME    : ifx_atm.h
** PROJECT      : UEIP
** MODULES      : ATM
**
** DATE         : 17 Jun 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global ATM driver header file
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 07 JUL 2009  Xu Liang        Init Version
*******************************************************************************/

#ifndef IFX_ATM_H
#define IFX_ATM_H



/*!
  \defgroup IFX_ATM UEIP Project - ATM driver module
  \brief UEIP Project - ATM driver module, support Danube, Amazon-SE, AR9, VR9.
 */

/*!
  \defgroup IFX_ATM_IOCTL IOCTL Commands
  \ingroup IFX_ATM
  \brief IOCTL Commands used by user application.
 */

/*!
  \defgroup IFX_ATM_STRUCT Structures
  \ingroup IFX_ATM
  \brief Structures used by user application.
 */

/*!
  \file ifx_atm.h
  \ingroup IFX_ATM
  \brief ATM driver header file
 */



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*!
  \addtogroup IFX_ATM_STRUCT
 */
/*@{*/

/*
 *  ATM MIB
 */

typedef struct {
	__u32	ifHCInOctets_h;     /*!< byte counter of ingress cells (upper 32 bits, total 64 bits)   */
	__u32	ifHCInOctets_l;     /*!< byte counter of ingress cells (lower 32 bits, total 64 bits)   */
	__u32	ifHCOutOctets_h;    /*!< byte counter of egress cells (upper 32 bits, total 64 bits)    */
	__u32	ifHCOutOctets_l;    /*!< byte counter of egress cells (lower 32 bits, total 64 bits)    */
	__u32	ifInErrors;         /*!< counter of error ingress cells     */
	__u32	ifInUnknownProtos;  /*!< counter of unknown ingress cells   */
	__u32	ifOutErrors;        /*!< counter of error egress cells      */
} atm_cell_ifEntry_t;

typedef struct {
	__u32	ifHCInOctets_h;     /*!< byte counter of ingress packets (upper 32 bits, total 64 bits) */
	__u32	ifHCInOctets_l;     /*!< byte counter of ingress packets (lower 32 bits, total 64 bits) */
	__u32	ifHCOutOctets_h;    /*!< byte counter of egress packets (upper 32 bits, total 64 bits)  */
	__u32	ifHCOutOctets_l;    /*!< byte counter of egress packets (lower 32 bits, total 64 bits)  */
	__u32	ifInUcastPkts;      /*!< counter of ingress packets         */
	__u32	ifOutUcastPkts;     /*!< counter of egress packets          */
	__u32	ifInErrors;         /*!< counter of error ingress packets   */
	__u32	ifInDiscards;       /*!< counter of dropped ingress packets */
	__u32	ifOutErros;         /*!< counter of error egress packets    */
	__u32	ifOutDiscards;      /*!< counter of dropped egress packets  */
} atm_aal5_ifEntry_t;

typedef struct {
	__u32	aal5VccCrcErrors;       /*!< counter of ingress packets with CRC error  */
	__u32	aal5VccSarTimeOuts;     /*!< counter of ingress packets with Re-assemble timeout    */  //no timer support yet
	__u32	aal5VccOverSizedSDUs;   /*!< counter of oversized ingress packets       */
} atm_aal5_vcc_t;

typedef struct {
	int             vpi;        /*!< VPI of the VCC to get MIB counters */
	int             vci;        /*!< VCI of the VCC to get MIB counters */
	atm_aal5_vcc_t  mib_vcc;    /*!< structure to get MIB counters      */
} atm_aal5_vcc_x_t;

/*@}*/



/*
 * ####################################
 *                IOCTL
 * ####################################
 */

/*!
  \addtogroup IFX_ATM_IOCTL
 */
/*@{*/

/*
 *  ioctl Command
 */
/*!
  \brief ATM IOCTL Magic Number
 */
#define PPE_ATM_IOC_MAGIC       'o'
/*!
  \brief ATM IOCTL Command - Get Cell Level MIB Counters

   This command is obsolete. User can get cell level MIB from DSL API.
   This command uses structure "atm_cell_ifEntry_t" as parameter for output of MIB counters.
 */
#define PPE_ATM_MIB_CELL        _IOW(PPE_ATM_IOC_MAGIC,  0, atm_cell_ifEntry_t)
/*!
  \brief ATM IOCTL Command - Get AAL5 Level MIB Counters

   Get AAL5 packet counters.
   This command uses structure "atm_aal5_ifEntry_t" as parameter for output of MIB counters.
 */
#define PPE_ATM_MIB_AAL5        _IOW(PPE_ATM_IOC_MAGIC,  1, atm_aal5_ifEntry_t)
/*!
  \brief ATM IOCTL Command - Get Per PVC MIB Counters

   Get AAL5 packet counters for each PVC.
   This command uses structure "atm_aal5_vcc_x_t" as parameter for input of VPI/VCI information and output of MIB counters.
 */
#define PPE_ATM_MIB_VCC         _IOWR(PPE_ATM_IOC_MAGIC, 2, atm_aal5_vcc_x_t)
/*!
  \brief Total Number of ATM IOCTL Commands
 */
#define PPE_ATM_IOC_MAXNR       3

/*@}*/



/*
 * ####################################
 *                 API
 * ####################################
 */

#ifdef __KERNEL__
struct port_cell_info {
    unsigned int    port_num;
    unsigned int    tx_link_rate[2];
};
#endif



#endif  //  IFX_ATM_H

