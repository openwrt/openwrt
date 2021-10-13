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
#ifndef AQ_APPIA_GLOBAL_REGS_DEFINES_HEADER
#define AQ_APPIA_GLOBAL_REGS_DEFINES_HEADER


/*-----------------------------------------------------------------------------*/
/*Access macro definitions                                                     */
/*-----------------------------------------------------------------------------*/
/*! \brief Base register address of structure AQ_GlobalStandardControl_1_APPIA */
#define AQ_GlobalStandardControl_1_APPIA_baseRegisterAddress 0x0000
/*! \brief MMD address of structure AQ_GlobalStandardControl_1_APPIA */
#define AQ_GlobalStandardControl_1_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure softReset in AQ_GlobalStandardControl_1_APPIA */
#define AQ_GlobalStandardControl_1_APPIA_softReset 0
/*! \brief Preprocessor variable to relate field to bit position in structure softReset in AQ_GlobalStandardControl_1_APPIA */
#define bits_AQ_GlobalStandardControl_1_APPIA_softReset u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure softReset in AQ_GlobalStandardControl_1_APPIA */
#define word_AQ_GlobalStandardControl_1_APPIA_softReset u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure hardReset in AQ_GlobalStandardControl_1_APPIA */
#define AQ_GlobalStandardControl_1_APPIA_hardReset 0
/*! \brief Preprocessor variable to relate field to bit position in structure hardReset in AQ_GlobalStandardControl_1_APPIA */
#define bits_AQ_GlobalStandardControl_1_APPIA_hardReset u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure hardReset in AQ_GlobalStandardControl_1_APPIA */
#define word_AQ_GlobalStandardControl_1_APPIA_hardReset u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowPower in AQ_GlobalStandardControl_1_APPIA */
#define AQ_GlobalStandardControl_1_APPIA_lowPower 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowPower in AQ_GlobalStandardControl_1_APPIA */
#define bits_AQ_GlobalStandardControl_1_APPIA_lowPower u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowPower in AQ_GlobalStandardControl_1_APPIA */
#define word_AQ_GlobalStandardControl_1_APPIA_lowPower u0.word_0

/*! \brief Base register address of structure AQ_GlobalStandardDeviceIdentifier_APPIA */
#define AQ_GlobalStandardDeviceIdentifier_APPIA_baseRegisterAddress 0x0002
/*! \brief MMD address of structure AQ_GlobalStandardDeviceIdentifier_APPIA */
#define AQ_GlobalStandardDeviceIdentifier_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure deviceIdMSW in AQ_GlobalStandardDeviceIdentifier_APPIA */
#define AQ_GlobalStandardDeviceIdentifier_APPIA_deviceIdMSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure deviceIdMSW in AQ_GlobalStandardDeviceIdentifier_APPIA */
#define bits_AQ_GlobalStandardDeviceIdentifier_APPIA_deviceIdMSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure deviceIdMSW in AQ_GlobalStandardDeviceIdentifier_APPIA */
#define word_AQ_GlobalStandardDeviceIdentifier_APPIA_deviceIdMSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure deviceIdLSW in AQ_GlobalStandardDeviceIdentifier_APPIA */
#define AQ_GlobalStandardDeviceIdentifier_APPIA_deviceIdLSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure deviceIdLSW in AQ_GlobalStandardDeviceIdentifier_APPIA */
#define bits_AQ_GlobalStandardDeviceIdentifier_APPIA_deviceIdLSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure deviceIdLSW in AQ_GlobalStandardDeviceIdentifier_APPIA */
#define word_AQ_GlobalStandardDeviceIdentifier_APPIA_deviceIdLSW u1.word_1

/*! \brief Base register address of structure AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_baseRegisterAddress 0x0005
/*! \brief MMD address of structure AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_autonegotiationPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardDevicesInPackage_APPIA_autonegotiationPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardDevicesInPackage_APPIA_autonegotiationPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure tcPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_tcPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure tcPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardDevicesInPackage_APPIA_tcPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure tcPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardDevicesInPackage_APPIA_tcPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure dteXsPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_dteXsPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure dteXsPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardDevicesInPackage_APPIA_dteXsPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure dteXsPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardDevicesInPackage_APPIA_dteXsPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_Present in AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_phyXS_Present 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_Present in AQ_GlobalStandardDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardDevicesInPackage_APPIA_phyXS_Present u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_Present in AQ_GlobalStandardDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardDevicesInPackage_APPIA_phyXS_Present u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_pcsPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardDevicesInPackage_APPIA_pcsPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardDevicesInPackage_APPIA_pcsPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure wisPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_wisPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure wisPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardDevicesInPackage_APPIA_wisPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure wisPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardDevicesInPackage_APPIA_wisPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pmaPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_pmaPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardDevicesInPackage_APPIA_pmaPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardDevicesInPackage_APPIA_pmaPresent u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure clause_22RegistersPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define AQ_GlobalStandardDevicesInPackage_APPIA_clause_22RegistersPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure clause_22RegistersPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardDevicesInPackage_APPIA_clause_22RegistersPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure clause_22RegistersPresent in AQ_GlobalStandardDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardDevicesInPackage_APPIA_clause_22RegistersPresent u0.word_0

/*! \brief Base register address of structure AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define AQ_GlobalStandardVendorDevicesInPackage_APPIA_baseRegisterAddress 0x0006
/*! \brief MMD address of structure AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define AQ_GlobalStandardVendorDevicesInPackage_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure vendorSpecificDevice_2Present in AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define AQ_GlobalStandardVendorDevicesInPackage_APPIA_vendorSpecificDevice_2Present 0
/*! \brief Preprocessor variable to relate field to bit position in structure vendorSpecificDevice_2Present in AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardVendorDevicesInPackage_APPIA_vendorSpecificDevice_2Present u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure vendorSpecificDevice_2Present in AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardVendorDevicesInPackage_APPIA_vendorSpecificDevice_2Present u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure vendorSpecificDevice_1Present in AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define AQ_GlobalStandardVendorDevicesInPackage_APPIA_vendorSpecificDevice_1Present 0
/*! \brief Preprocessor variable to relate field to bit position in structure vendorSpecificDevice_1Present in AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardVendorDevicesInPackage_APPIA_vendorSpecificDevice_1Present u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure vendorSpecificDevice_1Present in AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardVendorDevicesInPackage_APPIA_vendorSpecificDevice_1Present u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure clause_22ExtensionPresent in AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define AQ_GlobalStandardVendorDevicesInPackage_APPIA_clause_22ExtensionPresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure clause_22ExtensionPresent in AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define bits_AQ_GlobalStandardVendorDevicesInPackage_APPIA_clause_22ExtensionPresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure clause_22ExtensionPresent in AQ_GlobalStandardVendorDevicesInPackage_APPIA */
#define word_AQ_GlobalStandardVendorDevicesInPackage_APPIA_clause_22ExtensionPresent u0.word_0

/*! \brief Base register address of structure AQ_GlobalStandardStatus_2_APPIA */
#define AQ_GlobalStandardStatus_2_APPIA_baseRegisterAddress 0x0008
/*! \brief MMD address of structure AQ_GlobalStandardStatus_2_APPIA */
#define AQ_GlobalStandardStatus_2_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure devicePresent in AQ_GlobalStandardStatus_2_APPIA */
#define AQ_GlobalStandardStatus_2_APPIA_devicePresent 0
/*! \brief Preprocessor variable to relate field to bit position in structure devicePresent in AQ_GlobalStandardStatus_2_APPIA */
#define bits_AQ_GlobalStandardStatus_2_APPIA_devicePresent u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure devicePresent in AQ_GlobalStandardStatus_2_APPIA */
#define word_AQ_GlobalStandardStatus_2_APPIA_devicePresent u0.word_0

/*! \brief Base register address of structure AQ_GlobalStandardPackageIdentifier_APPIA */
#define AQ_GlobalStandardPackageIdentifier_APPIA_baseRegisterAddress 0x000E
/*! \brief MMD address of structure AQ_GlobalStandardPackageIdentifier_APPIA */
#define AQ_GlobalStandardPackageIdentifier_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure packageIdMSW in AQ_GlobalStandardPackageIdentifier_APPIA */
#define AQ_GlobalStandardPackageIdentifier_APPIA_packageIdMSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure packageIdMSW in AQ_GlobalStandardPackageIdentifier_APPIA */
#define bits_AQ_GlobalStandardPackageIdentifier_APPIA_packageIdMSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure packageIdMSW in AQ_GlobalStandardPackageIdentifier_APPIA */
#define word_AQ_GlobalStandardPackageIdentifier_APPIA_packageIdMSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure packageIdLSW in AQ_GlobalStandardPackageIdentifier_APPIA */
#define AQ_GlobalStandardPackageIdentifier_APPIA_packageIdLSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure packageIdLSW in AQ_GlobalStandardPackageIdentifier_APPIA */
#define bits_AQ_GlobalStandardPackageIdentifier_APPIA_packageIdLSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure packageIdLSW in AQ_GlobalStandardPackageIdentifier_APPIA */
#define word_AQ_GlobalStandardPackageIdentifier_APPIA_packageIdLSW u1.word_1

/*! \brief Base register address of structure AQ_GlobalFirmwareID_APPIA */
#define AQ_GlobalFirmwareID_APPIA_baseRegisterAddress 0x0020
/*! \brief MMD address of structure AQ_GlobalFirmwareID_APPIA */
#define AQ_GlobalFirmwareID_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure firmwareMajorRevisionNumber in AQ_GlobalFirmwareID_APPIA */
#define AQ_GlobalFirmwareID_APPIA_firmwareMajorRevisionNumber 0
/*! \brief Preprocessor variable to relate field to bit position in structure firmwareMajorRevisionNumber in AQ_GlobalFirmwareID_APPIA */
#define bits_AQ_GlobalFirmwareID_APPIA_firmwareMajorRevisionNumber u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure firmwareMajorRevisionNumber in AQ_GlobalFirmwareID_APPIA */
#define word_AQ_GlobalFirmwareID_APPIA_firmwareMajorRevisionNumber u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure firmwareMinorRevisionNumber in AQ_GlobalFirmwareID_APPIA */
#define AQ_GlobalFirmwareID_APPIA_firmwareMinorRevisionNumber 0
/*! \brief Preprocessor variable to relate field to bit position in structure firmwareMinorRevisionNumber in AQ_GlobalFirmwareID_APPIA */
#define bits_AQ_GlobalFirmwareID_APPIA_firmwareMinorRevisionNumber u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure firmwareMinorRevisionNumber in AQ_GlobalFirmwareID_APPIA */
#define word_AQ_GlobalFirmwareID_APPIA_firmwareMinorRevisionNumber u0.word_0

/*! \brief Base register address of structure AQ_GlobalChipIdentification_APPIA */
#define AQ_GlobalChipIdentification_APPIA_baseRegisterAddress 0x0021
/*! \brief MMD address of structure AQ_GlobalChipIdentification_APPIA */
#define AQ_GlobalChipIdentification_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure chipIdentification in AQ_GlobalChipIdentification_APPIA */
#define AQ_GlobalChipIdentification_APPIA_chipIdentification 0
/*! \brief Preprocessor variable to relate field to bit position in structure chipIdentification in AQ_GlobalChipIdentification_APPIA */
#define bits_AQ_GlobalChipIdentification_APPIA_chipIdentification u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure chipIdentification in AQ_GlobalChipIdentification_APPIA */
#define word_AQ_GlobalChipIdentification_APPIA_chipIdentification u0.word_0

/*! \brief Base register address of structure AQ_GlobalChipRevision_APPIA */
#define AQ_GlobalChipRevision_APPIA_baseRegisterAddress 0x0022
/*! \brief MMD address of structure AQ_GlobalChipRevision_APPIA */
#define AQ_GlobalChipRevision_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure chipRevision in AQ_GlobalChipRevision_APPIA */
#define AQ_GlobalChipRevision_APPIA_chipRevision 0
/*! \brief Preprocessor variable to relate field to bit position in structure chipRevision in AQ_GlobalChipRevision_APPIA */
#define bits_AQ_GlobalChipRevision_APPIA_chipRevision u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure chipRevision in AQ_GlobalChipRevision_APPIA */
#define word_AQ_GlobalChipRevision_APPIA_chipRevision u0.word_0

/*! \brief Base register address of structure AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_baseRegisterAddress 0x0100
/*! \brief MMD address of structure AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure nvrExecuteOperation in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrExecuteOperation 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrExecuteOperation in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrExecuteOperation u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrExecuteOperation in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrExecuteOperation u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrWriteMode in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrWriteMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrWriteMode in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrWriteMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrWriteMode in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrWriteMode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure freezeNvrCrc in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_freezeNvrCrc 0
/*! \brief Preprocessor variable to relate field to bit position in structure freezeNvrCrc in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_freezeNvrCrc u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure freezeNvrCrc in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_freezeNvrCrc u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure resetNvrCrc in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_resetNvrCrc 0
/*! \brief Preprocessor variable to relate field to bit position in structure resetNvrCrc in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_resetNvrCrc u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure resetNvrCrc in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_resetNvrCrc u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrBurst in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrBurst 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrBurst in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrBurst u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrBurst in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrBurst u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrBusy in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrBusy 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrBusy in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrBusy u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrBusy in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrBusy u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrOpcode in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrOpcode 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrOpcode in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrOpcode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrOpcode in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrOpcode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrMailboxCrc in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrMailboxCrc 1
/*! \brief Preprocessor variable to relate field to bit position in structure nvrMailboxCrc in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrMailboxCrc u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure nvrMailboxCrc in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrMailboxCrc u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrAddressMSW in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrAddressMSW 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrAddressMSW in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrAddressMSW u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrAddressMSW in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrAddressMSW u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure nvrAddressLSW in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrAddressLSW 3
/*! \brief Preprocessor variable to relate field to bit position in structure nvrAddressLSW in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrAddressLSW u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure nvrAddressLSW in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrAddressLSW u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure nvrDataMSW in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrDataMSW 4
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDataMSW in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrDataMSW u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure nvrDataMSW in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrDataMSW u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure nvrDataLSW in AQ_GlobalNvrInterface_APPIA */
#define AQ_GlobalNvrInterface_APPIA_nvrDataLSW 5
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDataLSW in AQ_GlobalNvrInterface_APPIA */
#define bits_AQ_GlobalNvrInterface_APPIA_nvrDataLSW u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure nvrDataLSW in AQ_GlobalNvrInterface_APPIA */
#define word_AQ_GlobalNvrInterface_APPIA_nvrDataLSW u5.word_5

/*! \brief Base register address of structure AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_baseRegisterAddress 0x0200
/*! \brief MMD address of structure AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxExecuteOperation in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_upMailboxExecuteOperation 0
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxExecuteOperation in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_upMailboxExecuteOperation u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxExecuteOperation in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_upMailboxExecuteOperation u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxWriteMode in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_upMailboxWriteMode 0
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxWriteMode in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_upMailboxWriteMode u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxWriteMode in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_upMailboxWriteMode u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure resetUpMailboxCrc in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_resetUpMailboxCrc 0
/*! \brief Preprocessor variable to relate field to bit position in structure resetUpMailboxCrc in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_resetUpMailboxCrc u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure resetUpMailboxCrc in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_resetUpMailboxCrc u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxBusy in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_upMailboxBusy 0
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxBusy in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_upMailboxBusy u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxBusy in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_upMailboxBusy u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxCrc in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_upMailboxCrc 1
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxCrc in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_upMailboxCrc u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxCrc in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_upMailboxCrc u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxAddressMSW in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_upMailboxAddressMSW 2
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxAddressMSW in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_upMailboxAddressMSW u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxAddressMSW in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_upMailboxAddressMSW u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxAddressLSW in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_upMailboxAddressLSW 3
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxAddressLSW in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_upMailboxAddressLSW u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxAddressLSW in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_upMailboxAddressLSW u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxAddressLSW_Don_tCare in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_upMailboxAddressLSW_Don_tCare 3
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxAddressLSW_Don_tCare in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_upMailboxAddressLSW_Don_tCare u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxAddressLSW_Don_tCare in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_upMailboxAddressLSW_Don_tCare u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxDataMSW in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_upMailboxDataMSW 4
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxDataMSW in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_upMailboxDataMSW u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxDataMSW in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_upMailboxDataMSW u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure upMailboxDataLSW in AQ_GlobalMailboxInterface_APPIA */
#define AQ_GlobalMailboxInterface_APPIA_upMailboxDataLSW 5
/*! \brief Preprocessor variable to relate field to bit position in structure upMailboxDataLSW in AQ_GlobalMailboxInterface_APPIA */
#define bits_AQ_GlobalMailboxInterface_APPIA_upMailboxDataLSW u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure upMailboxDataLSW in AQ_GlobalMailboxInterface_APPIA */
#define word_AQ_GlobalMailboxInterface_APPIA_upMailboxDataLSW u5.word_5

/*! \brief Base register address of structure AQ_GlobalMicroprocessorScratchPad_APPIA */
#define AQ_GlobalMicroprocessorScratchPad_APPIA_baseRegisterAddress 0x0300
/*! \brief MMD address of structure AQ_GlobalMicroprocessorScratchPad_APPIA */
#define AQ_GlobalMicroprocessorScratchPad_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure scratchPad_1 in AQ_GlobalMicroprocessorScratchPad_APPIA */
#define AQ_GlobalMicroprocessorScratchPad_APPIA_scratchPad_1 0
/*! \brief Preprocessor variable to relate field to bit position in structure scratchPad_1 in AQ_GlobalMicroprocessorScratchPad_APPIA */
#define bits_AQ_GlobalMicroprocessorScratchPad_APPIA_scratchPad_1 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure scratchPad_1 in AQ_GlobalMicroprocessorScratchPad_APPIA */
#define word_AQ_GlobalMicroprocessorScratchPad_APPIA_scratchPad_1 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure scratchPad_2 in AQ_GlobalMicroprocessorScratchPad_APPIA */
#define AQ_GlobalMicroprocessorScratchPad_APPIA_scratchPad_2 1
/*! \brief Preprocessor variable to relate field to bit position in structure scratchPad_2 in AQ_GlobalMicroprocessorScratchPad_APPIA */
#define bits_AQ_GlobalMicroprocessorScratchPad_APPIA_scratchPad_2 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure scratchPad_2 in AQ_GlobalMicroprocessorScratchPad_APPIA */
#define word_AQ_GlobalMicroprocessorScratchPad_APPIA_scratchPad_2 u1.word_1

/*! \brief Base register address of structure AQ_GlobalControl_APPIA */
#define AQ_GlobalControl_APPIA_baseRegisterAddress 0xC000
/*! \brief MMD address of structure AQ_GlobalControl_APPIA */
#define AQ_GlobalControl_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure upReset in AQ_GlobalControl_APPIA */
#define AQ_GlobalControl_APPIA_upReset 1
/*! \brief Preprocessor variable to relate field to bit position in structure upReset in AQ_GlobalControl_APPIA */
#define bits_AQ_GlobalControl_APPIA_upReset u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure upReset in AQ_GlobalControl_APPIA */
#define word_AQ_GlobalControl_APPIA_upReset u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure upRunStallOverride in AQ_GlobalControl_APPIA */
#define AQ_GlobalControl_APPIA_upRunStallOverride 1
/*! \brief Preprocessor variable to relate field to bit position in structure upRunStallOverride in AQ_GlobalControl_APPIA */
#define bits_AQ_GlobalControl_APPIA_upRunStallOverride u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure upRunStallOverride in AQ_GlobalControl_APPIA */
#define word_AQ_GlobalControl_APPIA_upRunStallOverride u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure upRunStall in AQ_GlobalControl_APPIA */
#define AQ_GlobalControl_APPIA_upRunStall 1
/*! \brief Preprocessor variable to relate field to bit position in structure upRunStall in AQ_GlobalControl_APPIA */
#define bits_AQ_GlobalControl_APPIA_upRunStall u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure upRunStall in AQ_GlobalControl_APPIA */
#define word_AQ_GlobalControl_APPIA_upRunStall u1.word_1

/*! \brief Base register address of structure AQ_GlobalResetControl_APPIA */
#define AQ_GlobalResetControl_APPIA_baseRegisterAddress 0xC006
/*! \brief MMD address of structure AQ_GlobalResetControl_APPIA */
#define AQ_GlobalResetControl_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure globalMMD_ResetDisable in AQ_GlobalResetControl_APPIA */
#define AQ_GlobalResetControl_APPIA_globalMMD_ResetDisable 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalMMD_ResetDisable in AQ_GlobalResetControl_APPIA */
#define bits_AQ_GlobalResetControl_APPIA_globalMMD_ResetDisable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalMMD_ResetDisable in AQ_GlobalResetControl_APPIA */
#define word_AQ_GlobalResetControl_APPIA_globalMMD_ResetDisable u0.word_0

/*! \brief Base register address of structure AQ_GlobalDiagnosticProvisioning_APPIA */
#define AQ_GlobalDiagnosticProvisioning_APPIA_baseRegisterAddress 0xC400
/*! \brief MMD address of structure AQ_GlobalDiagnosticProvisioning_APPIA */
#define AQ_GlobalDiagnosticProvisioning_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure enableDiagnostics in AQ_GlobalDiagnosticProvisioning_APPIA */
#define AQ_GlobalDiagnosticProvisioning_APPIA_enableDiagnostics 0
/*! \brief Preprocessor variable to relate field to bit position in structure enableDiagnostics in AQ_GlobalDiagnosticProvisioning_APPIA */
#define bits_AQ_GlobalDiagnosticProvisioning_APPIA_enableDiagnostics u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure enableDiagnostics in AQ_GlobalDiagnosticProvisioning_APPIA */
#define word_AQ_GlobalDiagnosticProvisioning_APPIA_enableDiagnostics u0.word_0

/*! \brief Base register address of structure AQ_GlobalThermalProvisioning_APPIA */
#define AQ_GlobalThermalProvisioning_APPIA_baseRegisterAddress 0xC420
/*! \brief MMD address of structure AQ_GlobalThermalProvisioning_APPIA */
#define AQ_GlobalThermalProvisioning_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure reserved_0 in AQ_GlobalThermalProvisioning_APPIA */
#define AQ_GlobalThermalProvisioning_APPIA_reserved_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_0 in AQ_GlobalThermalProvisioning_APPIA */
#define bits_AQ_GlobalThermalProvisioning_APPIA_reserved_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_0 in AQ_GlobalThermalProvisioning_APPIA */
#define word_AQ_GlobalThermalProvisioning_APPIA_reserved_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure highTempFailureThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define AQ_GlobalThermalProvisioning_APPIA_highTempFailureThreshold 1
/*! \brief Preprocessor variable to relate field to bit position in structure highTempFailureThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define bits_AQ_GlobalThermalProvisioning_APPIA_highTempFailureThreshold u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure highTempFailureThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define word_AQ_GlobalThermalProvisioning_APPIA_highTempFailureThreshold u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure lowTempFailureThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define AQ_GlobalThermalProvisioning_APPIA_lowTempFailureThreshold 2
/*! \brief Preprocessor variable to relate field to bit position in structure lowTempFailureThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define bits_AQ_GlobalThermalProvisioning_APPIA_lowTempFailureThreshold u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure lowTempFailureThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define word_AQ_GlobalThermalProvisioning_APPIA_lowTempFailureThreshold u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure highTempWarningThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define AQ_GlobalThermalProvisioning_APPIA_highTempWarningThreshold 3
/*! \brief Preprocessor variable to relate field to bit position in structure highTempWarningThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define bits_AQ_GlobalThermalProvisioning_APPIA_highTempWarningThreshold u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure highTempWarningThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define word_AQ_GlobalThermalProvisioning_APPIA_highTempWarningThreshold u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure lowTempWarningThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define AQ_GlobalThermalProvisioning_APPIA_lowTempWarningThreshold 4
/*! \brief Preprocessor variable to relate field to bit position in structure lowTempWarningThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define bits_AQ_GlobalThermalProvisioning_APPIA_lowTempWarningThreshold u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure lowTempWarningThreshold in AQ_GlobalThermalProvisioning_APPIA */
#define word_AQ_GlobalThermalProvisioning_APPIA_lowTempWarningThreshold u4.word_4

/*! \brief Base register address of structure AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_baseRegisterAddress 0xC430
/*! \brief MMD address of structure AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure led_0ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0ManualSet 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0ManualSet u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0ManualSet u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0_10Gb_sLinkEstablished 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0_10Gb_sLinkEstablished u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0_10Gb_sLinkEstablished u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0_1Gb_sLinkEstablished 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0_1Gb_sLinkEstablished u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0_1Gb_sLinkEstablished u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0_100Mb_sLinkEstablished 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0_100Mb_sLinkEstablished u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0_100Mb_sLinkEstablished u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0Connecting in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0Connecting 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0Connecting in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0Connecting u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0Connecting in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0Connecting u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0ReceiveActivity 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0ReceiveActivity u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0ReceiveActivity u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0TransmitActivity 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0TransmitActivity u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0TransmitActivity u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_0ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0ActivityStretch 0
/*! \brief Preprocessor variable to relate field to bit position in structure led_0ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0ActivityStretch u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure led_0ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0ActivityStretch u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure led_1ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1ManualSet 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1ManualSet u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1ManualSet u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1_10Gb_sLinkEstablished 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1_10Gb_sLinkEstablished u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1_10Gb_sLinkEstablished u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1_1Gb_sLinkEstablished 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1_1Gb_sLinkEstablished u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1_1Gb_sLinkEstablished u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1_100Mb_sLinkEstablished 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1_100Mb_sLinkEstablished u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1_100Mb_sLinkEstablished u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1Connecting in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1Connecting 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1Connecting in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1Connecting u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1Connecting in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1Connecting u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1ReceiveActivity 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1ReceiveActivity u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1ReceiveActivity u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1TransmitActivity 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1TransmitActivity u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1TransmitActivity u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_1ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1ActivityStretch 1
/*! \brief Preprocessor variable to relate field to bit position in structure led_1ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1ActivityStretch u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure led_1ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1ActivityStretch u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure led_2ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2ManualSet 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2ManualSet u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2ManualSet u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2_10Gb_sLinkEstablished 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2_10Gb_sLinkEstablished u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2_10Gb_sLinkEstablished u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2_1Gb_sLinkEstablished 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2_1Gb_sLinkEstablished u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2_1Gb_sLinkEstablished u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2_100Mb_sLinkEstablished 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2_100Mb_sLinkEstablished u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2_100Mb_sLinkEstablished u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2Connecting in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2Connecting 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2Connecting in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2Connecting u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2Connecting in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2Connecting u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2ReceiveActivity 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2ReceiveActivity u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2ReceiveActivity u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2TransmitActivity 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2TransmitActivity u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2TransmitActivity u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_2ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2ActivityStretch 2
/*! \brief Preprocessor variable to relate field to bit position in structure led_2ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2ActivityStretch u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure led_2ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2ActivityStretch u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure led_3ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3ManualSet 3
/*! \brief Preprocessor variable to relate field to bit position in structure led_3ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3ManualSet u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure led_3ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3ManualSet u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure led_3_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3_10Gb_sLinkEstablished 3
/*! \brief Preprocessor variable to relate field to bit position in structure led_3_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3_10Gb_sLinkEstablished u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure led_3_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3_10Gb_sLinkEstablished u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure led_3_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3_1Gb_sLinkEstablished 3
/*! \brief Preprocessor variable to relate field to bit position in structure led_3_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3_1Gb_sLinkEstablished u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure led_3_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3_1Gb_sLinkEstablished u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure led_3_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3_100Mb_sLinkEstablished 3
/*! \brief Preprocessor variable to relate field to bit position in structure led_3_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3_100Mb_sLinkEstablished u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure led_3_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3_100Mb_sLinkEstablished u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure led_3Connecting in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3Connecting 3
/*! \brief Preprocessor variable to relate field to bit position in structure led_3Connecting in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3Connecting u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure led_3Connecting in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3Connecting u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure led_3ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3ReceiveActivity 3
/*! \brief Preprocessor variable to relate field to bit position in structure led_3ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3ReceiveActivity u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure led_3ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3ReceiveActivity u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure led_3TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3TransmitActivity 3
/*! \brief Preprocessor variable to relate field to bit position in structure led_3TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3TransmitActivity u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure led_3TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3TransmitActivity u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure led_3ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3ActivityStretch 3
/*! \brief Preprocessor variable to relate field to bit position in structure led_3ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3ActivityStretch u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure led_3ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3ActivityStretch u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure led_4ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4ManualSet 4
/*! \brief Preprocessor variable to relate field to bit position in structure led_4ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4ManualSet u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure led_4ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4ManualSet u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure led_4_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4_10Gb_sLinkEstablished 4
/*! \brief Preprocessor variable to relate field to bit position in structure led_4_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4_10Gb_sLinkEstablished u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure led_4_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4_10Gb_sLinkEstablished u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure led_4_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4_1Gb_sLinkEstablished 4
/*! \brief Preprocessor variable to relate field to bit position in structure led_4_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4_1Gb_sLinkEstablished u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure led_4_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4_1Gb_sLinkEstablished u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure led_4_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4_100Mb_sLinkEstablished 4
/*! \brief Preprocessor variable to relate field to bit position in structure led_4_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4_100Mb_sLinkEstablished u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure led_4_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4_100Mb_sLinkEstablished u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure led_4Connecting in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4Connecting 4
/*! \brief Preprocessor variable to relate field to bit position in structure led_4Connecting in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4Connecting u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure led_4Connecting in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4Connecting u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure led_4ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4ReceiveActivity 4
/*! \brief Preprocessor variable to relate field to bit position in structure led_4ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4ReceiveActivity u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure led_4ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4ReceiveActivity u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure led_4TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4TransmitActivity 4
/*! \brief Preprocessor variable to relate field to bit position in structure led_4TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4TransmitActivity u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure led_4TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4TransmitActivity u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure led_4ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4ActivityStretch 4
/*! \brief Preprocessor variable to relate field to bit position in structure led_4ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4ActivityStretch u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure led_4ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4ActivityStretch u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure led_5ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5ManualSet 5
/*! \brief Preprocessor variable to relate field to bit position in structure led_5ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5ManualSet u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure led_5ManualSet in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5ManualSet u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure led_5_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5_10Gb_sLinkEstablished 5
/*! \brief Preprocessor variable to relate field to bit position in structure led_5_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5_10Gb_sLinkEstablished u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure led_5_10Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5_10Gb_sLinkEstablished u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure led_5_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5_1Gb_sLinkEstablished 5
/*! \brief Preprocessor variable to relate field to bit position in structure led_5_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5_1Gb_sLinkEstablished u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure led_5_1Gb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5_1Gb_sLinkEstablished u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure led_5_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5_100Mb_sLinkEstablished 5
/*! \brief Preprocessor variable to relate field to bit position in structure led_5_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5_100Mb_sLinkEstablished u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure led_5_100Mb_sLinkEstablished in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5_100Mb_sLinkEstablished u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure led_5Connecting in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5Connecting 5
/*! \brief Preprocessor variable to relate field to bit position in structure led_5Connecting in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5Connecting u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure led_5Connecting in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5Connecting u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure led_5ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5ReceiveActivity 5
/*! \brief Preprocessor variable to relate field to bit position in structure led_5ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5ReceiveActivity u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure led_5ReceiveActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5ReceiveActivity u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure led_5TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5TransmitActivity 5
/*! \brief Preprocessor variable to relate field to bit position in structure led_5TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5TransmitActivity u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure led_5TransmitActivity in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5TransmitActivity u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure led_5ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5ActivityStretch 5
/*! \brief Preprocessor variable to relate field to bit position in structure led_5ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5ActivityStretch u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure led_5ActivityStretch in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5ActivityStretch u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure ledOperationMode in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_ledOperationMode 7
/*! \brief Preprocessor variable to relate field to bit position in structure ledOperationMode in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_ledOperationMode u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure ledOperationMode in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_ledOperationMode u7.word_7
/*! \brief Preprocessor variable to relate field to word number in structure led_0DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0DriveThreeStateSelect 8
/*! \brief Preprocessor variable to relate field to bit position in structure led_0DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0DriveThreeStateSelect u8.bits_8
/*! \brief Preprocessor variable to relate field to word position in structure led_0DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0DriveThreeStateSelect u8.word_8
/*! \brief Preprocessor variable to relate field to word number in structure led_0ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0ActiveHighSelect 8
/*! \brief Preprocessor variable to relate field to bit position in structure led_0ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0ActiveHighSelect u8.bits_8
/*! \brief Preprocessor variable to relate field to word position in structure led_0ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0ActiveHighSelect u8.word_8
/*! \brief Preprocessor variable to relate field to word number in structure led_0ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_0ManualActiveSelect 8
/*! \brief Preprocessor variable to relate field to bit position in structure led_0ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_0ManualActiveSelect u8.bits_8
/*! \brief Preprocessor variable to relate field to word position in structure led_0ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_0ManualActiveSelect u8.word_8
/*! \brief Preprocessor variable to relate field to word number in structure led_1DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1DriveThreeStateSelect 9
/*! \brief Preprocessor variable to relate field to bit position in structure led_1DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1DriveThreeStateSelect u9.bits_9
/*! \brief Preprocessor variable to relate field to word position in structure led_1DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1DriveThreeStateSelect u9.word_9
/*! \brief Preprocessor variable to relate field to word number in structure led_1ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1ActiveHighSelect 9
/*! \brief Preprocessor variable to relate field to bit position in structure led_1ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1ActiveHighSelect u9.bits_9
/*! \brief Preprocessor variable to relate field to word position in structure led_1ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1ActiveHighSelect u9.word_9
/*! \brief Preprocessor variable to relate field to word number in structure led_1ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_1ManualActiveSelect 9
/*! \brief Preprocessor variable to relate field to bit position in structure led_1ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_1ManualActiveSelect u9.bits_9
/*! \brief Preprocessor variable to relate field to word position in structure led_1ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_1ManualActiveSelect u9.word_9
/*! \brief Preprocessor variable to relate field to word number in structure led_2DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2DriveThreeStateSelect 10
/*! \brief Preprocessor variable to relate field to bit position in structure led_2DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2DriveThreeStateSelect u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure led_2DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2DriveThreeStateSelect u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure led_2ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2ActiveHighSelect 10
/*! \brief Preprocessor variable to relate field to bit position in structure led_2ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2ActiveHighSelect u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure led_2ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2ActiveHighSelect u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure led_2ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_2ManualActiveSelect 10
/*! \brief Preprocessor variable to relate field to bit position in structure led_2ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_2ManualActiveSelect u10.bits_10
/*! \brief Preprocessor variable to relate field to word position in structure led_2ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_2ManualActiveSelect u10.word_10
/*! \brief Preprocessor variable to relate field to word number in structure led_3DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3DriveThreeStateSelect 11
/*! \brief Preprocessor variable to relate field to bit position in structure led_3DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3DriveThreeStateSelect u11.bits_11
/*! \brief Preprocessor variable to relate field to word position in structure led_3DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3DriveThreeStateSelect u11.word_11
/*! \brief Preprocessor variable to relate field to word number in structure led_3ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3ActiveHighSelect 11
/*! \brief Preprocessor variable to relate field to bit position in structure led_3ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3ActiveHighSelect u11.bits_11
/*! \brief Preprocessor variable to relate field to word position in structure led_3ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3ActiveHighSelect u11.word_11
/*! \brief Preprocessor variable to relate field to word number in structure led_3ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_3ManualActiveSelect 11
/*! \brief Preprocessor variable to relate field to bit position in structure led_3ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_3ManualActiveSelect u11.bits_11
/*! \brief Preprocessor variable to relate field to word position in structure led_3ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_3ManualActiveSelect u11.word_11
/*! \brief Preprocessor variable to relate field to word number in structure led_4DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4DriveThreeStateSelect 12
/*! \brief Preprocessor variable to relate field to bit position in structure led_4DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4DriveThreeStateSelect u12.bits_12
/*! \brief Preprocessor variable to relate field to word position in structure led_4DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4DriveThreeStateSelect u12.word_12
/*! \brief Preprocessor variable to relate field to word number in structure led_4ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4ActiveHighSelect 12
/*! \brief Preprocessor variable to relate field to bit position in structure led_4ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4ActiveHighSelect u12.bits_12
/*! \brief Preprocessor variable to relate field to word position in structure led_4ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4ActiveHighSelect u12.word_12
/*! \brief Preprocessor variable to relate field to word number in structure led_4ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_4ManualActiveSelect 12
/*! \brief Preprocessor variable to relate field to bit position in structure led_4ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_4ManualActiveSelect u12.bits_12
/*! \brief Preprocessor variable to relate field to word position in structure led_4ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_4ManualActiveSelect u12.word_12
/*! \brief Preprocessor variable to relate field to word number in structure led_5DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5DriveThreeStateSelect 13
/*! \brief Preprocessor variable to relate field to bit position in structure led_5DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5DriveThreeStateSelect u13.bits_13
/*! \brief Preprocessor variable to relate field to word position in structure led_5DriveThreeStateSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5DriveThreeStateSelect u13.word_13
/*! \brief Preprocessor variable to relate field to word number in structure led_5ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5ActiveHighSelect 13
/*! \brief Preprocessor variable to relate field to bit position in structure led_5ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5ActiveHighSelect u13.bits_13
/*! \brief Preprocessor variable to relate field to word position in structure led_5ActiveHighSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5ActiveHighSelect u13.word_13
/*! \brief Preprocessor variable to relate field to word number in structure led_5ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define AQ_GlobalLedProvisioning_APPIA_led_5ManualActiveSelect 13
/*! \brief Preprocessor variable to relate field to bit position in structure led_5ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define bits_AQ_GlobalLedProvisioning_APPIA_led_5ManualActiveSelect u13.bits_13
/*! \brief Preprocessor variable to relate field to word position in structure led_5ManualActiveSelect in AQ_GlobalLedProvisioning_APPIA */
#define word_AQ_GlobalLedProvisioning_APPIA_led_5ManualActiveSelect u13.word_13

/*! \brief Base register address of structure AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_baseRegisterAddress 0xC440
/*! \brief MMD address of structure AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure gangLoadMdioAddress in AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_gangLoadMdioAddress 0
/*! \brief Preprocessor variable to relate field to bit position in structure gangLoadMdioAddress in AQ_GlobalGeneralProvisioning_APPIA */
#define bits_AQ_GlobalGeneralProvisioning_APPIA_gangLoadMdioAddress u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gangLoadMdioAddress in AQ_GlobalGeneralProvisioning_APPIA */
#define word_AQ_GlobalGeneralProvisioning_APPIA_gangLoadMdioAddress u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure gangLoadMdioWriteOnly in AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_gangLoadMdioWriteOnly 0
/*! \brief Preprocessor variable to relate field to bit position in structure gangLoadMdioWriteOnly in AQ_GlobalGeneralProvisioning_APPIA */
#define bits_AQ_GlobalGeneralProvisioning_APPIA_gangLoadMdioWriteOnly u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gangLoadMdioWriteOnly in AQ_GlobalGeneralProvisioning_APPIA */
#define word_AQ_GlobalGeneralProvisioning_APPIA_gangLoadMdioWriteOnly u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure mdioBroadcastModeEnable in AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_mdioBroadcastModeEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioBroadcastModeEnable in AQ_GlobalGeneralProvisioning_APPIA */
#define bits_AQ_GlobalGeneralProvisioning_APPIA_mdioBroadcastModeEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioBroadcastModeEnable in AQ_GlobalGeneralProvisioning_APPIA */
#define word_AQ_GlobalGeneralProvisioning_APPIA_mdioBroadcastModeEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioReadMSW_FirstEnable in AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_mdioReadMSW_FirstEnable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioReadMSW_FirstEnable in AQ_GlobalGeneralProvisioning_APPIA */
#define bits_AQ_GlobalGeneralProvisioning_APPIA_mdioReadMSW_FirstEnable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioReadMSW_FirstEnable in AQ_GlobalGeneralProvisioning_APPIA */
#define word_AQ_GlobalGeneralProvisioning_APPIA_mdioReadMSW_FirstEnable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioDriveConfiguration in AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_mdioDriveConfiguration 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioDriveConfiguration in AQ_GlobalGeneralProvisioning_APPIA */
#define bits_AQ_GlobalGeneralProvisioning_APPIA_mdioDriveConfiguration u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioDriveConfiguration in AQ_GlobalGeneralProvisioning_APPIA */
#define word_AQ_GlobalGeneralProvisioning_APPIA_mdioDriveConfiguration u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioPreambleDetectionDisable in AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_mdioPreambleDetectionDisable 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioPreambleDetectionDisable in AQ_GlobalGeneralProvisioning_APPIA */
#define bits_AQ_GlobalGeneralProvisioning_APPIA_mdioPreambleDetectionDisable u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioPreambleDetectionDisable in AQ_GlobalGeneralProvisioning_APPIA */
#define word_AQ_GlobalGeneralProvisioning_APPIA_mdioPreambleDetectionDisable u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioAddressReset in AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_mdioAddressReset 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioAddressReset in AQ_GlobalGeneralProvisioning_APPIA */
#define bits_AQ_GlobalGeneralProvisioning_APPIA_mdioAddressReset u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioAddressReset in AQ_GlobalGeneralProvisioning_APPIA */
#define word_AQ_GlobalGeneralProvisioning_APPIA_mdioAddressReset u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure daisyChainReset in AQ_GlobalGeneralProvisioning_APPIA */
#define AQ_GlobalGeneralProvisioning_APPIA_daisyChainReset 2
/*! \brief Preprocessor variable to relate field to bit position in structure daisyChainReset in AQ_GlobalGeneralProvisioning_APPIA */
#define bits_AQ_GlobalGeneralProvisioning_APPIA_daisyChainReset u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure daisyChainReset in AQ_GlobalGeneralProvisioning_APPIA */
#define word_AQ_GlobalGeneralProvisioning_APPIA_daisyChainReset u2.word_2

/*! \brief Base register address of structure AQ_GlobalNvrProvisioning_APPIA */
#define AQ_GlobalNvrProvisioning_APPIA_baseRegisterAddress 0xC450
/*! \brief MMD address of structure AQ_GlobalNvrProvisioning_APPIA */
#define AQ_GlobalNvrProvisioning_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure nvrDataLength in AQ_GlobalNvrProvisioning_APPIA */
#define AQ_GlobalNvrProvisioning_APPIA_nvrDataLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDataLength in AQ_GlobalNvrProvisioning_APPIA */
#define bits_AQ_GlobalNvrProvisioning_APPIA_nvrDataLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrDataLength in AQ_GlobalNvrProvisioning_APPIA */
#define word_AQ_GlobalNvrProvisioning_APPIA_nvrDataLength u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrDummyLength in AQ_GlobalNvrProvisioning_APPIA */
#define AQ_GlobalNvrProvisioning_APPIA_nvrDummyLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDummyLength in AQ_GlobalNvrProvisioning_APPIA */
#define bits_AQ_GlobalNvrProvisioning_APPIA_nvrDummyLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrDummyLength in AQ_GlobalNvrProvisioning_APPIA */
#define word_AQ_GlobalNvrProvisioning_APPIA_nvrDummyLength u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrAddressLength in AQ_GlobalNvrProvisioning_APPIA */
#define AQ_GlobalNvrProvisioning_APPIA_nvrAddressLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure nvrAddressLength in AQ_GlobalNvrProvisioning_APPIA */
#define bits_AQ_GlobalNvrProvisioning_APPIA_nvrAddressLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nvrAddressLength in AQ_GlobalNvrProvisioning_APPIA */
#define word_AQ_GlobalNvrProvisioning_APPIA_nvrAddressLength u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nvrAddressLengthOverride in AQ_GlobalNvrProvisioning_APPIA */
#define AQ_GlobalNvrProvisioning_APPIA_nvrAddressLengthOverride 1
/*! \brief Preprocessor variable to relate field to bit position in structure nvrAddressLengthOverride in AQ_GlobalNvrProvisioning_APPIA */
#define bits_AQ_GlobalNvrProvisioning_APPIA_nvrAddressLengthOverride u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure nvrAddressLengthOverride in AQ_GlobalNvrProvisioning_APPIA */
#define word_AQ_GlobalNvrProvisioning_APPIA_nvrAddressLengthOverride u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrClockDivide in AQ_GlobalNvrProvisioning_APPIA */
#define AQ_GlobalNvrProvisioning_APPIA_nvrClockDivide 1
/*! \brief Preprocessor variable to relate field to bit position in structure nvrClockDivide in AQ_GlobalNvrProvisioning_APPIA */
#define bits_AQ_GlobalNvrProvisioning_APPIA_nvrClockDivide u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure nvrClockDivide in AQ_GlobalNvrProvisioning_APPIA */
#define word_AQ_GlobalNvrProvisioning_APPIA_nvrClockDivide u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrDaisyChainClockDivideOverride in AQ_GlobalNvrProvisioning_APPIA */
#define AQ_GlobalNvrProvisioning_APPIA_nvrDaisyChainClockDivideOverride 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDaisyChainClockDivideOverride in AQ_GlobalNvrProvisioning_APPIA */
#define bits_AQ_GlobalNvrProvisioning_APPIA_nvrDaisyChainClockDivideOverride u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrDaisyChainClockDivideOverride in AQ_GlobalNvrProvisioning_APPIA */
#define word_AQ_GlobalNvrProvisioning_APPIA_nvrDaisyChainClockDivideOverride u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure nvrDaisyChainDisable in AQ_GlobalNvrProvisioning_APPIA */
#define AQ_GlobalNvrProvisioning_APPIA_nvrDaisyChainDisable 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrDaisyChainDisable in AQ_GlobalNvrProvisioning_APPIA */
#define bits_AQ_GlobalNvrProvisioning_APPIA_nvrDaisyChainDisable u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrDaisyChainDisable in AQ_GlobalNvrProvisioning_APPIA */
#define word_AQ_GlobalNvrProvisioning_APPIA_nvrDaisyChainDisable u2.word_2

/*! \brief Base register address of structure AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_baseRegisterAddress 0xC470
/*! \brief MMD address of structure AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure diagnosticsSelect in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_diagnosticsSelect 0
/*! \brief Preprocessor variable to relate field to bit position in structure diagnosticsSelect in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_diagnosticsSelect u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure diagnosticsSelect in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_diagnosticsSelect u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure extendedMdiDiagnosticsSelect in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_extendedMdiDiagnosticsSelect 0
/*! \brief Preprocessor variable to relate field to bit position in structure extendedMdiDiagnosticsSelect in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_extendedMdiDiagnosticsSelect u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure extendedMdiDiagnosticsSelect in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_extendedMdiDiagnosticsSelect u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure initiateComponentDiagnostics in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_initiateComponentDiagnostics 0
/*! \brief Preprocessor variable to relate field to bit position in structure initiateComponentDiagnostics in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_initiateComponentDiagnostics u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure initiateComponentDiagnostics in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_initiateComponentDiagnostics u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure initiateCableDiagnostics in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_initiateCableDiagnostics 0
/*! \brief Preprocessor variable to relate field to bit position in structure initiateCableDiagnostics in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_initiateCableDiagnostics u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure initiateCableDiagnostics in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_initiateCableDiagnostics u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioning_2 in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_reservedProvisioning_2 1
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioning_2 in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_reservedProvisioning_2 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioning_2 in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_reservedProvisioning_2 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure enableDaisy_chainHop_countOverride in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_enableDaisy_chainHop_countOverride 1
/*! \brief Preprocessor variable to relate field to bit position in structure enableDaisy_chainHop_countOverride in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_enableDaisy_chainHop_countOverride u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure enableDaisy_chainHop_countOverride in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_enableDaisy_chainHop_countOverride u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure daisy_chainHop_countOverrideValue in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_daisy_chainHop_countOverrideValue 1
/*! \brief Preprocessor variable to relate field to bit position in structure daisy_chainHop_countOverrideValue in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_daisy_chainHop_countOverrideValue u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure daisy_chainHop_countOverrideValue in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_daisy_chainHop_countOverrideValue u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure enableLvddPowerSupplyTuning in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_enableLvddPowerSupplyTuning 2
/*! \brief Preprocessor variable to relate field to bit position in structure enableLvddPowerSupplyTuning in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_enableLvddPowerSupplyTuning u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure enableLvddPowerSupplyTuning in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_enableLvddPowerSupplyTuning u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure enableVddPowerSupplyTuning in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_enableVddPowerSupplyTuning 2
/*! \brief Preprocessor variable to relate field to bit position in structure enableVddPowerSupplyTuning in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_enableVddPowerSupplyTuning u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure enableVddPowerSupplyTuning in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_enableVddPowerSupplyTuning u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure tunableExternalLvddPowerSupplyPresent in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_tunableExternalLvddPowerSupplyPresent 2
/*! \brief Preprocessor variable to relate field to bit position in structure tunableExternalLvddPowerSupplyPresent in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_tunableExternalLvddPowerSupplyPresent u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure tunableExternalLvddPowerSupplyPresent in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_tunableExternalLvddPowerSupplyPresent u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure tunableExternalVddPowerSupplyPresent in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_tunableExternalVddPowerSupplyPresent 2
/*! \brief Preprocessor variable to relate field to bit position in structure tunableExternalVddPowerSupplyPresent in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_tunableExternalVddPowerSupplyPresent u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure tunableExternalVddPowerSupplyPresent in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_tunableExternalVddPowerSupplyPresent u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure enableXenpakRegisterSpace in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_enableXenpakRegisterSpace 2
/*! \brief Preprocessor variable to relate field to bit position in structure enableXenpakRegisterSpace in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_enableXenpakRegisterSpace u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure enableXenpakRegisterSpace in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_enableXenpakRegisterSpace u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure enable_5thChannelRfiCancellation in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_enable_5thChannelRfiCancellation 2
/*! \brief Preprocessor variable to relate field to bit position in structure enable_5thChannelRfiCancellation in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_enable_5thChannelRfiCancellation u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure enable_5thChannelRfiCancellation in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_enable_5thChannelRfiCancellation u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure trainingSNR in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_trainingSNR 4
/*! \brief Preprocessor variable to relate field to bit position in structure trainingSNR in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_trainingSNR u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure trainingSNR in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_trainingSNR u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure smartPower_downStatus in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_smartPower_downStatus 5
/*! \brief Preprocessor variable to relate field to bit position in structure smartPower_downStatus in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_smartPower_downStatus u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure smartPower_downStatus in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_smartPower_downStatus u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure reservedProvisioning_6 in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_reservedProvisioning_6 5
/*! \brief Preprocessor variable to relate field to bit position in structure reservedProvisioning_6 in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_reservedProvisioning_6 u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure reservedProvisioning_6 in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_reservedProvisioning_6 u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrLpDisableTimer in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_cfrLpDisableTimer 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrLpDisableTimer in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_cfrLpDisableTimer u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrLpDisableTimer in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_cfrLpDisableTimer u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrLpExtendedMaxwait in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_cfrLpExtendedMaxwait 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrLpExtendedMaxwait in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_cfrLpExtendedMaxwait u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrLpExtendedMaxwait in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_cfrLpExtendedMaxwait u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrLpTHP in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_cfrLpTHP 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrLpTHP in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_cfrLpTHP u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrLpTHP in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_cfrLpTHP u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrLpSupport in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_cfrLpSupport 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrLpSupport in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_cfrLpSupport u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrLpSupport in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_cfrLpSupport u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrDisableTimer in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_cfrDisableTimer 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrDisableTimer in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_cfrDisableTimer u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrDisableTimer in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_cfrDisableTimer u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrExtendedMaxwait in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_cfrExtendedMaxwait 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrExtendedMaxwait in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_cfrExtendedMaxwait u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrExtendedMaxwait in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_cfrExtendedMaxwait u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrTHP in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_cfrTHP 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrTHP in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_cfrTHP u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrTHP in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_cfrTHP u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure cfrSupport in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_cfrSupport 5
/*! \brief Preprocessor variable to relate field to bit position in structure cfrSupport in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_cfrSupport u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure cfrSupport in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_cfrSupport u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure deadlockAvoidanceEnable in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_deadlockAvoidanceEnable 5
/*! \brief Preprocessor variable to relate field to bit position in structure deadlockAvoidanceEnable in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_deadlockAvoidanceEnable u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure deadlockAvoidanceEnable in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_deadlockAvoidanceEnable u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure smartPower_downEnable in AQ_GlobalReservedProvisioning_APPIA */
#define AQ_GlobalReservedProvisioning_APPIA_smartPower_downEnable 5
/*! \brief Preprocessor variable to relate field to bit position in structure smartPower_downEnable in AQ_GlobalReservedProvisioning_APPIA */
#define bits_AQ_GlobalReservedProvisioning_APPIA_smartPower_downEnable u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure smartPower_downEnable in AQ_GlobalReservedProvisioning_APPIA */
#define word_AQ_GlobalReservedProvisioning_APPIA_smartPower_downEnable u5.word_5

/*! \brief Base register address of structure AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_baseRegisterAddress 0xC800
/*! \brief MMD address of structure AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pairAStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairAStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairAStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairAStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairBStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairBStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairBStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairBStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairBStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairBStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairCStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairCStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairCStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairCStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairCStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairCStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairDStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairDStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairDStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairDStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairDStatus in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairDStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairAReflection_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairAReflection_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairAReflection_1 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairAReflection_2 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairAReflection_2 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairAReflection_2 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure impulseResponseMSW in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_impulseResponseMSW 2
/*! \brief Preprocessor variable to relate field to bit position in structure impulseResponseMSW in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_impulseResponseMSW u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure impulseResponseMSW in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_impulseResponseMSW u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairBReflection_1 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairBReflection_1 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairBReflection_1 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairBReflection_2 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairBReflection_2 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairBReflection_2 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure impulseResponseLSW in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_impulseResponseLSW 4
/*! \brief Preprocessor variable to relate field to bit position in structure impulseResponseLSW in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_impulseResponseLSW u4.bits_4
/*! \brief Preprocessor variable to relate field to word position in structure impulseResponseLSW in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_impulseResponseLSW u4.word_4
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairCReflection_1 5
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairCReflection_1 u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairCReflection_1 u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairCReflection_2 5
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairCReflection_2 u5.bits_5
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairCReflection_2 u5.word_5
/*! \brief Preprocessor variable to relate field to word number in structure reserved_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_reserved_1 6
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_reserved_1 u6.bits_6
/*! \brief Preprocessor variable to relate field to word position in structure reserved_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_reserved_1 u6.word_6
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairDReflection_1 7
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairDReflection_1 u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_1 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairDReflection_1 u7.word_7
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_pairDReflection_2 7
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_pairDReflection_2 u7.bits_7
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_pairDReflection_2 u7.word_7
/*! \brief Preprocessor variable to relate field to word number in structure reserved_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define AQ_GlobalCableDiagnosticStatus_APPIA_reserved_2 8
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define bits_AQ_GlobalCableDiagnosticStatus_APPIA_reserved_2 u8.bits_8
/*! \brief Preprocessor variable to relate field to word position in structure reserved_2 in AQ_GlobalCableDiagnosticStatus_APPIA */
#define word_AQ_GlobalCableDiagnosticStatus_APPIA_reserved_2 u8.word_8

/*! \brief Base register address of structure AQ_GlobalThermalStatus_APPIA */
#define AQ_GlobalThermalStatus_APPIA_baseRegisterAddress 0xC820
/*! \brief MMD address of structure AQ_GlobalThermalStatus_APPIA */
#define AQ_GlobalThermalStatus_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure temperature in AQ_GlobalThermalStatus_APPIA */
#define AQ_GlobalThermalStatus_APPIA_temperature 0
/*! \brief Preprocessor variable to relate field to bit position in structure temperature in AQ_GlobalThermalStatus_APPIA */
#define bits_AQ_GlobalThermalStatus_APPIA_temperature u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure temperature in AQ_GlobalThermalStatus_APPIA */
#define word_AQ_GlobalThermalStatus_APPIA_temperature u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure temperatureReady in AQ_GlobalThermalStatus_APPIA */
#define AQ_GlobalThermalStatus_APPIA_temperatureReady 1
/*! \brief Preprocessor variable to relate field to bit position in structure temperatureReady in AQ_GlobalThermalStatus_APPIA */
#define bits_AQ_GlobalThermalStatus_APPIA_temperatureReady u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure temperatureReady in AQ_GlobalThermalStatus_APPIA */
#define word_AQ_GlobalThermalStatus_APPIA_temperatureReady u1.word_1

/*! \brief Base register address of structure AQ_GlobalGeneralStatus_APPIA */
#define AQ_GlobalGeneralStatus_APPIA_baseRegisterAddress 0xC830
/*! \brief MMD address of structure AQ_GlobalGeneralStatus_APPIA */
#define AQ_GlobalGeneralStatus_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureFailureState in AQ_GlobalGeneralStatus_APPIA */
#define AQ_GlobalGeneralStatus_APPIA_highTemperatureFailureState 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureFailureState in AQ_GlobalGeneralStatus_APPIA */
#define bits_AQ_GlobalGeneralStatus_APPIA_highTemperatureFailureState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureFailureState in AQ_GlobalGeneralStatus_APPIA */
#define word_AQ_GlobalGeneralStatus_APPIA_highTemperatureFailureState u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureFailureState in AQ_GlobalGeneralStatus_APPIA */
#define AQ_GlobalGeneralStatus_APPIA_lowTemperatureFailureState 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureFailureState in AQ_GlobalGeneralStatus_APPIA */
#define bits_AQ_GlobalGeneralStatus_APPIA_lowTemperatureFailureState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureFailureState in AQ_GlobalGeneralStatus_APPIA */
#define word_AQ_GlobalGeneralStatus_APPIA_lowTemperatureFailureState u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureWarningState in AQ_GlobalGeneralStatus_APPIA */
#define AQ_GlobalGeneralStatus_APPIA_highTemperatureWarningState 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureWarningState in AQ_GlobalGeneralStatus_APPIA */
#define bits_AQ_GlobalGeneralStatus_APPIA_highTemperatureWarningState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureWarningState in AQ_GlobalGeneralStatus_APPIA */
#define word_AQ_GlobalGeneralStatus_APPIA_highTemperatureWarningState u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureWarningState in AQ_GlobalGeneralStatus_APPIA */
#define AQ_GlobalGeneralStatus_APPIA_lowTemperatureWarningState 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureWarningState in AQ_GlobalGeneralStatus_APPIA */
#define bits_AQ_GlobalGeneralStatus_APPIA_lowTemperatureWarningState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureWarningState in AQ_GlobalGeneralStatus_APPIA */
#define word_AQ_GlobalGeneralStatus_APPIA_lowTemperatureWarningState u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure processorIntensiveMdioOperationIn_Progress in AQ_GlobalGeneralStatus_APPIA */
#define AQ_GlobalGeneralStatus_APPIA_processorIntensiveMdioOperationIn_Progress 1
/*! \brief Preprocessor variable to relate field to bit position in structure processorIntensiveMdioOperationIn_Progress in AQ_GlobalGeneralStatus_APPIA */
#define bits_AQ_GlobalGeneralStatus_APPIA_processorIntensiveMdioOperationIn_Progress u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure processorIntensiveMdioOperationIn_Progress in AQ_GlobalGeneralStatus_APPIA */
#define word_AQ_GlobalGeneralStatus_APPIA_processorIntensiveMdioOperationIn_Progress u1.word_1

/*! \brief Base register address of structure AQ_GlobalPinStatus_APPIA */
#define AQ_GlobalPinStatus_APPIA_baseRegisterAddress 0xC840
/*! \brief MMD address of structure AQ_GlobalPinStatus_APPIA */
#define AQ_GlobalPinStatus_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure mdioBootLoad in AQ_GlobalPinStatus_APPIA */
#define AQ_GlobalPinStatus_APPIA_mdioBootLoad 0
/*! \brief Preprocessor variable to relate field to bit position in structure mdioBootLoad in AQ_GlobalPinStatus_APPIA */
#define bits_AQ_GlobalPinStatus_APPIA_mdioBootLoad u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure mdioBootLoad in AQ_GlobalPinStatus_APPIA */
#define word_AQ_GlobalPinStatus_APPIA_mdioBootLoad u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure packageConnectivity in AQ_GlobalPinStatus_APPIA */
#define AQ_GlobalPinStatus_APPIA_packageConnectivity 0
/*! \brief Preprocessor variable to relate field to bit position in structure packageConnectivity in AQ_GlobalPinStatus_APPIA */
#define bits_AQ_GlobalPinStatus_APPIA_packageConnectivity u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure packageConnectivity in AQ_GlobalPinStatus_APPIA */
#define word_AQ_GlobalPinStatus_APPIA_packageConnectivity u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure txEnable in AQ_GlobalPinStatus_APPIA */
#define AQ_GlobalPinStatus_APPIA_txEnable 0
/*! \brief Preprocessor variable to relate field to bit position in structure txEnable in AQ_GlobalPinStatus_APPIA */
#define bits_AQ_GlobalPinStatus_APPIA_txEnable u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure txEnable in AQ_GlobalPinStatus_APPIA */
#define word_AQ_GlobalPinStatus_APPIA_txEnable u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure ledPullupState in AQ_GlobalPinStatus_APPIA */
#define AQ_GlobalPinStatus_APPIA_ledPullupState 0
/*! \brief Preprocessor variable to relate field to bit position in structure ledPullupState in AQ_GlobalPinStatus_APPIA */
#define bits_AQ_GlobalPinStatus_APPIA_ledPullupState u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure ledPullupState in AQ_GlobalPinStatus_APPIA */
#define word_AQ_GlobalPinStatus_APPIA_ledPullupState u0.word_0

/*! \brief Base register address of structure AQ_GlobalDaisyChainStatus_APPIA */
#define AQ_GlobalDaisyChainStatus_APPIA_baseRegisterAddress 0xC842
/*! \brief MMD address of structure AQ_GlobalDaisyChainStatus_APPIA */
#define AQ_GlobalDaisyChainStatus_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure rxDaisyChainCalculatedCrc in AQ_GlobalDaisyChainStatus_APPIA */
#define AQ_GlobalDaisyChainStatus_APPIA_rxDaisyChainCalculatedCrc 0
/*! \brief Preprocessor variable to relate field to bit position in structure rxDaisyChainCalculatedCrc in AQ_GlobalDaisyChainStatus_APPIA */
#define bits_AQ_GlobalDaisyChainStatus_APPIA_rxDaisyChainCalculatedCrc u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure rxDaisyChainCalculatedCrc in AQ_GlobalDaisyChainStatus_APPIA */
#define word_AQ_GlobalDaisyChainStatus_APPIA_rxDaisyChainCalculatedCrc u0.word_0

/*! \brief Base register address of structure AQ_GlobalFaultMessage_APPIA */
#define AQ_GlobalFaultMessage_APPIA_baseRegisterAddress 0xC850
/*! \brief MMD address of structure AQ_GlobalFaultMessage_APPIA */
#define AQ_GlobalFaultMessage_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure message in AQ_GlobalFaultMessage_APPIA */
#define AQ_GlobalFaultMessage_APPIA_message 0
/*! \brief Preprocessor variable to relate field to bit position in structure message in AQ_GlobalFaultMessage_APPIA */
#define bits_AQ_GlobalFaultMessage_APPIA_message u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure message in AQ_GlobalFaultMessage_APPIA */
#define word_AQ_GlobalFaultMessage_APPIA_message u0.word_0

/*! \brief Base register address of structure AQ_GlobalPrimaryStatus_APPIA */
#define AQ_GlobalPrimaryStatus_APPIA_baseRegisterAddress 0xC851
/*! \brief MMD address of structure AQ_GlobalPrimaryStatus_APPIA */
#define AQ_GlobalPrimaryStatus_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure primaryStatus in AQ_GlobalPrimaryStatus_APPIA */
#define AQ_GlobalPrimaryStatus_APPIA_primaryStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure primaryStatus in AQ_GlobalPrimaryStatus_APPIA */
#define bits_AQ_GlobalPrimaryStatus_APPIA_primaryStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure primaryStatus in AQ_GlobalPrimaryStatus_APPIA */
#define word_AQ_GlobalPrimaryStatus_APPIA_primaryStatus u0.word_0

/*! \brief Base register address of structure AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_baseRegisterAddress 0xC880
/*! \brief MMD address of structure AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure reserved_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_1 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_1 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_1 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_1 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_1 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_1 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reserved_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_2 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_2 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_2 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_2 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_2 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_2 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reserved_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_3 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_3 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_3 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_3 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_3 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_3 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reserved_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_4 0
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_4 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reserved_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_4 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pairAReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_4 0
/*! \brief Preprocessor variable to relate field to bit position in structure pairAReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_4 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pairAReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairAReflection_4 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reserved_5 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_5 1
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_5 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_5 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reserved_5 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_5 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_1 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_1 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_1 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reserved_6 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_6 1
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_6 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_6 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reserved_6 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_6 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_2 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_2 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_2 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reserved_7 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_7 1
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_7 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_7 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reserved_7 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_7 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_3 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_3 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_3 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reserved_8 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_8 1
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_8 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_8 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reserved_8 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_8 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure pairBReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_4 1
/*! \brief Preprocessor variable to relate field to bit position in structure pairBReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_4 u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure pairBReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairBReflection_4 u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reserved_9 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_9 2
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_9 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_9 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reserved_9 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_9 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_1 2
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_1 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_1 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reserved_10 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_10 2
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_10 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_10 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reserved_10 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_10 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_2 2
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_2 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_2 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reserved_11 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_11 2
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_11 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_11 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reserved_11 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_11 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_3 2
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_3 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_3 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reserved_12 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_12 2
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_12 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_12 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure reserved_12 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_12 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure pairCReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_4 2
/*! \brief Preprocessor variable to relate field to bit position in structure pairCReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_4 u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure pairCReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairCReflection_4 u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure reserved_13 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_13 3
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_13 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_13 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reserved_13 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_13 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_1 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_1 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_1 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_1 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure reserved_14 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_14 3
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_14 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_14 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reserved_14 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_14 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_2 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_2 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_2 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_2 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure reserved_15 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_15 3
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_15 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_15 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reserved_15 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_15 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_3 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_3 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_3 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_3 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure reserved_16 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_16 3
/*! \brief Preprocessor variable to relate field to bit position in structure reserved_16 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_16 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure reserved_16 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_reserved_16 u3.word_3
/*! \brief Preprocessor variable to relate field to word number in structure pairDReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_4 3
/*! \brief Preprocessor variable to relate field to bit position in structure pairDReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define bits_AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_4 u3.bits_3
/*! \brief Preprocessor variable to relate field to word position in structure pairDReflection_4 in AQ_GlobalCableDiagnosticImpedance_APPIA */
#define word_AQ_GlobalCableDiagnosticImpedance_APPIA_pairDReflection_4 u3.word_3

/*! \brief Base register address of structure AQ_GlobalStatus_APPIA */
#define AQ_GlobalStatus_APPIA_baseRegisterAddress 0xC884
/*! \brief MMD address of structure AQ_GlobalStatus_APPIA */
#define AQ_GlobalStatus_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure reservedStatus_0 in AQ_GlobalStatus_APPIA */
#define AQ_GlobalStatus_APPIA_reservedStatus_0 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedStatus_0 in AQ_GlobalStatus_APPIA */
#define bits_AQ_GlobalStatus_APPIA_reservedStatus_0 u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedStatus_0 in AQ_GlobalStatus_APPIA */
#define word_AQ_GlobalStatus_APPIA_reservedStatus_0 u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure cableLength in AQ_GlobalStatus_APPIA */
#define AQ_GlobalStatus_APPIA_cableLength 0
/*! \brief Preprocessor variable to relate field to bit position in structure cableLength in AQ_GlobalStatus_APPIA */
#define bits_AQ_GlobalStatus_APPIA_cableLength u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure cableLength in AQ_GlobalStatus_APPIA */
#define word_AQ_GlobalStatus_APPIA_cableLength u0.word_0

/*! \brief Base register address of structure AQ_GlobalReservedStatus_APPIA */
#define AQ_GlobalReservedStatus_APPIA_baseRegisterAddress 0xC885
/*! \brief MMD address of structure AQ_GlobalReservedStatus_APPIA */
#define AQ_GlobalReservedStatus_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure nearlySecondsMSW in AQ_GlobalReservedStatus_APPIA */
#define AQ_GlobalReservedStatus_APPIA_nearlySecondsMSW 0
/*! \brief Preprocessor variable to relate field to bit position in structure nearlySecondsMSW in AQ_GlobalReservedStatus_APPIA */
#define bits_AQ_GlobalReservedStatus_APPIA_nearlySecondsMSW u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure nearlySecondsMSW in AQ_GlobalReservedStatus_APPIA */
#define word_AQ_GlobalReservedStatus_APPIA_nearlySecondsMSW u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure xenpakNvrStatus in AQ_GlobalReservedStatus_APPIA */
#define AQ_GlobalReservedStatus_APPIA_xenpakNvrStatus 0
/*! \brief Preprocessor variable to relate field to bit position in structure xenpakNvrStatus in AQ_GlobalReservedStatus_APPIA */
#define bits_AQ_GlobalReservedStatus_APPIA_xenpakNvrStatus u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure xenpakNvrStatus in AQ_GlobalReservedStatus_APPIA */
#define word_AQ_GlobalReservedStatus_APPIA_xenpakNvrStatus u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure romRevision in AQ_GlobalReservedStatus_APPIA */
#define AQ_GlobalReservedStatus_APPIA_romRevision 0
/*! \brief Preprocessor variable to relate field to bit position in structure romRevision in AQ_GlobalReservedStatus_APPIA */
#define bits_AQ_GlobalReservedStatus_APPIA_romRevision u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure romRevision in AQ_GlobalReservedStatus_APPIA */
#define word_AQ_GlobalReservedStatus_APPIA_romRevision u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure nearlySecondsLSW in AQ_GlobalReservedStatus_APPIA */
#define AQ_GlobalReservedStatus_APPIA_nearlySecondsLSW 1
/*! \brief Preprocessor variable to relate field to bit position in structure nearlySecondsLSW in AQ_GlobalReservedStatus_APPIA */
#define bits_AQ_GlobalReservedStatus_APPIA_nearlySecondsLSW u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure nearlySecondsLSW in AQ_GlobalReservedStatus_APPIA */
#define word_AQ_GlobalReservedStatus_APPIA_nearlySecondsLSW u1.word_1

/*! \brief Base register address of structure AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_baseRegisterAddress 0xCC00
/*! \brief MMD address of structure AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureFailure in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_highTemperatureFailure 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureFailure in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_highTemperatureFailure u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureFailure in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_highTemperatureFailure u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureFailure in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_lowTemperatureFailure 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureFailure in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_lowTemperatureFailure u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureFailure in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_lowTemperatureFailure u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureWarning in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_highTemperatureWarning 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureWarning in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_highTemperatureWarning u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureWarning in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_highTemperatureWarning u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureWarning in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_lowTemperatureWarning 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureWarning in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_lowTemperatureWarning u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureWarning in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_lowTemperatureWarning u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure resetCompleted in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_resetCompleted 0
/*! \brief Preprocessor variable to relate field to bit position in structure resetCompleted in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_resetCompleted u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure resetCompleted in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_resetCompleted u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure deviceFault in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_deviceFault 0
/*! \brief Preprocessor variable to relate field to bit position in structure deviceFault in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_deviceFault u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure deviceFault in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_deviceFault u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmA in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_reservedAlarmA 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmA in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_reservedAlarmA u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmA in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_reservedAlarmA u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmB in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_reservedAlarmB 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmB in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_reservedAlarmB u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmB in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_reservedAlarmB u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmC in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_reservedAlarmC 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmC in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_reservedAlarmC u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmC in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_reservedAlarmC u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmD in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_reservedAlarmD 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmD in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_reservedAlarmD u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmD in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_reservedAlarmD u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure smartPower_downEntered in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_smartPower_downEntered 1
/*! \brief Preprocessor variable to relate field to bit position in structure smartPower_downEntered in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_smartPower_downEntered u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure smartPower_downEntered in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_smartPower_downEntered u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure xenpakAlarm in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_xenpakAlarm 1
/*! \brief Preprocessor variable to relate field to bit position in structure xenpakAlarm in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_xenpakAlarm u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure xenpakAlarm in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_xenpakAlarm u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarms in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_reservedAlarms 1
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarms in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_reservedAlarms u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarms in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_reservedAlarms u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioCommandHandlingOverflow in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_mdioCommandHandlingOverflow 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioCommandHandlingOverflow in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_mdioCommandHandlingOverflow u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioCommandHandlingOverflow in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_mdioCommandHandlingOverflow u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure diagnosticAlarm in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_diagnosticAlarm 1
/*! \brief Preprocessor variable to relate field to bit position in structure diagnosticAlarm in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_diagnosticAlarm u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure diagnosticAlarm in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_diagnosticAlarm u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrOperationComplete in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_nvrOperationComplete 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrOperationComplete in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_nvrOperationComplete u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrOperationComplete in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_nvrOperationComplete u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mailboxOperation_Complete in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_mailboxOperation_Complete 2
/*! \brief Preprocessor variable to relate field to bit position in structure mailboxOperation_Complete in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_mailboxOperation_Complete u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mailboxOperation_Complete in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_mailboxOperation_Complete u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upDramParityError in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_upDramParityError 2
/*! \brief Preprocessor variable to relate field to bit position in structure upDramParityError in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_upDramParityError u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upDramParityError in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_upDramParityError u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upIramParityError in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_upIramParityError 2
/*! \brief Preprocessor variable to relate field to bit position in structure upIramParityError in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_upIramParityError u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upIramParityError in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_upIramParityError u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure txEnableStateChange in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_txEnableStateChange 2
/*! \brief Preprocessor variable to relate field to bit position in structure txEnableStateChange in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_txEnableStateChange u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure txEnableStateChange in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_txEnableStateChange u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mdioMMD_Error in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_mdioMMD_Error 2
/*! \brief Preprocessor variable to relate field to bit position in structure mdioMMD_Error in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_mdioMMD_Error u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mdioMMD_Error in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_mdioMMD_Error u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mdioTimeoutError in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_mdioTimeoutError 2
/*! \brief Preprocessor variable to relate field to bit position in structure mdioTimeoutError in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_mdioTimeoutError u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mdioTimeoutError in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_mdioTimeoutError u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure watchdogTimerAlarm in AQ_GlobalAlarms_APPIA */
#define AQ_GlobalAlarms_APPIA_watchdogTimerAlarm 2
/*! \brief Preprocessor variable to relate field to bit position in structure watchdogTimerAlarm in AQ_GlobalAlarms_APPIA */
#define bits_AQ_GlobalAlarms_APPIA_watchdogTimerAlarm u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure watchdogTimerAlarm in AQ_GlobalAlarms_APPIA */
#define word_AQ_GlobalAlarms_APPIA_watchdogTimerAlarm u2.word_2

/*! \brief Base register address of structure AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_baseRegisterAddress 0xD400
/*! \brief MMD address of structure AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureFailureMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_highTemperatureFailureMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureFailureMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_highTemperatureFailureMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureFailureMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_highTemperatureFailureMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureFailureMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_lowTemperatureFailureMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureFailureMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_lowTemperatureFailureMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureFailureMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_lowTemperatureFailureMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure highTemperatureWarningMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_highTemperatureWarningMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure highTemperatureWarningMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_highTemperatureWarningMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure highTemperatureWarningMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_highTemperatureWarningMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure lowTemperatureWarningMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_lowTemperatureWarningMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure lowTemperatureWarningMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_lowTemperatureWarningMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure lowTemperatureWarningMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_lowTemperatureWarningMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure resetCompletedMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_resetCompletedMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure resetCompletedMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_resetCompletedMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure resetCompletedMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_resetCompletedMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure deviceFaultMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_deviceFaultMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure deviceFaultMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_deviceFaultMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure deviceFaultMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_deviceFaultMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmAMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_reservedAlarmAMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmAMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_reservedAlarmAMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmAMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_reservedAlarmAMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmBMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_reservedAlarmBMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmBMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_reservedAlarmBMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmBMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_reservedAlarmBMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmCMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_reservedAlarmCMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmCMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_reservedAlarmCMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmCMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_reservedAlarmCMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmDMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_reservedAlarmDMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmDMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_reservedAlarmDMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmDMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_reservedAlarmDMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure smartPower_downEnteredMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_smartPower_downEnteredMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure smartPower_downEnteredMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_smartPower_downEnteredMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure smartPower_downEnteredMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_smartPower_downEnteredMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure xenpakAlarmMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_xenpakAlarmMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure xenpakAlarmMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_xenpakAlarmMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure xenpakAlarmMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_xenpakAlarmMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure reservedAlarmsMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_reservedAlarmsMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure reservedAlarmsMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_reservedAlarmsMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure reservedAlarmsMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_reservedAlarmsMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure mdioCommandHandlingOverflowMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_mdioCommandHandlingOverflowMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure mdioCommandHandlingOverflowMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_mdioCommandHandlingOverflowMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure mdioCommandHandlingOverflowMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_mdioCommandHandlingOverflowMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure diagnosticAlarmMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_diagnosticAlarmMask 1
/*! \brief Preprocessor variable to relate field to bit position in structure diagnosticAlarmMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_diagnosticAlarmMask u1.bits_1
/*! \brief Preprocessor variable to relate field to word position in structure diagnosticAlarmMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_diagnosticAlarmMask u1.word_1
/*! \brief Preprocessor variable to relate field to word number in structure nvrOperationCompleteMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_nvrOperationCompleteMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure nvrOperationCompleteMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_nvrOperationCompleteMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure nvrOperationCompleteMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_nvrOperationCompleteMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mailboxOperationCompleteMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_mailboxOperationCompleteMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure mailboxOperationCompleteMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_mailboxOperationCompleteMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mailboxOperationCompleteMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_mailboxOperationCompleteMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upDramParityErrorMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_upDramParityErrorMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure upDramParityErrorMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_upDramParityErrorMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upDramParityErrorMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_upDramParityErrorMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure upIramParityErrorMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_upIramParityErrorMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure upIramParityErrorMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_upIramParityErrorMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure upIramParityErrorMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_upIramParityErrorMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure txEnableStateChangeMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_txEnableStateChangeMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure txEnableStateChangeMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_txEnableStateChangeMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure txEnableStateChangeMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_txEnableStateChangeMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mdioMMD_ErrorMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_mdioMMD_ErrorMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure mdioMMD_ErrorMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_mdioMMD_ErrorMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mdioMMD_ErrorMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_mdioMMD_ErrorMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure mdioTimeoutErrorMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_mdioTimeoutErrorMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure mdioTimeoutErrorMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_mdioTimeoutErrorMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure mdioTimeoutErrorMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_mdioTimeoutErrorMask u2.word_2
/*! \brief Preprocessor variable to relate field to word number in structure watchdogTimerAlarmMask in AQ_GlobalInterruptMask_APPIA */
#define AQ_GlobalInterruptMask_APPIA_watchdogTimerAlarmMask 2
/*! \brief Preprocessor variable to relate field to bit position in structure watchdogTimerAlarmMask in AQ_GlobalInterruptMask_APPIA */
#define bits_AQ_GlobalInterruptMask_APPIA_watchdogTimerAlarmMask u2.bits_2
/*! \brief Preprocessor variable to relate field to word position in structure watchdogTimerAlarmMask in AQ_GlobalInterruptMask_APPIA */
#define word_AQ_GlobalInterruptMask_APPIA_watchdogTimerAlarmMask u2.word_2

/*! \brief Base register address of structure AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_baseRegisterAddress 0xFC00
/*! \brief MMD address of structure AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pmaStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pmaStandardAlarm_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pmaStandardAlarm_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pmaStandardAlarm_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pmaStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pmaStandardAlarm_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pmaStandardAlarm_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pmaStandardAlarm_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pcsStandardAlarm_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pcsStandardAlarm_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pcsStandardAlarm_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pcsStandardAlarm_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pcsStandardAlarm_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pcsStandardAlarm_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_3Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pcsStandardAlarm_3Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_3Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pcsStandardAlarm_3Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_3Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_pcsStandardAlarm_3Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_StandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_phyXS_StandardAlarms_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_StandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_phyXS_StandardAlarms_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_StandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_phyXS_StandardAlarms_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_StandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_phyXS_StandardAlarms_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_StandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_phyXS_StandardAlarms_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_StandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_phyXS_StandardAlarms_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationStandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_autonegotiationStandardAlarms_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationStandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_autonegotiationStandardAlarms_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationStandardAlarms_1Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_autonegotiationStandardAlarms_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationStandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_autonegotiationStandardAlarms_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationStandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_autonegotiationStandardAlarms_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationStandardAlarms_2Interrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_autonegotiationStandardAlarms_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure gbeStandardAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_gbeStandardAlarmsInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure gbeStandardAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_gbeStandardAlarmsInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gbeStandardAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_gbeStandardAlarmsInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure allVendorAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define AQ_GlobalChip_wideStandardInterruptFlags_APPIA_allVendorAlarmsInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure allVendorAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_allVendorAlarmsInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure allVendorAlarmsInterrupt in AQ_GlobalChip_wideStandardInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideStandardInterruptFlags_APPIA_allVendorAlarmsInterrupt u0.word_0

/*! \brief Base register address of structure AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_baseRegisterAddress 0xFC01
/*! \brief MMD address of structure AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pmaVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_pmaVendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_pmaVendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_pmaVendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_pcsVendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_pcsVendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_pcsVendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_VendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_phyXS_VendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_VendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_phyXS_VendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_VendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_phyXS_VendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_autonegotiationVendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_autonegotiationVendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_autonegotiationVendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure gbeVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_gbeVendorAlarmInterrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure gbeVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_gbeVendorAlarmInterrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gbeVendorAlarmInterrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_gbeVendorAlarmInterrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_1Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_globalAlarms_1Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_1Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_globalAlarms_1Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_1Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_globalAlarms_1Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_2Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_globalAlarms_2Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_2Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_globalAlarms_2Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_2Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_globalAlarms_2Interrupt u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_3Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define AQ_GlobalChip_wideVendorInterruptFlags_APPIA_globalAlarms_3Interrupt 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_3Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define bits_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_globalAlarms_3Interrupt u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_3Interrupt in AQ_GlobalChip_wideVendorInterruptFlags_APPIA */
#define word_AQ_GlobalChip_wideVendorInterruptFlags_APPIA_globalAlarms_3Interrupt u0.word_0

/*! \brief Base register address of structure AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_baseRegisterAddress 0xFF00
/*! \brief MMD address of structure AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pmaStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_pmaStandardAlarm_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pmaStandardAlarm_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pmaStandardAlarm_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pmaStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_pmaStandardAlarm_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pmaStandardAlarm_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pmaStandardAlarm_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_pcsStandardAlarm_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pcsStandardAlarm_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pcsStandardAlarm_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_pcsStandardAlarm_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pcsStandardAlarm_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pcsStandardAlarm_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsStandardAlarm_3InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_pcsStandardAlarm_3InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsStandardAlarm_3InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pcsStandardAlarm_3InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsStandardAlarm_3InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_pcsStandardAlarm_3InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_StandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_phyXS_StandardAlarms_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_StandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_phyXS_StandardAlarms_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_StandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_phyXS_StandardAlarms_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_StandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_phyXS_StandardAlarms_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_StandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_phyXS_StandardAlarms_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_StandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_phyXS_StandardAlarms_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationStandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_autonegotiationStandardAlarms_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationStandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_autonegotiationStandardAlarms_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationStandardAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_autonegotiationStandardAlarms_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationStandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_autonegotiationStandardAlarms_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationStandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_autonegotiationStandardAlarms_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationStandardAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_autonegotiationStandardAlarms_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure gbeStandardAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_gbeStandardAlarmsInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure gbeStandardAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_gbeStandardAlarmsInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gbeStandardAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_gbeStandardAlarmsInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure allVendorAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define AQ_GlobalInterruptChip_wideStandardMask_APPIA_allVendorAlarmsInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure allVendorAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideStandardMask_APPIA_allVendorAlarmsInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure allVendorAlarmsInterruptMask in AQ_GlobalInterruptChip_wideStandardMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideStandardMask_APPIA_allVendorAlarmsInterruptMask u0.word_0

/*! \brief Base register address of structure AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_baseRegisterAddress 0xFF01
/*! \brief MMD address of structure AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_mmdAddress 0x1E
/*! \brief Preprocessor variable to relate field to word number in structure pmaVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_pmaVendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pmaVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_APPIA_pmaVendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pmaVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideVendorMask_APPIA_pmaVendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure pcsVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_pcsVendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure pcsVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_APPIA_pcsVendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure pcsVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideVendorMask_APPIA_pcsVendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure phyXS_VendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_phyXS_VendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure phyXS_VendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_APPIA_phyXS_VendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure phyXS_VendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideVendorMask_APPIA_phyXS_VendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure autonegotiationVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_autonegotiationVendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure autonegotiationVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_APPIA_autonegotiationVendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure autonegotiationVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideVendorMask_APPIA_autonegotiationVendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure gbeVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_gbeVendorAlarmInterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure gbeVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_APPIA_gbeVendorAlarmInterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure gbeVendorAlarmInterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideVendorMask_APPIA_gbeVendorAlarmInterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_globalAlarms_1InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_APPIA_globalAlarms_1InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_1InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideVendorMask_APPIA_globalAlarms_1InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_globalAlarms_2InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_APPIA_globalAlarms_2InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_2InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideVendorMask_APPIA_globalAlarms_2InterruptMask u0.word_0
/*! \brief Preprocessor variable to relate field to word number in structure globalAlarms_3InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define AQ_GlobalInterruptChip_wideVendorMask_APPIA_globalAlarms_3InterruptMask 0
/*! \brief Preprocessor variable to relate field to bit position in structure globalAlarms_3InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define bits_AQ_GlobalInterruptChip_wideVendorMask_APPIA_globalAlarms_3InterruptMask u0.bits_0
/*! \brief Preprocessor variable to relate field to word position in structure globalAlarms_3InterruptMask in AQ_GlobalInterruptChip_wideVendorMask_APPIA */
#define word_AQ_GlobalInterruptChip_wideVendorMask_APPIA_globalAlarms_3InterruptMask u0.word_0
#endif
/*@}*/
/*@}*/
