/*! \file 
*   This file contains the compiler assist macros and doxygen comments
*   for the Global Registers block.
*/

/*! \defgroup Global_registers_Defines Global Registers Defines
*   This module contains the compiler assist macros and doxygen comments
*   for the Global Registers block.
*/
/***********************************************************************
* Copyright (c) 2015, Aquantia
*
* Permission to use, copy, modify, and/or distribute this software for any 
* purpose with or without fee is hereby granted, provided that the above 
* copyright notice and this permission notice appear in all copies.
* 
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
* $File: //depot/icm/proj/Dena/rev1.0/c/Systems/tools/windows/regMapParser/src/gencheaders.py $
*
* $Revision: #10 $
*
* $DateTime: 2014/04/08 16:55:58 $
*
* $Author: joshd $
*
* $Label: $
*
* Description:
*
*   This file contains the compiler assist macros for the registers contained in the Global Registers block.
*
*
***********************************************************************/


/*@{*/
#ifndef AQ_HHD_GLOBAL_REGS_DEFINES_HEADER
#define AQ_HHD_GLOBAL_REGS_DEFINES_HEADER


/*-----------------------------------------------------------------------------*/
/*Access macro definitions                                                     */
/*-----------------------------------------------------------------------------*/
/*! \brief Base register address of structure AQ_GlobalStandardControl_1_HHD */
#define AQ_GlobalStandardControl_1_HHD_baseRegisterAddress 0x0000
/*! \brief MMD address of structure AQ_GlobalStandardControl_1_HHD */
#define AQ_GlobalStandardControl_1_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure softReset in AQ_GlobalStandardControl_1_HHD */
#define AQ_GlobalStandardControl_1_HHD_softReset 0
/*! \brief Preprocessor variable to relate field to bit position in structure softReset in AQ_GlobalStandardControl_1_HHD */
#define bits_AQ_GlobalStandardControl_1_HHD_softReset u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure softReset in AQ_GlobalStandardControl_1_HHD */
#define word_AQ_GlobalStandardControl_1_HHD_softReset u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowPower in AQ_GlobalStandardControl_1_HHD */
#define AQ_GlobalStandardControl_1_HHD_lowPower 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowPower in AQ_GlobalStandardControl_1_HHD */
#define bits_AQ_GlobalStandardControl_1_HHD_lowPower u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowPower in AQ_GlobalStandardControl_1_HHD */
#define word_AQ_GlobalStandardControl_1_HHD_lowPower u0.word_0

/*! \brief Base register address of structure AQ_GlobalStandardDeviceIdentifier_HHD */
#define AQ_GlobalStandardDeviceIdentifier_HHD_baseRegisterAddress 0x0002
/*! \brief MMD address of structure AQ_GlobalStandardDeviceIdentifier_HHD */
#define AQ_GlobalStandardDeviceIdentifier_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure deviceIdMSW in AQ_GlobalStandardDeviceIdentifier_HHD */
#define AQ_GlobalStandardDeviceIdentifier_HHD_deviceIdMSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure deviceIdMSW in AQ_GlobalStandardDeviceIdentifier_HHD */
#define bits_AQ_GlobalStandardDeviceIdentifier_HHD_deviceIdMSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure deviceIdMSW in AQ_GlobalStandardDeviceIdentifier_HHD */
#define word_AQ_GlobalStandardDeviceIdentifier_HHD_deviceIdMSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure deviceIdLSW in AQ_GlobalStandardDeviceIdentifier_HHD */
#define AQ_GlobalStandardDeviceIdentifier_HHD_deviceIdLSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure deviceIdLSW in AQ_GlobalStandardDeviceIdentifier_HHD */
#define bits_AQ_GlobalStandardDeviceIdentifier_HHD_deviceIdLSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure deviceIdLSW in AQ_GlobalStandardDeviceIdentifier_HHD */
#define word_AQ_GlobalStandardDeviceIdentifier_HHD_deviceIdLSW u1.word_1

/*! \brief Base register address of structure AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_baseRegisterAddress 0x0005
/*! \brief MMD address of structure AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_autonegotiationPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardDevicesInPackage_HHD_autonegotiationPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define word_AQ_GlobalStandardDevicesInPackage_HHD_autonegotiationPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure tcPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_tcPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure tcPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardDevicesInPackage_HHD_tcPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure tcPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define word_AQ_GlobalStandardDevicesInPackage_HHD_tcPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure dteXsPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_dteXsPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure dteXsPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardDevicesInPackage_HHD_dteXsPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure dteXsPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define word_AQ_GlobalStandardDevicesInPackage_HHD_dteXsPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_Present in AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_phyXS_Present 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_Present in AQ_GlobalStandardDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardDevicesInPackage_HHD_phyXS_Present u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_Present in AQ_GlobalStandardDevicesInPackage_HHD */
#define word_AQ_GlobalStandardDevicesInPackage_HHD_phyXS_Present u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_pcsPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardDevicesInPackage_HHD_pcsPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define word_AQ_GlobalStandardDevicesInPackage_HHD_pcsPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure wisPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_wisPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure wisPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardDevicesInPackage_HHD_wisPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure wisPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define word_AQ_GlobalStandardDevicesInPackage_HHD_wisPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pmaPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_pmaPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardDevicesInPackage_HHD_pmaPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define word_AQ_GlobalStandardDevicesInPackage_HHD_pmaPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure clause_22RegistersPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define AQ_GlobalStandardDevicesInPackage_HHD_clause_22RegistersPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure clause_22RegistersPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardDevicesInPackage_HHD_clause_22RegistersPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure clause_22RegistersPresent in AQ_GlobalStandardDevicesInPackage_HHD */
#define word_AQ_GlobalStandardDevicesInPackage_HHD_clause_22RegistersPresent u0.word_0

/*! \brief Base register address of structure AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define AQ_GlobalStandardVendorDevicesInPackage_HHD_baseRegisterAddress 0x0006
/*! \brief MMD address of structure AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define AQ_GlobalStandardVendorDevicesInPackage_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure vendorSpecificDevice_2Present in AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define AQ_GlobalStandardVendorDevicesInPackage_HHD_vendorSpecificDevice_2Present 0
/*! \brief Preprocessor variable to relate field to bit position in structure vendorSpecificDevice_2Present in AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardVendorDevicesInPackage_HHD_vendorSpecificDevice_2Present u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure vendorSpecificDevice_2Present in AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define word_AQ_GlobalStandardVendorDevicesInPackage_HHD_vendorSpecificDevice_2Present u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure vendorSpecificDevice_1Present in AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define AQ_GlobalStandardVendorDevicesInPackage_HHD_vendorSpecificDevice_1Present 0
/*! \brief Preprocessor variable to relate field to bit position in structure vendorSpecificDevice_1Present in AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardVendorDevicesInPackage_HHD_vendorSpecificDevice_1Present u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure vendorSpecificDevice_1Present in AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define word_AQ_GlobalStandardVendorDevicesInPackage_HHD_vendorSpecificDevice_1Present u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure clause_22ExtensionPresent in AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define AQ_GlobalStandardVendorDevicesInPackage_HHD_clause_22ExtensionPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure clause_22ExtensionPresent in AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define bits_AQ_GlobalStandardVendorDevicesInPackage_HHD_clause_22ExtensionPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure clause_22ExtensionPresent in AQ_GlobalStandardVendorDevicesInPackage_HHD */
#define word_AQ_GlobalStandardVendorDevicesInPackage_HHD_clause_22ExtensionPresent u0.word_0

/*! \brief Base register address of structure AQ_GlobalStandardStatus_2_HHD */
#define AQ_GlobalStandardStatus_2_HHD_baseRegisterAddress 0x0008
/*! \brief MMD address of structure AQ_GlobalStandardStatus_2_HHD */
#define AQ_GlobalStandardStatus_2_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure devicePresent in AQ_GlobalStandardStatus_2_HHD */
#define AQ_GlobalStandardStatus_2_HHD_devicePresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure devicePresent in AQ_GlobalStandardStatus_2_HHD */
#define bits_AQ_GlobalStandardStatus_2_HHD_devicePresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure devicePresent in AQ_GlobalStandardStatus_2_HHD */
#define word_AQ_GlobalStandardStatus_2_HHD_devicePresent u0.word_0

/*! \brief Base register address of structure AQ_GlobalStandardPackageIdentifier_HHD */
#define AQ_GlobalStandardPackageIdentifier_HHD_baseRegisterAddress 0x000E
/*! \brief MMD address of structure AQ_GlobalStandardPackageIdentifier_HHD */
#define AQ_GlobalStandardPackageIdentifier_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure packageIdMSW in AQ_GlobalStandardPackageIdentifier_HHD */
#define AQ_GlobalStandardPackageIdentifier_HHD_packageIdMSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure packageIdMSW in AQ_GlobalStandardPackageIdentifier_HHD */
#define bits_AQ_GlobalStandardPackageIdentifier_HHD_packageIdMSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure packageIdMSW in AQ_GlobalStandardPackageIdentifier_HHD */
#define word_AQ_GlobalStandardPackageIdentifier_HHD_packageIdMSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure packageIdLSW in AQ_GlobalStandardPackageIdentifier_HHD */
#define AQ_GlobalStandardPackageIdentifier_HHD_packageIdLSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure packageIdLSW in AQ_GlobalStandardPackageIdentifier_HHD */
#define bits_AQ_GlobalStandardPackageIdentifier_HHD_packageIdLSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure packageIdLSW in AQ_GlobalStandardPackageIdentifier_HHD */
#define word_AQ_GlobalStandardPackageIdentifier_HHD_packageIdLSW u1.word_1

/*! \brief Base register address of structure AQ_GlobalFirmwareID_HHD */
#define AQ_GlobalFirmwareID_HHD_baseRegisterAddress 0x0020
/*! \brief MMD address of structure AQ_GlobalFirmwareID_HHD */
#define AQ_GlobalFirmwareID_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure firmwareMajorRevisionNumber in AQ_GlobalFirmwareID_HHD */
#define AQ_GlobalFirmwareID_HHD_firmwareMajorRevisionNumber 0
/*! \brief Preprocessor variable to relate field to bit position in structure firmwareMajorRevisionNumber in AQ_GlobalFirmwareID_HHD */
#define bits_AQ_GlobalFirmwareID_HHD_firmwareMajorRevisionNumber u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure firmwareMajorRevisionNumber in AQ_GlobalFirmwareID_HHD */
#define word_AQ_GlobalFirmwareID_HHD_firmwareMajorRevisionNumber u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure firmwareMinorRevisionNumber in AQ_GlobalFirmwareID_HHD */
#define AQ_GlobalFirmwareID_HHD_firmwareMinorRevisionNumber 0
/*! \brief Preprocessor variable to relate field to bit position in structure firmwareMinorRevisionNumber in AQ_GlobalFirmwareID_HHD */
#define bits_AQ_GlobalFirmwareID_HHD_firmwareMinorRevisionNumber u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure firmwareMinorRevisionNumber in AQ_GlobalFirmwareID_HHD */
#define word_AQ_GlobalFirmwareID_HHD_firmwareMinorRevisionNumber u0.word_0

/*! \brief Base register address of structure AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_baseRegisterAddress 0x0100
/*! \brief MMD address of structure AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure nvrExecuteOperation in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrExecuteOperation 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrExecuteOperation in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrExecuteOperation u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrExecuteOperation in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrExecuteOperation u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrWriteMode in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrWriteMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrWriteMode in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrWriteMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrWriteMode in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrWriteMode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure freezeNvrCrc in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_freezeNvrCrc 0
/*! \brief Preprocessor variable to relate field to bit position in structure freezeNvrCrc in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_freezeNvrCrc u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure freezeNvrCrc in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_freezeNvrCrc u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure resetNvrCrc in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_resetNvrCrc 0
/*! \brief Preprocessor variable to relate field to bit position in structure resetNvrCrc in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_resetNvrCrc u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure resetNvrCrc in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_resetNvrCrc u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrBurst in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrBurst 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrBurst in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrBurst u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrBurst in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrBurst u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrBusy in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrBusy 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrBusy in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrBusy u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrBusy in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrBusy u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrOpcode in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrOpcode 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrOpcode in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrOpcode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrOpcode in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrOpcode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrMailboxCrc in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrMailboxCrc 1
/*! \brief Preprocessor variable to relate field to bit position in structure nvrMailboxCrc in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrMailboxCrc u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure nvrMailboxCrc in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrMailboxCrc u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrAddressMSW in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrAddressMSW 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrAddressMSW in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrAddressMSW u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrAddressMSW in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrAddressMSW u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure nvrAddressLSW in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrAddressLSW 3
/*! \brief Preprocessor variable to relate field to bit position in structure nvrAddressLSW in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrAddressLSW u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure nvrAddressLSW in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrAddressLSW u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure nvrDataMSW in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrDataMSW 4
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDataMSW in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrDataMSW u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure nvrDataMSW in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrDataMSW u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure nvrDataLSW in AQ_GlobalNvrInterface_HHD */
#define AQ_GlobalNvrInterface_HHD_nvrDataLSW 5
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDataLSW in AQ_GlobalNvrInterface_HHD */
#define bits_AQ_GlobalNvrInterface_HHD_nvrDataLSW u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure nvrDataLSW in AQ_GlobalNvrInterface_HHD */
#define word_AQ_GlobalNvrInterface_HHD_nvrDataLSW u5.word_5

/*! \brief Base register address of structure AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_baseRegisterAddress 0x0200
/*! \brief MMD address of structure AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxExecuteOperation in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxExecuteOperation 0
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxExecuteOperation in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxExecuteOperation u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxExecuteOperation in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxExecuteOperation u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxWriteMode in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxWriteMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxWriteMode in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxWriteMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxWriteMode in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxWriteMode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure resetUpMailboxCrc in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_resetUpMailboxCrc 0
/*! \brief Preprocessor variable to relate field to bit position in structure resetUpMailboxCrc in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_resetUpMailboxCrc u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure resetUpMailboxCrc in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_resetUpMailboxCrc u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxBusy in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxBusy 0
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxBusy in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxBusy u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxBusy in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxBusy u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxCrc in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxCrc 1
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxCrc in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxCrc u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxCrc in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxCrc u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxAddressMSW in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxAddressMSW 2
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxAddressMSW in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxAddressMSW u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxAddressMSW in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxAddressMSW u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxAddressLSW in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxAddressLSW 3
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxAddressLSW in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxAddressLSW u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxAddressLSW in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxAddressLSW u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxAddressLSW_Don_tCare in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxAddressLSW_Don_tCare 3
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxAddressLSW_Don_tCare in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxAddressLSW_Don_tCare u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxAddressLSW_Don_tCare in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxAddressLSW_Don_tCare u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxDataMSW in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxDataMSW 4
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxDataMSW in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxDataMSW u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxDataMSW in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxDataMSW u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxDataLSW in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxDataLSW 5
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxDataLSW in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxDataLSW u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxDataLSW in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxDataLSW u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxCrcReadEnable in AQ_GlobalMailboxInterface_HHD */
#define AQ_GlobalMailboxInterface_HHD_upMailboxCrcReadEnable 6
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxCrcReadEnable in AQ_GlobalMailboxInterface_HHD */
#define bits_AQ_GlobalMailboxInterface_HHD_upMailboxCrcReadEnable u6.bits_6
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxCrcReadEnable in AQ_GlobalMailboxInterface_HHD */
#define word_AQ_GlobalMailboxInterface_HHD_upMailboxCrcReadEnable u6.word_6

/*! \brief Base register address of structure AQ_GlobalMicroprocessorScratchPad_HHD */
#define AQ_GlobalMicroprocessorScratchPad_HHD_baseRegisterAddress 0x0300
/*! \brief MMD address of structure AQ_GlobalMicroprocessorScratchPad_HHD */
#define AQ_GlobalMicroprocessorScratchPad_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure scratchPad_1 in AQ_GlobalMicroprocessorScratchPad_HHD */
#define AQ_GlobalMicroprocessorScratchPad_HHD_scratchPad_1 0
/*! \brief Preprocessor variable to relate field to bit position in structure scratchPad_1 in AQ_GlobalMicroprocessorScratchPad_HHD */
#define bits_AQ_GlobalMicroprocessorScratchPad_HHD_scratchPad_1 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure scratchPad_1 in AQ_GlobalMicroprocessorScratchPad_HHD */
#define word_AQ_GlobalMicroprocessorScratchPad_HHD_scratchPad_1 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure scratchPad_2 in AQ_GlobalMicroprocessorScratchPad_HHD */
#define AQ_GlobalMicroprocessorScratchPad_HHD_scratchPad_2 1
/*! \brief Preprocessor variable to relate field to bit position in structure scratchPad_2 in AQ_GlobalMicroprocessorScratchPad_HHD */
#define bits_AQ_GlobalMicroprocessorScratchPad_HHD_scratchPad_2 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure scratchPad_2 in AQ_GlobalMicroprocessorScratchPad_HHD */
#define word_AQ_GlobalMicroprocessorScratchPad_HHD_scratchPad_2 u1.word_1

/*! \brief Base register address of structure AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_baseRegisterAddress 0x5002
/*! \brief MMD address of structure AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressEthertypeExplicitSectagLsb in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressEthertypeExplicitSectagLsb 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressEthertypeExplicitSectagLsb in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressEthertypeExplicitSectagLsb u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressEthertypeExplicitSectagLsb in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressEthertypeExplicitSectagLsb u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressClearGlobalTime in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressClearGlobalTime 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressClearGlobalTime in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressClearGlobalTime u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressClearGlobalTime in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressClearGlobalTime u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressClearCounter in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressClearCounter 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressClearCounter in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressClearCounter u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressClearCounter in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressClearCounter u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressHighPriority in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressHighPriority 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressHighPriority in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressHighPriority u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressHighPriority in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressHighPriority u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressIcvLsb_8BytesEnable in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressIcvLsb_8BytesEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressIcvLsb_8BytesEnable in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressIcvLsb_8BytesEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressIcvLsb_8BytesEnable in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressIcvLsb_8BytesEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressExternalClassificationEnable in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressExternalClassificationEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressExternalClassificationEnable in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressExternalClassificationEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressExternalClassificationEnable in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressExternalClassificationEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressExplicitSectagReportShortLength in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressExplicitSectagReportShortLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressExplicitSectagReportShortLength in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressExplicitSectagReportShortLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressExplicitSectagReportShortLength in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressExplicitSectagReportShortLength u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressDropInvalidSa_scPackets in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressDropInvalidSa_scPackets 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressDropInvalidSa_scPackets in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressDropInvalidSa_scPackets u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressDropInvalidSa_scPackets in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressDropInvalidSa_scPackets u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressUnmatchedUseSc_0 in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressUnmatchedUseSc_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressUnmatchedUseSc_0 in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressUnmatchedUseSc_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressUnmatchedUseSc_0 in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressUnmatchedUseSc_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgresssGcmTestMode in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgresssGcmTestMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgresssGcmTestMode in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgresssGcmTestMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgresssGcmTestMode in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgresssGcmTestMode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressGcmStart in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressGcmStart 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressGcmStart in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressGcmStart u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressGcmStart in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressGcmStart u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressDropEgprcLutMiss in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressDropEgprcLutMiss 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressDropEgprcLutMiss in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressDropEgprcLutMiss u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressDropEgprcLutMiss in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressDropEgprcLutMiss u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressDropKayPacket in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressDropKayPacket 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressDropKayPacket in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressDropKayPacket u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressDropKayPacket in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressDropKayPacket u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSoftReset in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressSoftReset 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSoftReset in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressSoftReset u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSoftReset in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressSoftReset u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressEthertypeExplicitSectagMsb in AQ_MssEgressControlRegister_HHD */
#define AQ_MssEgressControlRegister_HHD_mssEgressEthertypeExplicitSectagMsb 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressEthertypeExplicitSectagMsb in AQ_MssEgressControlRegister_HHD */
#define bits_AQ_MssEgressControlRegister_HHD_mssEgressEthertypeExplicitSectagMsb u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressEthertypeExplicitSectagMsb in AQ_MssEgressControlRegister_HHD */
#define word_AQ_MssEgressControlRegister_HHD_mssEgressEthertypeExplicitSectagMsb u1.word_1

/*! \brief Base register address of structure AQ_MssEgressVlanTpid_0Register_HHD */
#define AQ_MssEgressVlanTpid_0Register_HHD_baseRegisterAddress 0x5008
/*! \brief MMD address of structure AQ_MssEgressVlanTpid_0Register_HHD */
#define AQ_MssEgressVlanTpid_0Register_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanStagTpid in AQ_MssEgressVlanTpid_0Register_HHD */
#define AQ_MssEgressVlanTpid_0Register_HHD_mssEgressVlanStagTpid 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanStagTpid in AQ_MssEgressVlanTpid_0Register_HHD */
#define bits_AQ_MssEgressVlanTpid_0Register_HHD_mssEgressVlanStagTpid u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanStagTpid in AQ_MssEgressVlanTpid_0Register_HHD */
#define word_AQ_MssEgressVlanTpid_0Register_HHD_mssEgressVlanStagTpid u0.word_0

/*! \brief Base register address of structure AQ_MssEgressVlanTpid_1Register_HHD */
#define AQ_MssEgressVlanTpid_1Register_HHD_baseRegisterAddress 0x500A
/*! \brief MMD address of structure AQ_MssEgressVlanTpid_1Register_HHD */
#define AQ_MssEgressVlanTpid_1Register_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanQtagTpid in AQ_MssEgressVlanTpid_1Register_HHD */
#define AQ_MssEgressVlanTpid_1Register_HHD_mssEgressVlanQtagTpid 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanQtagTpid in AQ_MssEgressVlanTpid_1Register_HHD */
#define bits_AQ_MssEgressVlanTpid_1Register_HHD_mssEgressVlanQtagTpid u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanQtagTpid in AQ_MssEgressVlanTpid_1Register_HHD */
#define word_AQ_MssEgressVlanTpid_1Register_HHD_mssEgressVlanQtagTpid u0.word_0

/*! \brief Base register address of structure AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_baseRegisterAddress 0x500C
/*! \brief MMD address of structure AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanUpMapTable in AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanUpMapTable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanUpMapTable in AQ_MssEgressVlanControlRegister_HHD */
#define bits_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanUpMapTable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanUpMapTable in AQ_MssEgressVlanControlRegister_HHD */
#define word_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanUpMapTable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanQtagParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanQtagParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanQtagParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define bits_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanQtagParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanQtagParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define word_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanQtagParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanStagParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanStagParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanStagParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define bits_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanStagParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanStagParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define word_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanStagParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanQinqParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanQinqParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanQinqParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define bits_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanQinqParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanQinqParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define word_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanQinqParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanQtagUpParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanQtagUpParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanQtagUpParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define bits_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanQtagUpParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanQtagUpParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define word_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanQtagUpParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanStagUpParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanStagUpParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanStagUpParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define bits_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanStagUpParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanStagUpParseEnable in AQ_MssEgressVlanControlRegister_HHD */
#define word_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanStagUpParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanUpDefault in AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanUpDefault 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanUpDefault in AQ_MssEgressVlanControlRegister_HHD */
#define bits_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanUpDefault u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanUpDefault in AQ_MssEgressVlanControlRegister_HHD */
#define word_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanUpDefault u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressVlanUpMapTableMSW in AQ_MssEgressVlanControlRegister_HHD */
#define AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanUpMapTableMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressVlanUpMapTableMSW in AQ_MssEgressVlanControlRegister_HHD */
#define bits_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanUpMapTableMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressVlanUpMapTableMSW in AQ_MssEgressVlanControlRegister_HHD */
#define word_AQ_MssEgressVlanControlRegister_HHD_mssEgressVlanUpMapTableMSW u1.word_1

/*! \brief Base register address of structure AQ_MssEgressPnControlRegister_HHD */
#define AQ_MssEgressPnControlRegister_HHD_baseRegisterAddress 0x500E
/*! \brief MMD address of structure AQ_MssEgressPnControlRegister_HHD */
#define AQ_MssEgressPnControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaPnThresholdLSW in AQ_MssEgressPnControlRegister_HHD */
#define AQ_MssEgressPnControlRegister_HHD_mssEgressSaPnThresholdLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaPnThresholdLSW in AQ_MssEgressPnControlRegister_HHD */
#define bits_AQ_MssEgressPnControlRegister_HHD_mssEgressSaPnThresholdLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaPnThresholdLSW in AQ_MssEgressPnControlRegister_HHD */
#define word_AQ_MssEgressPnControlRegister_HHD_mssEgressSaPnThresholdLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaPnThresholdMSW in AQ_MssEgressPnControlRegister_HHD */
#define AQ_MssEgressPnControlRegister_HHD_mssEgressSaPnThresholdMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaPnThresholdMSW in AQ_MssEgressPnControlRegister_HHD */
#define bits_AQ_MssEgressPnControlRegister_HHD_mssEgressSaPnThresholdMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaPnThresholdMSW in AQ_MssEgressPnControlRegister_HHD */
#define word_AQ_MssEgressPnControlRegister_HHD_mssEgressSaPnThresholdMSW u1.word_1

/*! \brief Base register address of structure AQ_MssEgressMtuSizeControlRegister_HHD */
#define AQ_MssEgressMtuSizeControlRegister_HHD_baseRegisterAddress 0x5010
/*! \brief MMD address of structure AQ_MssEgressMtuSizeControlRegister_HHD */
#define AQ_MssEgressMtuSizeControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressControlledPacketMtuSize in AQ_MssEgressMtuSizeControlRegister_HHD */
#define AQ_MssEgressMtuSizeControlRegister_HHD_mssEgressControlledPacketMtuSize 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressControlledPacketMtuSize in AQ_MssEgressMtuSizeControlRegister_HHD */
#define bits_AQ_MssEgressMtuSizeControlRegister_HHD_mssEgressControlledPacketMtuSize u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressControlledPacketMtuSize in AQ_MssEgressMtuSizeControlRegister_HHD */
#define word_AQ_MssEgressMtuSizeControlRegister_HHD_mssEgressControlledPacketMtuSize u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressUncontrolledPacketMtuSize in AQ_MssEgressMtuSizeControlRegister_HHD */
#define AQ_MssEgressMtuSizeControlRegister_HHD_mssEgressUncontrolledPacketMtuSize 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressUncontrolledPacketMtuSize in AQ_MssEgressMtuSizeControlRegister_HHD */
#define bits_AQ_MssEgressMtuSizeControlRegister_HHD_mssEgressUncontrolledPacketMtuSize u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressUncontrolledPacketMtuSize in AQ_MssEgressMtuSizeControlRegister_HHD */
#define word_AQ_MssEgressMtuSizeControlRegister_HHD_mssEgressUncontrolledPacketMtuSize u1.word_1

/*! \brief Base register address of structure AQ_MssEgressInterruptStatusRegister_HHD */
#define AQ_MssEgressInterruptStatusRegister_HHD_baseRegisterAddress 0x505C
/*! \brief MMD address of structure AQ_MssEgressInterruptStatusRegister_HHD */
#define AQ_MssEgressInterruptStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressEccErrorInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define AQ_MssEgressInterruptStatusRegister_HHD_mssEgressEccErrorInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressEccErrorInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define bits_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressEccErrorInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressEccErrorInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define word_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressEccErrorInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressMibSaturationInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define AQ_MssEgressInterruptStatusRegister_HHD_mssEgressMibSaturationInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressMibSaturationInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define bits_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressMibSaturationInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressMibSaturationInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define word_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressMibSaturationInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaThresholdExpiredInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define AQ_MssEgressInterruptStatusRegister_HHD_mssEgressSaThresholdExpiredInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaThresholdExpiredInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define bits_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressSaThresholdExpiredInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaThresholdExpiredInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define word_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressSaThresholdExpiredInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaExpiredInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define AQ_MssEgressInterruptStatusRegister_HHD_mssEgressSaExpiredInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaExpiredInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define bits_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressSaExpiredInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaExpiredInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define word_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressSaExpiredInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressMasterInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define AQ_MssEgressInterruptStatusRegister_HHD_mssEgressMasterInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressMasterInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define bits_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressMasterInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressMasterInterrupt in AQ_MssEgressInterruptStatusRegister_HHD */
#define word_AQ_MssEgressInterruptStatusRegister_HHD_mssEgressMasterInterrupt u0.word_0

/*! \brief Base register address of structure AQ_MssEgressInterruptMaskRegister_HHD */
#define AQ_MssEgressInterruptMaskRegister_HHD_baseRegisterAddress 0x505E
/*! \brief MMD address of structure AQ_MssEgressInterruptMaskRegister_HHD */
#define AQ_MssEgressInterruptMaskRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressEccErrorInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define AQ_MssEgressInterruptMaskRegister_HHD_mssEgressEccErrorInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressEccErrorInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define bits_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressEccErrorInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressEccErrorInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define word_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressEccErrorInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressMibSaturationInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define AQ_MssEgressInterruptMaskRegister_HHD_mssEgressMibSaturationInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressMibSaturationInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define bits_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressMibSaturationInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressMibSaturationInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define word_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressMibSaturationInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaExpiredThresholdInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define AQ_MssEgressInterruptMaskRegister_HHD_mssEgressSaExpiredThresholdInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaExpiredThresholdInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define bits_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressSaExpiredThresholdInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaExpiredThresholdInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define word_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressSaExpiredThresholdInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaExpiredInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define AQ_MssEgressInterruptMaskRegister_HHD_mssEgressSaExpiredInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaExpiredInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define bits_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressSaExpiredInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaExpiredInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define word_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressSaExpiredInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressMasterInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define AQ_MssEgressInterruptMaskRegister_HHD_mssEgressMasterInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressMasterInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define bits_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressMasterInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressMasterInterruptEnable in AQ_MssEgressInterruptMaskRegister_HHD */
#define word_AQ_MssEgressInterruptMaskRegister_HHD_mssEgressMasterInterruptEnable u0.word_0

/*! \brief Base register address of structure AQ_MssEgressSaExpiredStatusRegister_HHD */
#define AQ_MssEgressSaExpiredStatusRegister_HHD_baseRegisterAddress 0x5060
/*! \brief MMD address of structure AQ_MssEgressSaExpiredStatusRegister_HHD */
#define AQ_MssEgressSaExpiredStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaExpiredLSW in AQ_MssEgressSaExpiredStatusRegister_HHD */
#define AQ_MssEgressSaExpiredStatusRegister_HHD_mssEgressSaExpiredLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaExpiredLSW in AQ_MssEgressSaExpiredStatusRegister_HHD */
#define bits_AQ_MssEgressSaExpiredStatusRegister_HHD_mssEgressSaExpiredLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaExpiredLSW in AQ_MssEgressSaExpiredStatusRegister_HHD */
#define word_AQ_MssEgressSaExpiredStatusRegister_HHD_mssEgressSaExpiredLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaExpiredMSW in AQ_MssEgressSaExpiredStatusRegister_HHD */
#define AQ_MssEgressSaExpiredStatusRegister_HHD_mssEgressSaExpiredMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaExpiredMSW in AQ_MssEgressSaExpiredStatusRegister_HHD */
#define bits_AQ_MssEgressSaExpiredStatusRegister_HHD_mssEgressSaExpiredMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaExpiredMSW in AQ_MssEgressSaExpiredStatusRegister_HHD */
#define word_AQ_MssEgressSaExpiredStatusRegister_HHD_mssEgressSaExpiredMSW u1.word_1

/*! \brief Base register address of structure AQ_MssEgressSaThresholdExpiredStatusRegister_HHD */
#define AQ_MssEgressSaThresholdExpiredStatusRegister_HHD_baseRegisterAddress 0x5062
/*! \brief MMD address of structure AQ_MssEgressSaThresholdExpiredStatusRegister_HHD */
#define AQ_MssEgressSaThresholdExpiredStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaThresholdExpiredLSW in AQ_MssEgressSaThresholdExpiredStatusRegister_HHD */
#define AQ_MssEgressSaThresholdExpiredStatusRegister_HHD_mssEgressSaThresholdExpiredLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaThresholdExpiredLSW in AQ_MssEgressSaThresholdExpiredStatusRegister_HHD */
#define bits_AQ_MssEgressSaThresholdExpiredStatusRegister_HHD_mssEgressSaThresholdExpiredLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaThresholdExpiredLSW in AQ_MssEgressSaThresholdExpiredStatusRegister_HHD */
#define word_AQ_MssEgressSaThresholdExpiredStatusRegister_HHD_mssEgressSaThresholdExpiredLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaThresholdExpiredMSW in AQ_MssEgressSaThresholdExpiredStatusRegister_HHD */
#define AQ_MssEgressSaThresholdExpiredStatusRegister_HHD_mssEgressSaThresholdExpiredMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaThresholdExpiredMSW in AQ_MssEgressSaThresholdExpiredStatusRegister_HHD */
#define bits_AQ_MssEgressSaThresholdExpiredStatusRegister_HHD_mssEgressSaThresholdExpiredMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaThresholdExpiredMSW in AQ_MssEgressSaThresholdExpiredStatusRegister_HHD */
#define word_AQ_MssEgressSaThresholdExpiredStatusRegister_HHD_mssEgressSaThresholdExpiredMSW u1.word_1

/*! \brief Base register address of structure AQ_MssEgressEccInterruptStatusRegister_HHD */
#define AQ_MssEgressEccInterruptStatusRegister_HHD_baseRegisterAddress 0x5064
/*! \brief MMD address of structure AQ_MssEgressEccInterruptStatusRegister_HHD */
#define AQ_MssEgressEccInterruptStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaEccErrorInterruptLSW in AQ_MssEgressEccInterruptStatusRegister_HHD */
#define AQ_MssEgressEccInterruptStatusRegister_HHD_mssEgressSaEccErrorInterruptLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaEccErrorInterruptLSW in AQ_MssEgressEccInterruptStatusRegister_HHD */
#define bits_AQ_MssEgressEccInterruptStatusRegister_HHD_mssEgressSaEccErrorInterruptLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaEccErrorInterruptLSW in AQ_MssEgressEccInterruptStatusRegister_HHD */
#define word_AQ_MssEgressEccInterruptStatusRegister_HHD_mssEgressSaEccErrorInterruptLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressSaEccErrorInterruptMSW in AQ_MssEgressEccInterruptStatusRegister_HHD */
#define AQ_MssEgressEccInterruptStatusRegister_HHD_mssEgressSaEccErrorInterruptMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressSaEccErrorInterruptMSW in AQ_MssEgressEccInterruptStatusRegister_HHD */
#define bits_AQ_MssEgressEccInterruptStatusRegister_HHD_mssEgressSaEccErrorInterruptMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressSaEccErrorInterruptMSW in AQ_MssEgressEccInterruptStatusRegister_HHD */
#define word_AQ_MssEgressEccInterruptStatusRegister_HHD_mssEgressSaEccErrorInterruptMSW u1.word_1

/*! \brief Base register address of structure AQ_MssEgressLutAddressControlRegister_HHD */
#define AQ_MssEgressLutAddressControlRegister_HHD_baseRegisterAddress 0x5080
/*! \brief MMD address of structure AQ_MssEgressLutAddressControlRegister_HHD */
#define AQ_MssEgressLutAddressControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutSelect in AQ_MssEgressLutAddressControlRegister_HHD */
#define AQ_MssEgressLutAddressControlRegister_HHD_mssEgressLutSelect 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutSelect in AQ_MssEgressLutAddressControlRegister_HHD */
#define bits_AQ_MssEgressLutAddressControlRegister_HHD_mssEgressLutSelect u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutSelect in AQ_MssEgressLutAddressControlRegister_HHD */
#define word_AQ_MssEgressLutAddressControlRegister_HHD_mssEgressLutSelect u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutAddress in AQ_MssEgressLutAddressControlRegister_HHD */
#define AQ_MssEgressLutAddressControlRegister_HHD_mssEgressLutAddress 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutAddress in AQ_MssEgressLutAddressControlRegister_HHD */
#define bits_AQ_MssEgressLutAddressControlRegister_HHD_mssEgressLutAddress u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutAddress in AQ_MssEgressLutAddressControlRegister_HHD */
#define word_AQ_MssEgressLutAddressControlRegister_HHD_mssEgressLutAddress u0.word_0

/*! \brief Base register address of structure AQ_MssEgressLutControlRegister_HHD */
#define AQ_MssEgressLutControlRegister_HHD_baseRegisterAddress 0x5081
/*! \brief MMD address of structure AQ_MssEgressLutControlRegister_HHD */
#define AQ_MssEgressLutControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutWrite in AQ_MssEgressLutControlRegister_HHD */
#define AQ_MssEgressLutControlRegister_HHD_mssEgressLutWrite 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutWrite in AQ_MssEgressLutControlRegister_HHD */
#define bits_AQ_MssEgressLutControlRegister_HHD_mssEgressLutWrite u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutWrite in AQ_MssEgressLutControlRegister_HHD */
#define word_AQ_MssEgressLutControlRegister_HHD_mssEgressLutWrite u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutRead in AQ_MssEgressLutControlRegister_HHD */
#define AQ_MssEgressLutControlRegister_HHD_mssEgressLutRead 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutRead in AQ_MssEgressLutControlRegister_HHD */
#define bits_AQ_MssEgressLutControlRegister_HHD_mssEgressLutRead u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutRead in AQ_MssEgressLutControlRegister_HHD */
#define word_AQ_MssEgressLutControlRegister_HHD_mssEgressLutRead u0.word_0

/*! \brief Base register address of structure AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_baseRegisterAddress 0x50A0
/*! \brief MMD address of structure AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_0 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_0 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_0 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_1 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_1 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_1 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_1 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_2 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_2 2
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_2 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_2 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_2 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_2 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_3 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_3 3
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_3 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_3 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_3 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_3 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_4 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_4 4
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_4 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_4 u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_4 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_4 u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_5 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_5 5
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_5 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_5 u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_5 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_5 u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_6 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_6 6
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_6 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_6 u6.bits_6
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_6 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_6 u6.word_6
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_7 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_7 7
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_7 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_7 u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_7 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_7 u7.word_7
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_8 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_8 8
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_8 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_8 u8.bits_8
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_8 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_8 u8.word_8
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_9 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_9 9
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_9 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_9 u9.bits_9
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_9 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_9 u9.word_9
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_10 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_10 10
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_10 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_10 u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_10 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_10 u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_11 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_11 11
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_11 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_11 u11.bits_11
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_11 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_11 u11.word_11
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_12 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_12 12
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_12 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_12 u12.bits_12
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_12 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_12 u12.word_12
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_13 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_13 13
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_13 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_13 u13.bits_13
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_13 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_13 u13.word_13
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_14 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_14 14
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_14 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_14 u14.bits_14
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_14 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_14 u14.word_14
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_15 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_15 15
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_15 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_15 u15.bits_15
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_15 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_15 u15.word_15
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_16 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_16 16
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_16 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_16 u16.bits_16
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_16 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_16 u16.word_16
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_17 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_17 17
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_17 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_17 u17.bits_17
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_17 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_17 u17.word_17
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_18 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_18 18
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_18 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_18 u18.bits_18
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_18 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_18 u18.word_18
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_19 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_19 19
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_19 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_19 u19.bits_19
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_19 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_19 u19.word_19
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_20 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_20 20
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_20 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_20 u20.bits_20
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_20 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_20 u20.word_20
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_21 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_21 21
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_21 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_21 u21.bits_21
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_21 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_21 u21.word_21
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_22 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_22 22
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_22 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_22 u22.bits_22
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_22 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_22 u22.word_22
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_23 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_23 23
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_23 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_23 u23.bits_23
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_23 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_23 u23.word_23
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_24 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_24 24
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_24 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_24 u24.bits_24
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_24 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_24 u24.word_24
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_25 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_25 25
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_25 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_25 u25.bits_25
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_25 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_25 u25.word_25
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_26 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_26 26
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_26 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_26 u26.bits_26
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_26 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_26 u26.word_26
/*! \brief Preprocessor variable to relate field to word number in structure mssEgressLutData_27 in AQ_MssEgressLutDataControlRegister_HHD */
#define AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_27 27
/*! \brief Preprocessor variable to relate field to bit position in structure mssEgressLutData_27 in AQ_MssEgressLutDataControlRegister_HHD */
#define bits_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_27 u27.bits_27
/*! \brief Preprocessor variable to relate field to word position in structure mssEgressLutData_27 in AQ_MssEgressLutDataControlRegister_HHD */
#define word_AQ_MssEgressLutDataControlRegister_HHD_mssEgressLutData_27 u27.word_27

/*! \brief Base register address of structure AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_baseRegisterAddress 0x6004
/*! \brief MMD address of structure AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemPhyTxEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPhyTxEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemPhyTxEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPhyTxEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemPhyTxEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPhyTxEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxErrorDiscard in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemRxErrorDiscard 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxErrorDiscard in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemRxErrorDiscard u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxErrorDiscard in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemRxErrorDiscard u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemControlFrameEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemControlFrameEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemControlFrameEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemControlFrameEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemControlFrameEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemControlFrameEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemSoftReset in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemSoftReset 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemSoftReset in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemSoftReset u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemSoftReset in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemSoftReset u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxPadEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxPadEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxPadEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxPadEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxPadEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxPadEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxCrcAppend in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxCrcAppend 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxCrcAppend in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxCrcAppend u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxCrcAppend in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxCrcAppend u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxAddressInsertEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxAddressInsertEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxAddressInsertEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxAddressInsertEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxAddressInsertEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxAddressInsertEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemPauseIgnore in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPauseIgnore 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemPauseIgnore in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPauseIgnore u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemPauseIgnore in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPauseIgnore u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemPauseForward in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPauseForward 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemPauseForward in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPauseForward u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemPauseForward in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPauseForward u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemCrcForward in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemCrcForward 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemCrcForward in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemCrcForward u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemCrcForward in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemCrcForward u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemPadEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPadEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemPadEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPadEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemPadEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPadEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemPromiscuousMode in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPromiscuousMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemPromiscuousMode in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPromiscuousMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemPromiscuousMode in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPromiscuousMode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemWanMode in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemWanMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemWanMode in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemWanMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemWanMode in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemWanMode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemRxEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemRxEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemRxEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxLowPowerIdleEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxLowPowerIdleEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxLowPowerIdleEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxLowPowerIdleEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxLowPowerIdleEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemTxLowPowerIdleEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemSfdCheckDisable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemSfdCheckDisable 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemSfdCheckDisable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemSfdCheckDisable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemSfdCheckDisable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemSfdCheckDisable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemPriorityFlowControlEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPriorityFlowControlEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemPriorityFlowControlEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPriorityFlowControlEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemPriorityFlowControlEnable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemPriorityFlowControlEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemIdleColumnCountExtend in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemIdleColumnCountExtend 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemIdleColumnCountExtend in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemIdleColumnCountExtend u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemIdleColumnCountExtend in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemIdleColumnCountExtend u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemLengthCheckDisable in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemLengthCheckDisable 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemLengthCheckDisable in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemLengthCheckDisable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemLengthCheckDisable in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemLengthCheckDisable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemForceSendIdle in AQ_MsmSystemGeneralControlRegister_HHD */
#define AQ_MsmSystemGeneralControlRegister_HHD_msmSystemForceSendIdle 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemForceSendIdle in AQ_MsmSystemGeneralControlRegister_HHD */
#define bits_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemForceSendIdle u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemForceSendIdle in AQ_MsmSystemGeneralControlRegister_HHD */
#define word_AQ_MsmSystemGeneralControlRegister_HHD_msmSystemForceSendIdle u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_baseRegisterAddress 0x600E
/*! \brief MMD address of structure AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxFifoFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoFullThreshold 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxFifoFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define bits_AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoFullThreshold u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxFifoFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define word_AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoFullThreshold u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxFifoEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoEmptyThreshold 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxFifoEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define bits_AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoEmptyThreshold u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxFifoEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define word_AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoEmptyThreshold u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxFifoFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoFullThreshold 2
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxFifoFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define bits_AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoFullThreshold u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxFifoFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define word_AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoFullThreshold u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxFifoEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoEmptyThreshold 3
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxFifoEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define bits_AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoEmptyThreshold u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxFifoEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define word_AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoEmptyThreshold u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxFifoAlmostFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoAlmostFullThreshold 4
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxFifoAlmostFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define bits_AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoAlmostFullThreshold u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxFifoAlmostFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define word_AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoAlmostFullThreshold u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxFifoAlmostEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoAlmostEmptyThreshold 5
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxFifoAlmostEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define bits_AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoAlmostEmptyThreshold u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxFifoAlmostEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define word_AQ_MsmSystemFifoControlRegister_HHD_msmSystemRxFifoAlmostEmptyThreshold u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxFifoAlmostFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoAlmostFullThreshold 6
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxFifoAlmostFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define bits_AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoAlmostFullThreshold u6.bits_6
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxFifoAlmostFullThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define word_AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoAlmostFullThreshold u6.word_6
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxFifoAlmostEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoAlmostEmptyThreshold 7
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxFifoAlmostEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define bits_AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoAlmostEmptyThreshold u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxFifoAlmostEmptyThreshold in AQ_MsmSystemFifoControlRegister_HHD */
#define word_AQ_MsmSystemFifoControlRegister_HHD_msmSystemTxFifoAlmostEmptyThreshold u7.word_7

/*! \brief Base register address of structure AQ_MsmSystemGeneralStatusRegister_HHD */
#define AQ_MsmSystemGeneralStatusRegister_HHD_baseRegisterAddress 0x6020
/*! \brief MMD address of structure AQ_MsmSystemGeneralStatusRegister_HHD */
#define AQ_MsmSystemGeneralStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxFifoEmpty in AQ_MsmSystemGeneralStatusRegister_HHD */
#define AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemTxFifoEmpty 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxFifoEmpty in AQ_MsmSystemGeneralStatusRegister_HHD */
#define bits_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemTxFifoEmpty u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxFifoEmpty in AQ_MsmSystemGeneralStatusRegister_HHD */
#define word_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemTxFifoEmpty u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxLowPowerIdle in AQ_MsmSystemGeneralStatusRegister_HHD */
#define AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemRxLowPowerIdle 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxLowPowerIdle in AQ_MsmSystemGeneralStatusRegister_HHD */
#define bits_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemRxLowPowerIdle u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxLowPowerIdle in AQ_MsmSystemGeneralStatusRegister_HHD */
#define word_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemRxLowPowerIdle u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTimestampAvailable in AQ_MsmSystemGeneralStatusRegister_HHD */
#define AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemTimestampAvailable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTimestampAvailable in AQ_MsmSystemGeneralStatusRegister_HHD */
#define bits_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemTimestampAvailable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTimestampAvailable in AQ_MsmSystemGeneralStatusRegister_HHD */
#define word_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemTimestampAvailable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemPhyLossOfSignal in AQ_MsmSystemGeneralStatusRegister_HHD */
#define AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemPhyLossOfSignal 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemPhyLossOfSignal in AQ_MsmSystemGeneralStatusRegister_HHD */
#define bits_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemPhyLossOfSignal u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemPhyLossOfSignal in AQ_MsmSystemGeneralStatusRegister_HHD */
#define word_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemPhyLossOfSignal u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxRemoteFault in AQ_MsmSystemGeneralStatusRegister_HHD */
#define AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemRxRemoteFault 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxRemoteFault in AQ_MsmSystemGeneralStatusRegister_HHD */
#define bits_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemRxRemoteFault u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxRemoteFault in AQ_MsmSystemGeneralStatusRegister_HHD */
#define word_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemRxRemoteFault u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxLocalFault in AQ_MsmSystemGeneralStatusRegister_HHD */
#define AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemRxLocalFault 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxLocalFault in AQ_MsmSystemGeneralStatusRegister_HHD */
#define bits_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemRxLocalFault u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxLocalFault in AQ_MsmSystemGeneralStatusRegister_HHD */
#define word_AQ_MsmSystemGeneralStatusRegister_HHD_msmSystemRxLocalFault u0.word_0

/*! \brief Base register address of structure AQ_MsmSystemTxIpgControlRegister_HHD */
#define AQ_MsmSystemTxIpgControlRegister_HHD_baseRegisterAddress 0x6022
/*! \brief MMD address of structure AQ_MsmSystemTxIpgControlRegister_HHD */
#define AQ_MsmSystemTxIpgControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxIpgLength in AQ_MsmSystemTxIpgControlRegister_HHD */
#define AQ_MsmSystemTxIpgControlRegister_HHD_msmSystemTxIpgLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxIpgLength in AQ_MsmSystemTxIpgControlRegister_HHD */
#define bits_AQ_MsmSystemTxIpgControlRegister_HHD_msmSystemTxIpgLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxIpgLength in AQ_MsmSystemTxIpgControlRegister_HHD */
#define word_AQ_MsmSystemTxIpgControlRegister_HHD_msmSystemTxIpgLength u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxIpgReserved in AQ_MsmSystemTxIpgControlRegister_HHD */
#define AQ_MsmSystemTxIpgControlRegister_HHD_msmSystemTxIpgReserved 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxIpgReserved in AQ_MsmSystemTxIpgControlRegister_HHD */
#define bits_AQ_MsmSystemTxIpgControlRegister_HHD_msmSystemTxIpgReserved u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxIpgReserved in AQ_MsmSystemTxIpgControlRegister_HHD */
#define word_AQ_MsmSystemTxIpgControlRegister_HHD_msmSystemTxIpgReserved u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemTxGoodFramesCounterRegister_HHD */
#define AQ_MsmSystemTxGoodFramesCounterRegister_HHD_baseRegisterAddress 0x6040
/*! \brief MMD address of structure AQ_MsmSystemTxGoodFramesCounterRegister_HHD */
#define AQ_MsmSystemTxGoodFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxGoodFramesCounter_0 in AQ_MsmSystemTxGoodFramesCounterRegister_HHD */
#define AQ_MsmSystemTxGoodFramesCounterRegister_HHD_msmSystemTxGoodFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxGoodFramesCounter_0 in AQ_MsmSystemTxGoodFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxGoodFramesCounterRegister_HHD_msmSystemTxGoodFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxGoodFramesCounter_0 in AQ_MsmSystemTxGoodFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxGoodFramesCounterRegister_HHD_msmSystemTxGoodFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxGoodFramesCounter_1 in AQ_MsmSystemTxGoodFramesCounterRegister_HHD */
#define AQ_MsmSystemTxGoodFramesCounterRegister_HHD_msmSystemTxGoodFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxGoodFramesCounter_1 in AQ_MsmSystemTxGoodFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxGoodFramesCounterRegister_HHD_msmSystemTxGoodFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxGoodFramesCounter_1 in AQ_MsmSystemTxGoodFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxGoodFramesCounterRegister_HHD_msmSystemTxGoodFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxGoodFramesCounterRegister_HHD */
#define AQ_MsmSystemRxGoodFramesCounterRegister_HHD_baseRegisterAddress 0x6044
/*! \brief MMD address of structure AQ_MsmSystemRxGoodFramesCounterRegister_HHD */
#define AQ_MsmSystemRxGoodFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxGoodFramesCounter_0 in AQ_MsmSystemRxGoodFramesCounterRegister_HHD */
#define AQ_MsmSystemRxGoodFramesCounterRegister_HHD_msmSystemRxGoodFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxGoodFramesCounter_0 in AQ_MsmSystemRxGoodFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxGoodFramesCounterRegister_HHD_msmSystemRxGoodFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxGoodFramesCounter_0 in AQ_MsmSystemRxGoodFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxGoodFramesCounterRegister_HHD_msmSystemRxGoodFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxGoodFramesCounter_1 in AQ_MsmSystemRxGoodFramesCounterRegister_HHD */
#define AQ_MsmSystemRxGoodFramesCounterRegister_HHD_msmSystemRxGoodFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxGoodFramesCounter_1 in AQ_MsmSystemRxGoodFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxGoodFramesCounterRegister_HHD_msmSystemRxGoodFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxGoodFramesCounter_1 in AQ_MsmSystemRxGoodFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxGoodFramesCounterRegister_HHD_msmSystemRxGoodFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxFcsErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxFcsErrorsCounterRegister_HHD_baseRegisterAddress 0x6048
/*! \brief MMD address of structure AQ_MsmSystemRxFcsErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxFcsErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemFcsErrorCounter_0 in AQ_MsmSystemRxFcsErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxFcsErrorsCounterRegister_HHD_msmSystemFcsErrorCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemFcsErrorCounter_0 in AQ_MsmSystemRxFcsErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxFcsErrorsCounterRegister_HHD_msmSystemFcsErrorCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemFcsErrorCounter_0 in AQ_MsmSystemRxFcsErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxFcsErrorsCounterRegister_HHD_msmSystemFcsErrorCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemFcsErrorCounter_1 in AQ_MsmSystemRxFcsErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxFcsErrorsCounterRegister_HHD_msmSystemFcsErrorCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemFcsErrorCounter_1 in AQ_MsmSystemRxFcsErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxFcsErrorsCounterRegister_HHD_msmSystemFcsErrorCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemFcsErrorCounter_1 in AQ_MsmSystemRxFcsErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxFcsErrorsCounterRegister_HHD_msmSystemFcsErrorCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD_baseRegisterAddress 0x604C
/*! \brief MMD address of structure AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemAlignmentErrorCounter_0 in AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD_msmSystemAlignmentErrorCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemAlignmentErrorCounter_0 in AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD_msmSystemAlignmentErrorCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemAlignmentErrorCounter_0 in AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD_msmSystemAlignmentErrorCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemAlignmentErrorCounter_1 in AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD_msmSystemAlignmentErrorCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemAlignmentErrorCounter_1 in AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD_msmSystemAlignmentErrorCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemAlignmentErrorCounter_1 in AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD_msmSystemAlignmentErrorCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemTxPauseFramesCounterRegister_HHD */
#define AQ_MsmSystemTxPauseFramesCounterRegister_HHD_baseRegisterAddress 0x6050
/*! \brief MMD address of structure AQ_MsmSystemTxPauseFramesCounterRegister_HHD */
#define AQ_MsmSystemTxPauseFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxPauseFramesCounter_0 in AQ_MsmSystemTxPauseFramesCounterRegister_HHD */
#define AQ_MsmSystemTxPauseFramesCounterRegister_HHD_msmSystemTxPauseFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxPauseFramesCounter_0 in AQ_MsmSystemTxPauseFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxPauseFramesCounterRegister_HHD_msmSystemTxPauseFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxPauseFramesCounter_0 in AQ_MsmSystemTxPauseFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxPauseFramesCounterRegister_HHD_msmSystemTxPauseFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxPauseFramesCounter_1 in AQ_MsmSystemTxPauseFramesCounterRegister_HHD */
#define AQ_MsmSystemTxPauseFramesCounterRegister_HHD_msmSystemTxPauseFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxPauseFramesCounter_1 in AQ_MsmSystemTxPauseFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxPauseFramesCounterRegister_HHD_msmSystemTxPauseFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxPauseFramesCounter_1 in AQ_MsmSystemTxPauseFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxPauseFramesCounterRegister_HHD_msmSystemTxPauseFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxPauseFramesCounterRegister_HHD */
#define AQ_MsmSystemRxPauseFramesCounterRegister_HHD_baseRegisterAddress 0x6054
/*! \brief MMD address of structure AQ_MsmSystemRxPauseFramesCounterRegister_HHD */
#define AQ_MsmSystemRxPauseFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxPauseFramesCounter_0 in AQ_MsmSystemRxPauseFramesCounterRegister_HHD */
#define AQ_MsmSystemRxPauseFramesCounterRegister_HHD_msmSystemRxPauseFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxPauseFramesCounter_0 in AQ_MsmSystemRxPauseFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxPauseFramesCounterRegister_HHD_msmSystemRxPauseFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxPauseFramesCounter_0 in AQ_MsmSystemRxPauseFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxPauseFramesCounterRegister_HHD_msmSystemRxPauseFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxPauseFramesCounter_1 in AQ_MsmSystemRxPauseFramesCounterRegister_HHD */
#define AQ_MsmSystemRxPauseFramesCounterRegister_HHD_msmSystemRxPauseFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxPauseFramesCounter_1 in AQ_MsmSystemRxPauseFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxPauseFramesCounterRegister_HHD_msmSystemRxPauseFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxPauseFramesCounter_1 in AQ_MsmSystemRxPauseFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxPauseFramesCounterRegister_HHD_msmSystemRxPauseFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD_baseRegisterAddress 0x6058
/*! \brief MMD address of structure AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxTooLongErrorsCounter_0 in AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD_msmSystemRxTooLongErrorsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxTooLongErrorsCounter_0 in AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD_msmSystemRxTooLongErrorsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxTooLongErrorsCounter_0 in AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD_msmSystemRxTooLongErrorsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxTooLongErrorsCounter_1 in AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD_msmSystemRxTooLongErrorsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxTooLongErrorsCounter_1 in AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD_msmSystemRxTooLongErrorsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxTooLongErrorsCounter_1 in AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD_msmSystemRxTooLongErrorsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD_baseRegisterAddress 0x605C
/*! \brief MMD address of structure AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxInRangeLengthErrorsCounter_0 in AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD_msmSystemRxInRangeLengthErrorsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxInRangeLengthErrorsCounter_0 in AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD_msmSystemRxInRangeLengthErrorsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxInRangeLengthErrorsCounter_0 in AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD_msmSystemRxInRangeLengthErrorsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxInRangeLengthErrorsCounter_1 in AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD_msmSystemRxInRangeLengthErrorsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxInRangeLengthErrorsCounter_1 in AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD_msmSystemRxInRangeLengthErrorsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxInRangeLengthErrorsCounter_1 in AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD_msmSystemRxInRangeLengthErrorsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemTxVlanFramesCounterRegister_HHD */
#define AQ_MsmSystemTxVlanFramesCounterRegister_HHD_baseRegisterAddress 0x6060
/*! \brief MMD address of structure AQ_MsmSystemTxVlanFramesCounterRegister_HHD */
#define AQ_MsmSystemTxVlanFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxVlanFramesCounter_0 in AQ_MsmSystemTxVlanFramesCounterRegister_HHD */
#define AQ_MsmSystemTxVlanFramesCounterRegister_HHD_msmSystemTxVlanFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxVlanFramesCounter_0 in AQ_MsmSystemTxVlanFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxVlanFramesCounterRegister_HHD_msmSystemTxVlanFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxVlanFramesCounter_0 in AQ_MsmSystemTxVlanFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxVlanFramesCounterRegister_HHD_msmSystemTxVlanFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxVlanFramesCounter_1 in AQ_MsmSystemTxVlanFramesCounterRegister_HHD */
#define AQ_MsmSystemTxVlanFramesCounterRegister_HHD_msmSystemTxVlanFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxVlanFramesCounter_1 in AQ_MsmSystemTxVlanFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxVlanFramesCounterRegister_HHD_msmSystemTxVlanFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxVlanFramesCounter_1 in AQ_MsmSystemTxVlanFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxVlanFramesCounterRegister_HHD_msmSystemTxVlanFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxVlanFramesCounterRegister_HHD */
#define AQ_MsmSystemRxVlanFramesCounterRegister_HHD_baseRegisterAddress 0x6064
/*! \brief MMD address of structure AQ_MsmSystemRxVlanFramesCounterRegister_HHD */
#define AQ_MsmSystemRxVlanFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxVlanFramesCounter_0 in AQ_MsmSystemRxVlanFramesCounterRegister_HHD */
#define AQ_MsmSystemRxVlanFramesCounterRegister_HHD_msmSystemRxVlanFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxVlanFramesCounter_0 in AQ_MsmSystemRxVlanFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxVlanFramesCounterRegister_HHD_msmSystemRxVlanFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxVlanFramesCounter_0 in AQ_MsmSystemRxVlanFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxVlanFramesCounterRegister_HHD_msmSystemRxVlanFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxVlanFramesCounter_1 in AQ_MsmSystemRxVlanFramesCounterRegister_HHD */
#define AQ_MsmSystemRxVlanFramesCounterRegister_HHD_msmSystemRxVlanFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxVlanFramesCounter_1 in AQ_MsmSystemRxVlanFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxVlanFramesCounterRegister_HHD_msmSystemRxVlanFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxVlanFramesCounter_1 in AQ_MsmSystemRxVlanFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxVlanFramesCounterRegister_HHD_msmSystemRxVlanFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define AQ_MsmSystemTxOctetsCounterRegister_HHD_baseRegisterAddress 0x6068
/*! \brief MMD address of structure AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define AQ_MsmSystemTxOctetsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxOctetsCounter_0 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxOctetsCounter_0 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define bits_AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxOctetsCounter_0 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define word_AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxOctetsCounter_1 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxOctetsCounter_1 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define bits_AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxOctetsCounter_1 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define word_AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_1 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxOctetsCounter_2 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_2 2
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxOctetsCounter_2 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define bits_AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_2 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxOctetsCounter_2 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define word_AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_2 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxOctetsCounter_3 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_3 3
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxOctetsCounter_3 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define bits_AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_3 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxOctetsCounter_3 in AQ_MsmSystemTxOctetsCounterRegister_HHD */
#define word_AQ_MsmSystemTxOctetsCounterRegister_HHD_msmSystemTxOctetsCounter_3 u3.word_3

/*! \brief Base register address of structure AQ_MsmSystemRxOctetsCounterRegister_HHD */
#define AQ_MsmSystemRxOctetsCounterRegister_HHD_baseRegisterAddress 0x606C
/*! \brief MMD address of structure AQ_MsmSystemRxOctetsCounterRegister_HHD */
#define AQ_MsmSystemRxOctetsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxOctetsCounter_0 in AQ_MsmSystemRxOctetsCounterRegister_HHD */
#define AQ_MsmSystemRxOctetsCounterRegister_HHD_msmSystemRxOctetsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxOctetsCounter_0 in AQ_MsmSystemRxOctetsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxOctetsCounterRegister_HHD_msmSystemRxOctetsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxOctetsCounter_0 in AQ_MsmSystemRxOctetsCounterRegister_HHD */
#define word_AQ_MsmSystemRxOctetsCounterRegister_HHD_msmSystemRxOctetsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxOctetsCounter_1 in AQ_MsmSystemRxOctetsCounterRegister_HHD */
#define AQ_MsmSystemRxOctetsCounterRegister_HHD_msmSystemRxOctetsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxOctetsCounter_1 in AQ_MsmSystemRxOctetsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxOctetsCounterRegister_HHD_msmSystemRxOctetsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxOctetsCounter_1 in AQ_MsmSystemRxOctetsCounterRegister_HHD */
#define word_AQ_MsmSystemRxOctetsCounterRegister_HHD_msmSystemRxOctetsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxUnicastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxUnicastFramesCounterRegister_HHD_baseRegisterAddress 0x6070
/*! \brief MMD address of structure AQ_MsmSystemRxUnicastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxUnicastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxUnicastFramesCounter_0 in AQ_MsmSystemRxUnicastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxUnicastFramesCounterRegister_HHD_msmSystemRxUnicastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxUnicastFramesCounter_0 in AQ_MsmSystemRxUnicastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxUnicastFramesCounterRegister_HHD_msmSystemRxUnicastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxUnicastFramesCounter_0 in AQ_MsmSystemRxUnicastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxUnicastFramesCounterRegister_HHD_msmSystemRxUnicastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxUnicastFramesCounter_1 in AQ_MsmSystemRxUnicastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxUnicastFramesCounterRegister_HHD_msmSystemRxUnicastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxUnicastFramesCounter_1 in AQ_MsmSystemRxUnicastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxUnicastFramesCounterRegister_HHD_msmSystemRxUnicastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxUnicastFramesCounter_1 in AQ_MsmSystemRxUnicastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxUnicastFramesCounterRegister_HHD_msmSystemRxUnicastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxMulticastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxMulticastFramesCounterRegister_HHD_baseRegisterAddress 0x6074
/*! \brief MMD address of structure AQ_MsmSystemRxMulticastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxMulticastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxMulticastFramesCounter_0 in AQ_MsmSystemRxMulticastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxMulticastFramesCounterRegister_HHD_msmSystemRxMulticastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxMulticastFramesCounter_0 in AQ_MsmSystemRxMulticastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxMulticastFramesCounterRegister_HHD_msmSystemRxMulticastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxMulticastFramesCounter_0 in AQ_MsmSystemRxMulticastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxMulticastFramesCounterRegister_HHD_msmSystemRxMulticastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxMulticastFramesCounter_1 in AQ_MsmSystemRxMulticastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxMulticastFramesCounterRegister_HHD_msmSystemRxMulticastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxMulticastFramesCounter_1 in AQ_MsmSystemRxMulticastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxMulticastFramesCounterRegister_HHD_msmSystemRxMulticastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxMulticastFramesCounter_1 in AQ_MsmSystemRxMulticastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxMulticastFramesCounterRegister_HHD_msmSystemRxMulticastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD_baseRegisterAddress 0x6078
/*! \brief MMD address of structure AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxBroadcastFramesCounter_0 in AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD_msmSystemRxBroadcastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxBroadcastFramesCounter_0 in AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD_msmSystemRxBroadcastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxBroadcastFramesCounter_0 in AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD_msmSystemRxBroadcastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxBroadcastFramesCounter_1 in AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD_msmSystemRxBroadcastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxBroadcastFramesCounter_1 in AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD_msmSystemRxBroadcastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxBroadcastFramesCounter_1 in AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD_msmSystemRxBroadcastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemTxErrorsCounterRegister_HHD */
#define AQ_MsmSystemTxErrorsCounterRegister_HHD_baseRegisterAddress 0x607C
/*! \brief MMD address of structure AQ_MsmSystemTxErrorsCounterRegister_HHD */
#define AQ_MsmSystemTxErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxErrorsCounter_0 in AQ_MsmSystemTxErrorsCounterRegister_HHD */
#define AQ_MsmSystemTxErrorsCounterRegister_HHD_msmSystemTxErrorsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxErrorsCounter_0 in AQ_MsmSystemTxErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemTxErrorsCounterRegister_HHD_msmSystemTxErrorsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxErrorsCounter_0 in AQ_MsmSystemTxErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemTxErrorsCounterRegister_HHD_msmSystemTxErrorsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxErrorsCounter_1 in AQ_MsmSystemTxErrorsCounterRegister_HHD */
#define AQ_MsmSystemTxErrorsCounterRegister_HHD_msmSystemTxErrorsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxErrorsCounter_1 in AQ_MsmSystemTxErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemTxErrorsCounterRegister_HHD_msmSystemTxErrorsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxErrorsCounter_1 in AQ_MsmSystemTxErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemTxErrorsCounterRegister_HHD_msmSystemTxErrorsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemTxUnicastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxUnicastFramesCounterRegister_HHD_baseRegisterAddress 0x6084
/*! \brief MMD address of structure AQ_MsmSystemTxUnicastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxUnicastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxUnicastFramesCounter_0 in AQ_MsmSystemTxUnicastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxUnicastFramesCounterRegister_HHD_msmSystemTxUnicastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxUnicastFramesCounter_0 in AQ_MsmSystemTxUnicastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxUnicastFramesCounterRegister_HHD_msmSystemTxUnicastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxUnicastFramesCounter_0 in AQ_MsmSystemTxUnicastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxUnicastFramesCounterRegister_HHD_msmSystemTxUnicastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxUnicastFramesCounter_1 in AQ_MsmSystemTxUnicastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxUnicastFramesCounterRegister_HHD_msmSystemTxUnicastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxUnicastFramesCounter_1 in AQ_MsmSystemTxUnicastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxUnicastFramesCounterRegister_HHD_msmSystemTxUnicastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxUnicastFramesCounter_1 in AQ_MsmSystemTxUnicastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxUnicastFramesCounterRegister_HHD_msmSystemTxUnicastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemTxMulticastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxMulticastFramesCounterRegister_HHD_baseRegisterAddress 0x6088
/*! \brief MMD address of structure AQ_MsmSystemTxMulticastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxMulticastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxMulticastFramesCounter_0 in AQ_MsmSystemTxMulticastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxMulticastFramesCounterRegister_HHD_msmSystemTxMulticastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxMulticastFramesCounter_0 in AQ_MsmSystemTxMulticastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxMulticastFramesCounterRegister_HHD_msmSystemTxMulticastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxMulticastFramesCounter_0 in AQ_MsmSystemTxMulticastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxMulticastFramesCounterRegister_HHD_msmSystemTxMulticastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxMulticastFramesCounter_1 in AQ_MsmSystemTxMulticastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxMulticastFramesCounterRegister_HHD_msmSystemTxMulticastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxMulticastFramesCounter_1 in AQ_MsmSystemTxMulticastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxMulticastFramesCounterRegister_HHD_msmSystemTxMulticastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxMulticastFramesCounter_1 in AQ_MsmSystemTxMulticastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxMulticastFramesCounterRegister_HHD_msmSystemTxMulticastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD_baseRegisterAddress 0x608C
/*! \brief MMD address of structure AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxBroadcastFramesCounter_0 in AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD_msmSystemTxBroadcastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxBroadcastFramesCounter_0 in AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD_msmSystemTxBroadcastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxBroadcastFramesCounter_0 in AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD_msmSystemTxBroadcastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemTxBroadcastFramesCounter_1 in AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD_msmSystemTxBroadcastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemTxBroadcastFramesCounter_1 in AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD */
#define bits_AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD_msmSystemTxBroadcastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemTxBroadcastFramesCounter_1 in AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD */
#define word_AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD_msmSystemTxBroadcastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmSystemRxErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxErrorsCounterRegister_HHD_baseRegisterAddress 0x60C8
/*! \brief MMD address of structure AQ_MsmSystemRxErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxErrorsCounter_0 in AQ_MsmSystemRxErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxErrorsCounterRegister_HHD_msmSystemRxErrorsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxErrorsCounter_0 in AQ_MsmSystemRxErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxErrorsCounterRegister_HHD_msmSystemRxErrorsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxErrorsCounter_0 in AQ_MsmSystemRxErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxErrorsCounterRegister_HHD_msmSystemRxErrorsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmSystemRxErrorsCounter_1 in AQ_MsmSystemRxErrorsCounterRegister_HHD */
#define AQ_MsmSystemRxErrorsCounterRegister_HHD_msmSystemRxErrorsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmSystemRxErrorsCounter_1 in AQ_MsmSystemRxErrorsCounterRegister_HHD */
#define bits_AQ_MsmSystemRxErrorsCounterRegister_HHD_msmSystemRxErrorsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmSystemRxErrorsCounter_1 in AQ_MsmSystemRxErrorsCounterRegister_HHD */
#define word_AQ_MsmSystemRxErrorsCounterRegister_HHD_msmSystemRxErrorsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MssIngressVlanTpid_0Register_HHD */
#define AQ_MssIngressVlanTpid_0Register_HHD_baseRegisterAddress 0x8006
/*! \brief MMD address of structure AQ_MssIngressVlanTpid_0Register_HHD */
#define AQ_MssIngressVlanTpid_0Register_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanStag in AQ_MssIngressVlanTpid_0Register_HHD */
#define AQ_MssIngressVlanTpid_0Register_HHD_mssIngressVlanStag 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanStag in AQ_MssIngressVlanTpid_0Register_HHD */
#define bits_AQ_MssIngressVlanTpid_0Register_HHD_mssIngressVlanStag u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanStag in AQ_MssIngressVlanTpid_0Register_HHD */
#define word_AQ_MssIngressVlanTpid_0Register_HHD_mssIngressVlanStag u0.word_0

/*! \brief Base register address of structure AQ_MssIngressVlanTpid_1Register_HHD */
#define AQ_MssIngressVlanTpid_1Register_HHD_baseRegisterAddress 0x8008
/*! \brief MMD address of structure AQ_MssIngressVlanTpid_1Register_HHD */
#define AQ_MssIngressVlanTpid_1Register_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanQtag in AQ_MssIngressVlanTpid_1Register_HHD */
#define AQ_MssIngressVlanTpid_1Register_HHD_mssIngressVlanQtag 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanQtag in AQ_MssIngressVlanTpid_1Register_HHD */
#define bits_AQ_MssIngressVlanTpid_1Register_HHD_mssIngressVlanQtag u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanQtag in AQ_MssIngressVlanTpid_1Register_HHD */
#define word_AQ_MssIngressVlanTpid_1Register_HHD_mssIngressVlanQtag u0.word_0

/*! \brief Base register address of structure AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_baseRegisterAddress 0x800A
/*! \brief MMD address of structure AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanUpMapTableLSW in AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanUpMapTableLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanUpMapTableLSW in AQ_MssIngressVlanControlRegister_HHD */
#define bits_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanUpMapTableLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanUpMapTableLSW in AQ_MssIngressVlanControlRegister_HHD */
#define word_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanUpMapTableLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanQtagParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanQtagParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanQtagParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define bits_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanQtagParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanQtagParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define word_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanQtagParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanStagParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanStagParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanStagParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define bits_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanStagParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanStagParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define word_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanStagParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanQinqParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanQinqParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanQinqParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define bits_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanQinqParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanQinqParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define word_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanQinqParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanQtagUpParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanQtagUpParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanQtagUpParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define bits_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanQtagUpParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanQtagUpParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define word_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanQtagUpParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanStagUpParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanStagUpParseEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanStagUpParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define bits_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanStagUpParseEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanStagUpParseEnable in AQ_MssIngressVlanControlRegister_HHD */
#define word_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanStagUpParseEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanUpDefault in AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanUpDefault 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanUpDefault in AQ_MssIngressVlanControlRegister_HHD */
#define bits_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanUpDefault u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanUpDefault in AQ_MssIngressVlanControlRegister_HHD */
#define word_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanUpDefault u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressVlanUpMapTableMSW in AQ_MssIngressVlanControlRegister_HHD */
#define AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanUpMapTableMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressVlanUpMapTableMSW in AQ_MssIngressVlanControlRegister_HHD */
#define bits_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanUpMapTableMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressVlanUpMapTableMSW in AQ_MssIngressVlanControlRegister_HHD */
#define word_AQ_MssIngressVlanControlRegister_HHD_mssIngressVlanUpMapTableMSW u1.word_1

/*! \brief Base register address of structure AQ_MssIngressMtuSizeControlRegister_HHD */
#define AQ_MssIngressMtuSizeControlRegister_HHD_baseRegisterAddress 0x800C
/*! \brief MMD address of structure AQ_MssIngressMtuSizeControlRegister_HHD */
#define AQ_MssIngressMtuSizeControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressControlledPacketMtuSize in AQ_MssIngressMtuSizeControlRegister_HHD */
#define AQ_MssIngressMtuSizeControlRegister_HHD_mssIngressControlledPacketMtuSize 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressControlledPacketMtuSize in AQ_MssIngressMtuSizeControlRegister_HHD */
#define bits_AQ_MssIngressMtuSizeControlRegister_HHD_mssIngressControlledPacketMtuSize u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressControlledPacketMtuSize in AQ_MssIngressMtuSizeControlRegister_HHD */
#define word_AQ_MssIngressMtuSizeControlRegister_HHD_mssIngressControlledPacketMtuSize u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressUncontrolledPacketMtuSize in AQ_MssIngressMtuSizeControlRegister_HHD */
#define AQ_MssIngressMtuSizeControlRegister_HHD_mssIngressUncontrolledPacketMtuSize 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressUncontrolledPacketMtuSize in AQ_MssIngressMtuSizeControlRegister_HHD */
#define bits_AQ_MssIngressMtuSizeControlRegister_HHD_mssIngressUncontrolledPacketMtuSize u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressUncontrolledPacketMtuSize in AQ_MssIngressMtuSizeControlRegister_HHD */
#define word_AQ_MssIngressMtuSizeControlRegister_HHD_mssIngressUncontrolledPacketMtuSize u1.word_1

/*! \brief Base register address of structure AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_baseRegisterAddress 0x800E
/*! \brief MMD address of structure AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressIcvLsb_8BytesEnable in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressIcvLsb_8BytesEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressIcvLsb_8BytesEnable in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressIcvLsb_8BytesEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressIcvLsb_8BytesEnable in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressIcvLsb_8BytesEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressGlobalValidateFrames in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressGlobalValidateFrames 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressGlobalValidateFrames in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressGlobalValidateFrames u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressGlobalValidateFrames in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressGlobalValidateFrames u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressRemoveSectag in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressRemoveSectag 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressRemoveSectag in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressRemoveSectag u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressRemoveSectag in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressRemoveSectag u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressHighPriority in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressHighPriority 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressHighPriority in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressHighPriority u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressHighPriority in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressHighPriority u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressClearCount in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressClearCount 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressClearCount in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressClearCount u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressClearCount in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressClearCount u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressClearGlobalTime in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressClearGlobalTime 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressClearGlobalTime in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressClearGlobalTime u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressClearGlobalTime in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressClearGlobalTime u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressCheckIcv in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressCheckIcv 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressCheckIcv in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressCheckIcv u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressCheckIcv in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressCheckIcv u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressDropIgprcMiss in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressDropIgprcMiss 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressDropIgprcMiss in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressDropIgprcMiss u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressDropIgprcMiss in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressDropIgprcMiss u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressDropKayPacket in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressDropKayPacket 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressDropKayPacket in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressDropKayPacket u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressDropKayPacket in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressDropKayPacket u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressMaskShortLengthError in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressMaskShortLengthError 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressMaskShortLengthError in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressMaskShortLengthError u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressMaskShortLengthError in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressMaskShortLengthError u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressCreateSci in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressCreateSci 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressCreateSci in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressCreateSci u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressCreateSci in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressCreateSci u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressOperationPointToPoint in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressOperationPointToPoint 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressOperationPointToPoint in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressOperationPointToPoint u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressOperationPointToPoint in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressOperationPointToPoint u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSoftReset in AQ_MssIngressControlRegister_HHD */
#define AQ_MssIngressControlRegister_HHD_mssIngressSoftReset 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSoftReset in AQ_MssIngressControlRegister_HHD */
#define bits_AQ_MssIngressControlRegister_HHD_mssIngressSoftReset u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSoftReset in AQ_MssIngressControlRegister_HHD */
#define word_AQ_MssIngressControlRegister_HHD_mssIngressSoftReset u0.word_0

/*! \brief Base register address of structure AQ_MssIngressSaControlRegister_HHD */
#define AQ_MssIngressSaControlRegister_HHD_baseRegisterAddress 0x8010
/*! \brief MMD address of structure AQ_MssIngressSaControlRegister_HHD */
#define AQ_MssIngressSaControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaThresholdLSW in AQ_MssIngressSaControlRegister_HHD */
#define AQ_MssIngressSaControlRegister_HHD_mssIngressSaThresholdLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaThresholdLSW in AQ_MssIngressSaControlRegister_HHD */
#define bits_AQ_MssIngressSaControlRegister_HHD_mssIngressSaThresholdLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaThresholdLSW in AQ_MssIngressSaControlRegister_HHD */
#define word_AQ_MssIngressSaControlRegister_HHD_mssIngressSaThresholdLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaThresholdMSW in AQ_MssIngressSaControlRegister_HHD */
#define AQ_MssIngressSaControlRegister_HHD_mssIngressSaThresholdMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaThresholdMSW in AQ_MssIngressSaControlRegister_HHD */
#define bits_AQ_MssIngressSaControlRegister_HHD_mssIngressSaThresholdMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaThresholdMSW in AQ_MssIngressSaControlRegister_HHD */
#define word_AQ_MssIngressSaControlRegister_HHD_mssIngressSaThresholdMSW u1.word_1

/*! \brief Base register address of structure AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_baseRegisterAddress 0x802E
/*! \brief MMD address of structure AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressIgpocMissInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mssIngressIgpocMissInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressIgpocMissInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressIgpocMissInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressIgpocMissInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define word_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressIgpocMissInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressTciE_cErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mssIngressTciE_cErrorInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressTciE_cErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressTciE_cErrorInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressTciE_cErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define word_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressTciE_cErrorInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressEccErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mssIngressEccErrorInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressEccErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressEccErrorInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressEccErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define word_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressEccErrorInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressMibSaturationInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mssIngressMibSaturationInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressMibSaturationInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressMibSaturationInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressMibSaturationInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define word_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressMibSaturationInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressReplayErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mssIngressReplayErrorInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressReplayErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressReplayErrorInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressReplayErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define word_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressReplayErrorInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressIcvErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mssIngressIcvErrorInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressIcvErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressIcvErrorInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressIcvErrorInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define word_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressIcvErrorInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaThresholdExpiredInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mssIngressSaThresholdExpiredInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaThresholdExpiredInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressSaThresholdExpiredInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaThresholdExpiredInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define word_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressSaThresholdExpiredInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaExpiredInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mssIngressSaExpiredInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaExpiredInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressSaExpiredInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaExpiredInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define word_AQ_MssIngressInterruptStatusRegister_HHD_mssIngressSaExpiredInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssMasterIngressInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define AQ_MssIngressInterruptStatusRegister_HHD_mssMasterIngressInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssMasterIngressInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressInterruptStatusRegister_HHD_mssMasterIngressInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssMasterIngressInterrupt in AQ_MssIngressInterruptStatusRegister_HHD */
#define word_AQ_MssIngressInterruptStatusRegister_HHD_mssMasterIngressInterrupt u0.word_0

/*! \brief Base register address of structure AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_baseRegisterAddress 0x8030
/*! \brief MMD address of structure AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressIgpocMissInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mssIngressIgpocMissInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressIgpocMissInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define bits_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressIgpocMissInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressIgpocMissInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define word_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressIgpocMissInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressTciE_cErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mssIngressTciE_cErrorInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressTciE_cErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define bits_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressTciE_cErrorInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressTciE_cErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define word_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressTciE_cErrorInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressEccErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mssIngressEccErrorInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressEccErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define bits_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressEccErrorInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressEccErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define word_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressEccErrorInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressMibSaturationInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mssIngressMibSaturationInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressMibSaturationInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define bits_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressMibSaturationInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressMibSaturationInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define word_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressMibSaturationInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressReplayErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mssIngressReplayErrorInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressReplayErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define bits_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressReplayErrorInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressReplayErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define word_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressReplayErrorInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressIcvErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mssIngressIcvErrorInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressIcvErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define bits_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressIcvErrorInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressIcvErrorInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define word_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressIcvErrorInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaThresholdExpiredInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mssIngressSaThresholdExpiredInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaThresholdExpiredInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define bits_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressSaThresholdExpiredInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaThresholdExpiredInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define word_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressSaThresholdExpiredInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaExpiredInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mssIngressSaExpiredInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaExpiredInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define bits_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressSaExpiredInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaExpiredInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define word_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressSaExpiredInterruptEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressMasterInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define AQ_MssIngressInterruptMaskRegister_HHD_mssIngressMasterInterruptEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressMasterInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define bits_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressMasterInterruptEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressMasterInterruptEnable in AQ_MssIngressInterruptMaskRegister_HHD */
#define word_AQ_MssIngressInterruptMaskRegister_HHD_mssIngressMasterInterruptEnable u0.word_0

/*! \brief Base register address of structure AQ_MssIngressSaIcvErrorStatusRegister_HHD */
#define AQ_MssIngressSaIcvErrorStatusRegister_HHD_baseRegisterAddress 0x8032
/*! \brief MMD address of structure AQ_MssIngressSaIcvErrorStatusRegister_HHD */
#define AQ_MssIngressSaIcvErrorStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaIcvErrorLSW in AQ_MssIngressSaIcvErrorStatusRegister_HHD */
#define AQ_MssIngressSaIcvErrorStatusRegister_HHD_mssIngressSaIcvErrorLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaIcvErrorLSW in AQ_MssIngressSaIcvErrorStatusRegister_HHD */
#define bits_AQ_MssIngressSaIcvErrorStatusRegister_HHD_mssIngressSaIcvErrorLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaIcvErrorLSW in AQ_MssIngressSaIcvErrorStatusRegister_HHD */
#define word_AQ_MssIngressSaIcvErrorStatusRegister_HHD_mssIngressSaIcvErrorLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaIcvErrorMSW in AQ_MssIngressSaIcvErrorStatusRegister_HHD */
#define AQ_MssIngressSaIcvErrorStatusRegister_HHD_mssIngressSaIcvErrorMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaIcvErrorMSW in AQ_MssIngressSaIcvErrorStatusRegister_HHD */
#define bits_AQ_MssIngressSaIcvErrorStatusRegister_HHD_mssIngressSaIcvErrorMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaIcvErrorMSW in AQ_MssIngressSaIcvErrorStatusRegister_HHD */
#define word_AQ_MssIngressSaIcvErrorStatusRegister_HHD_mssIngressSaIcvErrorMSW u1.word_1

/*! \brief Base register address of structure AQ_MssIngressSaReplayErrorStatusRegister_HHD */
#define AQ_MssIngressSaReplayErrorStatusRegister_HHD_baseRegisterAddress 0x8034
/*! \brief MMD address of structure AQ_MssIngressSaReplayErrorStatusRegister_HHD */
#define AQ_MssIngressSaReplayErrorStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaReplayErrorLSW in AQ_MssIngressSaReplayErrorStatusRegister_HHD */
#define AQ_MssIngressSaReplayErrorStatusRegister_HHD_mssIngressSaReplayErrorLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaReplayErrorLSW in AQ_MssIngressSaReplayErrorStatusRegister_HHD */
#define bits_AQ_MssIngressSaReplayErrorStatusRegister_HHD_mssIngressSaReplayErrorLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaReplayErrorLSW in AQ_MssIngressSaReplayErrorStatusRegister_HHD */
#define word_AQ_MssIngressSaReplayErrorStatusRegister_HHD_mssIngressSaReplayErrorLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaReplayErrorMSW in AQ_MssIngressSaReplayErrorStatusRegister_HHD */
#define AQ_MssIngressSaReplayErrorStatusRegister_HHD_mssIngressSaReplayErrorMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaReplayErrorMSW in AQ_MssIngressSaReplayErrorStatusRegister_HHD */
#define bits_AQ_MssIngressSaReplayErrorStatusRegister_HHD_mssIngressSaReplayErrorMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaReplayErrorMSW in AQ_MssIngressSaReplayErrorStatusRegister_HHD */
#define word_AQ_MssIngressSaReplayErrorStatusRegister_HHD_mssIngressSaReplayErrorMSW u1.word_1

/*! \brief Base register address of structure AQ_MssIngressSaExpiredStatusRegister_HHD */
#define AQ_MssIngressSaExpiredStatusRegister_HHD_baseRegisterAddress 0x8036
/*! \brief MMD address of structure AQ_MssIngressSaExpiredStatusRegister_HHD */
#define AQ_MssIngressSaExpiredStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaExpiredLSW in AQ_MssIngressSaExpiredStatusRegister_HHD */
#define AQ_MssIngressSaExpiredStatusRegister_HHD_mssIngressSaExpiredLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaExpiredLSW in AQ_MssIngressSaExpiredStatusRegister_HHD */
#define bits_AQ_MssIngressSaExpiredStatusRegister_HHD_mssIngressSaExpiredLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaExpiredLSW in AQ_MssIngressSaExpiredStatusRegister_HHD */
#define word_AQ_MssIngressSaExpiredStatusRegister_HHD_mssIngressSaExpiredLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaExpiredMSW in AQ_MssIngressSaExpiredStatusRegister_HHD */
#define AQ_MssIngressSaExpiredStatusRegister_HHD_mssIngressSaExpiredMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaExpiredMSW in AQ_MssIngressSaExpiredStatusRegister_HHD */
#define bits_AQ_MssIngressSaExpiredStatusRegister_HHD_mssIngressSaExpiredMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaExpiredMSW in AQ_MssIngressSaExpiredStatusRegister_HHD */
#define word_AQ_MssIngressSaExpiredStatusRegister_HHD_mssIngressSaExpiredMSW u1.word_1

/*! \brief Base register address of structure AQ_MssIngressSaThresholdExpiredStatusRegister_HHD */
#define AQ_MssIngressSaThresholdExpiredStatusRegister_HHD_baseRegisterAddress 0x8038
/*! \brief MMD address of structure AQ_MssIngressSaThresholdExpiredStatusRegister_HHD */
#define AQ_MssIngressSaThresholdExpiredStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaThresholdExpiredLSW in AQ_MssIngressSaThresholdExpiredStatusRegister_HHD */
#define AQ_MssIngressSaThresholdExpiredStatusRegister_HHD_mssIngressSaThresholdExpiredLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaThresholdExpiredLSW in AQ_MssIngressSaThresholdExpiredStatusRegister_HHD */
#define bits_AQ_MssIngressSaThresholdExpiredStatusRegister_HHD_mssIngressSaThresholdExpiredLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaThresholdExpiredLSW in AQ_MssIngressSaThresholdExpiredStatusRegister_HHD */
#define word_AQ_MssIngressSaThresholdExpiredStatusRegister_HHD_mssIngressSaThresholdExpiredLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaThresholdExpiredMSW in AQ_MssIngressSaThresholdExpiredStatusRegister_HHD */
#define AQ_MssIngressSaThresholdExpiredStatusRegister_HHD_mssIngressSaThresholdExpiredMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaThresholdExpiredMSW in AQ_MssIngressSaThresholdExpiredStatusRegister_HHD */
#define bits_AQ_MssIngressSaThresholdExpiredStatusRegister_HHD_mssIngressSaThresholdExpiredMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaThresholdExpiredMSW in AQ_MssIngressSaThresholdExpiredStatusRegister_HHD */
#define word_AQ_MssIngressSaThresholdExpiredStatusRegister_HHD_mssIngressSaThresholdExpiredMSW u1.word_1

/*! \brief Base register address of structure AQ_MssIngressEccInterruptStatusRegister_HHD */
#define AQ_MssIngressEccInterruptStatusRegister_HHD_baseRegisterAddress 0x803A
/*! \brief MMD address of structure AQ_MssIngressEccInterruptStatusRegister_HHD */
#define AQ_MssIngressEccInterruptStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaEccErrorInterruptLSW in AQ_MssIngressEccInterruptStatusRegister_HHD */
#define AQ_MssIngressEccInterruptStatusRegister_HHD_mssIngressSaEccErrorInterruptLSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaEccErrorInterruptLSW in AQ_MssIngressEccInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressEccInterruptStatusRegister_HHD_mssIngressSaEccErrorInterruptLSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaEccErrorInterruptLSW in AQ_MssIngressEccInterruptStatusRegister_HHD */
#define word_AQ_MssIngressEccInterruptStatusRegister_HHD_mssIngressSaEccErrorInterruptLSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressSaEccErrorInterruptMSW in AQ_MssIngressEccInterruptStatusRegister_HHD */
#define AQ_MssIngressEccInterruptStatusRegister_HHD_mssIngressSaEccErrorInterruptMSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressSaEccErrorInterruptMSW in AQ_MssIngressEccInterruptStatusRegister_HHD */
#define bits_AQ_MssIngressEccInterruptStatusRegister_HHD_mssIngressSaEccErrorInterruptMSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressSaEccErrorInterruptMSW in AQ_MssIngressEccInterruptStatusRegister_HHD */
#define word_AQ_MssIngressEccInterruptStatusRegister_HHD_mssIngressSaEccErrorInterruptMSW u1.word_1

/*! \brief Base register address of structure AQ_MssIngressLutAddressControlRegister_HHD */
#define AQ_MssIngressLutAddressControlRegister_HHD_baseRegisterAddress 0x8080
/*! \brief MMD address of structure AQ_MssIngressLutAddressControlRegister_HHD */
#define AQ_MssIngressLutAddressControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutSelect in AQ_MssIngressLutAddressControlRegister_HHD */
#define AQ_MssIngressLutAddressControlRegister_HHD_mssIngressLutSelect 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutSelect in AQ_MssIngressLutAddressControlRegister_HHD */
#define bits_AQ_MssIngressLutAddressControlRegister_HHD_mssIngressLutSelect u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutSelect in AQ_MssIngressLutAddressControlRegister_HHD */
#define word_AQ_MssIngressLutAddressControlRegister_HHD_mssIngressLutSelect u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutAddress in AQ_MssIngressLutAddressControlRegister_HHD */
#define AQ_MssIngressLutAddressControlRegister_HHD_mssIngressLutAddress 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutAddress in AQ_MssIngressLutAddressControlRegister_HHD */
#define bits_AQ_MssIngressLutAddressControlRegister_HHD_mssIngressLutAddress u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutAddress in AQ_MssIngressLutAddressControlRegister_HHD */
#define word_AQ_MssIngressLutAddressControlRegister_HHD_mssIngressLutAddress u0.word_0

/*! \brief Base register address of structure AQ_MssIngressLutControlRegister_HHD */
#define AQ_MssIngressLutControlRegister_HHD_baseRegisterAddress 0x8081
/*! \brief MMD address of structure AQ_MssIngressLutControlRegister_HHD */
#define AQ_MssIngressLutControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutWrite in AQ_MssIngressLutControlRegister_HHD */
#define AQ_MssIngressLutControlRegister_HHD_mssIngressLutWrite 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutWrite in AQ_MssIngressLutControlRegister_HHD */
#define bits_AQ_MssIngressLutControlRegister_HHD_mssIngressLutWrite u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutWrite in AQ_MssIngressLutControlRegister_HHD */
#define word_AQ_MssIngressLutControlRegister_HHD_mssIngressLutWrite u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutRead in AQ_MssIngressLutControlRegister_HHD */
#define AQ_MssIngressLutControlRegister_HHD_mssIngressLutRead 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutRead in AQ_MssIngressLutControlRegister_HHD */
#define bits_AQ_MssIngressLutControlRegister_HHD_mssIngressLutRead u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutRead in AQ_MssIngressLutControlRegister_HHD */
#define word_AQ_MssIngressLutControlRegister_HHD_mssIngressLutRead u0.word_0

/*! \brief Base register address of structure AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_baseRegisterAddress 0x80A0
/*! \brief MMD address of structure AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_0 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_0 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_0 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_1 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_1 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_1 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_1 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_2 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_2 2
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_2 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_2 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_2 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_2 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_3 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_3 3
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_3 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_3 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_3 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_3 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_4 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_4 4
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_4 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_4 u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_4 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_4 u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_5 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_5 5
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_5 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_5 u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_5 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_5 u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_6 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_6 6
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_6 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_6 u6.bits_6
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_6 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_6 u6.word_6
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_7 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_7 7
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_7 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_7 u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_7 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_7 u7.word_7
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_8 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_8 8
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_8 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_8 u8.bits_8
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_8 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_8 u8.word_8
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_9 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_9 9
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_9 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_9 u9.bits_9
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_9 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_9 u9.word_9
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_10 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_10 10
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_10 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_10 u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_10 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_10 u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_11 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_11 11
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_11 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_11 u11.bits_11
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_11 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_11 u11.word_11
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_12 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_12 12
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_12 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_12 u12.bits_12
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_12 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_12 u12.word_12
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_13 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_13 13
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_13 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_13 u13.bits_13
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_13 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_13 u13.word_13
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_14 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_14 14
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_14 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_14 u14.bits_14
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_14 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_14 u14.word_14
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_15 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_15 15
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_15 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_15 u15.bits_15
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_15 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_15 u15.word_15
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_16 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_16 16
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_16 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_16 u16.bits_16
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_16 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_16 u16.word_16
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_17 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_17 17
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_17 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_17 u17.bits_17
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_17 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_17 u17.word_17
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_18 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_18 18
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_18 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_18 u18.bits_18
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_18 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_18 u18.word_18
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_19 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_19 19
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_19 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_19 u19.bits_19
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_19 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_19 u19.word_19
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_20 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_20 20
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_20 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_20 u20.bits_20
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_20 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_20 u20.word_20
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_21 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_21 21
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_21 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_21 u21.bits_21
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_21 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_21 u21.word_21
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_22 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_22 22
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_22 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_22 u22.bits_22
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_22 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_22 u22.word_22
/*! \brief Preprocessor variable to relate field to word number in structure mssIngressLutData_23 in AQ_MssIngressLutDataControlRegister_HHD */
#define AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_23 23
/*! \brief Preprocessor variable to relate field to bit position in structure mssIngressLutData_23 in AQ_MssIngressLutDataControlRegister_HHD */
#define bits_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_23 u23.bits_23
/*! \brief Preprocessor variable to relate field to word position in structure mssIngressLutData_23 in AQ_MssIngressLutDataControlRegister_HHD */
#define word_AQ_MssIngressLutDataControlRegister_HHD_mssIngressLutData_23 u23.word_23

/*! \brief Base register address of structure AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_baseRegisterAddress 0x9004
/*! \brief MMD address of structure AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLinePhyTxEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLinePhyTxEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLinePhyTxEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLinePhyTxEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLinePhyTxEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLinePhyTxEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxErrorDiscard in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineRxErrorDiscard 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxErrorDiscard in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineRxErrorDiscard u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxErrorDiscard in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineRxErrorDiscard u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineControlFrameEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineControlFrameEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineControlFrameEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineControlFrameEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineControlFrameEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineControlFrameEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineSoftReset in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineSoftReset 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineSoftReset in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineSoftReset u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineSoftReset in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineSoftReset u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxPadEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineTxPadEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxPadEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxPadEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxPadEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxPadEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxCrcAppend in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineTxCrcAppend 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxCrcAppend in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxCrcAppend u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxCrcAppend in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxCrcAppend u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxAddressInsertEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineTxAddressInsertEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxAddressInsertEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxAddressInsertEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxAddressInsertEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxAddressInsertEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLinePauseIgnore in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLinePauseIgnore 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLinePauseIgnore in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLinePauseIgnore u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLinePauseIgnore in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLinePauseIgnore u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLinePauseForward in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLinePauseForward 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLinePauseForward in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLinePauseForward u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLinePauseForward in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLinePauseForward u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineCrcForward in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineCrcForward 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineCrcForward in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineCrcForward u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineCrcForward in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineCrcForward u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLinePadEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLinePadEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLinePadEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLinePadEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLinePadEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLinePadEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLinePromiscuousMode in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLinePromiscuousMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLinePromiscuousMode in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLinePromiscuousMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLinePromiscuousMode in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLinePromiscuousMode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineWanMode in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineWanMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineWanMode in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineWanMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineWanMode in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineWanMode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineRxEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineRxEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineRxEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineTxEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxLowPowerIdleEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineTxLowPowerIdleEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxLowPowerIdleEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxLowPowerIdleEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxLowPowerIdleEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineTxLowPowerIdleEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmLineSfdCheckDisable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineSfdCheckDisable 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineSfdCheckDisable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineSfdCheckDisable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineSfdCheckDisable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineSfdCheckDisable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmLinePriorityFlowControlEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLinePriorityFlowControlEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLinePriorityFlowControlEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLinePriorityFlowControlEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLinePriorityFlowControlEnable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLinePriorityFlowControlEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmLineIdleColumnCountExtend in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineIdleColumnCountExtend 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineIdleColumnCountExtend in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineIdleColumnCountExtend u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineIdleColumnCountExtend in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineIdleColumnCountExtend u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmLineLengthCheckDisable in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineLengthCheckDisable 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineLengthCheckDisable in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineLengthCheckDisable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineLengthCheckDisable in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineLengthCheckDisable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmLineForceSendIdle in AQ_MsmLineGeneralControlRegister_HHD */
#define AQ_MsmLineGeneralControlRegister_HHD_msmLineForceSendIdle 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineForceSendIdle in AQ_MsmLineGeneralControlRegister_HHD */
#define bits_AQ_MsmLineGeneralControlRegister_HHD_msmLineForceSendIdle u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineForceSendIdle in AQ_MsmLineGeneralControlRegister_HHD */
#define word_AQ_MsmLineGeneralControlRegister_HHD_msmLineForceSendIdle u1.word_1

/*! \brief Base register address of structure AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_baseRegisterAddress 0x900E
/*! \brief MMD address of structure AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxFifoFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoFullThreshold 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxFifoFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define bits_AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoFullThreshold u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxFifoFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define word_AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoFullThreshold u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxFifoEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoEmptyThreshold 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxFifoEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define bits_AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoEmptyThreshold u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxFifoEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define word_AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoEmptyThreshold u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxFifoFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoFullThreshold 2
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxFifoFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define bits_AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoFullThreshold u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxFifoFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define word_AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoFullThreshold u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxFifoEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoEmptyThreshold 3
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxFifoEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define bits_AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoEmptyThreshold u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxFifoEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define word_AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoEmptyThreshold u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxFifoAlmostFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoAlmostFullThreshold 4
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxFifoAlmostFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define bits_AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoAlmostFullThreshold u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxFifoAlmostFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define word_AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoAlmostFullThreshold u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxFifoAlmostEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoAlmostEmptyThreshold 5
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxFifoAlmostEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define bits_AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoAlmostEmptyThreshold u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxFifoAlmostEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define word_AQ_MsmLineFifoControlRegister_HHD_msmLineRxFifoAlmostEmptyThreshold u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxFifoAlmostFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoAlmostFullThreshold 6
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxFifoAlmostFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define bits_AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoAlmostFullThreshold u6.bits_6
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxFifoAlmostFullThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define word_AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoAlmostFullThreshold u6.word_6
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxFifoAlmostEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoAlmostEmptyThreshold 7
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxFifoAlmostEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define bits_AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoAlmostEmptyThreshold u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxFifoAlmostEmptyThreshold in AQ_MsmLineFifoControlRegister_HHD */
#define word_AQ_MsmLineFifoControlRegister_HHD_msmLineTxFifoAlmostEmptyThreshold u7.word_7

/*! \brief Base register address of structure AQ_MsmLineGeneralStatusRegister_HHD */
#define AQ_MsmLineGeneralStatusRegister_HHD_baseRegisterAddress 0x9020
/*! \brief MMD address of structure AQ_MsmLineGeneralStatusRegister_HHD */
#define AQ_MsmLineGeneralStatusRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxFifoEmpty in AQ_MsmLineGeneralStatusRegister_HHD */
#define AQ_MsmLineGeneralStatusRegister_HHD_msmLineTxFifoEmpty 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxFifoEmpty in AQ_MsmLineGeneralStatusRegister_HHD */
#define bits_AQ_MsmLineGeneralStatusRegister_HHD_msmLineTxFifoEmpty u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxFifoEmpty in AQ_MsmLineGeneralStatusRegister_HHD */
#define word_AQ_MsmLineGeneralStatusRegister_HHD_msmLineTxFifoEmpty u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxLowPowerIdle in AQ_MsmLineGeneralStatusRegister_HHD */
#define AQ_MsmLineGeneralStatusRegister_HHD_msmLineRxLowPowerIdle 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxLowPowerIdle in AQ_MsmLineGeneralStatusRegister_HHD */
#define bits_AQ_MsmLineGeneralStatusRegister_HHD_msmLineRxLowPowerIdle u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxLowPowerIdle in AQ_MsmLineGeneralStatusRegister_HHD */
#define word_AQ_MsmLineGeneralStatusRegister_HHD_msmLineRxLowPowerIdle u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTimestampAvailable in AQ_MsmLineGeneralStatusRegister_HHD */
#define AQ_MsmLineGeneralStatusRegister_HHD_msmLineTimestampAvailable 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTimestampAvailable in AQ_MsmLineGeneralStatusRegister_HHD */
#define bits_AQ_MsmLineGeneralStatusRegister_HHD_msmLineTimestampAvailable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTimestampAvailable in AQ_MsmLineGeneralStatusRegister_HHD */
#define word_AQ_MsmLineGeneralStatusRegister_HHD_msmLineTimestampAvailable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLinePhyLossOfSignal in AQ_MsmLineGeneralStatusRegister_HHD */
#define AQ_MsmLineGeneralStatusRegister_HHD_msmLinePhyLossOfSignal 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLinePhyLossOfSignal in AQ_MsmLineGeneralStatusRegister_HHD */
#define bits_AQ_MsmLineGeneralStatusRegister_HHD_msmLinePhyLossOfSignal u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLinePhyLossOfSignal in AQ_MsmLineGeneralStatusRegister_HHD */
#define word_AQ_MsmLineGeneralStatusRegister_HHD_msmLinePhyLossOfSignal u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxRemoteFault in AQ_MsmLineGeneralStatusRegister_HHD */
#define AQ_MsmLineGeneralStatusRegister_HHD_msmLineRxRemoteFault 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxRemoteFault in AQ_MsmLineGeneralStatusRegister_HHD */
#define bits_AQ_MsmLineGeneralStatusRegister_HHD_msmLineRxRemoteFault u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxRemoteFault in AQ_MsmLineGeneralStatusRegister_HHD */
#define word_AQ_MsmLineGeneralStatusRegister_HHD_msmLineRxRemoteFault u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxLocalFault in AQ_MsmLineGeneralStatusRegister_HHD */
#define AQ_MsmLineGeneralStatusRegister_HHD_msmLineRxLocalFault 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxLocalFault in AQ_MsmLineGeneralStatusRegister_HHD */
#define bits_AQ_MsmLineGeneralStatusRegister_HHD_msmLineRxLocalFault u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxLocalFault in AQ_MsmLineGeneralStatusRegister_HHD */
#define word_AQ_MsmLineGeneralStatusRegister_HHD_msmLineRxLocalFault u0.word_0

/*! \brief Base register address of structure AQ_MsmLineTxIpgControlRegister_HHD */
#define AQ_MsmLineTxIpgControlRegister_HHD_baseRegisterAddress 0x9022
/*! \brief MMD address of structure AQ_MsmLineTxIpgControlRegister_HHD */
#define AQ_MsmLineTxIpgControlRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxIpgLength in AQ_MsmLineTxIpgControlRegister_HHD */
#define AQ_MsmLineTxIpgControlRegister_HHD_msmLineTxIpgLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxIpgLength in AQ_MsmLineTxIpgControlRegister_HHD */
#define bits_AQ_MsmLineTxIpgControlRegister_HHD_msmLineTxIpgLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxIpgLength in AQ_MsmLineTxIpgControlRegister_HHD */
#define word_AQ_MsmLineTxIpgControlRegister_HHD_msmLineTxIpgLength u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxIpgReserved in AQ_MsmLineTxIpgControlRegister_HHD */
#define AQ_MsmLineTxIpgControlRegister_HHD_msmLineTxIpgReserved 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxIpgReserved in AQ_MsmLineTxIpgControlRegister_HHD */
#define bits_AQ_MsmLineTxIpgControlRegister_HHD_msmLineTxIpgReserved u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxIpgReserved in AQ_MsmLineTxIpgControlRegister_HHD */
#define word_AQ_MsmLineTxIpgControlRegister_HHD_msmLineTxIpgReserved u1.word_1

/*! \brief Base register address of structure AQ_MsmLineTxGoodFramesCounterRegister_HHD */
#define AQ_MsmLineTxGoodFramesCounterRegister_HHD_baseRegisterAddress 0x9040
/*! \brief MMD address of structure AQ_MsmLineTxGoodFramesCounterRegister_HHD */
#define AQ_MsmLineTxGoodFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxGoodFramesCounter_0 in AQ_MsmLineTxGoodFramesCounterRegister_HHD */
#define AQ_MsmLineTxGoodFramesCounterRegister_HHD_msmLineTxGoodFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxGoodFramesCounter_0 in AQ_MsmLineTxGoodFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxGoodFramesCounterRegister_HHD_msmLineTxGoodFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxGoodFramesCounter_0 in AQ_MsmLineTxGoodFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxGoodFramesCounterRegister_HHD_msmLineTxGoodFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxGoodFramesCounter_1 in AQ_MsmLineTxGoodFramesCounterRegister_HHD */
#define AQ_MsmLineTxGoodFramesCounterRegister_HHD_msmLineTxGoodFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxGoodFramesCounter_1 in AQ_MsmLineTxGoodFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxGoodFramesCounterRegister_HHD_msmLineTxGoodFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxGoodFramesCounter_1 in AQ_MsmLineTxGoodFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxGoodFramesCounterRegister_HHD_msmLineTxGoodFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxGoodFramesCounterRegister_HHD */
#define AQ_MsmLineRxGoodFramesCounterRegister_HHD_baseRegisterAddress 0x9044
/*! \brief MMD address of structure AQ_MsmLineRxGoodFramesCounterRegister_HHD */
#define AQ_MsmLineRxGoodFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxGoodFramesCounter_0 in AQ_MsmLineRxGoodFramesCounterRegister_HHD */
#define AQ_MsmLineRxGoodFramesCounterRegister_HHD_msmLineRxGoodFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxGoodFramesCounter_0 in AQ_MsmLineRxGoodFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxGoodFramesCounterRegister_HHD_msmLineRxGoodFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxGoodFramesCounter_0 in AQ_MsmLineRxGoodFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxGoodFramesCounterRegister_HHD_msmLineRxGoodFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxGoodFramesCounter_1 in AQ_MsmLineRxGoodFramesCounterRegister_HHD */
#define AQ_MsmLineRxGoodFramesCounterRegister_HHD_msmLineRxGoodFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxGoodFramesCounter_1 in AQ_MsmLineRxGoodFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxGoodFramesCounterRegister_HHD_msmLineRxGoodFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxGoodFramesCounter_1 in AQ_MsmLineRxGoodFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxGoodFramesCounterRegister_HHD_msmLineRxGoodFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxFcsErrorsCounterRegister_HHD */
#define AQ_MsmLineRxFcsErrorsCounterRegister_HHD_baseRegisterAddress 0x9048
/*! \brief MMD address of structure AQ_MsmLineRxFcsErrorsCounterRegister_HHD */
#define AQ_MsmLineRxFcsErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineFcsErrorCounter_0 in AQ_MsmLineRxFcsErrorsCounterRegister_HHD */
#define AQ_MsmLineRxFcsErrorsCounterRegister_HHD_msmLineFcsErrorCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineFcsErrorCounter_0 in AQ_MsmLineRxFcsErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxFcsErrorsCounterRegister_HHD_msmLineFcsErrorCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineFcsErrorCounter_0 in AQ_MsmLineRxFcsErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxFcsErrorsCounterRegister_HHD_msmLineFcsErrorCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineFcsErrorCounter_1 in AQ_MsmLineRxFcsErrorsCounterRegister_HHD */
#define AQ_MsmLineRxFcsErrorsCounterRegister_HHD_msmLineFcsErrorCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineFcsErrorCounter_1 in AQ_MsmLineRxFcsErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxFcsErrorsCounterRegister_HHD_msmLineFcsErrorCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineFcsErrorCounter_1 in AQ_MsmLineRxFcsErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxFcsErrorsCounterRegister_HHD_msmLineFcsErrorCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD */
#define AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD_baseRegisterAddress 0x904C
/*! \brief MMD address of structure AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD */
#define AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineAlignmentErrorCounter_0 in AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD */
#define AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD_msmLineAlignmentErrorCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineAlignmentErrorCounter_0 in AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD_msmLineAlignmentErrorCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineAlignmentErrorCounter_0 in AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD_msmLineAlignmentErrorCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineAlignmentErrorCounter_1 in AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD */
#define AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD_msmLineAlignmentErrorCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineAlignmentErrorCounter_1 in AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD_msmLineAlignmentErrorCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineAlignmentErrorCounter_1 in AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD_msmLineAlignmentErrorCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineTxPauseFramesCounterRegister_HHD */
#define AQ_MsmLineTxPauseFramesCounterRegister_HHD_baseRegisterAddress 0x9050
/*! \brief MMD address of structure AQ_MsmLineTxPauseFramesCounterRegister_HHD */
#define AQ_MsmLineTxPauseFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxPauseFramesCounter_0 in AQ_MsmLineTxPauseFramesCounterRegister_HHD */
#define AQ_MsmLineTxPauseFramesCounterRegister_HHD_msmLineTxPauseFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxPauseFramesCounter_0 in AQ_MsmLineTxPauseFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxPauseFramesCounterRegister_HHD_msmLineTxPauseFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxPauseFramesCounter_0 in AQ_MsmLineTxPauseFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxPauseFramesCounterRegister_HHD_msmLineTxPauseFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxPauseFramesCounter_1 in AQ_MsmLineTxPauseFramesCounterRegister_HHD */
#define AQ_MsmLineTxPauseFramesCounterRegister_HHD_msmLineTxPauseFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxPauseFramesCounter_1 in AQ_MsmLineTxPauseFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxPauseFramesCounterRegister_HHD_msmLineTxPauseFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxPauseFramesCounter_1 in AQ_MsmLineTxPauseFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxPauseFramesCounterRegister_HHD_msmLineTxPauseFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxPauseFramesCounterRegister_HHD */
#define AQ_MsmLineRxPauseFramesCounterRegister_HHD_baseRegisterAddress 0x9054
/*! \brief MMD address of structure AQ_MsmLineRxPauseFramesCounterRegister_HHD */
#define AQ_MsmLineRxPauseFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxPauseFramesCounter_0 in AQ_MsmLineRxPauseFramesCounterRegister_HHD */
#define AQ_MsmLineRxPauseFramesCounterRegister_HHD_msmLineRxPauseFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxPauseFramesCounter_0 in AQ_MsmLineRxPauseFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxPauseFramesCounterRegister_HHD_msmLineRxPauseFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxPauseFramesCounter_0 in AQ_MsmLineRxPauseFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxPauseFramesCounterRegister_HHD_msmLineRxPauseFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxPauseFramesCounter_1 in AQ_MsmLineRxPauseFramesCounterRegister_HHD */
#define AQ_MsmLineRxPauseFramesCounterRegister_HHD_msmLineRxPauseFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxPauseFramesCounter_1 in AQ_MsmLineRxPauseFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxPauseFramesCounterRegister_HHD_msmLineRxPauseFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxPauseFramesCounter_1 in AQ_MsmLineRxPauseFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxPauseFramesCounterRegister_HHD_msmLineRxPauseFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxTooLongErrorsCounterRegister_HHD */
#define AQ_MsmLineRxTooLongErrorsCounterRegister_HHD_baseRegisterAddress 0x9058
/*! \brief MMD address of structure AQ_MsmLineRxTooLongErrorsCounterRegister_HHD */
#define AQ_MsmLineRxTooLongErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxTooLongErrorsCounter_0 in AQ_MsmLineRxTooLongErrorsCounterRegister_HHD */
#define AQ_MsmLineRxTooLongErrorsCounterRegister_HHD_msmLineRxTooLongErrorsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxTooLongErrorsCounter_0 in AQ_MsmLineRxTooLongErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxTooLongErrorsCounterRegister_HHD_msmLineRxTooLongErrorsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxTooLongErrorsCounter_0 in AQ_MsmLineRxTooLongErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxTooLongErrorsCounterRegister_HHD_msmLineRxTooLongErrorsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxTooLongErrorsCounter_1 in AQ_MsmLineRxTooLongErrorsCounterRegister_HHD */
#define AQ_MsmLineRxTooLongErrorsCounterRegister_HHD_msmLineRxTooLongErrorsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxTooLongErrorsCounter_1 in AQ_MsmLineRxTooLongErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxTooLongErrorsCounterRegister_HHD_msmLineRxTooLongErrorsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxTooLongErrorsCounter_1 in AQ_MsmLineRxTooLongErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxTooLongErrorsCounterRegister_HHD_msmLineRxTooLongErrorsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD */
#define AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD_baseRegisterAddress 0x905C
/*! \brief MMD address of structure AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD */
#define AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxInRangeLengthErrorsCounter_0 in AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD */
#define AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD_msmLineRxInRangeLengthErrorsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxInRangeLengthErrorsCounter_0 in AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD_msmLineRxInRangeLengthErrorsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxInRangeLengthErrorsCounter_0 in AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD_msmLineRxInRangeLengthErrorsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxInRangeLengthErrorsCounter_1 in AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD */
#define AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD_msmLineRxInRangeLengthErrorsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxInRangeLengthErrorsCounter_1 in AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD_msmLineRxInRangeLengthErrorsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxInRangeLengthErrorsCounter_1 in AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD_msmLineRxInRangeLengthErrorsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineTxVlanFramesCounterRegister_HHD */
#define AQ_MsmLineTxVlanFramesCounterRegister_HHD_baseRegisterAddress 0x9060
/*! \brief MMD address of structure AQ_MsmLineTxVlanFramesCounterRegister_HHD */
#define AQ_MsmLineTxVlanFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxVlanFramesCounter_0 in AQ_MsmLineTxVlanFramesCounterRegister_HHD */
#define AQ_MsmLineTxVlanFramesCounterRegister_HHD_msmLineTxVlanFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxVlanFramesCounter_0 in AQ_MsmLineTxVlanFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxVlanFramesCounterRegister_HHD_msmLineTxVlanFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxVlanFramesCounter_0 in AQ_MsmLineTxVlanFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxVlanFramesCounterRegister_HHD_msmLineTxVlanFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxVlanFramesCounter_1 in AQ_MsmLineTxVlanFramesCounterRegister_HHD */
#define AQ_MsmLineTxVlanFramesCounterRegister_HHD_msmLineTxVlanFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxVlanFramesCounter_1 in AQ_MsmLineTxVlanFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxVlanFramesCounterRegister_HHD_msmLineTxVlanFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxVlanFramesCounter_1 in AQ_MsmLineTxVlanFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxVlanFramesCounterRegister_HHD_msmLineTxVlanFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxVlanFramesCounterRegister_HHD */
#define AQ_MsmLineRxVlanFramesCounterRegister_HHD_baseRegisterAddress 0x9064
/*! \brief MMD address of structure AQ_MsmLineRxVlanFramesCounterRegister_HHD */
#define AQ_MsmLineRxVlanFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxVlanFramesCounter_0 in AQ_MsmLineRxVlanFramesCounterRegister_HHD */
#define AQ_MsmLineRxVlanFramesCounterRegister_HHD_msmLineRxVlanFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxVlanFramesCounter_0 in AQ_MsmLineRxVlanFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxVlanFramesCounterRegister_HHD_msmLineRxVlanFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxVlanFramesCounter_0 in AQ_MsmLineRxVlanFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxVlanFramesCounterRegister_HHD_msmLineRxVlanFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxVlanFramesCounter_1 in AQ_MsmLineRxVlanFramesCounterRegister_HHD */
#define AQ_MsmLineRxVlanFramesCounterRegister_HHD_msmLineRxVlanFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxVlanFramesCounter_1 in AQ_MsmLineRxVlanFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxVlanFramesCounterRegister_HHD_msmLineRxVlanFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxVlanFramesCounter_1 in AQ_MsmLineRxVlanFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxVlanFramesCounterRegister_HHD_msmLineRxVlanFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineTxOctetsCounterRegister_HHD */
#define AQ_MsmLineTxOctetsCounterRegister_HHD_baseRegisterAddress 0x9068
/*! \brief MMD address of structure AQ_MsmLineTxOctetsCounterRegister_HHD */
#define AQ_MsmLineTxOctetsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxOctetsCounter_0 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxOctetsCounter_0 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define bits_AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxOctetsCounter_0 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define word_AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxOctetsCounter_1 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxOctetsCounter_1 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define bits_AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxOctetsCounter_1 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define word_AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_1 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxOctetsCounter_2 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_2 2
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxOctetsCounter_2 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define bits_AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_2 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxOctetsCounter_2 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define word_AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_2 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxOctetsCounter_3 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_3 3
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxOctetsCounter_3 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define bits_AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_3 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxOctetsCounter_3 in AQ_MsmLineTxOctetsCounterRegister_HHD */
#define word_AQ_MsmLineTxOctetsCounterRegister_HHD_msmLineTxOctetsCounter_3 u3.word_3

/*! \brief Base register address of structure AQ_MsmLineRxOctetsCounterRegister_HHD */
#define AQ_MsmLineRxOctetsCounterRegister_HHD_baseRegisterAddress 0x906C
/*! \brief MMD address of structure AQ_MsmLineRxOctetsCounterRegister_HHD */
#define AQ_MsmLineRxOctetsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxOctetsCounter_0 in AQ_MsmLineRxOctetsCounterRegister_HHD */
#define AQ_MsmLineRxOctetsCounterRegister_HHD_msmLineRxOctetsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxOctetsCounter_0 in AQ_MsmLineRxOctetsCounterRegister_HHD */
#define bits_AQ_MsmLineRxOctetsCounterRegister_HHD_msmLineRxOctetsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxOctetsCounter_0 in AQ_MsmLineRxOctetsCounterRegister_HHD */
#define word_AQ_MsmLineRxOctetsCounterRegister_HHD_msmLineRxOctetsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxOctetsCounter_1 in AQ_MsmLineRxOctetsCounterRegister_HHD */
#define AQ_MsmLineRxOctetsCounterRegister_HHD_msmLineRxOctetsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxOctetsCounter_1 in AQ_MsmLineRxOctetsCounterRegister_HHD */
#define bits_AQ_MsmLineRxOctetsCounterRegister_HHD_msmLineRxOctetsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxOctetsCounter_1 in AQ_MsmLineRxOctetsCounterRegister_HHD */
#define word_AQ_MsmLineRxOctetsCounterRegister_HHD_msmLineRxOctetsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxUnicastFramesCounterRegister_HHD */
#define AQ_MsmLineRxUnicastFramesCounterRegister_HHD_baseRegisterAddress 0x9070
/*! \brief MMD address of structure AQ_MsmLineRxUnicastFramesCounterRegister_HHD */
#define AQ_MsmLineRxUnicastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxUnicastFramesCounter_0 in AQ_MsmLineRxUnicastFramesCounterRegister_HHD */
#define AQ_MsmLineRxUnicastFramesCounterRegister_HHD_msmLineRxUnicastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxUnicastFramesCounter_0 in AQ_MsmLineRxUnicastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxUnicastFramesCounterRegister_HHD_msmLineRxUnicastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxUnicastFramesCounter_0 in AQ_MsmLineRxUnicastFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxUnicastFramesCounterRegister_HHD_msmLineRxUnicastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxUnicastFramesCounter_1 in AQ_MsmLineRxUnicastFramesCounterRegister_HHD */
#define AQ_MsmLineRxUnicastFramesCounterRegister_HHD_msmLineRxUnicastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxUnicastFramesCounter_1 in AQ_MsmLineRxUnicastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxUnicastFramesCounterRegister_HHD_msmLineRxUnicastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxUnicastFramesCounter_1 in AQ_MsmLineRxUnicastFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxUnicastFramesCounterRegister_HHD_msmLineRxUnicastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxMulticastFramesCounterRegister_HHD */
#define AQ_MsmLineRxMulticastFramesCounterRegister_HHD_baseRegisterAddress 0x9074
/*! \brief MMD address of structure AQ_MsmLineRxMulticastFramesCounterRegister_HHD */
#define AQ_MsmLineRxMulticastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxMulticastFramesCounter_0 in AQ_MsmLineRxMulticastFramesCounterRegister_HHD */
#define AQ_MsmLineRxMulticastFramesCounterRegister_HHD_msmLineRxMulticastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxMulticastFramesCounter_0 in AQ_MsmLineRxMulticastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxMulticastFramesCounterRegister_HHD_msmLineRxMulticastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxMulticastFramesCounter_0 in AQ_MsmLineRxMulticastFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxMulticastFramesCounterRegister_HHD_msmLineRxMulticastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxMulticastFramesCounter_1 in AQ_MsmLineRxMulticastFramesCounterRegister_HHD */
#define AQ_MsmLineRxMulticastFramesCounterRegister_HHD_msmLineRxMulticastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxMulticastFramesCounter_1 in AQ_MsmLineRxMulticastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxMulticastFramesCounterRegister_HHD_msmLineRxMulticastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxMulticastFramesCounter_1 in AQ_MsmLineRxMulticastFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxMulticastFramesCounterRegister_HHD_msmLineRxMulticastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmLineRxBroadcastFramesCounterRegister_HHD_baseRegisterAddress 0x9078
/*! \brief MMD address of structure AQ_MsmLineRxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmLineRxBroadcastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxBroadcastFramesCounter_0 in AQ_MsmLineRxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmLineRxBroadcastFramesCounterRegister_HHD_msmLineRxBroadcastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxBroadcastFramesCounter_0 in AQ_MsmLineRxBroadcastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxBroadcastFramesCounterRegister_HHD_msmLineRxBroadcastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxBroadcastFramesCounter_0 in AQ_MsmLineRxBroadcastFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxBroadcastFramesCounterRegister_HHD_msmLineRxBroadcastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxBroadcastFramesCounter_1 in AQ_MsmLineRxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmLineRxBroadcastFramesCounterRegister_HHD_msmLineRxBroadcastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxBroadcastFramesCounter_1 in AQ_MsmLineRxBroadcastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineRxBroadcastFramesCounterRegister_HHD_msmLineRxBroadcastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxBroadcastFramesCounter_1 in AQ_MsmLineRxBroadcastFramesCounterRegister_HHD */
#define word_AQ_MsmLineRxBroadcastFramesCounterRegister_HHD_msmLineRxBroadcastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineTxErrorsCounterRegister_HHD */
#define AQ_MsmLineTxErrorsCounterRegister_HHD_baseRegisterAddress 0x907C
/*! \brief MMD address of structure AQ_MsmLineTxErrorsCounterRegister_HHD */
#define AQ_MsmLineTxErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxErrorsCounter_0 in AQ_MsmLineTxErrorsCounterRegister_HHD */
#define AQ_MsmLineTxErrorsCounterRegister_HHD_msmLineTxErrorsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxErrorsCounter_0 in AQ_MsmLineTxErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineTxErrorsCounterRegister_HHD_msmLineTxErrorsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxErrorsCounter_0 in AQ_MsmLineTxErrorsCounterRegister_HHD */
#define word_AQ_MsmLineTxErrorsCounterRegister_HHD_msmLineTxErrorsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxErrorsCounter_1 in AQ_MsmLineTxErrorsCounterRegister_HHD */
#define AQ_MsmLineTxErrorsCounterRegister_HHD_msmLineTxErrorsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxErrorsCounter_1 in AQ_MsmLineTxErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineTxErrorsCounterRegister_HHD_msmLineTxErrorsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxErrorsCounter_1 in AQ_MsmLineTxErrorsCounterRegister_HHD */
#define word_AQ_MsmLineTxErrorsCounterRegister_HHD_msmLineTxErrorsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineTxUnicastFramesCounterRegister_HHD */
#define AQ_MsmLineTxUnicastFramesCounterRegister_HHD_baseRegisterAddress 0x9084
/*! \brief MMD address of structure AQ_MsmLineTxUnicastFramesCounterRegister_HHD */
#define AQ_MsmLineTxUnicastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxUnicastFramesCounter_0 in AQ_MsmLineTxUnicastFramesCounterRegister_HHD */
#define AQ_MsmLineTxUnicastFramesCounterRegister_HHD_msmLineTxUnicastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxUnicastFramesCounter_0 in AQ_MsmLineTxUnicastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxUnicastFramesCounterRegister_HHD_msmLineTxUnicastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxUnicastFramesCounter_0 in AQ_MsmLineTxUnicastFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxUnicastFramesCounterRegister_HHD_msmLineTxUnicastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxUnicastFramesCounter_1 in AQ_MsmLineTxUnicastFramesCounterRegister_HHD */
#define AQ_MsmLineTxUnicastFramesCounterRegister_HHD_msmLineTxUnicastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxUnicastFramesCounter_1 in AQ_MsmLineTxUnicastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxUnicastFramesCounterRegister_HHD_msmLineTxUnicastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxUnicastFramesCounter_1 in AQ_MsmLineTxUnicastFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxUnicastFramesCounterRegister_HHD_msmLineTxUnicastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineTxMulticastFramesCounterRegister_HHD */
#define AQ_MsmLineTxMulticastFramesCounterRegister_HHD_baseRegisterAddress 0x9088
/*! \brief MMD address of structure AQ_MsmLineTxMulticastFramesCounterRegister_HHD */
#define AQ_MsmLineTxMulticastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxMulticastFramesCounter_0 in AQ_MsmLineTxMulticastFramesCounterRegister_HHD */
#define AQ_MsmLineTxMulticastFramesCounterRegister_HHD_msmLineTxMulticastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxMulticastFramesCounter_0 in AQ_MsmLineTxMulticastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxMulticastFramesCounterRegister_HHD_msmLineTxMulticastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxMulticastFramesCounter_0 in AQ_MsmLineTxMulticastFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxMulticastFramesCounterRegister_HHD_msmLineTxMulticastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxMulticastFramesCounter_1 in AQ_MsmLineTxMulticastFramesCounterRegister_HHD */
#define AQ_MsmLineTxMulticastFramesCounterRegister_HHD_msmLineTxMulticastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxMulticastFramesCounter_1 in AQ_MsmLineTxMulticastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxMulticastFramesCounterRegister_HHD_msmLineTxMulticastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxMulticastFramesCounter_1 in AQ_MsmLineTxMulticastFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxMulticastFramesCounterRegister_HHD_msmLineTxMulticastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineTxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmLineTxBroadcastFramesCounterRegister_HHD_baseRegisterAddress 0x908C
/*! \brief MMD address of structure AQ_MsmLineTxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmLineTxBroadcastFramesCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxBroadcastFramesCounter_0 in AQ_MsmLineTxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmLineTxBroadcastFramesCounterRegister_HHD_msmLineTxBroadcastFramesCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxBroadcastFramesCounter_0 in AQ_MsmLineTxBroadcastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxBroadcastFramesCounterRegister_HHD_msmLineTxBroadcastFramesCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxBroadcastFramesCounter_0 in AQ_MsmLineTxBroadcastFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxBroadcastFramesCounterRegister_HHD_msmLineTxBroadcastFramesCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineTxBroadcastFramesCounter_1 in AQ_MsmLineTxBroadcastFramesCounterRegister_HHD */
#define AQ_MsmLineTxBroadcastFramesCounterRegister_HHD_msmLineTxBroadcastFramesCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineTxBroadcastFramesCounter_1 in AQ_MsmLineTxBroadcastFramesCounterRegister_HHD */
#define bits_AQ_MsmLineTxBroadcastFramesCounterRegister_HHD_msmLineTxBroadcastFramesCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineTxBroadcastFramesCounter_1 in AQ_MsmLineTxBroadcastFramesCounterRegister_HHD */
#define word_AQ_MsmLineTxBroadcastFramesCounterRegister_HHD_msmLineTxBroadcastFramesCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_MsmLineRxErrorsCounterRegister_HHD */
#define AQ_MsmLineRxErrorsCounterRegister_HHD_baseRegisterAddress 0x90C8
/*! \brief MMD address of structure AQ_MsmLineRxErrorsCounterRegister_HHD */
#define AQ_MsmLineRxErrorsCounterRegister_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxErrorsCounter_0 in AQ_MsmLineRxErrorsCounterRegister_HHD */
#define AQ_MsmLineRxErrorsCounterRegister_HHD_msmLineRxErrorsCounter_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxErrorsCounter_0 in AQ_MsmLineRxErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxErrorsCounterRegister_HHD_msmLineRxErrorsCounter_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxErrorsCounter_0 in AQ_MsmLineRxErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxErrorsCounterRegister_HHD_msmLineRxErrorsCounter_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure msmLineRxErrorsCounter_1 in AQ_MsmLineRxErrorsCounterRegister_HHD */
#define AQ_MsmLineRxErrorsCounterRegister_HHD_msmLineRxErrorsCounter_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure msmLineRxErrorsCounter_1 in AQ_MsmLineRxErrorsCounterRegister_HHD */
#define bits_AQ_MsmLineRxErrorsCounterRegister_HHD_msmLineRxErrorsCounter_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure msmLineRxErrorsCounter_1 in AQ_MsmLineRxErrorsCounterRegister_HHD */
#define word_AQ_MsmLineRxErrorsCounterRegister_HHD_msmLineRxErrorsCounter_1 u1.word_1

/*! \brief Base register address of structure AQ_GlobalControl_HHD */
#define AQ_GlobalControl_HHD_baseRegisterAddress 0xC000
/*! \brief MMD address of structure AQ_GlobalControl_HHD */
#define AQ_GlobalControl_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure upReset in AQ_GlobalControl_HHD */
#define AQ_GlobalControl_HHD_upReset 1
/*! \brief Preprocessor variable to relate field to bit position in structure upReset in AQ_GlobalControl_HHD */
#define bits_AQ_GlobalControl_HHD_upReset u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure upReset in AQ_GlobalControl_HHD */
#define word_AQ_GlobalControl_HHD_upReset u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure upRunStallOverride in AQ_GlobalControl_HHD */
#define AQ_GlobalControl_HHD_upRunStallOverride 1
/*! \brief Preprocessor variable to relate field to bit position in structure upRunStallOverride in AQ_GlobalControl_HHD */
#define bits_AQ_GlobalControl_HHD_upRunStallOverride u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure upRunStallOverride in AQ_GlobalControl_HHD */
#define word_AQ_GlobalControl_HHD_upRunStallOverride u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure upRunStall in AQ_GlobalControl_HHD */
#define AQ_GlobalControl_HHD_upRunStall 1
/*! \brief Preprocessor variable to relate field to bit position in structure upRunStall in AQ_GlobalControl_HHD */
#define bits_AQ_GlobalControl_HHD_upRunStall u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure upRunStall in AQ_GlobalControl_HHD */
#define word_AQ_GlobalControl_HHD_upRunStall u1.word_1

/*! \brief Base register address of structure AQ_GlobalResetControl_HHD */
#define AQ_GlobalResetControl_HHD_baseRegisterAddress 0xC006
/*! \brief MMD address of structure AQ_GlobalResetControl_HHD */
#define AQ_GlobalResetControl_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure globalMMD_ResetDisable in AQ_GlobalResetControl_HHD */
#define AQ_GlobalResetControl_HHD_globalMMD_ResetDisable 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalMMD_ResetDisable in AQ_GlobalResetControl_HHD */
#define bits_AQ_GlobalResetControl_HHD_globalMMD_ResetDisable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalMMD_ResetDisable in AQ_GlobalResetControl_HHD */
#define word_AQ_GlobalResetControl_HHD_globalMMD_ResetDisable u0.word_0

/*! \brief Base register address of structure AQ_GlobalDiagnosticProvisioning_HHD */
#define AQ_GlobalDiagnosticProvisioning_HHD_baseRegisterAddress 0xC400
/*! \brief MMD address of structure AQ_GlobalDiagnosticProvisioning_HHD */
#define AQ_GlobalDiagnosticProvisioning_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure enableDiagnostics in AQ_GlobalDiagnosticProvisioning_HHD */
#define AQ_GlobalDiagnosticProvisioning_HHD_enableDiagnostics 0
/*! \brief Preprocessor variable to relate field to bit position in structure enableDiagnostics in AQ_GlobalDiagnosticProvisioning_HHD */
#define bits_AQ_GlobalDiagnosticProvisioning_HHD_enableDiagnostics u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure enableDiagnostics in AQ_GlobalDiagnosticProvisioning_HHD */
#define word_AQ_GlobalDiagnosticProvisioning_HHD_enableDiagnostics u0.word_0

/*! \brief Base register address of structure AQ_GlobalThermalProvisioning_HHD */
#define AQ_GlobalThermalProvisioning_HHD_baseRegisterAddress 0xC420
/*! \brief MMD address of structure AQ_GlobalThermalProvisioning_HHD */
#define AQ_GlobalThermalProvisioning_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure reserved_0 in AQ_GlobalThermalProvisioning_HHD */
#define AQ_GlobalThermalProvisioning_HHD_reserved_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_0 in AQ_GlobalThermalProvisioning_HHD */
#define bits_AQ_GlobalThermalProvisioning_HHD_reserved_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_0 in AQ_GlobalThermalProvisioning_HHD */
#define word_AQ_GlobalThermalProvisioning_HHD_reserved_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure highTempFailureThreshold in AQ_GlobalThermalProvisioning_HHD */
#define AQ_GlobalThermalProvisioning_HHD_highTempFailureThreshold 1
/*! \brief Preprocessor variable to relate field to bit position in structure highTempFailureThreshold in AQ_GlobalThermalProvisioning_HHD */
#define bits_AQ_GlobalThermalProvisioning_HHD_highTempFailureThreshold u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure highTempFailureThreshold in AQ_GlobalThermalProvisioning_HHD */
#define word_AQ_GlobalThermalProvisioning_HHD_highTempFailureThreshold u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure lowTempFailureThreshold in AQ_GlobalThermalProvisioning_HHD */
#define AQ_GlobalThermalProvisioning_HHD_lowTempFailureThreshold 2
/*! \brief Preprocessor variable to relate field to bit position in structure lowTempFailureThreshold in AQ_GlobalThermalProvisioning_HHD */
#define bits_AQ_GlobalThermalProvisioning_HHD_lowTempFailureThreshold u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure lowTempFailureThreshold in AQ_GlobalThermalProvisioning_HHD */
#define word_AQ_GlobalThermalProvisioning_HHD_lowTempFailureThreshold u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure highTempWarningThreshold in AQ_GlobalThermalProvisioning_HHD */
#define AQ_GlobalThermalProvisioning_HHD_highTempWarningThreshold 3
/*! \brief Preprocessor variable to relate field to bit position in structure highTempWarningThreshold in AQ_GlobalThermalProvisioning_HHD */
#define bits_AQ_GlobalThermalProvisioning_HHD_highTempWarningThreshold u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure highTempWarningThreshold in AQ_GlobalThermalProvisioning_HHD */
#define word_AQ_GlobalThermalProvisioning_HHD_highTempWarningThreshold u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure lowTempWarningThreshold in AQ_GlobalThermalProvisioning_HHD */
#define AQ_GlobalThermalProvisioning_HHD_lowTempWarningThreshold 4
/*! \brief Preprocessor variable to relate field to bit position in structure lowTempWarningThreshold in AQ_GlobalThermalProvisioning_HHD */
#define bits_AQ_GlobalThermalProvisioning_HHD_lowTempWarningThreshold u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure lowTempWarningThreshold in AQ_GlobalThermalProvisioning_HHD */
#define word_AQ_GlobalThermalProvisioning_HHD_lowTempWarningThreshold u4.word_4

/*! \brief Base register address of structure AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_baseRegisterAddress 0xC430
/*! \brief MMD address of structure AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioningC430 in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_reservedProvisioningC430 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioningC430 in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_reservedProvisioningC430 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioningC430 in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_reservedProvisioningC430 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0ManualSet in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_0ManualSet 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0ManualSet in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_0ManualSet u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0ManualSet in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_0ManualSet u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_0_10Gb_sLinkEstablished 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_0_10Gb_sLinkEstablished u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_0_10Gb_sLinkEstablished u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_0_1Gb_sLinkEstablished 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_0_1Gb_sLinkEstablished u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_0_1Gb_sLinkEstablished u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_0_100Mb_sLinkEstablished 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_0_100Mb_sLinkEstablished u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_0_100Mb_sLinkEstablished u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0Connecting in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_0Connecting 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0Connecting in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_0Connecting u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0Connecting in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_0Connecting u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0ReceiveActivity in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_0ReceiveActivity 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0ReceiveActivity in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_0ReceiveActivity u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0ReceiveActivity in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_0ReceiveActivity u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0TransmitActivity in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_0TransmitActivity 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0TransmitActivity in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_0TransmitActivity u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0TransmitActivity in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_0TransmitActivity u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0ActivityStretch in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_0ActivityStretch 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0ActivityStretch in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_0ActivityStretch u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0ActivityStretch in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_0ActivityStretch u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioningC431 in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_reservedProvisioningC431 1
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioningC431 in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_reservedProvisioningC431 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioningC431 in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_reservedProvisioningC431 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1ManualSet in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_1ManualSet 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1ManualSet in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_1ManualSet u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1ManualSet in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_1ManualSet u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_1_10Gb_sLinkEstablished 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_1_10Gb_sLinkEstablished u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_1_10Gb_sLinkEstablished u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_1_1Gb_sLinkEstablished 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_1_1Gb_sLinkEstablished u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_1_1Gb_sLinkEstablished u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_1_100Mb_sLinkEstablished 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_1_100Mb_sLinkEstablished u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_1_100Mb_sLinkEstablished u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1Connecting in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_1Connecting 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1Connecting in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_1Connecting u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1Connecting in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_1Connecting u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1ReceiveActivity in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_1ReceiveActivity 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1ReceiveActivity in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_1ReceiveActivity u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1ReceiveActivity in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_1ReceiveActivity u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1TransmitActivity in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_1TransmitActivity 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1TransmitActivity in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_1TransmitActivity u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1TransmitActivity in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_1TransmitActivity u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1ActivityStretch in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_1ActivityStretch 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1ActivityStretch in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_1ActivityStretch u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1ActivityStretch in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_1ActivityStretch u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioningC432 in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_reservedProvisioningC432 2
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioningC432 in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_reservedProvisioningC432 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioningC432 in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_reservedProvisioningC432 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2ManualSet in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_2ManualSet 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2ManualSet in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_2ManualSet u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2ManualSet in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_2ManualSet u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_2_10Gb_sLinkEstablished 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_2_10Gb_sLinkEstablished u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_2_10Gb_sLinkEstablished u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_2_1Gb_sLinkEstablished 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_2_1Gb_sLinkEstablished u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_2_1Gb_sLinkEstablished u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_2_100Mb_sLinkEstablished 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_2_100Mb_sLinkEstablished u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_2_100Mb_sLinkEstablished u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2Connecting in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_2Connecting 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2Connecting in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_2Connecting u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2Connecting in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_2Connecting u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2ReceiveActivity in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_2ReceiveActivity 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2ReceiveActivity in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_2ReceiveActivity u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2ReceiveActivity in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_2ReceiveActivity u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2TransmitActivity in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_2TransmitActivity 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2TransmitActivity in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_2TransmitActivity u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2TransmitActivity in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_2TransmitActivity u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2ActivityStretch in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_led_2ActivityStretch 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2ActivityStretch in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_led_2ActivityStretch u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2ActivityStretch in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_led_2ActivityStretch u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure ledOperationMode in AQ_GlobalLedProvisioning_HHD */
#define AQ_GlobalLedProvisioning_HHD_ledOperationMode 7
/*! \brief Preprocessor variable to relate field to bit position in structure ledOperationMode in AQ_GlobalLedProvisioning_HHD */
#define bits_AQ_GlobalLedProvisioning_HHD_ledOperationMode u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure ledOperationMode in AQ_GlobalLedProvisioning_HHD */
#define word_AQ_GlobalLedProvisioning_HHD_ledOperationMode u7.word_7

/*! \brief Base register address of structure AQ_GlobalGeneralProvisioning_HHD */
#define AQ_GlobalGeneralProvisioning_HHD_baseRegisterAddress 0xC440
/*! \brief MMD address of structure AQ_GlobalGeneralProvisioning_HHD */
#define AQ_GlobalGeneralProvisioning_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mdioBroadcastModeEnable in AQ_GlobalGeneralProvisioning_HHD */
#define AQ_GlobalGeneralProvisioning_HHD_mdioBroadcastModeEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioBroadcastModeEnable in AQ_GlobalGeneralProvisioning_HHD */
#define bits_AQ_GlobalGeneralProvisioning_HHD_mdioBroadcastModeEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioBroadcastModeEnable in AQ_GlobalGeneralProvisioning_HHD */
#define word_AQ_GlobalGeneralProvisioning_HHD_mdioBroadcastModeEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioReadMSW_FirstEnable in AQ_GlobalGeneralProvisioning_HHD */
#define AQ_GlobalGeneralProvisioning_HHD_mdioReadMSW_FirstEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioReadMSW_FirstEnable in AQ_GlobalGeneralProvisioning_HHD */
#define bits_AQ_GlobalGeneralProvisioning_HHD_mdioReadMSW_FirstEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioReadMSW_FirstEnable in AQ_GlobalGeneralProvisioning_HHD */
#define word_AQ_GlobalGeneralProvisioning_HHD_mdioReadMSW_FirstEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioDriveConfiguration in AQ_GlobalGeneralProvisioning_HHD */
#define AQ_GlobalGeneralProvisioning_HHD_mdioDriveConfiguration 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioDriveConfiguration in AQ_GlobalGeneralProvisioning_HHD */
#define bits_AQ_GlobalGeneralProvisioning_HHD_mdioDriveConfiguration u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioDriveConfiguration in AQ_GlobalGeneralProvisioning_HHD */
#define word_AQ_GlobalGeneralProvisioning_HHD_mdioDriveConfiguration u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioPreambleDetectionDisable in AQ_GlobalGeneralProvisioning_HHD */
#define AQ_GlobalGeneralProvisioning_HHD_mdioPreambleDetectionDisable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioPreambleDetectionDisable in AQ_GlobalGeneralProvisioning_HHD */
#define bits_AQ_GlobalGeneralProvisioning_HHD_mdioPreambleDetectionDisable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioPreambleDetectionDisable in AQ_GlobalGeneralProvisioning_HHD */
#define word_AQ_GlobalGeneralProvisioning_HHD_mdioPreambleDetectionDisable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure daisyChainReset in AQ_GlobalGeneralProvisioning_HHD */
#define AQ_GlobalGeneralProvisioning_HHD_daisyChainReset 2
/*! \brief Preprocessor variable to relate field to bit position in structure daisyChainReset in AQ_GlobalGeneralProvisioning_HHD */
#define bits_AQ_GlobalGeneralProvisioning_HHD_daisyChainReset u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure daisyChainReset in AQ_GlobalGeneralProvisioning_HHD */
#define word_AQ_GlobalGeneralProvisioning_HHD_daisyChainReset u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mdioBroadcastAddressConfiguration in AQ_GlobalGeneralProvisioning_HHD */
#define AQ_GlobalGeneralProvisioning_HHD_mdioBroadcastAddressConfiguration 7
/*! \brief Preprocessor variable to relate field to bit position in structure mdioBroadcastAddressConfiguration in AQ_GlobalGeneralProvisioning_HHD */
#define bits_AQ_GlobalGeneralProvisioning_HHD_mdioBroadcastAddressConfiguration u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure mdioBroadcastAddressConfiguration in AQ_GlobalGeneralProvisioning_HHD */
#define word_AQ_GlobalGeneralProvisioning_HHD_mdioBroadcastAddressConfiguration u7.word_7
/*! \brief Preprocessor variable to relate field to word number in structure mdioPreambleLength in AQ_GlobalGeneralProvisioning_HHD */
#define AQ_GlobalGeneralProvisioning_HHD_mdioPreambleLength 9
/*! \brief Preprocessor variable to relate field to bit position in structure mdioPreambleLength in AQ_GlobalGeneralProvisioning_HHD */
#define bits_AQ_GlobalGeneralProvisioning_HHD_mdioPreambleLength u9.bits_9
/*! \brief Preprocessor variable to relate field to word position in structure mdioPreambleLength in AQ_GlobalGeneralProvisioning_HHD */
#define word_AQ_GlobalGeneralProvisioning_HHD_mdioPreambleLength u9.word_9

/*! \brief Base register address of structure AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_baseRegisterAddress 0xC450
/*! \brief MMD address of structure AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure nvrDataLength in AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_nvrDataLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDataLength in AQ_GlobalNvrProvisioning_HHD */
#define bits_AQ_GlobalNvrProvisioning_HHD_nvrDataLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrDataLength in AQ_GlobalNvrProvisioning_HHD */
#define word_AQ_GlobalNvrProvisioning_HHD_nvrDataLength u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrDummyLength in AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_nvrDummyLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDummyLength in AQ_GlobalNvrProvisioning_HHD */
#define bits_AQ_GlobalNvrProvisioning_HHD_nvrDummyLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrDummyLength in AQ_GlobalNvrProvisioning_HHD */
#define word_AQ_GlobalNvrProvisioning_HHD_nvrDummyLength u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrAddressLength in AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_nvrAddressLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrAddressLength in AQ_GlobalNvrProvisioning_HHD */
#define bits_AQ_GlobalNvrProvisioning_HHD_nvrAddressLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrAddressLength in AQ_GlobalNvrProvisioning_HHD */
#define word_AQ_GlobalNvrProvisioning_HHD_nvrAddressLength u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrAddressLengthOverride in AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_nvrAddressLengthOverride 1
/*! \brief Preprocessor variable to relate field to bit position in structure nvrAddressLengthOverride in AQ_GlobalNvrProvisioning_HHD */
#define bits_AQ_GlobalNvrProvisioning_HHD_nvrAddressLengthOverride u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure nvrAddressLengthOverride in AQ_GlobalNvrProvisioning_HHD */
#define word_AQ_GlobalNvrProvisioning_HHD_nvrAddressLengthOverride u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrClockDivide in AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_nvrClockDivide 1
/*! \brief Preprocessor variable to relate field to bit position in structure nvrClockDivide in AQ_GlobalNvrProvisioning_HHD */
#define bits_AQ_GlobalNvrProvisioning_HHD_nvrClockDivide u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure nvrClockDivide in AQ_GlobalNvrProvisioning_HHD */
#define word_AQ_GlobalNvrProvisioning_HHD_nvrClockDivide u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrDaisyChainClockDivideOverride in AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_nvrDaisyChainClockDivideOverride 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDaisyChainClockDivideOverride in AQ_GlobalNvrProvisioning_HHD */
#define bits_AQ_GlobalNvrProvisioning_HHD_nvrDaisyChainClockDivideOverride u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrDaisyChainClockDivideOverride in AQ_GlobalNvrProvisioning_HHD */
#define word_AQ_GlobalNvrProvisioning_HHD_nvrDaisyChainClockDivideOverride u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure nvrDaisyChainDisable in AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_nvrDaisyChainDisable 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDaisyChainDisable in AQ_GlobalNvrProvisioning_HHD */
#define bits_AQ_GlobalNvrProvisioning_HHD_nvrDaisyChainDisable u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrDaisyChainDisable in AQ_GlobalNvrProvisioning_HHD */
#define word_AQ_GlobalNvrProvisioning_HHD_nvrDaisyChainDisable u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure nvrReset in AQ_GlobalNvrProvisioning_HHD */
#define AQ_GlobalNvrProvisioning_HHD_nvrReset 3
/*! \brief Preprocessor variable to relate field to bit position in structure nvrReset in AQ_GlobalNvrProvisioning_HHD */
#define bits_AQ_GlobalNvrProvisioning_HHD_nvrReset u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure nvrReset in AQ_GlobalNvrProvisioning_HHD */
#define word_AQ_GlobalNvrProvisioning_HHD_nvrReset u3.word_3

/*! \brief Base register address of structure AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_baseRegisterAddress 0xC470
/*! \brief MMD address of structure AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure diagnosticsSelect in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_diagnosticsSelect 0
/*! \brief Preprocessor variable to relate field to bit position in structure diagnosticsSelect in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_diagnosticsSelect u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure diagnosticsSelect in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_diagnosticsSelect u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure extendedMdiDiagnosticsSelect in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_extendedMdiDiagnosticsSelect 0
/*! \brief Preprocessor variable to relate field to bit position in structure extendedMdiDiagnosticsSelect in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_extendedMdiDiagnosticsSelect u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure extendedMdiDiagnosticsSelect in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_extendedMdiDiagnosticsSelect u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure initiateCableDiagnostics in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_initiateCableDiagnostics 0
/*! \brief Preprocessor variable to relate field to bit position in structure initiateCableDiagnostics in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_initiateCableDiagnostics u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure initiateCableDiagnostics in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_initiateCableDiagnostics u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure enableDaisy_chainHop_countOverride in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_enableDaisy_chainHop_countOverride 1
/*! \brief Preprocessor variable to relate field to bit position in structure enableDaisy_chainHop_countOverride in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_enableDaisy_chainHop_countOverride u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure enableDaisy_chainHop_countOverride in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_enableDaisy_chainHop_countOverride u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure daisy_chainHop_countOverrideValue in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_daisy_chainHop_countOverrideValue 1
/*! \brief Preprocessor variable to relate field to bit position in structure daisy_chainHop_countOverrideValue in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_daisy_chainHop_countOverrideValue u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure daisy_chainHop_countOverrideValue in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_daisy_chainHop_countOverrideValue u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure enableVddPowerSupplyTuning in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_enableVddPowerSupplyTuning 2
/*! \brief Preprocessor variable to relate field to bit position in structure enableVddPowerSupplyTuning in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_enableVddPowerSupplyTuning u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure enableVddPowerSupplyTuning in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_enableVddPowerSupplyTuning u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure tunableExternalVddPowerSupplyPresent in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_tunableExternalVddPowerSupplyPresent 2
/*! \brief Preprocessor variable to relate field to bit position in structure tunableExternalVddPowerSupplyPresent in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_tunableExternalVddPowerSupplyPresent u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure tunableExternalVddPowerSupplyPresent in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_tunableExternalVddPowerSupplyPresent u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure externalVddChangeRequest in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_externalVddChangeRequest 2
/*! \brief Preprocessor variable to relate field to bit position in structure externalVddChangeRequest in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_externalVddChangeRequest u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure externalVddChangeRequest in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_externalVddChangeRequest u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure enableXenpakRegisterSpace in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_enableXenpakRegisterSpace 2
/*! \brief Preprocessor variable to relate field to bit position in structure enableXenpakRegisterSpace in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_enableXenpakRegisterSpace u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure enableXenpakRegisterSpace in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_enableXenpakRegisterSpace u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure enable_5thChannelRfiCancellation in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_enable_5thChannelRfiCancellation 2
/*! \brief Preprocessor variable to relate field to bit position in structure enable_5thChannelRfiCancellation in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_enable_5thChannelRfiCancellation u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure enable_5thChannelRfiCancellation in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_enable_5thChannelRfiCancellation u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure rateTransitionRequest in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_rateTransitionRequest 3
/*! \brief Preprocessor variable to relate field to bit position in structure rateTransitionRequest in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_rateTransitionRequest u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure rateTransitionRequest in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_rateTransitionRequest u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure trainingSNR in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_trainingSNR 3
/*! \brief Preprocessor variable to relate field to bit position in structure trainingSNR in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_trainingSNR u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure trainingSNR in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_trainingSNR u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioning_5 in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_5 4
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioning_5 in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_5 u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioning_5 in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_5 u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure nvrDaisyChainKickstart in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_nvrDaisyChainKickstart 4
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDaisyChainKickstart in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_nvrDaisyChainKickstart u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure nvrDaisyChainKickstart in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_nvrDaisyChainKickstart u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure smartPower_downStatus in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_smartPower_downStatus 5
/*! \brief Preprocessor variable to relate field to bit position in structure smartPower_downStatus in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_smartPower_downStatus u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure smartPower_downStatus in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_smartPower_downStatus u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioning_6 in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_6 5
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioning_6 in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_6 u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioning_6 in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_6 u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrLpDisableTimer in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_cfrLpDisableTimer 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrLpDisableTimer in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_cfrLpDisableTimer u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrLpDisableTimer in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_cfrLpDisableTimer u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrLpExtendedMaxwait in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_cfrLpExtendedMaxwait 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrLpExtendedMaxwait in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_cfrLpExtendedMaxwait u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrLpExtendedMaxwait in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_cfrLpExtendedMaxwait u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrLpTHP in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_cfrLpTHP 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrLpTHP in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_cfrLpTHP u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrLpTHP in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_cfrLpTHP u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrLpSupport in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_cfrLpSupport 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrLpSupport in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_cfrLpSupport u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrLpSupport in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_cfrLpSupport u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrDisableTimer in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_cfrDisableTimer 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrDisableTimer in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_cfrDisableTimer u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrDisableTimer in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_cfrDisableTimer u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrExtendedMaxwait in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_cfrExtendedMaxwait 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrExtendedMaxwait in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_cfrExtendedMaxwait u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrExtendedMaxwait in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_cfrExtendedMaxwait u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrTHP in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_cfrTHP 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrTHP in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_cfrTHP u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrTHP in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_cfrTHP u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrSupport in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_cfrSupport 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrSupport in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_cfrSupport u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrSupport in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_cfrSupport u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure deadlockAvoidanceEnable in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_deadlockAvoidanceEnable 5
/*! \brief Preprocessor variable to relate field to bit position in structure deadlockAvoidanceEnable in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_deadlockAvoidanceEnable u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure deadlockAvoidanceEnable in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_deadlockAvoidanceEnable u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure smartPower_downEnable in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_smartPower_downEnable 5
/*! \brief Preprocessor variable to relate field to bit position in structure smartPower_downEnable in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_smartPower_downEnable u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure smartPower_downEnable in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_smartPower_downEnable u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure dteEnable in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_dteEnable 8
/*! \brief Preprocessor variable to relate field to bit position in structure dteEnable in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_dteEnable u8.bits_8
/*! \brief Preprocessor variable to relate field to word position in structure dteEnable in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_dteEnable u8.word_8
/*! \brief Preprocessor variable to relate field to word number in structure dteDropReportingTimer in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_dteDropReportingTimer 8
/*! \brief Preprocessor variable to relate field to bit position in structure dteDropReportingTimer in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_dteDropReportingTimer u8.bits_8
/*! \brief Preprocessor variable to relate field to word position in structure dteDropReportingTimer in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_dteDropReportingTimer u8.word_8
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioning_9 in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_9 8
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioning_9 in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_9 u8.bits_8
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioning_9 in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_9 u8.word_8
/*! \brief Preprocessor variable to relate field to word number in structure powerUpStall in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_powerUpStall 9
/*! \brief Preprocessor variable to relate field to bit position in structure powerUpStall in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_powerUpStall u9.bits_9
/*! \brief Preprocessor variable to relate field to word position in structure powerUpStall in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_powerUpStall u9.word_9
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioning_10 in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_10 9
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioning_10 in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_10 u9.bits_9
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioning_10 in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_10 u9.word_9
/*! \brief Preprocessor variable to relate field to word number in structure loopbackControl in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_loopbackControl 10
/*! \brief Preprocessor variable to relate field to bit position in structure loopbackControl in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_loopbackControl u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure loopbackControl in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_loopbackControl u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioning_11 in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_11 10
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioning_11 in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_11 u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioning_11 in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_11 u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure mdiPacketGeneration in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_mdiPacketGeneration 10
/*! \brief Preprocessor variable to relate field to bit position in structure mdiPacketGeneration in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_mdiPacketGeneration u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure mdiPacketGeneration in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_mdiPacketGeneration u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure look_asidePortPacketGeneration in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_look_asidePortPacketGeneration 10
/*! \brief Preprocessor variable to relate field to bit position in structure look_asidePortPacketGeneration in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_look_asidePortPacketGeneration u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure look_asidePortPacketGeneration in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_look_asidePortPacketGeneration u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure systemI_fPacketGeneration in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_systemI_fPacketGeneration 10
/*! \brief Preprocessor variable to relate field to bit position in structure systemI_fPacketGeneration in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_systemI_fPacketGeneration u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure systemI_fPacketGeneration in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_systemI_fPacketGeneration u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioning_11a in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_11a 10
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioning_11a in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_11a u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioning_11a in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_11a u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure rate in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_rate 10
/*! \brief Preprocessor variable to relate field to bit position in structure rate in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_rate u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure rate in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_rate u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioning_12 in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_12 11
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioning_12 in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_12 u11.bits_11
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioning_12 in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_reservedProvisioning_12 u11.word_11
/*! \brief Preprocessor variable to relate field to word number in structure enableMacsec in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_enableMacsec 11
/*! \brief Preprocessor variable to relate field to bit position in structure enableMacsec in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_enableMacsec u11.bits_11
/*! \brief Preprocessor variable to relate field to word position in structure enableMacsec in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_enableMacsec u11.word_11
/*! \brief Preprocessor variable to relate field to word number in structure enablePtp in AQ_GlobalReservedProvisioning_HHD */
#define AQ_GlobalReservedProvisioning_HHD_enablePtp 11
/*! \brief Preprocessor variable to relate field to bit position in structure enablePtp in AQ_GlobalReservedProvisioning_HHD */
#define bits_AQ_GlobalReservedProvisioning_HHD_enablePtp u11.bits_11
/*! \brief Preprocessor variable to relate field to word position in structure enablePtp in AQ_GlobalReservedProvisioning_HHD */
#define word_AQ_GlobalReservedProvisioning_HHD_enablePtp u11.word_11

/*! \brief Base register address of structure AQ_PifMailboxControl_HHD */
#define AQ_PifMailboxControl_HHD_baseRegisterAddress 0xC47C
/*! \brief MMD address of structure AQ_PifMailboxControl_HHD */
#define AQ_PifMailboxControl_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pifMailboxAddress in AQ_PifMailboxControl_HHD */
#define AQ_PifMailboxControl_HHD_pifMailboxAddress 0
/*! \brief Preprocessor variable to relate field to bit position in structure pifMailboxAddress in AQ_PifMailboxControl_HHD */
#define bits_AQ_PifMailboxControl_HHD_pifMailboxAddress u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pifMailboxAddress in AQ_PifMailboxControl_HHD */
#define word_AQ_PifMailboxControl_HHD_pifMailboxAddress u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pifMailboxData in AQ_PifMailboxControl_HHD */
#define AQ_PifMailboxControl_HHD_pifMailboxData 1
/*! \brief Preprocessor variable to relate field to bit position in structure pifMailboxData in AQ_PifMailboxControl_HHD */
#define bits_AQ_PifMailboxControl_HHD_pifMailboxData u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pifMailboxData in AQ_PifMailboxControl_HHD */
#define word_AQ_PifMailboxControl_HHD_pifMailboxData u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reservedPifMailboxControl_3 in AQ_PifMailboxControl_HHD */
#define AQ_PifMailboxControl_HHD_reservedPifMailboxControl_3 2
/*! \brief Preprocessor variable to relate field to bit position in structure reservedPifMailboxControl_3 in AQ_PifMailboxControl_HHD */
#define bits_AQ_PifMailboxControl_HHD_reservedPifMailboxControl_3 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reservedPifMailboxControl_3 in AQ_PifMailboxControl_HHD */
#define word_AQ_PifMailboxControl_HHD_reservedPifMailboxControl_3 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pifMailboxCommandType in AQ_PifMailboxControl_HHD */
#define AQ_PifMailboxControl_HHD_pifMailboxCommandType 2
/*! \brief Preprocessor variable to relate field to bit position in structure pifMailboxCommandType in AQ_PifMailboxControl_HHD */
#define bits_AQ_PifMailboxControl_HHD_pifMailboxCommandType u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pifMailboxCommandType in AQ_PifMailboxControl_HHD */
#define word_AQ_PifMailboxControl_HHD_pifMailboxCommandType u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pifMailboxMMD in AQ_PifMailboxControl_HHD */
#define AQ_PifMailboxControl_HHD_pifMailboxMMD 2
/*! \brief Preprocessor variable to relate field to bit position in structure pifMailboxMMD in AQ_PifMailboxControl_HHD */
#define bits_AQ_PifMailboxControl_HHD_pifMailboxMMD u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pifMailboxMMD in AQ_PifMailboxControl_HHD */
#define word_AQ_PifMailboxControl_HHD_pifMailboxMMD u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reservedPifMailboxControl_4 in AQ_PifMailboxControl_HHD */
#define AQ_PifMailboxControl_HHD_reservedPifMailboxControl_4 3
/*! \brief Preprocessor variable to relate field to bit position in structure reservedPifMailboxControl_4 in AQ_PifMailboxControl_HHD */
#define bits_AQ_PifMailboxControl_HHD_reservedPifMailboxControl_4 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reservedPifMailboxControl_4 in AQ_PifMailboxControl_HHD */
#define word_AQ_PifMailboxControl_HHD_reservedPifMailboxControl_4 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pifMailboxCommandStatus in AQ_PifMailboxControl_HHD */
#define AQ_PifMailboxControl_HHD_pifMailboxCommandStatus 3
/*! \brief Preprocessor variable to relate field to bit position in structure pifMailboxCommandStatus in AQ_PifMailboxControl_HHD */
#define bits_AQ_PifMailboxControl_HHD_pifMailboxCommandStatus u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pifMailboxCommandStatus in AQ_PifMailboxControl_HHD */
#define word_AQ_PifMailboxControl_HHD_pifMailboxCommandStatus u3.word_3

/*! \brief Base register address of structure AQ_GlobalSmbus_0Provisioning_HHD */
#define AQ_GlobalSmbus_0Provisioning_HHD_baseRegisterAddress 0xC485
/*! \brief MMD address of structure AQ_GlobalSmbus_0Provisioning_HHD */
#define AQ_GlobalSmbus_0Provisioning_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure smb_0SlaveAddress in AQ_GlobalSmbus_0Provisioning_HHD */
#define AQ_GlobalSmbus_0Provisioning_HHD_smb_0SlaveAddress 0
/*! \brief Preprocessor variable to relate field to bit position in structure smb_0SlaveAddress in AQ_GlobalSmbus_0Provisioning_HHD */
#define bits_AQ_GlobalSmbus_0Provisioning_HHD_smb_0SlaveAddress u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure smb_0SlaveAddress in AQ_GlobalSmbus_0Provisioning_HHD */
#define word_AQ_GlobalSmbus_0Provisioning_HHD_smb_0SlaveAddress u0.word_0

/*! \brief Base register address of structure AQ_GlobalSmbus_1Provisioning_HHD */
#define AQ_GlobalSmbus_1Provisioning_HHD_baseRegisterAddress 0xC495
/*! \brief MMD address of structure AQ_GlobalSmbus_1Provisioning_HHD */
#define AQ_GlobalSmbus_1Provisioning_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure smb_1SlaveAddress in AQ_GlobalSmbus_1Provisioning_HHD */
#define AQ_GlobalSmbus_1Provisioning_HHD_smb_1SlaveAddress 0
/*! \brief Preprocessor variable to relate field to bit position in structure smb_1SlaveAddress in AQ_GlobalSmbus_1Provisioning_HHD */
#define bits_AQ_GlobalSmbus_1Provisioning_HHD_smb_1SlaveAddress u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure smb_1SlaveAddress in AQ_GlobalSmbus_1Provisioning_HHD */
#define word_AQ_GlobalSmbus_1Provisioning_HHD_smb_1SlaveAddress u0.word_0

/*! \brief Base register address of structure AQ_GlobalEeeProvisioning_HHD */
#define AQ_GlobalEeeProvisioning_HHD_baseRegisterAddress 0xC4A0
/*! \brief MMD address of structure AQ_GlobalEeeProvisioning_HHD */
#define AQ_GlobalEeeProvisioning_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure eeeMode in AQ_GlobalEeeProvisioning_HHD */
#define AQ_GlobalEeeProvisioning_HHD_eeeMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure eeeMode in AQ_GlobalEeeProvisioning_HHD */
#define bits_AQ_GlobalEeeProvisioning_HHD_eeeMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure eeeMode in AQ_GlobalEeeProvisioning_HHD */
#define word_AQ_GlobalEeeProvisioning_HHD_eeeMode u0.word_0

/*! \brief Base register address of structure AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_baseRegisterAddress 0xC800
/*! \brief MMD address of structure AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pairAStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairAStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairAStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairAStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairBStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairBStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairBStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairBStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairBStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairBStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairCStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairCStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairCStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairCStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairCStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairCStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairDStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairDStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairDStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairDStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairDStatus in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairDStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairAReflection_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairAReflection_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairAReflection_1 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairAReflection_2 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairAReflection_2 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairAReflection_2 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure impulseResponseMSW in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_impulseResponseMSW 2
/*! \brief Preprocessor variable to relate field to bit position in structure impulseResponseMSW in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_impulseResponseMSW u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure impulseResponseMSW in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_impulseResponseMSW u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairBReflection_1 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairBReflection_1 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairBReflection_1 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairBReflection_2 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairBReflection_2 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairBReflection_2 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure impulseResponseLSW in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_impulseResponseLSW 4
/*! \brief Preprocessor variable to relate field to bit position in structure impulseResponseLSW in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_impulseResponseLSW u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure impulseResponseLSW in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_impulseResponseLSW u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairCReflection_1 5
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairCReflection_1 u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairCReflection_1 u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairCReflection_2 5
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairCReflection_2 u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairCReflection_2 u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure reserved_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_reserved_1 6
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_reserved_1 u6.bits_6
/*! \brief Preprocessor variable to relate field to word position in structure reserved_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_reserved_1 u6.word_6
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairDReflection_1 7
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairDReflection_1 u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_1 in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairDReflection_1 u7.word_7
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define AQ_GlobalCableDiagnosticStatus_HHD_pairDReflection_2 7
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define bits_AQ_GlobalCableDiagnosticStatus_HHD_pairDReflection_2 u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_2 in AQ_GlobalCableDiagnosticStatus_HHD */
#define word_AQ_GlobalCableDiagnosticStatus_HHD_pairDReflection_2 u7.word_7

/*! \brief Base register address of structure AQ_GlobalThermalStatus_HHD */
#define AQ_GlobalThermalStatus_HHD_baseRegisterAddress 0xC820
/*! \brief MMD address of structure AQ_GlobalThermalStatus_HHD */
#define AQ_GlobalThermalStatus_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure temperature in AQ_GlobalThermalStatus_HHD */
#define AQ_GlobalThermalStatus_HHD_temperature 0
/*! \brief Preprocessor variable to relate field to bit position in structure temperature in AQ_GlobalThermalStatus_HHD */
#define bits_AQ_GlobalThermalStatus_HHD_temperature u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure temperature in AQ_GlobalThermalStatus_HHD */
#define word_AQ_GlobalThermalStatus_HHD_temperature u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure temperatureReady in AQ_GlobalThermalStatus_HHD */
#define AQ_GlobalThermalStatus_HHD_temperatureReady 1
/*! \brief Preprocessor variable to relate field to bit position in structure temperatureReady in AQ_GlobalThermalStatus_HHD */
#define bits_AQ_GlobalThermalStatus_HHD_temperatureReady u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure temperatureReady in AQ_GlobalThermalStatus_HHD */
#define word_AQ_GlobalThermalStatus_HHD_temperatureReady u1.word_1

/*! \brief Base register address of structure AQ_GlobalGeneralStatus_HHD */
#define AQ_GlobalGeneralStatus_HHD_baseRegisterAddress 0xC830
/*! \brief MMD address of structure AQ_GlobalGeneralStatus_HHD */
#define AQ_GlobalGeneralStatus_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureFailureState in AQ_GlobalGeneralStatus_HHD */
#define AQ_GlobalGeneralStatus_HHD_highTemperatureFailureState 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureFailureState in AQ_GlobalGeneralStatus_HHD */
#define bits_AQ_GlobalGeneralStatus_HHD_highTemperatureFailureState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureFailureState in AQ_GlobalGeneralStatus_HHD */
#define word_AQ_GlobalGeneralStatus_HHD_highTemperatureFailureState u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureFailureState in AQ_GlobalGeneralStatus_HHD */
#define AQ_GlobalGeneralStatus_HHD_lowTemperatureFailureState 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureFailureState in AQ_GlobalGeneralStatus_HHD */
#define bits_AQ_GlobalGeneralStatus_HHD_lowTemperatureFailureState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureFailureState in AQ_GlobalGeneralStatus_HHD */
#define word_AQ_GlobalGeneralStatus_HHD_lowTemperatureFailureState u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureWarningState in AQ_GlobalGeneralStatus_HHD */
#define AQ_GlobalGeneralStatus_HHD_highTemperatureWarningState 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureWarningState in AQ_GlobalGeneralStatus_HHD */
#define bits_AQ_GlobalGeneralStatus_HHD_highTemperatureWarningState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureWarningState in AQ_GlobalGeneralStatus_HHD */
#define word_AQ_GlobalGeneralStatus_HHD_highTemperatureWarningState u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureWarningState in AQ_GlobalGeneralStatus_HHD */
#define AQ_GlobalGeneralStatus_HHD_lowTemperatureWarningState 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureWarningState in AQ_GlobalGeneralStatus_HHD */
#define bits_AQ_GlobalGeneralStatus_HHD_lowTemperatureWarningState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureWarningState in AQ_GlobalGeneralStatus_HHD */
#define word_AQ_GlobalGeneralStatus_HHD_lowTemperatureWarningState u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure processorIntensiveMdioOperationIn_Progress in AQ_GlobalGeneralStatus_HHD */
#define AQ_GlobalGeneralStatus_HHD_processorIntensiveMdioOperationIn_Progress 1
/*! \brief Preprocessor variable to relate field to bit position in structure processorIntensiveMdioOperationIn_Progress in AQ_GlobalGeneralStatus_HHD */
#define bits_AQ_GlobalGeneralStatus_HHD_processorIntensiveMdioOperationIn_Progress u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure processorIntensiveMdioOperationIn_Progress in AQ_GlobalGeneralStatus_HHD */
#define word_AQ_GlobalGeneralStatus_HHD_processorIntensiveMdioOperationIn_Progress u1.word_1

/*! \brief Base register address of structure AQ_GlobalPinStatus_HHD */
#define AQ_GlobalPinStatus_HHD_baseRegisterAddress 0xC840
/*! \brief MMD address of structure AQ_GlobalPinStatus_HHD */
#define AQ_GlobalPinStatus_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure dcMasterN in AQ_GlobalPinStatus_HHD */
#define AQ_GlobalPinStatus_HHD_dcMasterN 0
/*! \brief Preprocessor variable to relate field to bit position in structure dcMasterN in AQ_GlobalPinStatus_HHD */
#define bits_AQ_GlobalPinStatus_HHD_dcMasterN u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure dcMasterN in AQ_GlobalPinStatus_HHD */
#define word_AQ_GlobalPinStatus_HHD_dcMasterN u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure packageConnectivity in AQ_GlobalPinStatus_HHD */
#define AQ_GlobalPinStatus_HHD_packageConnectivity 0
/*! \brief Preprocessor variable to relate field to bit position in structure packageConnectivity in AQ_GlobalPinStatus_HHD */
#define bits_AQ_GlobalPinStatus_HHD_packageConnectivity u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure packageConnectivity in AQ_GlobalPinStatus_HHD */
#define word_AQ_GlobalPinStatus_HHD_packageConnectivity u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure txEnable in AQ_GlobalPinStatus_HHD */
#define AQ_GlobalPinStatus_HHD_txEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure txEnable in AQ_GlobalPinStatus_HHD */
#define bits_AQ_GlobalPinStatus_HHD_txEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure txEnable in AQ_GlobalPinStatus_HHD */
#define word_AQ_GlobalPinStatus_HHD_txEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure ledPullupState in AQ_GlobalPinStatus_HHD */
#define AQ_GlobalPinStatus_HHD_ledPullupState 0
/*! \brief Preprocessor variable to relate field to bit position in structure ledPullupState in AQ_GlobalPinStatus_HHD */
#define bits_AQ_GlobalPinStatus_HHD_ledPullupState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure ledPullupState in AQ_GlobalPinStatus_HHD */
#define word_AQ_GlobalPinStatus_HHD_ledPullupState u0.word_0

/*! \brief Base register address of structure AQ_GlobalDaisyChainStatus_HHD */
#define AQ_GlobalDaisyChainStatus_HHD_baseRegisterAddress 0xC842
/*! \brief MMD address of structure AQ_GlobalDaisyChainStatus_HHD */
#define AQ_GlobalDaisyChainStatus_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure rxDaisyChainCalculatedCrc in AQ_GlobalDaisyChainStatus_HHD */
#define AQ_GlobalDaisyChainStatus_HHD_rxDaisyChainCalculatedCrc 0
/*! \brief Preprocessor variable to relate field to bit position in structure rxDaisyChainCalculatedCrc in AQ_GlobalDaisyChainStatus_HHD */
#define bits_AQ_GlobalDaisyChainStatus_HHD_rxDaisyChainCalculatedCrc u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure rxDaisyChainCalculatedCrc in AQ_GlobalDaisyChainStatus_HHD */
#define word_AQ_GlobalDaisyChainStatus_HHD_rxDaisyChainCalculatedCrc u0.word_0

/*! \brief Base register address of structure AQ_GlobalFaultMessage_HHD */
#define AQ_GlobalFaultMessage_HHD_baseRegisterAddress 0xC850
/*! \brief MMD address of structure AQ_GlobalFaultMessage_HHD */
#define AQ_GlobalFaultMessage_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure message in AQ_GlobalFaultMessage_HHD */
#define AQ_GlobalFaultMessage_HHD_message 0
/*! \brief Preprocessor variable to relate field to bit position in structure message in AQ_GlobalFaultMessage_HHD */
#define bits_AQ_GlobalFaultMessage_HHD_message u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure message in AQ_GlobalFaultMessage_HHD */
#define word_AQ_GlobalFaultMessage_HHD_message u0.word_0

/*! \brief Base register address of structure AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_baseRegisterAddress 0xC880
/*! \brief MMD address of structure AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure reserved_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_1 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_1 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_1 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_1 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_1 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_1 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reserved_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_2 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_2 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_2 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_2 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_2 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_2 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reserved_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_3 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_3 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_3 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_3 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_3 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_3 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reserved_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_4 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_4 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_4 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_4 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_4 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairAReflection_4 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reserved_5 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_5 1
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_5 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_5 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reserved_5 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_5 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_1 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reserved_6 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_6 1
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_6 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_6 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reserved_6 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_6 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_2 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_2 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_2 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reserved_7 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_7 1
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_7 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_7 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reserved_7 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_7 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_3 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_3 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_3 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reserved_8 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_8 1
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_8 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_8 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reserved_8 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_8 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_4 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_4 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairBReflection_4 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reserved_9 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_9 2
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_9 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_9 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reserved_9 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_9 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_1 2
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_1 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_1 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reserved_10 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_10 2
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_10 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_10 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reserved_10 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_10 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_2 2
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_2 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_2 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reserved_11 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_11 2
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_11 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_11 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reserved_11 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_11 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_3 2
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_3 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_3 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reserved_12 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_12 2
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_12 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_12 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reserved_12 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_12 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_4 2
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_4 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairCReflection_4 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reserved_13 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_13 3
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_13 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_13 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reserved_13 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_13 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_1 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_1 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_1 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_1 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure reserved_14 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_14 3
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_14 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_14 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reserved_14 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_14 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_2 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_2 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_2 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_2 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure reserved_15 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_15 3
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_15 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_15 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reserved_15 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_15 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_3 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_3 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_3 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_3 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure reserved_16 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_reserved_16 3
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_16 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_16 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reserved_16 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_reserved_16 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_4 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define bits_AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_4 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_4 in AQ_GlobalCableDiagnosticImpedance_HHD */
#define word_AQ_GlobalCableDiagnosticImpedance_HHD_pairDReflection_4 u3.word_3

/*! \brief Base register address of structure AQ_GlobalStatus_HHD */
#define AQ_GlobalStatus_HHD_baseRegisterAddress 0xC884
/*! \brief MMD address of structure AQ_GlobalStatus_HHD */
#define AQ_GlobalStatus_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure reservedStatus_0 in AQ_GlobalStatus_HHD */
#define AQ_GlobalStatus_HHD_reservedStatus_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedStatus_0 in AQ_GlobalStatus_HHD */
#define bits_AQ_GlobalStatus_HHD_reservedStatus_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedStatus_0 in AQ_GlobalStatus_HHD */
#define word_AQ_GlobalStatus_HHD_reservedStatus_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure cableLength in AQ_GlobalStatus_HHD */
#define AQ_GlobalStatus_HHD_cableLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure cableLength in AQ_GlobalStatus_HHD */
#define bits_AQ_GlobalStatus_HHD_cableLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure cableLength in AQ_GlobalStatus_HHD */
#define word_AQ_GlobalStatus_HHD_cableLength u0.word_0

/*! \brief Base register address of structure AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_baseRegisterAddress 0xC885
/*! \brief MMD address of structure AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure nearlySecondsMSW in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_nearlySecondsMSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure nearlySecondsMSW in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_nearlySecondsMSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nearlySecondsMSW in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_nearlySecondsMSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure xenpakNvrStatus in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_xenpakNvrStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure xenpakNvrStatus in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_xenpakNvrStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure xenpakNvrStatus in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_xenpakNvrStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure firmwareBuildID in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_firmwareBuildID 0
/*! \brief Preprocessor variable to relate field to bit position in structure firmwareBuildID in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_firmwareBuildID u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure firmwareBuildID in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_firmwareBuildID u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure provisioningID in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_provisioningID 0
/*! \brief Preprocessor variable to relate field to bit position in structure provisioningID in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_provisioningID u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure provisioningID in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_provisioningID u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nearlySecondsLSW in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_nearlySecondsLSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure nearlySecondsLSW in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_nearlySecondsLSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure nearlySecondsLSW in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_nearlySecondsLSW u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure dteStatus in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_dteStatus 2
/*! \brief Preprocessor variable to relate field to bit position in structure dteStatus in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_dteStatus u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure dteStatus in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_dteStatus u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure powerUpStallStatus in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_powerUpStallStatus 2
/*! \brief Preprocessor variable to relate field to bit position in structure powerUpStallStatus in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_powerUpStallStatus u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure powerUpStallStatus in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_powerUpStallStatus u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reservedStatus_3 in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_reservedStatus_3 2
/*! \brief Preprocessor variable to relate field to bit position in structure reservedStatus_3 in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_reservedStatus_3 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reservedStatus_3 in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_reservedStatus_3 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure loopbackStatus in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_loopbackStatus 3
/*! \brief Preprocessor variable to relate field to bit position in structure loopbackStatus in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_loopbackStatus u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure loopbackStatus in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_loopbackStatus u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure reservedStatus_4 in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_reservedStatus_4 3
/*! \brief Preprocessor variable to relate field to bit position in structure reservedStatus_4 in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_reservedStatus_4 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reservedStatus_4 in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_reservedStatus_4 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure mdiPacketGenerationStatus in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_mdiPacketGenerationStatus 3
/*! \brief Preprocessor variable to relate field to bit position in structure mdiPacketGenerationStatus in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_mdiPacketGenerationStatus u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure mdiPacketGenerationStatus in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_mdiPacketGenerationStatus u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure look_asidePortPacketGenerationStatus in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_look_asidePortPacketGenerationStatus 3
/*! \brief Preprocessor variable to relate field to bit position in structure look_asidePortPacketGenerationStatus in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_look_asidePortPacketGenerationStatus u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure look_asidePortPacketGenerationStatus in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_look_asidePortPacketGenerationStatus u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure systemI_fPacketGenerationStatus in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_systemI_fPacketGenerationStatus 3
/*! \brief Preprocessor variable to relate field to bit position in structure systemI_fPacketGenerationStatus in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_systemI_fPacketGenerationStatus u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure systemI_fPacketGenerationStatus in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_systemI_fPacketGenerationStatus u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure reservedStatus_4a in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_reservedStatus_4a 3
/*! \brief Preprocessor variable to relate field to bit position in structure reservedStatus_4a in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_reservedStatus_4a u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reservedStatus_4a in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_reservedStatus_4a u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure rate in AQ_GlobalReservedStatus_HHD */
#define AQ_GlobalReservedStatus_HHD_rate 3
/*! \brief Preprocessor variable to relate field to bit position in structure rate in AQ_GlobalReservedStatus_HHD */
#define bits_AQ_GlobalReservedStatus_HHD_rate u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure rate in AQ_GlobalReservedStatus_HHD */
#define word_AQ_GlobalReservedStatus_HHD_rate u3.word_3

/*! \brief Base register address of structure AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_baseRegisterAddress 0xCC00
/*! \brief MMD address of structure AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureFailure in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_highTemperatureFailure 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureFailure in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_highTemperatureFailure u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureFailure in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_highTemperatureFailure u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureFailure in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_lowTemperatureFailure 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureFailure in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_lowTemperatureFailure u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureFailure in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_lowTemperatureFailure u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureWarning in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_highTemperatureWarning 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureWarning in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_highTemperatureWarning u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureWarning in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_highTemperatureWarning u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureWarning in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_lowTemperatureWarning 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureWarning in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_lowTemperatureWarning u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureWarning in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_lowTemperatureWarning u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure resetCompleted in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_resetCompleted 0
/*! \brief Preprocessor variable to relate field to bit position in structure resetCompleted in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_resetCompleted u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure resetCompleted in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_resetCompleted u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure deviceFault in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_deviceFault 0
/*! \brief Preprocessor variable to relate field to bit position in structure deviceFault in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_deviceFault u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure deviceFault in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_deviceFault u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmA in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_reservedAlarmA 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmA in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_reservedAlarmA u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmA in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_reservedAlarmA u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmB in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_reservedAlarmB 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmB in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_reservedAlarmB u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmB in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_reservedAlarmB u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmC in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_reservedAlarmC 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmC in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_reservedAlarmC u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmC in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_reservedAlarmC u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmD in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_reservedAlarmD 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmD in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_reservedAlarmD u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmD in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_reservedAlarmD u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure smartPower_downEntered in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_smartPower_downEntered 1
/*! \brief Preprocessor variable to relate field to bit position in structure smartPower_downEntered in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_smartPower_downEntered u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure smartPower_downEntered in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_smartPower_downEntered u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure xenpakAlarm in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_xenpakAlarm 1
/*! \brief Preprocessor variable to relate field to bit position in structure xenpakAlarm in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_xenpakAlarm u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure xenpakAlarm in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_xenpakAlarm u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure ipPhoneDetect in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_ipPhoneDetect 1
/*! \brief Preprocessor variable to relate field to bit position in structure ipPhoneDetect in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_ipPhoneDetect u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure ipPhoneDetect in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_ipPhoneDetect u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure dteStatusChange in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_dteStatusChange 1
/*! \brief Preprocessor variable to relate field to bit position in structure dteStatusChange in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_dteStatusChange u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure dteStatusChange in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_dteStatusChange u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarms in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_reservedAlarms 1
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarms in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_reservedAlarms u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarms in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_reservedAlarms u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioCommandHandlingOverflow in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_mdioCommandHandlingOverflow 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioCommandHandlingOverflow in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_mdioCommandHandlingOverflow u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioCommandHandlingOverflow in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_mdioCommandHandlingOverflow u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrOperationComplete in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_nvrOperationComplete 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrOperationComplete in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_nvrOperationComplete u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrOperationComplete in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_nvrOperationComplete u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mailboxOperation_Complete in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_mailboxOperation_Complete 2
/*! \brief Preprocessor variable to relate field to bit position in structure mailboxOperation_Complete in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_mailboxOperation_Complete u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mailboxOperation_Complete in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_mailboxOperation_Complete u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upDramParityError in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_upDramParityError 2
/*! \brief Preprocessor variable to relate field to bit position in structure upDramParityError in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_upDramParityError u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upDramParityError in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_upDramParityError u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upIramParityError in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_upIramParityError 2
/*! \brief Preprocessor variable to relate field to bit position in structure upIramParityError in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_upIramParityError u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upIramParityError in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_upIramParityError u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure txEnableStateChange in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_txEnableStateChange 2
/*! \brief Preprocessor variable to relate field to bit position in structure txEnableStateChange in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_txEnableStateChange u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure txEnableStateChange in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_txEnableStateChange u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mdioMMD_Error in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_mdioMMD_Error 2
/*! \brief Preprocessor variable to relate field to bit position in structure mdioMMD_Error in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_mdioMMD_Error u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mdioMMD_Error in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_mdioMMD_Error u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mdioTimeoutError in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_mdioTimeoutError 2
/*! \brief Preprocessor variable to relate field to bit position in structure mdioTimeoutError in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_mdioTimeoutError u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mdioTimeoutError in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_mdioTimeoutError u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure watchdogTimerAlarm in AQ_GlobalAlarms_HHD */
#define AQ_GlobalAlarms_HHD_watchdogTimerAlarm 2
/*! \brief Preprocessor variable to relate field to bit position in structure watchdogTimerAlarm in AQ_GlobalAlarms_HHD */
#define bits_AQ_GlobalAlarms_HHD_watchdogTimerAlarm u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure watchdogTimerAlarm in AQ_GlobalAlarms_HHD */
#define word_AQ_GlobalAlarms_HHD_watchdogTimerAlarm u2.word_2

/*! \brief Base register address of structure AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_baseRegisterAddress 0xD400
/*! \brief MMD address of structure AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureFailureMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_highTemperatureFailureMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureFailureMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_highTemperatureFailureMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureFailureMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_highTemperatureFailureMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureFailureMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_lowTemperatureFailureMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureFailureMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_lowTemperatureFailureMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureFailureMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_lowTemperatureFailureMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureWarningMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_highTemperatureWarningMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureWarningMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_highTemperatureWarningMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureWarningMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_highTemperatureWarningMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureWarningMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_lowTemperatureWarningMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureWarningMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_lowTemperatureWarningMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureWarningMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_lowTemperatureWarningMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure resetCompletedMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_resetCompletedMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure resetCompletedMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_resetCompletedMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure resetCompletedMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_resetCompletedMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure deviceFaultMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_deviceFaultMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure deviceFaultMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_deviceFaultMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure deviceFaultMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_deviceFaultMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmAMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_reservedAlarmAMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmAMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_reservedAlarmAMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmAMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_reservedAlarmAMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmBMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_reservedAlarmBMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmBMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_reservedAlarmBMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmBMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_reservedAlarmBMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmCMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_reservedAlarmCMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmCMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_reservedAlarmCMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmCMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_reservedAlarmCMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmDMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_reservedAlarmDMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmDMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_reservedAlarmDMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmDMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_reservedAlarmDMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure smartPower_downEnteredMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_smartPower_downEnteredMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure smartPower_downEnteredMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_smartPower_downEnteredMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure smartPower_downEnteredMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_smartPower_downEnteredMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure xenpakAlarmMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_xenpakAlarmMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure xenpakAlarmMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_xenpakAlarmMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure xenpakAlarmMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_xenpakAlarmMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure ipPhoneDetectMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_ipPhoneDetectMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure ipPhoneDetectMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_ipPhoneDetectMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure ipPhoneDetectMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_ipPhoneDetectMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure dteStatusChangeMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_dteStatusChangeMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure dteStatusChangeMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_dteStatusChangeMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure dteStatusChangeMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_dteStatusChangeMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmsMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_reservedAlarmsMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmsMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_reservedAlarmsMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmsMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_reservedAlarmsMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioCommandHandlingOverflowMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_mdioCommandHandlingOverflowMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioCommandHandlingOverflowMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_mdioCommandHandlingOverflowMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioCommandHandlingOverflowMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_mdioCommandHandlingOverflowMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure diagnosticAlarmMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_diagnosticAlarmMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure diagnosticAlarmMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_diagnosticAlarmMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure diagnosticAlarmMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_diagnosticAlarmMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrOperationCompleteMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_nvrOperationCompleteMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrOperationCompleteMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_nvrOperationCompleteMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrOperationCompleteMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_nvrOperationCompleteMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mailboxOperationCompleteMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_mailboxOperationCompleteMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure mailboxOperationCompleteMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_mailboxOperationCompleteMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mailboxOperationCompleteMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_mailboxOperationCompleteMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upDramParityErrorMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_upDramParityErrorMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure upDramParityErrorMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_upDramParityErrorMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upDramParityErrorMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_upDramParityErrorMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upIramParityErrorMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_upIramParityErrorMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure upIramParityErrorMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_upIramParityErrorMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upIramParityErrorMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_upIramParityErrorMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure txEnableStateChangeMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_txEnableStateChangeMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure txEnableStateChangeMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_txEnableStateChangeMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure txEnableStateChangeMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_txEnableStateChangeMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mdioMMD_ErrorMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_mdioMMD_ErrorMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure mdioMMD_ErrorMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_mdioMMD_ErrorMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mdioMMD_ErrorMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_mdioMMD_ErrorMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mdioTimeoutErrorMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_mdioTimeoutErrorMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure mdioTimeoutErrorMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_mdioTimeoutErrorMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mdioTimeoutErrorMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_mdioTimeoutErrorMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure watchdogTimerAlarmMask in AQ_GlobalInterruptMask_HHD */
#define AQ_GlobalInterruptMask_HHD_watchdogTimerAlarmMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure watchdogTimerAlarmMask in AQ_GlobalInterruptMask_HHD */
#define bits_AQ_GlobalInterruptMask_HHD_watchdogTimerAlarmMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure watchdogTimerAlarmMask in AQ_GlobalInterruptMask_HHD */
#define word_AQ_GlobalInterruptMask_HHD_watchdogTimerAlarmMask u2.word_2

/*! \brief Base register address of structure AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_baseRegisterAddress 0xFC00
/*! \brief MMD address of structure AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pmaStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_pmaStandardAlarm_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pmaStandardAlarm_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pmaStandardAlarm_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pmaStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_pmaStandardAlarm_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pmaStandardAlarm_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pmaStandardAlarm_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_pcsStandardAlarm_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pcsStandardAlarm_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pcsStandardAlarm_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_pcsStandardAlarm_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pcsStandardAlarm_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pcsStandardAlarm_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_3Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_pcsStandardAlarm_3Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_3Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pcsStandardAlarm_3Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_3Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_pcsStandardAlarm_3Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_StandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_phyXS_StandardAlarms_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_StandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_phyXS_StandardAlarms_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_StandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_phyXS_StandardAlarms_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_StandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_phyXS_StandardAlarms_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_StandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_phyXS_StandardAlarms_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_StandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_phyXS_StandardAlarms_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationStandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_autonegotiationStandardAlarms_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationStandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_autonegotiationStandardAlarms_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationStandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_autonegotiationStandardAlarms_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationStandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_autonegotiationStandardAlarms_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationStandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_autonegotiationStandardAlarms_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationStandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_autonegotiationStandardAlarms_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure gbeStandardAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_gbeStandardAlarmsInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure gbeStandardAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_gbeStandardAlarmsInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gbeStandardAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_gbeStandardAlarmsInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure allVendorAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define AQ_GlobalChip_wideStandardInterruptFlags_HHD_allVendorAlarmsInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure allVendorAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_HHD_allVendorAlarmsInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure allVendorAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_HHD_allVendorAlarmsInterrupt u0.word_0

/*! \brief Base register address of structure AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_baseRegisterAddress 0xFC01
/*! \brief MMD address of structure AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pmaVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_pmaVendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_HHD_pmaVendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_HHD_pmaVendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_pcsVendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_HHD_pcsVendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_HHD_pcsVendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_VendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_phyXS_VendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_VendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_HHD_phyXS_VendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_VendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_HHD_phyXS_VendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_autonegotiationVendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_HHD_autonegotiationVendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_HHD_autonegotiationVendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure gbeVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_gbeVendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure gbeVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_HHD_gbeVendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gbeVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_HHD_gbeVendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_1Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_globalAlarms_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_1Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_HHD_globalAlarms_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_1Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_HHD_globalAlarms_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_2Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_globalAlarms_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_2Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_HHD_globalAlarms_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_2Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_HHD_globalAlarms_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_3Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define AQ_GlobalChip_wideVendorInterruptFlags_HHD_globalAlarms_3Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_3Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_HHD_globalAlarms_3Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_3Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_HHD */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_HHD_globalAlarms_3Interrupt u0.word_0

/*! \brief Base register address of structure AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_baseRegisterAddress 0xFF00
/*! \brief MMD address of structure AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pmaStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_pmaStandardAlarm_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_pmaStandardAlarm_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_pmaStandardAlarm_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pmaStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_pmaStandardAlarm_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_pmaStandardAlarm_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_pmaStandardAlarm_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_pcsStandardAlarm_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_pcsStandardAlarm_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_pcsStandardAlarm_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_pcsStandardAlarm_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_pcsStandardAlarm_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_pcsStandardAlarm_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_3InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_pcsStandardAlarm_3InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_3InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_pcsStandardAlarm_3InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_3InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_pcsStandardAlarm_3InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_StandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_phyXS_StandardAlarms_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_StandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_phyXS_StandardAlarms_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_StandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_phyXS_StandardAlarms_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_StandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_phyXS_StandardAlarms_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_StandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_phyXS_StandardAlarms_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_StandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_phyXS_StandardAlarms_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationStandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_autonegotiationStandardAlarms_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationStandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_autonegotiationStandardAlarms_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationStandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_autonegotiationStandardAlarms_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationStandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_autonegotiationStandardAlarms_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationStandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_autonegotiationStandardAlarms_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationStandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_autonegotiationStandardAlarms_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure gbeStandardAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_gbeStandardAlarmsInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure gbeStandardAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_gbeStandardAlarmsInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gbeStandardAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_gbeStandardAlarmsInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure allVendorAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define AQ_GlobalInterruptChip_wideStandardMask_HHD_allVendorAlarmsInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure allVendorAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_HHD_allVendorAlarmsInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure allVendorAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_HHD */
#define word_AQ_GlobalInterruptChip_wideStandardMask_HHD_allVendorAlarmsInterruptMask u0.word_0

/*! \brief Base register address of structure AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_baseRegisterAddress 0xFF01
/*! \brief MMD address of structure AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pmaVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_pmaVendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_HHD_pmaVendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define word_AQ_GlobalInterruptChip_wideVendorMask_HHD_pmaVendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_pcsVendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_HHD_pcsVendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define word_AQ_GlobalInterruptChip_wideVendorMask_HHD_pcsVendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_VendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_phyXS_VendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_VendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_HHD_phyXS_VendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_VendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define word_AQ_GlobalInterruptChip_wideVendorMask_HHD_phyXS_VendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_autonegotiationVendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_HHD_autonegotiationVendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define word_AQ_GlobalInterruptChip_wideVendorMask_HHD_autonegotiationVendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure gbeVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_gbeVendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure gbeVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_HHD_gbeVendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gbeVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define word_AQ_GlobalInterruptChip_wideVendorMask_HHD_gbeVendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_globalAlarms_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_HHD_globalAlarms_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define word_AQ_GlobalInterruptChip_wideVendorMask_HHD_globalAlarms_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_globalAlarms_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_HHD_globalAlarms_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define word_AQ_GlobalInterruptChip_wideVendorMask_HHD_globalAlarms_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_3InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define AQ_GlobalInterruptChip_wideVendorMask_HHD_globalAlarms_3InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_3InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_HHD_globalAlarms_3InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_3InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_HHD */
#define word_AQ_GlobalInterruptChip_wideVendorMask_HHD_globalAlarms_3InterruptMask u0.word_0
#endif
/*@}*/
/*@}*/
