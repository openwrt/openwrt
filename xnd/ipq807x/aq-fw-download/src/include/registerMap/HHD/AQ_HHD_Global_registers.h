/*! \file 
*   This file contains the data structures and doxygen comments
*   for the Global Registers block.
 */        

/*! \addtogroup registerMap
  @{
*/

/*! \defgroup Global_registers Global Registers
*   This module contains the data structures and doxygen comments
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
* $Date: 2014/04/08 $
*
* $Label: $
*
* Description:
*
*   This file contains the c header structures for the registers contained in the Global Registers block.
*
*   The bit fields in this structure are from LSbit to MSbit
*
***********************************************************************/


/*@{*/
#ifndef AQ_HHD_GLOBAL_REGS_HEADER
#define AQ_HHD_GLOBAL_REGS_HEADER


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Standard Control 1: 1E.0000 */
/*                  Global Standard Control 1: 1E.0000 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Standard Control 1 */
  union
  {
    struct
    {
      unsigned int   reserved1 : 11;
                    /*! \brief 1E.0000.B R/WPD Low Power
                        AQ_GlobalStandardControl_1_HHD.u0.bits_0.lowPower

                        Provisionable Default = 0x0

                        1 = Low-power mode
                        0 = Normal operation
                        

                 <B>Notes:</B>
                        A one written to this register causes the chip to enter low-power mode. This bit puts the entire chip in low-power mode, with only the MDIO and microprocessor functioning, and turns off the analog front-end: i.e. places it in high-impedance mode. Setting this bit also sets all of the Low Power bits in the other MMDs.  */
      unsigned int   lowPower : 1;    /* 1E.0000.B  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Low-power mode
                        0 = Normal operation
                          */
      unsigned int   reserved0 : 3;
                    /*! \brief 1E.0000.F R/WSC Soft Reset
                        AQ_GlobalStandardControl_1_HHD.u0.bits_0.softReset

                        Default = 0x1

                        1 = Global soft reset
                        0 = Normal operation
                        

                 <B>Notes:</B>
                        Resets the entire PHY.
                        Setting this bit initiates a global soft reset on all of the digital logic not including the microprocessor (i.e. microprocessor is not reset). Upon completion of the reset sequence, this bit is set back to 0by the microprocessor. Note this bit is OR'ed with the individual MMD resets. This bit should be set to 0 before setting the individual MMD resets.  */
      unsigned int   softReset : 1;    /* 1E.0000.F  R/WSC      Default = 0x1 */
                     /* 1 = Global soft reset
                        0 = Normal operation
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalStandardControl_1_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Standard Device Identifier: 1E.0002 */
/*                  Global Standard Device Identifier: 1E.0002 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Standard Device Identifier */
  union
  {
    struct
    {
                    /*! \brief 1E.0002.F:0 RO Device ID MSW [1F:10]
                        AQ_GlobalStandardDeviceIdentifier_HHD.u0.bits_0.deviceIdMSW

                        

                        Bits 31 - 16 of Device ID
  */
      unsigned int   deviceIdMSW : 16;    /* 1E.0002.F:0  RO       */
                     /* Bits 31 - 16 of Device ID  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Standard Device Identifier */
  union
  {
    struct
    {
                    /*! \brief 1E.0003.F:0 RO Device ID LSW [F:0]
                        AQ_GlobalStandardDeviceIdentifier_HHD.u1.bits_1.deviceIdLSW

                        

                        Bits 15 - 0 of Device ID
  */
      unsigned int   deviceIdLSW : 16;    /* 1E.0003.F:0  RO       */
                     /* Bits 15 - 0 of Device ID  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_GlobalStandardDeviceIdentifier_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Standard Devices in Package: 1E.0005 */
/*                  Global Standard Devices in Package: 1E.0005 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Standard Devices in Package */
  union
  {
    struct
    {
                    /*! \brief 1E.0005.0 ROS Clause 22 Registers Present
                        AQ_GlobalStandardDevicesInPackage_HHD.u0.bits_0.clause_22RegistersPresent

                        Default = 0x0

                        1 = Clause 22 registers are present in package
                        0 = Clause 22 registers are not present in package

                 <B>Notes:</B>
                        This is always set to 0 in the PHY, as there are no Clause 22 registers in the device.  */
      unsigned int   clause_22RegistersPresent : 1;    /* 1E.0005.0  ROS      Default = 0x0 */
                     /* 1 = Clause 22 registers are present in package
                        0 = Clause 22 registers are not present in package  */
                    /*! \brief 1E.0005.1 ROS PMA Present
                        AQ_GlobalStandardDevicesInPackage_HHD.u0.bits_0.pmaPresent

                        Default = 0x1

                        1 = PMA is present in package
                        0 = PMA is not present 

                 <B>Notes:</B>
                        This is always set to 1 as there is PMA functionality in the PHY.  */
      unsigned int   pmaPresent : 1;    /* 1E.0005.1  ROS      Default = 0x1 */
                     /* 1 = PMA is present in package
                        0 = PMA is not present   */
                    /*! \brief 1E.0005.2 ROS WIS Present
                        AQ_GlobalStandardDevicesInPackage_HHD.u0.bits_0.wisPresent

                        Default = 0x0

                        1 = WIS is present in package
                        0 = WIS is not present in package

                 <B>Notes:</B>
                        This is always set to 0, as there is no WIS functionality in the PHY.  */
      unsigned int   wisPresent : 1;    /* 1E.0005.2  ROS      Default = 0x0 */
                     /* 1 = WIS is present in package
                        0 = WIS is not present in package  */
                    /*! \brief 1E.0005.3 ROS PCS Present
                        AQ_GlobalStandardDevicesInPackage_HHD.u0.bits_0.pcsPresent

                        Default = 0x1

                        1 = PCS is present in package
                        0 = PCS is not present in package

                 <B>Notes:</B>
                        This is always set to 1 as there is PCS functionality in the PHY.  */
      unsigned int   pcsPresent : 1;    /* 1E.0005.3  ROS      Default = 0x1 */
                     /* 1 = PCS is present in package
                        0 = PCS is not present in package  */
                    /*! \brief 1E.0005.4 ROS PHY XS Present
                        AQ_GlobalStandardDevicesInPackage_HHD.u0.bits_0.phyXS_Present

                        Default = 0x1

                        1 = PHY XS is present in package
                        0 = PHY XS is not present in package

                 <B>Notes:</B>
                        This is always set to 1 as there is a PHY XS interface in the PHY.  */
      unsigned int   phyXS_Present : 1;    /* 1E.0005.4  ROS      Default = 0x1 */
                     /* 1 = PHY XS is present in package
                        0 = PHY XS is not present in package  */
                    /*! \brief 1E.0005.5 ROS DTE XS Present
                        AQ_GlobalStandardDevicesInPackage_HHD.u0.bits_0.dteXsPresent

                        Default = 0x0

                        1 = DTE XS is present in package
                        0 = DTE XS is not present in package
                        

                 <B>Notes:</B>
                        This is always set to 0, as there is no DTE XAUI interface in the PHY.  */
      unsigned int   dteXsPresent : 1;    /* 1E.0005.5  ROS      Default = 0x0 */
                     /* 1 = DTE XS is present in package
                        0 = DTE XS is not present in package
                          */
                    /*! \brief 1E.0005.6 ROS TC Present
                        AQ_GlobalStandardDevicesInPackage_HHD.u0.bits_0.tcPresent

                        Default = 0x0

                        1 = TC is present in package
                        0 = TC is not present in package

                 <B>Notes:</B>
                        This is always set to 0, as there is no TC functionality in the PHY.  */
      unsigned int   tcPresent : 1;    /* 1E.0005.6  ROS      Default = 0x0 */
                     /* 1 = TC is present in package
                        0 = TC is not present in package  */
                    /*! \brief 1E.0005.7 ROS Autonegotiation Present
                        AQ_GlobalStandardDevicesInPackage_HHD.u0.bits_0.autonegotiationPresent

                        Default = 0x1

                        1 = Autonegotiation is present in package
                        0 = Autonegotiation is not present in package

                 <B>Notes:</B>
                        This is always set to 1, as there is Autonegotiation in the PHY.  */
      unsigned int   autonegotiationPresent : 1;    /* 1E.0005.7  ROS      Default = 0x1 */
                     /* 1 = Autonegotiation is present in package
                        0 = Autonegotiation is not present in package  */
      unsigned int   reserved0 : 8;
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalStandardDevicesInPackage_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Standard Vendor Devices in Package: 1E.0006 */
/*                  Global Standard Vendor Devices in Package: 1E.0006 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Standard Vendor Devices in Package */
  union
  {
    struct
    {
      unsigned int   reserved0 : 13;
                    /*! \brief 1E.0006.D ROS Clause 22 Extension Present
                        AQ_GlobalStandardVendorDevicesInPackage_HHD.u0.bits_0.clause_22ExtensionPresent

                        Default = 0x1

                        1 = Clause 22 Extension is present in package
                        0 = Clause 22 Extension is not present in package

                 <B>Notes:</B>
                        This is always set to 1 as the PHY utilizes this device for the GbE registers.  */
      unsigned int   clause_22ExtensionPresent : 1;    /* 1E.0006.D  ROS      Default = 0x1 */
                     /* 1 = Clause 22 Extension is present in package
                        0 = Clause 22 Extension is not present in package  */
                    /*! \brief 1E.0006.E ROS Vendor Specific Device #1 Present
                        AQ_GlobalStandardVendorDevicesInPackage_HHD.u0.bits_0.vendorSpecificDevice_1Present

                        Default = 0x1

                        1 = Device #1 is present in package
                        0 = Device #1 is not present in package

                 <B>Notes:</B>
                        This is always set to 1 as the PHY utilizes this device for the global control registers.  */
      unsigned int   vendorSpecificDevice_1Present : 1;    /* 1E.0006.E  ROS      Default = 0x1 */
                     /* 1 = Device #1 is present in package
                        0 = Device #1 is not present in package  */
                    /*! \brief 1E.0006.F ROS Vendor Specific Device #2 Present
                        AQ_GlobalStandardVendorDevicesInPackage_HHD.u0.bits_0.vendorSpecificDevice_2Present

                        Default = 0x1

                        1 = Device #2 is present in package
                        0 = Device #2 is not present in package

                 <B>Notes:</B>
                        This is always set to 1 as the PHY utilizes this device for the DSP PMA registers.  */
      unsigned int   vendorSpecificDevice_2Present : 1;    /* 1E.0006.F  ROS      Default = 0x1 */
                     /* 1 = Device #2 is present in package
                        0 = Device #2 is not present in package  */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalStandardVendorDevicesInPackage_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Standard Status 2: 1E.0008 */
/*                  Global Standard Status 2: 1E.0008 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Standard Status 2 */
  union
  {
    struct
    {
      unsigned int   reserved0 : 14;
                    /*! \brief 1E.0008.F:E ROS Device Present [1:0]
                        AQ_GlobalStandardStatus_2_HHD.u0.bits_0.devicePresent

                        Default = 0x2

                        [F:E]
                        0x3 = No device at this address
                        0x2 = Device present at this address
                        0x1 = No device at this address
                        0x0 = No device at this address

                 <B>Notes:</B>
                        This field is always set to 0x2, as the Global MMD resides here in the PHY.  */
      unsigned int   devicePresent : 2;    /* 1E.0008.F:E  ROS      Default = 0x2 */
                     /* [F:E]
                        0x3 = No device at this address
                        0x2 = Device present at this address
                        0x1 = No device at this address
                        0x0 = No device at this address  */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalStandardStatus_2_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Standard Package Identifier: 1E.000E */
/*                  Global Standard Package Identifier: 1E.000E */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Standard Package Identifier */
  union
  {
    struct
    {
                    /*! \brief 1E.000E.F:0 RO Package ID MSW [1F:10]
                        AQ_GlobalStandardPackageIdentifier_HHD.u0.bits_0.packageIdMSW

                        

                        Bits 31- 16 of Package ID
  */
      unsigned int   packageIdMSW : 16;    /* 1E.000E.F:0  RO       */
                     /* Bits 31- 16 of Package ID  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Standard Package Identifier */
  union
  {
    struct
    {
                    /*! \brief 1E.000F.F:0 RO Package ID LSW [F:0]
                        AQ_GlobalStandardPackageIdentifier_HHD.u1.bits_1.packageIdLSW

                        

                        Bits 15 - 0 of Package ID
  */
      unsigned int   packageIdLSW : 16;    /* 1E.000F.F:0  RO       */
                     /* Bits 15 - 0 of Package ID  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_GlobalStandardPackageIdentifier_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Firmware ID: 1E.0020 */
/*                  Global Firmware ID: 1E.0020 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Firmware ID */
  union
  {
    struct
    {
                    /*! \brief 1E.0020.7:0 RO Firmware Minor Revision Number [7:0]
                        AQ_GlobalFirmwareID_HHD.u0.bits_0.firmwareMinorRevisionNumber

                        

                        [7:0] = Minor revision number

                 <B>Notes:</B>
                        
                        
                        The lower six bits of major and minor firmware revision are exchanged in autonegotiation when the PHYID message is sent.  */
      unsigned int   firmwareMinorRevisionNumber : 8;    /* 1E.0020.7:0  RO       */
                     /* [7:0] = Minor revision number  */
                    /*! \brief 1E.0020.F:8 RO Firmware Major Revision Number [7:0]
                        AQ_GlobalFirmwareID_HHD.u0.bits_0.firmwareMajorRevisionNumber

                        

                        [F:8] = Major revision number

                 <B>Notes:</B>
                        
                        
                        The lower six bits of major and minor firmware revision are exchanged in autonegotiation when the PHYID message is sent.  */
      unsigned int   firmwareMajorRevisionNumber : 8;    /* 1E.0020.F:8  RO       */
                     /* [F:8] = Major revision number  */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalFirmwareID_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global NVR Interface: 1E.0100 */
/*                  Global NVR Interface: 1E.0100 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global NVR Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0100.7:0 R/W NVR Opcode [7:0]
                        AQ_GlobalNvrInterface_HHD.u0.bits_0.nvrOpcode

                        Default = 0x03

                        NVR instruction opcode
                        
  */
      unsigned int   nvrOpcode : 8;    /* 1E.0100.7:0  R/W      Default = 0x03 */
                     /* NVR instruction opcode
                          */
                    /*! \brief 1E.0100.8 RO NVR Busy
                        AQ_GlobalNvrInterface_HHD.u0.bits_0.nvrBusy

                        

                        1 = NVR is busy
                        0 = NVR is ready
                        

                 <B>Notes:</B>
                        When set to 1, the NVR is busy. A new NVR operation should not occur until this bit is 0. If the NVR clock is greater than 64/63 of the MDIO clock, this bit never needs to be polled when operating over the MDIO.   */
      unsigned int   nvrBusy : 1;    /* 1E.0100.8  RO       */
                     /* 1 = NVR is busy
                        0 = NVR is ready
                          */
      unsigned int   reserved1 : 1;
                    /*! \brief 1E.0100.A R/W NVR Burst
                        AQ_GlobalNvrInterface_HHD.u0.bits_0.nvrBurst

                        Default = 0x0

                        0 = Single read or write operation of up to 4 bytes
                        1 = Burst operation
                        

                 <B>Notes:</B>
                        When this bit is set, the operation is a burst operation where more than 32-bits is read from the NVR or written to the NVR. This bit should be set to one until the last burst in the read or write operation, when it should be set to zero. It operates by gating the SPI clock, and not restarting it until new data is ready to be written, or the previous contents have been read. Each burst of data requires the NVR Execute Operation bit to be set to initiate the next phase.  */
      unsigned int   nvrBurst : 1;    /* 1E.0100.A  R/W      Default = 0x0 */
                     /* 0 = Single read or write operation of up to 4 bytes
                        1 = Burst operation
                          */
      unsigned int   reserved0 : 1;
                    /*! \brief 1E.0100.C R/WSC Reset NVR CRC
                        AQ_GlobalNvrInterface_HHD.u0.bits_0.resetNvrCrc

                        Default = 0x0

                        1 = Reset NVR Mailbox CRC calculation register
                        
                        

                 <B>Notes:</B>
                        To prevent an erroneous answer, this bit should not be set at the same time the  See NVR Operation Valid bit is set.  */
      unsigned int   resetNvrCrc : 1;    /* 1E.0100.C  R/WSC      Default = 0x0 */
                     /* 1 = Reset NVR Mailbox CRC calculation register
                        
                          */
                    /*! \brief 1E.0100.D R/W Freeze NVR CRC
                        AQ_GlobalNvrInterface_HHD.u0.bits_0.freezeNvrCrc

                        Default = 0x0

                        1 = Freeze NVR Mailbox CRC calculation register
                        

                 <B>Notes:</B>
                        To prevent an erroneous answer, this bit should not be set at the same time the  See NVR Operation Valid bit is set.  */
      unsigned int   freezeNvrCrc : 1;    /* 1E.0100.D  R/W      Default = 0x0 */
                     /* 1 = Freeze NVR Mailbox CRC calculation register
                          */
                    /*! \brief 1E.0100.E R/W NVR Write Mode
                        AQ_GlobalNvrInterface_HHD.u0.bits_0.nvrWriteMode

                        Default = 0x0

                        1 = Write to NVR
                        0 = Read from NVR
                        
  */
      unsigned int   nvrWriteMode : 1;    /* 1E.0100.E  R/W      Default = 0x0 */
                     /* 1 = Write to NVR
                        0 = Read from NVR
                          */
                    /*! \brief 1E.0100.F R/WSC NVR Execute Operation
                        AQ_GlobalNvrInterface_HHD.u0.bits_0.nvrExecuteOperation

                        Default = 0x0

                        1 = Start NVR Operation
                        
                        

                 <B>Notes:</B>
                        When set to 1, the NVR operation will begin. Ensure that the uP is stalled using the  See MCP Run Stall bit to ensure no NVR contention.  */
      unsigned int   nvrExecuteOperation : 1;    /* 1E.0100.F  R/WSC      Default = 0x0 */
                     /* 1 = Start NVR Operation
                        
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global NVR Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0101.F:0 RO NVR Mailbox CRC [F:0]
                        AQ_GlobalNvrInterface_HHD.u1.bits_1.nvrMailboxCrc

                        

                        The running CRC-16 of everything passing through the NVR interface
                        

                 <B>Notes:</B>
                        The CRC-16 over all data written or read through the NVR interface. The CRC-16 is calculated by dividing the data by:
                        x^16 + x^12 + x^5 + 1  */
      unsigned int   nvrMailboxCrc : 16;    /* 1E.0101.F:0  RO       */
                     /* The running CRC-16 of everything passing through the NVR interface
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global NVR Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0102.7:0 R/W NVR Address MSW [17:10]
                        AQ_GlobalNvrInterface_HHD.u2.bits_2.nvrAddressMSW

                        Default = 0x00

                        NVR address MSW bits [17:10]
                        

                 <B>Notes:</B>
                        The address of where to read and write from in the NVR. This is self-incrementing and will automatically increment after each read or write operation. The increment amount is based on the data length (i.e. increments by 4 if the data length is 4 bytes)  */
      unsigned int   nvrAddressMSW : 8;    /* 1E.0102.7:0  R/W      Default = 0x00 */
                     /* NVR address MSW bits [17:10]
                          */
      unsigned int   reserved0 : 8;
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global NVR Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0103.F:0 R/W NVR Address LSW [F:0]
                        AQ_GlobalNvrInterface_HHD.u3.bits_3.nvrAddressLSW

                        Default = 0x0000

                        NVR address LSW bits [F:0]
                        

                 <B>Notes:</B>
                        The address of where to read and write from in the NVR. This is self-incrementing and will automatically increment after each read or write operation.  */
      unsigned int   nvrAddressLSW : 16;    /* 1E.0103.F:0  R/W      Default = 0x0000 */
                     /* NVR address LSW bits [F:0]
                          */
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of Global NVR Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0104.F:0 R/W NVR Data MSW [1F:10]
                        AQ_GlobalNvrInterface_HHD.u4.bits_4.nvrDataMSW

                        Default = 0x0000

                        NVR data MSW bits [1F:10]
                        

                 <B>Notes:</B>
                        Data is stored and read-out from these registers in little-endian format for operations such as FLASH device ID, and for programming the processor.
                        
                        For instance the 64K Atmel device code reads out as two bytes 0x651F into the LSW register, whereas the datasheet indicates that 1F is the first byte read, followed by 65 as the second byte.
                        
                        To burst read and write these 4 bytes in the correct order (where DD is written to address x), they should be stored as:
                        
                        AA BB in the MSW
                        CC DD in the LSW.  */
      unsigned int   nvrDataMSW : 16;    /* 1E.0104.F:0  R/W      Default = 0x0000 */
                     /* NVR data MSW bits [1F:10]
                          */
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of Global NVR Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0105.F:0 R/W NVR Data LSW [F:0]
                        AQ_GlobalNvrInterface_HHD.u5.bits_5.nvrDataLSW

                        Default = 0x0000

                        NVR data LSW bits [F:0]
                        

                 <B>Notes:</B>
                        Data is stored and read-out from these registers in little-endian format for operations such as FLASH device ID, and for programming the processor.
                        
                        For instance the 64K Atmel device code reads out as two bytes 0x651F into the LSW register, whereas the datasheet indicates that 1F is the first byte read, followed by 65 as the second byte.
                        To burst read and write these 4 bytes in the correct order (where DD is written to address x), they should be stored as:
                        
                        AA BB in the MSW
                        CC DD in the LSW.  */
      unsigned int   nvrDataLSW : 16;    /* 1E.0105.F:0  R/W      Default = 0x0000 */
                     /* NVR data LSW bits [F:0]
                          */
    } bits_5;
    uint16_t word_5;
  } u5;
} AQ_GlobalNvrInterface_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Mailbox Interface: 1E.0200 */
/*                  Global Mailbox Interface: 1E.0200 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Mailbox Interface */
  union
  {
    struct
    {
      unsigned int   reserved2 : 8;
                    /*! \brief 1E.0200.8 RO uP Mailbox Busy
                        AQ_GlobalMailboxInterface_HHD.u0.bits_0.upMailboxBusy

                        

                        1 = uP mailbox busy
                        0 = uP mailbox ready
                        

                 <B>Notes:</B>
                        In general the uP will respond within a few processor cycles to any PIF slave request, much faster than the MDIO. If the busy is asserted over multiple MDIO polling cycles, then a H/W error may have occurred and a Global S/W reset or uP reset is required.  */
      unsigned int   upMailboxBusy : 1;    /* 1E.0200.8  RO       */
                     /* 1 = uP mailbox busy
                        0 = uP mailbox ready
                          */
      unsigned int   reserved1 : 3;
                    /*! \brief 1E.0200.C R/WSC Reset uP Mailbox CRC
                        AQ_GlobalMailboxInterface_HHD.u0.bits_0.resetUpMailboxCrc

                        Default = 0x0

                        1 = Reset uP mailbox CRC calculation register
                        
                        
  */
      unsigned int   resetUpMailboxCrc : 1;    /* 1E.0200.C  R/WSC      Default = 0x0 */
                     /* 1 = Reset uP mailbox CRC calculation register
                        
                          */
      unsigned int   reserved0 : 1;
                    /*! \brief 1E.0200.E R/W uP Mailbox Write Mode
                        AQ_GlobalMailboxInterface_HHD.u0.bits_0.upMailboxWriteMode

                        Default = 0x0

                        1 = Write
                        0 = Read
                        

                 <B>Notes:</B>
                        Mailbox direction  */
      unsigned int   upMailboxWriteMode : 1;    /* 1E.0200.E  R/W      Default = 0x0 */
                     /* 1 = Write
                        0 = Read
                          */
                    /*! \brief 1E.0200.F R/WSC uP Mailbox Execute Operation
                        AQ_GlobalMailboxInterface_HHD.u0.bits_0.upMailboxExecuteOperation

                        Default = 0x0

                        1 = Start of mailbox Operation
                        
                        

                 <B>Notes:</B>
                        Indicates mailbox is loaded and ready  */
      unsigned int   upMailboxExecuteOperation : 1;    /* 1E.0200.F  R/WSC      Default = 0x0 */
                     /* 1 = Start of mailbox Operation
                        
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Mailbox Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0201.F:0 RO uP Mailbox CRC [F:0]
                        AQ_GlobalMailboxInterface_HHD.u1.bits_1.upMailboxCrc

                        

                        The running CRC-16 of everything passing through the mailbox interface
                        
  */
      unsigned int   upMailboxCrc : 16;    /* 1E.0201.F:0  RO       */
                     /* The running CRC-16 of everything passing through the mailbox interface
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global Mailbox Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0202.F:0 R/W uP Mailbox Address MSW [1F:10]
                        AQ_GlobalMailboxInterface_HHD.u2.bits_2.upMailboxAddressMSW

                        Default = 0x0000

                        uP Mailbox MSW address
                        

                 <B>Notes:</B>
                        The address of where to read and write from in the Microcontroller Mailbox. This is self-incrementing and automatically increments after each read and write operation.PHY  */
      unsigned int   upMailboxAddressMSW : 16;    /* 1E.0202.F:0  R/W      Default = 0x0000 */
                     /* uP Mailbox MSW address
                          */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global Mailbox Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0203.1:0 RO uP Mailbox Address LSW Don't Care [1:0]
                        AQ_GlobalMailboxInterface_HHD.u3.bits_3.upMailboxAddressLSW_Don_tCare

                        

                        Least significant uP LSW Mailbox address bits [1:0]
                        

                 <B>Notes:</B>
                        These bits are always set to 0 since each memory access is on a 4-byte boundary.  */
      unsigned int   upMailboxAddressLSW_Don_tCare : 2;    /* 1E.0203.1:0  RO       */
                     /* Least significant uP LSW Mailbox address bits [1:0]
                          */
                    /*! \brief 1E.0203.F:2 R/W uP Mailbox Address LSW [F:2]
                        AQ_GlobalMailboxInterface_HHD.u3.bits_3.upMailboxAddressLSW

                        Default = 0x0000

                        uP LSW Mailbox address [F:2]
                        

                 <B>Notes:</B>
                        The address of where to read and write from in the Microcontroller Mailbox. This is self-incrementing and automatically increments after each read and write operation.PHY  */
      unsigned int   upMailboxAddressLSW : 14;    /* 1E.0203.F:2  R/W      Default = 0x0000 */
                     /* uP LSW Mailbox address [F:2]
                          */
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of Global Mailbox Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0204.F:0 R/W uP Mailbox Data MSW [1F:10]
                        AQ_GlobalMailboxInterface_HHD.u4.bits_4.upMailboxDataMSW

                        Default = 0x0000

                        uP Mailbox data MSW
                        
  */
      unsigned int   upMailboxDataMSW : 16;    /* 1E.0204.F:0  R/W      Default = 0x0000 */
                     /* uP Mailbox data MSW
                          */
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of Global Mailbox Interface */
  union
  {
    struct
    {
                    /*! \brief 1E.0205.F:0 R/W uP Mailbox Data LSW [F:0]
                        AQ_GlobalMailboxInterface_HHD.u5.bits_5.upMailboxDataLSW

                        Default = 0x0000

                        uP Mailbox data LSW
                        
  */
      unsigned int   upMailboxDataLSW : 16;    /* 1E.0205.F:0  R/W      Default = 0x0000 */
                     /* uP Mailbox data LSW
                          */
    } bits_5;
    uint16_t word_5;
  } u5;
  /*! \brief Union for bit and word level access of word 6 of Global Mailbox Interface */
  union
  {
    struct
    {
      unsigned int   reserved1 : 1;
                    /*! \brief 1E.0206.1 R/W uP Mailbox CRC Read Enable
                        AQ_GlobalMailboxInterface_HHD.u6.bits_6.upMailboxCrcReadEnable

                        Default = 0x0

                        1 = Update uP mailbox CRC on read
                        
  */
      unsigned int   upMailboxCrcReadEnable : 1;    /* 1E.0206.1  R/W      Default = 0x0 */
                     /* 1 = Update uP mailbox CRC on read
                          */
      unsigned int   reserved0 : 14;
    } bits_6;
    uint16_t word_6;
  } u6;
} AQ_GlobalMailboxInterface_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Microprocessor Scratch Pad: 1E.0300 */
/*                  Global Microprocessor Scratch Pad: 1E.0300 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Microprocessor Scratch Pad */
  union
  {
    struct
    {
                    /*! \brief 1E.0300.F:0 R/W Scratch Pad 1[F:0]
                        AQ_GlobalMicroprocessorScratchPad_HHD.u0.bits_0.scratchPad_1

                        Default = 0x0000

                        General Purpose Scratch Pad
  */
      unsigned int   scratchPad_1 : 16;    /* 1E.0300.F:0  R/W      Default = 0x0000 */
                     /* General Purpose Scratch Pad  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Microprocessor Scratch Pad */
  union
  {
    struct
    {
                    /*! \brief 1E.0301.F:0 R/W Scratch Pad 2 [F:0]
                        AQ_GlobalMicroprocessorScratchPad_HHD.u1.bits_1.scratchPad_2

                        Default = 0x0000

                        General Purpose Scratch Pad
  */
      unsigned int   scratchPad_2 : 16;    /* 1E.0301.F:0  R/W      Default = 0x0000 */
                     /* General Purpose Scratch Pad  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_GlobalMicroprocessorScratchPad_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress Control Register: 1E.5002 */
/*                  MSS Egress Control Register: 1E.5002 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5002.0 R/W MSS Egress Soft Reset
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressSoftReset

                        Default = 0x0

                        1 = Soft reset
                        

                 <B>Notes:</B>
                        S/W reset  */
      unsigned int   mssEgressSoftReset : 1;    /* 1E.5002.0  R/W      Default = 0x0 */
                     /* 1 = Soft reset
                          */
                    /*! \brief 1E.5002.1 R/W MSS Egress Drop KAY Packet
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressDropKayPacket

                        Default = 0x0

                        1 = Drop KAY packet
                        

                 <B>Notes:</B>
                        Decides whether KAY packets have to be dropped  */
      unsigned int   mssEgressDropKayPacket : 1;    /* 1E.5002.1  R/W      Default = 0x0 */
                     /* 1 = Drop KAY packet
                          */
                    /*! \brief 1E.5002.2 R/W MSS Egress Drop EGPRC LUT Miss
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressDropEgprcLutMiss

                        Default = 0x0

                        1 = Drop Egress Classification LUT miss packets
                        
                        

                 <B>Notes:</B>
                        Decides whether Egress Pre-Security Classification (EGPRC) LUT miss packets are to be dropped  */
      unsigned int   mssEgressDropEgprcLutMiss : 1;    /* 1E.5002.2  R/W      Default = 0x0 */
                     /* 1 = Drop Egress Classification LUT miss packets
                        
                          */
                    /*! \brief 1E.5002.3 R/W MSS Egress GCM Start
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressGcmStart

                        Default = 0x0

                        1 = Start GCM
                        
                        

                 <B>Notes:</B>
                        Indicates GCM to start  */
      unsigned int   mssEgressGcmStart : 1;    /* 1E.5002.3  R/W      Default = 0x0 */
                     /* 1 = Start GCM
                        
                          */
                    /*! \brief 1E.5002.4 R/W MSS Egresss GCM Test Mode
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgresssGcmTestMode

                        Default = 0x0

                        1 = Enable GCM test mode
                        
                        

                 <B>Notes:</B>
                        Enables GCM test mode  */
      unsigned int   mssEgresssGcmTestMode : 1;    /* 1E.5002.4  R/W      Default = 0x0 */
                     /* 1 = Enable GCM test mode
                        
                          */
                    /*! \brief 1E.5002.5 R/W MSS Egress Unmatched Use SC 0
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressUnmatchedUseSc_0

                        Default = 0x0

                        1 = Use SC 0 for unmatched packets
                        0 = Unmatched packets are uncontrolled packets
                        
                        

                 <B>Notes:</B>
                        Use SC-Index 0 as default SC for unmatched packets. Otherwise the packets are treated as uncontrolled packets.  */
      unsigned int   mssEgressUnmatchedUseSc_0 : 1;    /* 1E.5002.5  R/W      Default = 0x0 */
                     /* 1 = Use SC 0 for unmatched packets
                        0 = Unmatched packets are uncontrolled packets
                        
                          */
                    /*! \brief 1E.5002.6 R/W MSS Egress Drop Invalid SA/SC Packets
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressDropInvalidSa_scPackets

                        Default = 0x0

                        1 = Drop invalid SA/SC packets
                        
                        

                 <B>Notes:</B>
                        Enables dropping of invalid SA/SC packets.  */
      unsigned int   mssEgressDropInvalidSa_scPackets : 1;    /* 1E.5002.6  R/W      Default = 0x0 */
                     /* 1 = Drop invalid SA/SC packets
                        
                          */
                    /*! \brief 1E.5002.7 R/W MSS Egress Explicit SECTag Report Short Length
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressExplicitSectagReportShortLength

                        Default = 0x0

                        Reserved
                        
                        

                 <B>Notes:</B>
                        Unused.  */
      unsigned int   mssEgressExplicitSectagReportShortLength : 1;    /* 1E.5002.7  R/W      Default = 0x0 */
                     /* Reserved
                        
                          */
                    /*! \brief 1E.5002.8 R/W MSS Egress External Classification Enable
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressExternalClassificationEnable

                        Default = 0x0

                        1 = Drop EGPRC miss packets
                        
                        

                 <B>Notes:</B>
                        If set, internal classification is bypassed. Should always be set to 0.  */
      unsigned int   mssEgressExternalClassificationEnable : 1;    /* 1E.5002.8  R/W      Default = 0x0 */
                     /* 1 = Drop EGPRC miss packets
                        
                          */
                    /*! \brief 1E.5002.9 R/W MSS Egress ICV LSB 8 Bytes Enable
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressIcvLsb_8BytesEnable

                        Default = 0x0

                        1 = Use LSB
                        0 = Use MSB
                        
                        

                 <B>Notes:</B>
                        This bit selects MSB or LSB 8 bytes selection in the case where the ICV is 8 bytes.
                        0 = MSB is used.  */
      unsigned int   mssEgressIcvLsb_8BytesEnable : 1;    /* 1E.5002.9  R/W      Default = 0x0 */
                     /* 1 = Use LSB
                        0 = Use MSB
                        
                          */
                    /*! \brief 1E.5002.A R/W MSS Egress High Priority
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressHighPriority

                        Default = 0x0

                        1 = MIB counter clear on read enable
                        
                        

                 <B>Notes:</B>
                        If this bit is set to 1, read is given high priority and the MIB count value becomes 0 after read.  */
      unsigned int   mssEgressHighPriority : 1;    /* 1E.5002.A  R/W      Default = 0x0 */
                     /* 1 = MIB counter clear on read enable
                        
                          */
                    /*! \brief 1E.5002.B R/W MSS Egress Clear Counter
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressClearCounter

                        Default = 0x0

                        1 = Clear all MIB counters
                        
                        

                 <B>Notes:</B>
                        If this bit is set to 1, all MIB counters will be cleared.  */
      unsigned int   mssEgressClearCounter : 1;    /* 1E.5002.B  R/W      Default = 0x0 */
                     /* 1 = Clear all MIB counters
                        
                          */
                    /*! \brief 1E.5002.C R/W MSS Egress Clear Global Time
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressClearGlobalTime

                        Default = 0x0

                        1 = Clear global time
                        
                        

                 <B>Notes:</B>
                        Clear global time.  */
      unsigned int   mssEgressClearGlobalTime : 1;    /* 1E.5002.C  R/W      Default = 0x0 */
                     /* 1 = Clear global time
                        
                          */
                    /*! \brief 1E.5002.F:D R/W MSS Egress Ethertype Explicit SECTag LSB [2:0]
                        AQ_MssEgressControlRegister_HHD.u0.bits_0.mssEgressEthertypeExplicitSectagLsb

                        Default = 0x0

                        Ethertype for explicit SECTag bits 2:0.
                        

                 <B>Notes:</B>
                        Ethertype for explicity SECTag.  */
      unsigned int   mssEgressEthertypeExplicitSectagLsb : 3;    /* 1E.5002.F:D  R/W      Default = 0x0 */
                     /* Ethertype for explicit SECTag bits 2:0.
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5003.C:0 R/W MSS Egress Ethertype Explicit SECTag MSB  [F:3]
                        AQ_MssEgressControlRegister_HHD.u1.bits_1.mssEgressEthertypeExplicitSectagMsb

                        Default = 0x0000

                        Ethertype for explicit SECTag bits 15:3.
                        

                 <B>Notes:</B>
                        Ethertype for explicity SECTag.  */
      unsigned int   mssEgressEthertypeExplicitSectagMsb : 13;    /* 1E.5003.C:0  R/W      Default = 0x0000 */
                     /* Ethertype for explicit SECTag bits 15:3.
                          */
      unsigned int   reserved0 : 3;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress VLAN TPID 0 Register: 1E.5008 */
/*                  MSS Egress VLAN TPID 0 Register: 1E.5008 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress VLAN TPID 0 Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5008.F:0 R/W MSS Egress VLAN STag TPID [F:0]
                        AQ_MssEgressVlanTpid_0Register_HHD.u0.bits_0.mssEgressVlanStagTpid

                        Default = 0x0000

                        STag TPID 
                        

                 <B>Notes:</B>
                        Service Tag Protocol Identifier (TPID) values to identify a VLAN tag. The " See SEC Egress VLAN CP Tag Parse STag " bit must be set to 1 for the incoming packet's TPID to be parsed.  */
      unsigned int   mssEgressVlanStagTpid : 16;    /* 1E.5008.F:0  R/W      Default = 0x0000 */
                     /* STag TPID 
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress VLAN TPID 0 Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressVlanTpid_0Register_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress VLAN TPID 1 Register: 1E.500A */
/*                  MSS Egress VLAN TPID 1 Register: 1E.500A */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress VLAN TPID 1 Register */
  union
  {
    struct
    {
                    /*! \brief 1E.500A.F:0 R/W MSS Egress VLAN QTag TPID [F:0]
                        AQ_MssEgressVlanTpid_1Register_HHD.u0.bits_0.mssEgressVlanQtagTpid

                        Default = 0x0000

                        QTag TPID
                        

                 <B>Notes:</B>
                        Customer Tag Protocol Identifier (TPID) values to identify a VLAN tag. The " See SEC Egress VLAN CP Tag Parse QTag " bit must be set to 1 for the incoming packet's TPID to be parsed.  */
      unsigned int   mssEgressVlanQtagTpid : 16;    /* 1E.500A.F:0  R/W      Default = 0x0000 */
                     /* QTag TPID
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress VLAN TPID 1 Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressVlanTpid_1Register_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress VLAN Control Register: 1E.500C */
/*                  MSS Egress VLAN Control Register: 1E.500C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress VLAN Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.500C.F:0 R/W MSS Egress VLAN UP Map Table [F:0]
                        AQ_MssEgressVlanControlRegister_HHD.u0.bits_0.mssEgressVlanUpMapTable

                        Default = 0x0000

                        UP Map table bits 15:0
                        

                 <B>Notes:</B>
                        If there is a customer TPID Tag match and no service TPID Tag match or the service TPID Tag match is disabled, the outer TAG's PCP is used to index into this map table to generate the packets user priority.
                        2:0 : UP value for customer Tag PCP 0x0
                        5:3: UP value for customer Tag PCP 0x0
                        8:6 : UP value for customer Tag PCP 0x0
                        11:9 : UP value for customer Tag PCP 0x0
                        14:12 : UP value for customer Tag PCP 0x0
                        17:15 : UP value for customer Tag PCP 0x0
                        20:18 : UP value for customer Tag PCP 0x0
                        23:21 : UP value for customer Tag PCP 0x0  */
      unsigned int   mssEgressVlanUpMapTable : 16;    /* 1E.500C.F:0  R/W      Default = 0x0000 */
                     /* UP Map table bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress VLAN Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.500D.7:0 R/W MSS Egress VLAN UP Map Table MSW [17:10]
                        AQ_MssEgressVlanControlRegister_HHD.u1.bits_1.mssEgressVlanUpMapTableMSW

                        Default = 0x00

                        UP Map table bits 23:16
                        

                 <B>Notes:</B>
                        If there is a customer TPID Tag match and no service TPID Tag match or the service TPID Tag match is disabled, the outer TAG's PCP is used to index into this map table to generate the packets user priority.
                        2:0 : UP value for customer Tag PCP 0x0
                        5:3: UP value for customer Tag PCP 0x0
                        8:6 : UP value for customer Tag PCP 0x0
                        11:9 : UP value for customer Tag PCP 0x0
                        14:12 : UP value for customer Tag PCP 0x0
                        17:15 : UP value for customer Tag PCP 0x0
                        20:18 : UP value for customer Tag PCP 0x0
                        23:21 : UP value for customer Tag PCP 0x0  */
      unsigned int   mssEgressVlanUpMapTableMSW : 8;    /* 1E.500D.7:0  R/W      Default = 0x00 */
                     /* UP Map table bits 23:16
                          */
                    /*! \brief 1E.500D.A:8 R/W MSS Egress VLAN UP Default [2:0]
                        AQ_MssEgressVlanControlRegister_HHD.u1.bits_1.mssEgressVlanUpDefault

                        Default = 0x0

                        UP default
                        

                 <B>Notes:</B>
                        User priority default  */
      unsigned int   mssEgressVlanUpDefault : 3;    /* 1E.500D.A:8  R/W      Default = 0x0 */
                     /* UP default
                          */
                    /*! \brief 1E.500D.B R/W MSS Egress VLAN STag UP Parse Enable
                        AQ_MssEgressVlanControlRegister_HHD.u1.bits_1.mssEgressVlanStagUpParseEnable

                        Default = 0x0

                        VLAN CP Tag STag UP enable
                        

                 <B>Notes:</B>
                        Enable controlled port service VLAN service Tag user priority field parsing.  */
      unsigned int   mssEgressVlanStagUpParseEnable : 1;    /* 1E.500D.B  R/W      Default = 0x0 */
                     /* VLAN CP Tag STag UP enable
                          */
                    /*! \brief 1E.500D.C R/W MSS Egress VLAN QTag UP Parse Enable
                        AQ_MssEgressVlanControlRegister_HHD.u1.bits_1.mssEgressVlanQtagUpParseEnable

                        Default = 0x0

                        VLAN CP Tag QTag UP enable
                        

                 <B>Notes:</B>
                        Enable controlled port customer VLAN customer Tag user priority field parsing.  */
      unsigned int   mssEgressVlanQtagUpParseEnable : 1;    /* 1E.500D.C  R/W      Default = 0x0 */
                     /* VLAN CP Tag QTag UP enable
                          */
                    /*! \brief 1E.500D.D R/W MSS Egress VLAN QinQ Parse Enable
                        AQ_MssEgressVlanControlRegister_HHD.u1.bits_1.mssEgressVlanQinqParseEnable

                        Default = 0x0

                        VLAN CP Tag Parse QinQ
                        

                 <B>Notes:</B>
                        Enable controlled port VLAN QinQ Tag parsing. When this bit is set to 1 both the outer and inner VLAN Tags will be parsed.  */
      unsigned int   mssEgressVlanQinqParseEnable : 1;    /* 1E.500D.D  R/W      Default = 0x0 */
                     /* VLAN CP Tag Parse QinQ
                          */
                    /*! \brief 1E.500D.E R/W MSS Egress VLAN STag Parse Enable
                        AQ_MssEgressVlanControlRegister_HHD.u1.bits_1.mssEgressVlanStagParseEnable

                        Default = 0x0

                        1 = Enable VLAN STag parsing
                        

                 <B>Notes:</B>
                        Enable controlled port VLAN service Tag parsing. When this bit is set to 1, the incoming packets outer TPID will be compared with the configured " See SEC Egress TPID 0 [F:0] " for matching. If the " See SEC Egress VLAN CP Tag Parse QinQ " bit is set to1, this will also be used to compare the incoming packet's inner TPID.  */
      unsigned int   mssEgressVlanStagParseEnable : 1;    /* 1E.500D.E  R/W      Default = 0x0 */
                     /* 1 = Enable VLAN STag parsing
                          */
                    /*! \brief 1E.500D.F R/W MSS Egress VLAN QTag Parse Enable
                        AQ_MssEgressVlanControlRegister_HHD.u1.bits_1.mssEgressVlanQtagParseEnable

                        Default = 0x0

                        1 = Enable VLAN QTag parsing
                        

                 <B>Notes:</B>
                        Enable controlled port VLAN customer Tag parsing. When this bit is set to 1, the incoming packet's outer TPID will be compared with the configured " See SEC Egress TPID 1 [F:0] " for matching. If the " See SEC Egress VLAN CP Tag Parse QinQ " bit is set to1, this will also be used to compare the incoming packet's inner TPID.  */
      unsigned int   mssEgressVlanQtagParseEnable : 1;    /* 1E.500D.F  R/W      Default = 0x0 */
                     /* 1 = Enable VLAN QTag parsing
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressVlanControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress PN Control Register: 1E.500E */
/*                  MSS Egress PN Control Register: 1E.500E */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress PN Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.500E.F:0 R/W MSS Egress SA PN Threshold LSW [F:0]
                        AQ_MssEgressPnControlRegister_HHD.u0.bits_0.mssEgressSaPnThresholdLSW

                        Default = 0x0000

                        PN threshold bits 15:0
                        

                 <B>Notes:</B>
                        Egress PN threshold to generate SA threshold interrupt.  */
      unsigned int   mssEgressSaPnThresholdLSW : 16;    /* 1E.500E.F:0  R/W      Default = 0x0000 */
                     /* PN threshold bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress PN Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.500F.F:0 R/W MSS Egress SA PN Threshold MSW [1F:10]
                        AQ_MssEgressPnControlRegister_HHD.u1.bits_1.mssEgressSaPnThresholdMSW

                        Default = 0x0000

                        PN threshold bits 31:16
                        

                 <B>Notes:</B>
                        Egress PN threshold to generate SA threshold interrupt.  */
      unsigned int   mssEgressSaPnThresholdMSW : 16;    /* 1E.500F.F:0  R/W      Default = 0x0000 */
                     /* PN threshold bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressPnControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress MTU Size Control Register: 1E.5010 */
/*                  MSS Egress MTU Size Control Register: 1E.5010 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress MTU Size Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5010.F:0 R/W MSS Egress Controlled Packet MTU Size [F:0]
                        AQ_MssEgressMtuSizeControlRegister_HHD.u0.bits_0.mssEgressControlledPacketMtuSize

                        Default = 0x05DC

                        Maximum transmission unit for controlled packet
                        

                 <B>Notes:</B>
                        Maximum transmission unit of controlled packet  */
      unsigned int   mssEgressControlledPacketMtuSize : 16;    /* 1E.5010.F:0  R/W      Default = 0x05DC */
                     /* Maximum transmission unit for controlled packet
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress MTU Size Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5011.F:0 R/W MSS Egress Uncontrolled Packet MTU Size [F:0]
                        AQ_MssEgressMtuSizeControlRegister_HHD.u1.bits_1.mssEgressUncontrolledPacketMtuSize

                        Default = 0x05DC

                        Maximum transmission unit for uncontrolled packet
                        

                 <B>Notes:</B>
                        Maximum transmission unit of uncontrolled packet  */
      unsigned int   mssEgressUncontrolledPacketMtuSize : 16;    /* 1E.5011.F:0  R/W      Default = 0x05DC */
                     /* Maximum transmission unit for uncontrolled packet
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressMtuSizeControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress Interrupt Status Register: 1E.505C */
/*                  MSS Egress Interrupt Status Register: 1E.505C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress Interrupt Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.505C.0 COW MSS Egress Master Interrupt
                        AQ_MssEgressInterruptStatusRegister_HHD.u0.bits_0.mssEgressMasterInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when any one of the above interrupt and the corresponding interrupt enable are both set. The interrupt enable for this bit must also be set for this bit to be set.  */
      unsigned int   mssEgressMasterInterrupt : 1;    /* 1E.505C.0  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.505C.1 COW MSS Egress SA Expired Interrupt
                        AQ_MssEgressInterruptStatusRegister_HHD.u0.bits_0.mssEgressSaExpiredInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when the SA PN reaches all ones saturation.  */
      unsigned int   mssEgressSaExpiredInterrupt : 1;    /* 1E.505C.1  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.505C.2 COW MSS Egress SA Threshold Expired Interrupt
                        AQ_MssEgressInterruptStatusRegister_HHD.u0.bits_0.mssEgressSaThresholdExpiredInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when the SA PN reaches the  See SEC Egress PN Threshold [F:0] and  See SEC Egress PN Threshold [1F:10] .  */
      unsigned int   mssEgressSaThresholdExpiredInterrupt : 1;    /* 1E.505C.2  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.505C.3 COW MSS Egress MIB Saturation Interrupt
                        AQ_MssEgressInterruptStatusRegister_HHD.u0.bits_0.mssEgressMibSaturationInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when the MIB counters reaches all ones saturation.  */
      unsigned int   mssEgressMibSaturationInterrupt : 1;    /* 1E.505C.3  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.505C.4 COW MSS Egress ECC Error Interrupt
                        AQ_MssEgressInterruptStatusRegister_HHD.u0.bits_0.mssEgressEccErrorInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when anyone of the memories detects an ECC error.  */
      unsigned int   mssEgressEccErrorInterrupt : 1;    /* 1E.505C.4  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
      unsigned int   reserved0 : 11;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress Interrupt Status Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressInterruptStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress Interrupt Mask Register: 1E.505E */
/*                  MSS Egress Interrupt Mask Register: 1E.505E */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress Interrupt Mask Register */
  union
  {
    struct
    {
                    /*! \brief 1E.505E.0 COW MSS Egress Master Interrupt Enable
                        AQ_MssEgressInterruptMaskRegister_HHD.u0.bits_0.mssEgressMasterInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        

                 <B>Notes:</B>
                        Write to 1 to clear.  */
      unsigned int   mssEgressMasterInterruptEnable : 1;    /* 1E.505E.0  COW      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.505E.1 COW MSS Egress SA Expired Interrupt Enable
                        AQ_MssEgressInterruptMaskRegister_HHD.u0.bits_0.mssEgressSaExpiredInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when the SA PN reaches all ones saturation.  */
      unsigned int   mssEgressSaExpiredInterruptEnable : 1;    /* 1E.505E.1  COW      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.505E.2 COW MSS Egress SA Expired Threshold Interrupt Enable
                        AQ_MssEgressInterruptMaskRegister_HHD.u0.bits_0.mssEgressSaExpiredThresholdInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when the SA PN reaches the configured threshold  See SEC Egress PN Threshold [F:0] and  See SEC Egress PN Threshold [1F:10] .  */
      unsigned int   mssEgressSaExpiredThresholdInterruptEnable : 1;    /* 1E.505E.2  COW      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.505E.3 COW MSS Egress MIB Saturation Interrupt Enable
                        AQ_MssEgressInterruptMaskRegister_HHD.u0.bits_0.mssEgressMibSaturationInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when the MIB counters reaches all ones saturation.  */
      unsigned int   mssEgressMibSaturationInterruptEnable : 1;    /* 1E.505E.3  COW      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.505E.4 COW MSS Egress ECC Error Interrupt Enable
                        AQ_MssEgressInterruptMaskRegister_HHD.u0.bits_0.mssEgressEccErrorInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when anyone of the memories detects an ECC error.  */
      unsigned int   mssEgressEccErrorInterruptEnable : 1;    /* 1E.505E.4  COW      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
      unsigned int   reserved0 : 11;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress Interrupt Mask Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressInterruptMaskRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress SA Expired Status Register: 1E.5060 */
/*                  MSS Egress SA Expired Status Register: 1E.5060 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress SA Expired Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5060.F:0 COW MSS Egress SA Expired LSW [F:0]
                        AQ_MssEgressSaExpiredStatusRegister_HHD.u0.bits_0.mssEgressSaExpiredLSW

                        Default = 0x0000

                        SA expired bits 15:0
                        

                 <B>Notes:</B>
                        Write these bits to 1 to clear.
                        When set, these bits identify the SA that has expired when the SA PN reaches all-ones saturation.  */
      unsigned int   mssEgressSaExpiredLSW : 16;    /* 1E.5060.F:0  COW      Default = 0x0000 */
                     /* SA expired bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress SA Expired Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5061.F:0 COW MSS Egress SA Expired MSW [1F:10]
                        AQ_MssEgressSaExpiredStatusRegister_HHD.u1.bits_1.mssEgressSaExpiredMSW

                        Default = 0x0000

                        SA expired bits 31:16
                        

                 <B>Notes:</B>
                        Write these bits to 1 to clear.
                        When set, these bits identify the SA that has expired when the SA PN reaches all-ones saturation.  */
      unsigned int   mssEgressSaExpiredMSW : 16;    /* 1E.5061.F:0  COW      Default = 0x0000 */
                     /* SA expired bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressSaExpiredStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress SA Threshold Expired Status Register: 1E.5062 */
/*                  MSS Egress SA Threshold Expired Status Register: 1E.5062 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress SA Threshold Expired Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5062.F:0 COW MSS Egress SA Threshold Expired LSW [F:0]
                        AQ_MssEgressSaThresholdExpiredStatusRegister_HHD.u0.bits_0.mssEgressSaThresholdExpiredLSW

                        Default = 0x0000

                        SA threshold expired bits 15:0
                        

                 <B>Notes:</B>
                        Write these bits to 1 to clear.
                        When set, these bits identify the SA that has expired when the SA PN has reached the configured threshold  See SEC Egress PN Threshold [F:0] and  See SEC Egress PN Threshold [1F:10] .  */
      unsigned int   mssEgressSaThresholdExpiredLSW : 16;    /* 1E.5062.F:0  COW      Default = 0x0000 */
                     /* SA threshold expired bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress SA Threshold Expired Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5063.F:0 COW MSS Egress SA Threshold Expired MSW [1F:10]
                        AQ_MssEgressSaThresholdExpiredStatusRegister_HHD.u1.bits_1.mssEgressSaThresholdExpiredMSW

                        Default = 0x0000

                        SA threshold expired bits 31:16
                        

                 <B>Notes:</B>
                        Write these bits to 1 to clear.
                        When set, these bits identify the SA that has expired when the SA PN has reached the configured threshold  See SEC Egress PN Threshold [F:0] and  See SEC Egress PN Threshold [1F:10] .   */
      unsigned int   mssEgressSaThresholdExpiredMSW : 16;    /* 1E.5063.F:0  COW      Default = 0x0000 */
                     /* SA threshold expired bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressSaThresholdExpiredStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress ECC Interrupt Status Register: 1E.5064 */
/*                  MSS Egress ECC Interrupt Status Register: 1E.5064 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress ECC Interrupt Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5064.F:0 COW MSS Egress SA ECC Error Interrupt LSW [F:0]
                        AQ_MssEgressEccInterruptStatusRegister_HHD.u0.bits_0.mssEgressSaEccErrorInterruptLSW

                        Default = 0x0000

                        SA ECC error interrupt bits 15:0
                        

                 <B>Notes:</B>
                        Write these bits to 1 to clear.
                        When set to 1, indicates that an ECC error occured for the SA.  */
      unsigned int   mssEgressSaEccErrorInterruptLSW : 16;    /* 1E.5064.F:0  COW      Default = 0x0000 */
                     /* SA ECC error interrupt bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress ECC Interrupt Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5065.F:0 COW MSS Egress SA ECC Error Interrupt MSW [1F:10]
                        AQ_MssEgressEccInterruptStatusRegister_HHD.u1.bits_1.mssEgressSaEccErrorInterruptMSW

                        Default = 0x0000

                        SA ECC error interrupt bits 31:16
                        

                 <B>Notes:</B>
                        Write these bits to 1 to clear.
                        When set to 1, indicates that an ECC error occured for the SA.  */
      unsigned int   mssEgressSaEccErrorInterruptMSW : 16;    /* 1E.5065.F:0  COW      Default = 0x0000 */
                     /* SA ECC error interrupt bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssEgressEccInterruptStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress LUT Address Control Register: 1E.5080 */
/*                  MSS Egress LUT Address Control Register: 1E.5080 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress LUT Address Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.5080.8:0 R/W MSS Egress LUT Address [8:0]
                        AQ_MssEgressLutAddressControlRegister_HHD.u0.bits_0.mssEgressLutAddress

                        Default = 0x000

                        LUT address
                        
  */
      unsigned int   mssEgressLutAddress : 9;    /* 1E.5080.8:0  R/W      Default = 0x000 */
                     /* LUT address
                          */
      unsigned int   reserved0 : 3;
                    /*! \brief 1E.5080.F:C R/W MSS Egress LUT Select [3:0]
                        AQ_MssEgressLutAddressControlRegister_HHD.u0.bits_0.mssEgressLutSelect

                        Default = 0x0

                        LUT select
                        

                 <B>Notes:</B>
                        0x0 : Egress MAC Control FIlter (CTLF) LUT
                        0x1 : Egress Classification LUT
                        0x2 : Egress SC/SA LUT
                        0x3 : Egress SMIB  */
      unsigned int   mssEgressLutSelect : 4;    /* 1E.5080.F:C  R/W      Default = 0x0 */
                     /* LUT select
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_MssEgressLutAddressControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress LUT Control Register: 1E.5081 */
/*                  MSS Egress LUT Control Register: 1E.5081 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress LUT Control Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 14;
                    /*! \brief 1E.5081.E R/W MSS Egress LUT Read
                        AQ_MssEgressLutControlRegister_HHD.u0.bits_0.mssEgressLutRead

                        Default = 0x0

                        1 = LUT read
                        

                 <B>Notes:</B>
                        Setting this bit to 1, will read the LUT. This bit will automatically clear to 0.  */
      unsigned int   mssEgressLutRead : 1;    /* 1E.5081.E  R/W      Default = 0x0 */
                     /* 1 = LUT read
                          */
                    /*! \brief 1E.5081.F R/W MSS Egress LUT Write
                        AQ_MssEgressLutControlRegister_HHD.u0.bits_0.mssEgressLutWrite

                        Default = 0x0

                        1 = LUT write
                        

                 <B>Notes:</B>
                        Setting this bit to 1, will write the LUT. This bit will automatically clear to 0.  */
      unsigned int   mssEgressLutWrite : 1;    /* 1E.5081.F  R/W      Default = 0x0 */
                     /* 1 = LUT write
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_MssEgressLutControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Egress LUT Data Control Register: 1E.50A0 */
/*                  MSS Egress LUT Data Control Register: 1E.50A0 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A0.F:0 R/W MSS Egress LUT Data 0 [F:0]
                        AQ_MssEgressLutDataControlRegister_HHD.u0.bits_0.mssEgressLutData_0

                        Default = 0x0000

                        LUT data bits 15:0
                        
  */
      unsigned int   mssEgressLutData_0 : 16;    /* 1E.50A0.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A1.F:0 R/W MSS Egress LUT Data 1 [1F:10]
                        AQ_MssEgressLutDataControlRegister_HHD.u1.bits_1.mssEgressLutData_1

                        Default = 0x0000

                        LUT data bits 31:16
                        
  */
      unsigned int   mssEgressLutData_1 : 16;    /* 1E.50A1.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A2.F:0 R/W MSS Egress LUT Data 2 [2F:20]
                        AQ_MssEgressLutDataControlRegister_HHD.u2.bits_2.mssEgressLutData_2

                        Default = 0x0000

                        LUT data bits 47:32
                        
  */
      unsigned int   mssEgressLutData_2 : 16;    /* 1E.50A2.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 47:32
                          */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A3.F:0 R/W MSS Egress LUT Data 3 [3F:30]
                        AQ_MssEgressLutDataControlRegister_HHD.u3.bits_3.mssEgressLutData_3

                        Default = 0x0000

                        LUT data bits 63:48
                        
  */
      unsigned int   mssEgressLutData_3 : 16;    /* 1E.50A3.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 63:48
                          */
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A4.F:0 R/W MSS Egress LUT Data 4 [4F:40]
                        AQ_MssEgressLutDataControlRegister_HHD.u4.bits_4.mssEgressLutData_4

                        Default = 0x0000

                        LUT data bits 79:64
                        
  */
      unsigned int   mssEgressLutData_4 : 16;    /* 1E.50A4.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 79:64
                          */
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A5.F:0 R/W MSS Egress LUT Data 5 [5F:50]
                        AQ_MssEgressLutDataControlRegister_HHD.u5.bits_5.mssEgressLutData_5

                        Default = 0x0000

                        LUT data bits 95:80
                        
  */
      unsigned int   mssEgressLutData_5 : 16;    /* 1E.50A5.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 95:80
                          */
    } bits_5;
    uint16_t word_5;
  } u5;
  /*! \brief Union for bit and word level access of word 6 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A6.F:0 R/W MSS Egress LUT Data 6 [6F:60]
                        AQ_MssEgressLutDataControlRegister_HHD.u6.bits_6.mssEgressLutData_6

                        Default = 0x0000

                        LUT data bits 111:96
                        
  */
      unsigned int   mssEgressLutData_6 : 16;    /* 1E.50A6.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 111:96
                          */
    } bits_6;
    uint16_t word_6;
  } u6;
  /*! \brief Union for bit and word level access of word 7 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A7.F:0 R/W MSS Egress LUT Data 7 [7F:70]
                        AQ_MssEgressLutDataControlRegister_HHD.u7.bits_7.mssEgressLutData_7

                        Default = 0x0000

                        LUT data bits 127:112
                        
  */
      unsigned int   mssEgressLutData_7 : 16;    /* 1E.50A7.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 127:112
                          */
    } bits_7;
    uint16_t word_7;
  } u7;
  /*! \brief Union for bit and word level access of word 8 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A8.F:0 R/W MSS Egress LUT Data 8 [8F:80]
                        AQ_MssEgressLutDataControlRegister_HHD.u8.bits_8.mssEgressLutData_8

                        Default = 0x0000

                        LUT data bits 143:128
                        
  */
      unsigned int   mssEgressLutData_8 : 16;    /* 1E.50A8.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 143:128
                          */
    } bits_8;
    uint16_t word_8;
  } u8;
  /*! \brief Union for bit and word level access of word 9 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50A9.F:0 R/W MSS Egress LUT Data 9 [9F:90]
                        AQ_MssEgressLutDataControlRegister_HHD.u9.bits_9.mssEgressLutData_9

                        Default = 0x0000

                        LUT data bits 159:144
                        
  */
      unsigned int   mssEgressLutData_9 : 16;    /* 1E.50A9.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 159:144
                          */
    } bits_9;
    uint16_t word_9;
  } u9;
  /*! \brief Union for bit and word level access of word 10 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50AA.F:0 R/W MSS Egress LUT Data 10 [AF:A0]
                        AQ_MssEgressLutDataControlRegister_HHD.u10.bits_10.mssEgressLutData_10

                        Default = 0x0000

                        LUT data bits 175:160
                        
  */
      unsigned int   mssEgressLutData_10 : 16;    /* 1E.50AA.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 175:160
                          */
    } bits_10;
    uint16_t word_10;
  } u10;
  /*! \brief Union for bit and word level access of word 11 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50AB.F:0 R/W MSS Egress LUT Data 11 [BF:B0]
                        AQ_MssEgressLutDataControlRegister_HHD.u11.bits_11.mssEgressLutData_11

                        Default = 0x0000

                        LUT data bits 191:176
                        
  */
      unsigned int   mssEgressLutData_11 : 16;    /* 1E.50AB.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 191:176
                          */
    } bits_11;
    uint16_t word_11;
  } u11;
  /*! \brief Union for bit and word level access of word 12 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50AC.F:0 R/W MSS Egress LUT Data 12 [CF:C0]
                        AQ_MssEgressLutDataControlRegister_HHD.u12.bits_12.mssEgressLutData_12

                        Default = 0x0000

                        LUT data bits 207:192
                        
  */
      unsigned int   mssEgressLutData_12 : 16;    /* 1E.50AC.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 207:192
                          */
    } bits_12;
    uint16_t word_12;
  } u12;
  /*! \brief Union for bit and word level access of word 13 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50AD.F:0 R/W MSS Egress LUT Data 13 [DF:D0]
                        AQ_MssEgressLutDataControlRegister_HHD.u13.bits_13.mssEgressLutData_13

                        Default = 0x0000

                        LUT data bits 223:208
                        
  */
      unsigned int   mssEgressLutData_13 : 16;    /* 1E.50AD.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 223:208
                          */
    } bits_13;
    uint16_t word_13;
  } u13;
  /*! \brief Union for bit and word level access of word 14 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50AE.F:0 R/W MSS Egress LUT Data 14 [EF:E0]
                        AQ_MssEgressLutDataControlRegister_HHD.u14.bits_14.mssEgressLutData_14

                        Default = 0x0000

                        LUT data bits 239:224
                        
  */
      unsigned int   mssEgressLutData_14 : 16;    /* 1E.50AE.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 239:224
                          */
    } bits_14;
    uint16_t word_14;
  } u14;
  /*! \brief Union for bit and word level access of word 15 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50AF.F:0 R/W MSS Egress LUT Data 15 [FF:F0]
                        AQ_MssEgressLutDataControlRegister_HHD.u15.bits_15.mssEgressLutData_15

                        Default = 0x0000

                        LUT data bits 255:240
                        
  */
      unsigned int   mssEgressLutData_15 : 16;    /* 1E.50AF.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 255:240
                          */
    } bits_15;
    uint16_t word_15;
  } u15;
  /*! \brief Union for bit and word level access of word 16 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B0.F:0 R/W MSS Egress LUT Data 16 [10F:100]
                        AQ_MssEgressLutDataControlRegister_HHD.u16.bits_16.mssEgressLutData_16

                        Default = 0x0000

                        LUT data bits 271:256
                        
  */
      unsigned int   mssEgressLutData_16 : 16;    /* 1E.50B0.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 271:256
                          */
    } bits_16;
    uint16_t word_16;
  } u16;
  /*! \brief Union for bit and word level access of word 17 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B1.F:0 R/W MSS Egress LUT Data 17 [11F:110]
                        AQ_MssEgressLutDataControlRegister_HHD.u17.bits_17.mssEgressLutData_17

                        Default = 0x0000

                        LUT data bits 287:272
                        
  */
      unsigned int   mssEgressLutData_17 : 16;    /* 1E.50B1.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 287:272
                          */
    } bits_17;
    uint16_t word_17;
  } u17;
  /*! \brief Union for bit and word level access of word 18 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B2.F:0 R/W MSS Egress LUT Data 18 [12F:120]
                        AQ_MssEgressLutDataControlRegister_HHD.u18.bits_18.mssEgressLutData_18

                        Default = 0x0000

                        LUT data bits 303:288
                        
  */
      unsigned int   mssEgressLutData_18 : 16;    /* 1E.50B2.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 303:288
                          */
    } bits_18;
    uint16_t word_18;
  } u18;
  /*! \brief Union for bit and word level access of word 19 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B3.F:0 R/W MSS Egress LUT Data 19 [13F:130]
                        AQ_MssEgressLutDataControlRegister_HHD.u19.bits_19.mssEgressLutData_19

                        Default = 0x0000

                        LUT data bits 319:304
                        
  */
      unsigned int   mssEgressLutData_19 : 16;    /* 1E.50B3.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 319:304
                          */
    } bits_19;
    uint16_t word_19;
  } u19;
  /*! \brief Union for bit and word level access of word 20 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B4.F:0 R/W MSS Egress LUT Data 20 [14F:140]
                        AQ_MssEgressLutDataControlRegister_HHD.u20.bits_20.mssEgressLutData_20

                        Default = 0x0000

                        LUT data bits 335:320
                        
  */
      unsigned int   mssEgressLutData_20 : 16;    /* 1E.50B4.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 335:320
                          */
    } bits_20;
    uint16_t word_20;
  } u20;
  /*! \brief Union for bit and word level access of word 21 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B5.F:0 R/W MSS Egress LUT Data 21 [15F:150]
                        AQ_MssEgressLutDataControlRegister_HHD.u21.bits_21.mssEgressLutData_21

                        Default = 0x0000

                        LUT data bits 351:336
                        
  */
      unsigned int   mssEgressLutData_21 : 16;    /* 1E.50B5.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 351:336
                          */
    } bits_21;
    uint16_t word_21;
  } u21;
  /*! \brief Union for bit and word level access of word 22 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B6.F:0 R/W MSS Egress LUT Data 22 [16F:160]
                        AQ_MssEgressLutDataControlRegister_HHD.u22.bits_22.mssEgressLutData_22

                        Default = 0x0000

                        LUT data bits 367:352
                        
  */
      unsigned int   mssEgressLutData_22 : 16;    /* 1E.50B6.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 367:352
                          */
    } bits_22;
    uint16_t word_22;
  } u22;
  /*! \brief Union for bit and word level access of word 23 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B7.F:0 R/W MSS Egress LUT Data 23 [17F:170]
                        AQ_MssEgressLutDataControlRegister_HHD.u23.bits_23.mssEgressLutData_23

                        Default = 0x0000

                        LUT data bits 383:368
                        
  */
      unsigned int   mssEgressLutData_23 : 16;    /* 1E.50B7.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 383:368
                          */
    } bits_23;
    uint16_t word_23;
  } u23;
  /*! \brief Union for bit and word level access of word 24 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B8.F:0 R/W MSS Egress LUT Data 24 [18F:180]
                        AQ_MssEgressLutDataControlRegister_HHD.u24.bits_24.mssEgressLutData_24

                        Default = 0x0000

                        LUT data bits 399:384
                        
  */
      unsigned int   mssEgressLutData_24 : 16;    /* 1E.50B8.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 399:384
                          */
    } bits_24;
    uint16_t word_24;
  } u24;
  /*! \brief Union for bit and word level access of word 25 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50B9.F:0 R/W MSS Egress LUT Data 25 [19F:190]
                        AQ_MssEgressLutDataControlRegister_HHD.u25.bits_25.mssEgressLutData_25

                        Default = 0x0000

                        LUT data bits 415:400
                        
  */
      unsigned int   mssEgressLutData_25 : 16;    /* 1E.50B9.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 415:400
                          */
    } bits_25;
    uint16_t word_25;
  } u25;
  /*! \brief Union for bit and word level access of word 26 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50BA.F:0 R/W MSS Egress LUT Data 26 [1AF:1A0]
                        AQ_MssEgressLutDataControlRegister_HHD.u26.bits_26.mssEgressLutData_26

                        Default = 0x0000

                        LUT data bits 431:416
                        
  */
      unsigned int   mssEgressLutData_26 : 16;    /* 1E.50BA.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 431:416
                          */
    } bits_26;
    uint16_t word_26;
  } u26;
  /*! \brief Union for bit and word level access of word 27 of MSS Egress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.50BB.F:0 R/W MSS Egress LUT Data 27 [1BF:1B0]
                        AQ_MssEgressLutDataControlRegister_HHD.u27.bits_27.mssEgressLutData_27

                        Default = 0x0000

                        LUT data bits 447:432
                        
  */
      unsigned int   mssEgressLutData_27 : 16;    /* 1E.50BB.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 447:432
                          */
    } bits_27;
    uint16_t word_27;
  } u27;
} AQ_MssEgressLutDataControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System General Control Register: 1E.6004 */
/*                  MSM System General Control Register: 1E.6004 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System General Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6004.0 R/W MSM System Tx Enable
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemTxEnable

                        Default = 0x0

                        1 = Tx enable

                 <B>Notes:</B>
                        MAC Rx path enable. Should be set to 1 to enable the MAC Rx path. Should be set to 0 to disable the MAC Rx path.  */
      unsigned int   msmSystemTxEnable : 1;    /* 1E.6004.0  R/W      Default = 0x0 */
                     /* 1 = Tx enable  */
                    /*! \brief 1E.6004.1 R/W MSM System Rx Enable
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemRxEnable

                        Default = 0x0

                        1 = Rx enable

                 <B>Notes:</B>
                        MAC Tx path enable. Should be set to 1 to enable the MAC Tx path. Should be set to 0 to disable the MAC Tx path.  */
      unsigned int   msmSystemRxEnable : 1;    /* 1E.6004.1  R/W      Default = 0x0 */
                     /* 1 = Rx enable  */
      unsigned int   reserved0 : 1;
                    /*! \brief 1E.6004.3 R/W MSM System WAN Mode
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemWanMode

                        Default = 0x0

                        1 = WAN mode
                        0 = LAN mode
                        

                 <B>Notes:</B>
                        WAN mode enable. Sets WAN mode when set to 1 and LAN mode when set to 0. Note: When changing the mode, verifiy correct setting of the Tx IPG.  */
      unsigned int   msmSystemWanMode : 1;    /* 1E.6004.3  R/W      Default = 0x0 */
                     /* 1 = WAN mode
                        0 = LAN mode
                          */
                    /*! \brief 1E.6004.4 R/W MSM System Promiscuous Mode
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemPromiscuousMode

                        Default = 0x0

                        1 = Promiscuous mode
                        

                 <B>Notes:</B>
                        When set to 1, all frames are received without any MAC address filtering.  */
      unsigned int   msmSystemPromiscuousMode : 1;    /* 1E.6004.4  R/W      Default = 0x0 */
                     /* 1 = Promiscuous mode
                          */
                    /*! \brief 1E.6004.5 R/W MSM System PAD Enable
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemPadEnable

                        Default = 0x0

                        1 = Enable frame padding removal on Rx
                        

                 <B>Notes:</B>
                        When set to 1, enable frame padding removal on the Rx path. If enabled, padding is removed before the frame is transferred to the MAC client application. If disabled, no padding is removed on the Rx by the MAC.
                        Note : On Tx, the MAC always adds padding as required.  */
      unsigned int   msmSystemPadEnable : 1;    /* 1E.6004.5  R/W      Default = 0x0 */
                     /* 1 = Enable frame padding removal on Rx
                          */
                    /*! \brief 1E.6004.6 R/W MSM System CRC Forward
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemCrcForward

                        Default = 0x0

                        1 = Enable CRC forwarding
                        

                 <B>Notes:</B>
                        When set to 1, the CRC field of the received frames is forwarded with the frame to the user application. If disabled, the CRC field is stripped from the frame.
                        Note : If padding is enabled ( See MAC PAD Enable  set to 1), this bit is ignored.  */
      unsigned int   msmSystemCrcForward : 1;    /* 1E.6004.6  R/W      Default = 0x0 */
                     /* 1 = Enable CRC forwarding
                          */
                    /*! \brief 1E.6004.7 R/W MSM System Pause Forward
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemPauseForward

                        Default = 0x0

                        1 = Enable Pause forwarding
                        

                 <B>Notes:</B>
                        Terminate or forward pause frames. If set to 1, pause frames are forwarded to the user application. In normal mode, when set to 0, pause frames are terminated and discarded within the MAC.  */
      unsigned int   msmSystemPauseForward : 1;    /* 1E.6004.7  R/W      Default = 0x0 */
                     /* 1 = Enable Pause forwarding
                          */
                    /*! \brief 1E.6004.8 R/W MSM System Pause Ignore
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemPauseIgnore

                        Default = 0x0

                        1 = Ignore pause frames
                        

                 <B>Notes:</B>
                        Ignore pause frame quanta. If set to 1, received pause frames are ignored by the MAC. When set to 0, the Tx is stopped for the amount of time specified in the pause quanta received within the pause frame.  */
      unsigned int   msmSystemPauseIgnore : 1;    /* 1E.6004.8  R/W      Default = 0x0 */
                     /* 1 = Ignore pause frames
                          */
                    /*! \brief 1E.6004.9 R/W MSM System Tx Address Insert Enable
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemTxAddressInsertEnable

                        Default = 0x0

                        1 = Insert Tx MAC source address
                        

                 <B>Notes:</B>
                        Set the source MAC address on transmit. If set to 1, the MAC overwrites the source MAC address with the MAC programmed address in all transmitted frames. When set to 0, the source MAC address is transmitted unmodified from the MAC Tx client application.  */
      unsigned int   msmSystemTxAddressInsertEnable : 1;    /* 1E.6004.9  R/W      Default = 0x0 */
                     /* 1 = Insert Tx MAC source address
                          */
                    /*! \brief 1E.6004.A R/W MSM System Tx CRC Append
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemTxCrcAppend

                        Default = 0x0

                        1 = Append Tx CRC
                        

                 <B>Notes:</B>
                        Permanently enable CRC append on transmit. If set to 1, the Tx will append a CRC to all transmitted frames. If set to 0, CRC append can be controlled on a per frame basis using the pin ff_tx_crc.
                        This configuration bit is OR'ed with the external ff_tx_crc pin to instruct the Tx to append a CRC to transmitted frames. The ff_tx_crc pin is tied to 0.  */
      unsigned int   msmSystemTxCrcAppend : 1;    /* 1E.6004.A  R/W      Default = 0x0 */
                     /* 1 = Append Tx CRC
                          */
                    /*! \brief 1E.6004.B R/W MSM System Tx Pad Enable
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemTxPadEnable

                        Default = 0x1

                        1 = Enable Tx padding
                        

                 <B>Notes:</B>
                        When set to 1, enable padding of frames in the Tx direction. When set to 0, the MAC will not extend frames from the application to a minimum of 64 bytes, allowing to transmit short frames (violating the Ethernet mimimum size requirements). Must be set to 1 for normal operation.  */
      unsigned int   msmSystemTxPadEnable : 1;    /* 1E.6004.B  R/W      Default = 0x1 */
                     /* 1 = Enable Tx padding
                          */
                    /*! \brief 1E.6004.C R/WSC MSM System Soft Reset
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemSoftReset

                        Default = 0x0

                        1 = Soft reset
                        

                 <B>Notes:</B>
                        Software reset. Self clearing bit. When set to 1, resets all statistic counters as well as the Tx and Rx FIFOs. It should be issued after all traffic has been stopped as a result of clearing the Rx/Tx enable bits ( See MAC Rx Enable  set to 0 and  See MAC Tx Enable  set to 0).
                        Note : Can lead to an Rx interface (ff_rx_xxx) violations to the application if the reset is issued in the middle of a receive frame transfer. Then the end of packet (assertion of ff_rx_eop) is lost and the application should be prepeared to handle this exception.  */
      unsigned int   msmSystemSoftReset : 1;    /* 1E.6004.C  R/WSC      Default = 0x0 */
                     /* 1 = Soft reset
                          */
                    /*! \brief 1E.6004.D R/W MSM System Control Frame Enable
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemControlFrameEnable

                        Default = 0x0

                        1 = Control frame enabled
                        

                 <B>Notes:</B>
                        MAC control frame enable. When set to 1, the MAC control frames with any Opcode other than 0x0001 are accepted and forwarded to the client interface. When set to 0, MAC control frames with any opcode other than 0x0001 are silently discarded.  */
      unsigned int   msmSystemControlFrameEnable : 1;    /* 1E.6004.D  R/W      Default = 0x0 */
                     /* 1 = Control frame enabled
                          */
                    /*! \brief 1E.6004.E R/W MSM System Rx Error Discard
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemRxErrorDiscard

                        Default = 0x0

                        1 = Enable discard of received errored frames
                        

                 <B>Notes:</B>
                        Rx errored frame discard enable. When set to 1, any frame received with an error is discarded and not forwarded to the client interface. When set to 0, errored frames are forwarded to the client interface with ff_rx_err asserted.
                        Note : It is recommended to set this bit to 1 only when store and forward operation is enabled (RX_SECTION_FULL TBD).  */
      unsigned int   msmSystemRxErrorDiscard : 1;    /* 1E.6004.E  R/W      Default = 0x0 */
                     /* 1 = Enable discard of received errored frames
                          */
                    /*! \brief 1E.6004.F R/W MSM System PHY Tx Enable
                        AQ_MsmSystemGeneralControlRegister_HHD.u0.bits_0.msmSystemPhyTxEnable

                        Default = 0x0

                        1 = Enable PHY Tx
                        

                 <B>Notes:</B>
                        Directly controls the phy_tx_ena pin.  */
      unsigned int   msmSystemPhyTxEnable : 1;    /* 1E.6004.F  R/W      Default = 0x0 */
                     /* 1 = Enable PHY Tx
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System General Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6005.0 R/W MSM System Force Send IDLE
                        AQ_MsmSystemGeneralControlRegister_HHD.u1.bits_1.msmSystemForceSendIdle

                        Default = 0x0

                        1 = Force send idle

                 <B>Notes:</B>
                        When set to 1, suppress any frame transmissions and forces IDLE n the Tx interface instead of frames. This control affects the MAC reconciliation layer (RS) which acts after all MAC datapath has processed the frame.
                        Note : Does not have an effect on fault handling (i.e. reception of local fault will still cause transmit of remote fault).
                        Must be 0 for normal operation.  */
      unsigned int   msmSystemForceSendIdle : 1;    /* 1E.6005.0  R/W      Default = 0x0 */
                     /* 1 = Force send idle  */
                    /*! \brief 1E.6005.1 R/W MSM System Length Check Disable
                        AQ_MsmSystemGeneralControlRegister_HHD.u1.bits_1.msmSystemLengthCheckDisable

                        Default = 0x0

                        1 = Disable length check

                 <B>Notes:</B>
                        Payload length check disable. When set to 0, the MAC checks the frames payload length with the frame length/type field. When set to 1, the payload length check is disabled.  */
      unsigned int   msmSystemLengthCheckDisable : 1;    /* 1E.6005.1  R/W      Default = 0x0 */
                     /* 1 = Disable length check  */
                    /*! \brief 1E.6005.2 R/W MSM System IDLE Column Count Extend
                        AQ_MsmSystemGeneralControlRegister_HHD.u1.bits_1.msmSystemIdleColumnCountExtend

                        Default = 0x0

                        1 = Extend IDLE column count

                 <B>Notes:</B>
                        When set to 1, extends the RS layer IDLE column counter by 2x. The IEEE 802.3ae defines the fault condition to be cleared after 128 columns of IDLE have been received. If the MAC operates together with a WAN mode PCS (WIS) it may may happen (depending on PCS) that this period is too short to bridge the IDLE stuffing occurring in this mode, leading to a toggling fault indication. In this case, extending the counter helps to aoivd toggling fault indications.  */
      unsigned int   msmSystemIdleColumnCountExtend : 1;    /* 1E.6005.2  R/W      Default = 0x0 */
                     /* 1 = Extend IDLE column count  */
                    /*! \brief 1E.6005.3 R/W MSM System Priority Flow Control Enable
                        AQ_MsmSystemGeneralControlRegister_HHD.u1.bits_1.msmSystemPriorityFlowControlEnable

                        Default = 0x0

                        1 = Enable priority flow control
                        0 = Enable link flow control
                        

                 <B>Notes:</B>
                        Enable priority flow control (PFC) mode of operation. When set to 0, the MAC uses standard link pause frames. When set to 1, the MAC will transmit and accept PFC frames.  */
      unsigned int   msmSystemPriorityFlowControlEnable : 1;    /* 1E.6005.3  R/W      Default = 0x0 */
                     /* 1 = Enable priority flow control
                        0 = Enable link flow control
                          */
      unsigned int   reserved2 : 1;
                    /*! \brief 1E.6005.5 R/W MSM System SFD Check Disable
                        AQ_MsmSystemGeneralControlRegister_HHD.u1.bits_1.msmSystemSfdCheckDisable

                        Default = 0x0

                        1 = Disable SFD check
                        

                 <B>Notes:</B>
                        Disable check of SFD (0xD5) character at frame start. When set to 1, the frame is accepted even if the SFD byte following the preamble is not 0xD5. When set to 0, a frame is accepted only if the SFD byte is found with the value 0xD5.  */
      unsigned int   msmSystemSfdCheckDisable : 1;    /* 1E.6005.5  R/W      Default = 0x0 */
                     /* 1 = Disable SFD check
                          */
      unsigned int   reserved1 : 1;
                    /*! \brief 1E.6005.7 R/W MSM System Tx Low Power IDLE Enable
                        AQ_MsmSystemGeneralControlRegister_HHD.u1.bits_1.msmSystemTxLowPowerIdleEnable

                        Default = 0x0

                        1 = Transmit LPI enable
                        

                 <B>Notes:</B>
                        Transmit low power IDLE enable. When set to 1, the MAC completes the transmission of the current frame and generates low power IDLE sequences (LPI) to the XGMII/SGMII. When set to 0, the MAC operates in normal mode. This bit is OR'ed with the reg_lowp_ena pin.  */
      unsigned int   msmSystemTxLowPowerIdleEnable : 1;    /* 1E.6005.7  R/W      Default = 0x0 */
                     /* 1 = Transmit LPI enable
                          */
      unsigned int   reserved0 : 8;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemGeneralControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System FIFO Control Register: 1E.600E */
/*                  MSM System FIFO Control Register: 1E.600E */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.600E.7:0 R/W MSM System Rx FIFO Full Threshold [7:0]
                        AQ_MsmSystemFifoControlRegister_HHD.u0.bits_0.msmSystemRxFifoFullThreshold

                        Default = 0x08

                        Rx FIFO full threshold

                 <B>Notes:</B>
                        All threshold values are in steps of FIFO words.  */
      unsigned int   msmSystemRxFifoFullThreshold : 8;    /* 1E.600E.7:0  R/W      Default = 0x08 */
                     /* Rx FIFO full threshold  */
      unsigned int   reserved0 : 8;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.600F.7:0 R/W MSM System Rx FIFO Empty Threshold [7:0]
                        AQ_MsmSystemFifoControlRegister_HHD.u1.bits_1.msmSystemRxFifoEmptyThreshold

                        Default = 0x00

                        Rx FIFO empty threshold

                 <B>Notes:</B>
                        All threshold values are in steps of FIFO words.  */
      unsigned int   msmSystemRxFifoEmptyThreshold : 8;    /* 1E.600F.7:0  R/W      Default = 0x00 */
                     /* Rx FIFO empty threshold  */
      unsigned int   reserved0 : 8;
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of MSM System FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6010.5:0 R/W MSM System Tx FIFO Full Threshold [5:0]
                        AQ_MsmSystemFifoControlRegister_HHD.u2.bits_2.msmSystemTxFifoFullThreshold

                        Default = 0x08

                        Tx FIFO full threshold

                 <B>Notes:</B>
                        All threshold values are in steps of FIFO words.  */
      unsigned int   msmSystemTxFifoFullThreshold : 6;    /* 1E.6010.5:0  R/W      Default = 0x08 */
                     /* Tx FIFO full threshold  */
      unsigned int   reserved0 : 10;
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of MSM System FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6011.5:0 R/W MSM System Tx FIFO Empty Threshold [5:0]
                        AQ_MsmSystemFifoControlRegister_HHD.u3.bits_3.msmSystemTxFifoEmptyThreshold

                        Default = 0x00

                        Tx FIFO empty threshold

                 <B>Notes:</B>
                        All threshold values are in steps of FIFO words.  */
      unsigned int   msmSystemTxFifoEmptyThreshold : 6;    /* 1E.6011.5:0  R/W      Default = 0x00 */
                     /* Tx FIFO empty threshold  */
      unsigned int   reserved0 : 10;
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of MSM System FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6012.7:0 ROS MSM System Rx FIFO Almost Full Threshold [7:0]
                        AQ_MsmSystemFifoControlRegister_HHD.u4.bits_4.msmSystemRxFifoAlmostFullThreshold

                        Default = 0x00

                        Rx FIFO almost full threshold

                 <B>Notes:</B>
                        Unused.  */
      unsigned int   msmSystemRxFifoAlmostFullThreshold : 8;    /* 1E.6012.7:0  ROS      Default = 0x00 */
                     /* Rx FIFO almost full threshold  */
      unsigned int   reserved0 : 8;
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of MSM System FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6013.7:0 ROS MSM System Rx FIFO Almost Empty Threshold [7:0]
                        AQ_MsmSystemFifoControlRegister_HHD.u5.bits_5.msmSystemRxFifoAlmostEmptyThreshold

                        Default = 0x00

                        Rx FIFO almost empty threshold

                 <B>Notes:</B>
                        Unused.  */
      unsigned int   msmSystemRxFifoAlmostEmptyThreshold : 8;    /* 1E.6013.7:0  ROS      Default = 0x00 */
                     /* Rx FIFO almost empty threshold  */
      unsigned int   reserved0 : 8;
    } bits_5;
    uint16_t word_5;
  } u5;
  /*! \brief Union for bit and word level access of word 6 of MSM System FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6014.7:0 ROS MSM System Tx FIFO Almost Full Threshold [7:0]
                        AQ_MsmSystemFifoControlRegister_HHD.u6.bits_6.msmSystemTxFifoAlmostFullThreshold

                        Default = 0x00

                        Tx FIFO almost full threshold

                 <B>Notes:</B>
                        Unused.  */
      unsigned int   msmSystemTxFifoAlmostFullThreshold : 8;    /* 1E.6014.7:0  ROS      Default = 0x00 */
                     /* Tx FIFO almost full threshold  */
      unsigned int   reserved0 : 8;
    } bits_6;
    uint16_t word_6;
  } u6;
  /*! \brief Union for bit and word level access of word 7 of MSM System FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6015.7:0 ROS MSM System Tx FIFO Almost Empty Threshold [7:0]
                        AQ_MsmSystemFifoControlRegister_HHD.u7.bits_7.msmSystemTxFifoAlmostEmptyThreshold

                        Default = 0x00

                        Tx FIFO almost empty threshold

                 <B>Notes:</B>
                        Unused.  */
      unsigned int   msmSystemTxFifoAlmostEmptyThreshold : 8;    /* 1E.6015.7:0  ROS      Default = 0x00 */
                     /* Tx FIFO almost empty threshold  */
      unsigned int   reserved0 : 8;
    } bits_7;
    uint16_t word_7;
  } u7;
} AQ_MsmSystemFifoControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System General Status Register: 1E.6020 */
/*                  MSM System General Status Register: 1E.6020 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System General Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6020.0 BLH MSM System Rx Local Fault
                        AQ_MsmSystemGeneralStatusRegister_HHD.u0.bits_0.msmSystemRxLocalFault

                        

                        Rx local fault detected

                 <B>Notes:</B>
                        Latch high local fault status. Set to 1, whent he MAC detects Rx local fault sequences on the Rx interface. Reset to 0 after read and after reset.  */
      unsigned int   msmSystemRxLocalFault : 1;    /* 1E.6020.0  BLH       */
                     /* Rx local fault detected  */
                    /*! \brief 1E.6020.1 BLH MSM System Rx Remote Fault
                        AQ_MsmSystemGeneralStatusRegister_HHD.u0.bits_0.msmSystemRxRemoteFault

                        

                        Rx remote fault detected

                 <B>Notes:</B>
                        Latch high local fault status. Set to 1, whent he MAC detects Rx local fault sequences on the Rx interface. Reset to 0 after read and after reset.  */
      unsigned int   msmSystemRxRemoteFault : 1;    /* 1E.6020.1  BLH       */
                     /* Rx remote fault detected  */
                    /*! \brief 1E.6020.2 RO MSM System PHY Loss of Signal
                        AQ_MsmSystemGeneralStatusRegister_HHD.u0.bits_0.msmSystemPhyLossOfSignal

                        

                        PHY loss of signal

                 <B>Notes:</B>
                        PHY indicates loss of signal. This is the value of pin phy_los which is tied to 0.  */
      unsigned int   msmSystemPhyLossOfSignal : 1;    /* 1E.6020.2  RO       */
                     /* PHY loss of signal  */
                    /*! \brief 1E.6020.3 R/W MSM System Timestamp Available
                        AQ_MsmSystemGeneralStatusRegister_HHD.u0.bits_0.msmSystemTimestampAvailable

                        Default = 0x0

                        Timestamp available

                 <B>Notes:</B>
                        Transmit timestamp available. Indicates that the timestamp of the last transmitted event frame (which had ff_tx_ts_frm=1) is available in the register  See MAC Time Stamp Status 0 [F:0]  and  See MAC Time Stamp Status 1 [F:0] . To clear this bit, the bit must be written with a 1.
                          */
      unsigned int   msmSystemTimestampAvailable : 1;    /* 1E.6020.3  R/W      Default = 0x0 */
                     /* Timestamp available  */
                    /*! \brief 1E.6020.4 RO MSM System Rx Low Power IDLE
                        AQ_MsmSystemGeneralStatusRegister_HHD.u0.bits_0.msmSystemRxLowPowerIdle

                        

                        Rx LPI detected

                 <B>Notes:</B>
                        Receive low power IDLE (LPI). Set to 1 when LPI is currently detected on the MAC Rx interface. Set to 0, when the MAC currently operates in normal mode.  */
      unsigned int   msmSystemRxLowPowerIdle : 1;    /* 1E.6020.4  RO       */
                     /* Rx LPI detected  */
                    /*! \brief 1E.6020.5 RO MSM System Tx FIFO Empty
                        AQ_MsmSystemGeneralStatusRegister_HHD.u0.bits_0.msmSystemTxFifoEmpty

                        

                        Tx FIFO empty

                 <B>Notes:</B>
                        When set to 1, indicates the Tx FIFO is empty. When set to 0, Tx FIFO is non-empty.  */
      unsigned int   msmSystemTxFifoEmpty : 1;    /* 1E.6020.5  RO       */
                     /* Tx FIFO empty  */
      unsigned int   reserved0 : 10;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System General Status Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemGeneralStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Tx IPG Control Register: 1E.6022 */
/*                  MSM System Tx IPG Control Register: 1E.6022 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Tx IPG Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6022.5:0 R/W MSM System Tx IPG Length [5:0]
                        AQ_MsmSystemTxIpgControlRegister_HHD.u0.bits_0.msmSystemTxIpgLength

                        Default = 0x0C

                        Tx IPG length

                 <B>Notes:</B>
                        Tx inter-packet gap (IPG) value. Depending on LAN or WAN mode of operation.
                        LAN Mode : Number of octets in steps of 4. Valid values are 8, 12, 16,..., 100. DIC is supported for any setting > 8. A default of 12 must be set to conform to IEEE802.3ae.
                        WAN Mode : Stretch factor. Valid values are 4 ... 15. The stretch factor is calculated as (value+1)*8. A default of 12 must be set to conform to IEEE802.3ae (i.e. 13*8=104). A larger value shrinks the IPG (increasing bandwidth).
                        The reset value of 12 leads to IEEE802.3ae conformant behavior in both modes.
                        Note : WAN mode is only available in 10G mode of operation.  */
      unsigned int   msmSystemTxIpgLength : 6;    /* 1E.6022.5:0  R/W      Default = 0x0C */
                     /* Tx IPG length  */
      unsigned int   reserved0 : 10;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Tx IPG Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6023.F:0  MSM System Tx IPG Reserved
                        AQ_MsmSystemTxIpgControlRegister_HHD.u1.bits_1.msmSystemTxIpgReserved

                        

                        Value always 0, writes ignored
  */
      unsigned int   msmSystemTxIpgReserved : 16;    /* 1E.6023.F:0         */
                     /* Value always 0, writes ignored  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemTxIpgControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Tx Good Frames Counter Register: 1E.6040 */
/*                  MSM System Tx Good Frames Counter Register: 1E.6040 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Tx Good Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6040.F:0 ROS MSM System Tx Good Frames Counter 0 [F:0]
                        AQ_MsmSystemTxGoodFramesCounterRegister_HHD.u0.bits_0.msmSystemTxGoodFramesCounter_0

                        Default = 0x0000

                        Tx good frame counter bits 15:0

                 <B>Notes:</B>
                        Count of frames transmitted without error (Including pause frames).  */
      unsigned int   msmSystemTxGoodFramesCounter_0 : 16;    /* 1E.6040.F:0  ROS      Default = 0x0000 */
                     /* Tx good frame counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Tx Good Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6041.F:0 ROS MSM System Tx Good Frames Counter 1 [F:0]
                        AQ_MsmSystemTxGoodFramesCounterRegister_HHD.u1.bits_1.msmSystemTxGoodFramesCounter_1

                        Default = 0x0000

                        Tx good frame counter bits 31:16
                        

                 <B>Notes:</B>
                        Count of frames transmitted without error (Including pause frames).  */
      unsigned int   msmSystemTxGoodFramesCounter_1 : 16;    /* 1E.6041.F:0  ROS      Default = 0x0000 */
                     /* Tx good frame counter bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemTxGoodFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx Good Frames Counter Register: 1E.6044 */
/*                  MSM System Rx Good Frames Counter Register: 1E.6044 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx Good Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6044.F:0 ROS MSM System Rx Good Frames Counter 0 [F:0]
                        AQ_MsmSystemRxGoodFramesCounterRegister_HHD.u0.bits_0.msmSystemRxGoodFramesCounter_0

                        Default = 0x0000

                        Rx good frame counter bits 15:0

                 <B>Notes:</B>
                        Count of frames received without error (Including pause frames).  */
      unsigned int   msmSystemRxGoodFramesCounter_0 : 16;    /* 1E.6044.F:0  ROS      Default = 0x0000 */
                     /* Rx good frame counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx Good Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6045.F:0 ROS MSM System Rx Good Frames Counter 1 [F:0]
                        AQ_MsmSystemRxGoodFramesCounterRegister_HHD.u1.bits_1.msmSystemRxGoodFramesCounter_1

                        Default = 0x0000

                        Rx good frame counter bits 31:16

                 <B>Notes:</B>
                        Count of frames received without error (Including pause frames).  */
      unsigned int   msmSystemRxGoodFramesCounter_1 : 16;    /* 1E.6045.F:0  ROS      Default = 0x0000 */
                     /* Rx good frame counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxGoodFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx FCS Errors Counter Register: 1E.6048 */
/*                  MSM System Rx FCS Errors Counter Register: 1E.6048 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx FCS Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6048.F:0 ROS MSM System FCS Error Counter 0 [F:0]
                        AQ_MsmSystemRxFcsErrorsCounterRegister_HHD.u0.bits_0.msmSystemFcsErrorCounter_0

                        Default = 0x0000

                        Frame check sequence error counter bits 15:0

                 <B>Notes:</B>
                        Count of frames for which a CRC-32 Error is detected but the frame is otherwise of correct length.  */
      unsigned int   msmSystemFcsErrorCounter_0 : 16;    /* 1E.6048.F:0  ROS      Default = 0x0000 */
                     /* Frame check sequence error counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx FCS Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6049.F:0 ROS MSM System FCS Error Counter 1 [F:0]
                        AQ_MsmSystemRxFcsErrorsCounterRegister_HHD.u1.bits_1.msmSystemFcsErrorCounter_1

                        Default = 0x0000

                        Frame check sequence error counter bits 31:16

                 <B>Notes:</B>
                        Count of frames for which a CRC-32 Error is detected but the frame is otherwise of correct length.  */
      unsigned int   msmSystemFcsErrorCounter_1 : 16;    /* 1E.6049.F:0  ROS      Default = 0x0000 */
                     /* Frame check sequence error counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxFcsErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx Alignment Errors Counter Register: 1E.604C */
/*                  MSM System Rx Alignment Errors Counter Register: 1E.604C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx Alignment Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.604C.F:0 ROS MSM System Alignment Error Counter 0 [F:0]
                        AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD.u0.bits_0.msmSystemAlignmentErrorCounter_0

                        Default = 0x0000

                        Alignment error counter bits 15:0

                 <B>Notes:</B>
                        Count of frames received with an alignment error.  */
      unsigned int   msmSystemAlignmentErrorCounter_0 : 16;    /* 1E.604C.F:0  ROS      Default = 0x0000 */
                     /* Alignment error counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx Alignment Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.604D.F:0 ROS MSM System Alignment Error Counter 1 [F:0]
                        AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD.u1.bits_1.msmSystemAlignmentErrorCounter_1

                        Default = 0x0000

                        Alignment error counter bits 31:16

                 <B>Notes:</B>
                        Count of frames received with an alignment error.  */
      unsigned int   msmSystemAlignmentErrorCounter_1 : 16;    /* 1E.604D.F:0  ROS      Default = 0x0000 */
                     /* Alignment error counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxAlignmentErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Tx Pause Frames Counter Register: 1E.6050 */
/*                  MSM System Tx Pause Frames Counter Register: 1E.6050 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Tx Pause Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6050.F:0 ROS MSM System Tx Pause Frames Counter 0 [F:0]
                        AQ_MsmSystemTxPauseFramesCounterRegister_HHD.u0.bits_0.msmSystemTxPauseFramesCounter_0

                        Default = 0x0000

                        Tx pause frame counter bits 15:0

                 <B>Notes:</B>
                        Valid pause frames transmitted.  */
      unsigned int   msmSystemTxPauseFramesCounter_0 : 16;    /* 1E.6050.F:0  ROS      Default = 0x0000 */
                     /* Tx pause frame counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Tx Pause Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6051.F:0 ROS MSM System Tx Pause Frames Counter 1 [F:0]
                        AQ_MsmSystemTxPauseFramesCounterRegister_HHD.u1.bits_1.msmSystemTxPauseFramesCounter_1

                        Default = 0x0000

                        Tx pause frame counter bits 31:16
                        

                 <B>Notes:</B>
                        Valid pause frames transmitted.  */
      unsigned int   msmSystemTxPauseFramesCounter_1 : 16;    /* 1E.6051.F:0  ROS      Default = 0x0000 */
                     /* Tx pause frame counter bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemTxPauseFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx Pause Frames Counter Register: 1E.6054 */
/*                  MSM System Rx Pause Frames Counter Register: 1E.6054 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx Pause Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6054.F:0 ROS MSM System Rx Pause Frames Counter 0 [F:0]
                        AQ_MsmSystemRxPauseFramesCounterRegister_HHD.u0.bits_0.msmSystemRxPauseFramesCounter_0

                        Default = 0x0000

                        Rx pause frame counter bits 15:0

                 <B>Notes:</B>
                        Valid pause frames received.  */
      unsigned int   msmSystemRxPauseFramesCounter_0 : 16;    /* 1E.6054.F:0  ROS      Default = 0x0000 */
                     /* Rx pause frame counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx Pause Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6055.F:0 ROS MSM System Rx Pause Frames Counter 1 [F:0]
                        AQ_MsmSystemRxPauseFramesCounterRegister_HHD.u1.bits_1.msmSystemRxPauseFramesCounter_1

                        Default = 0x0000

                        Rx pause frame counter bits 31:16

                 <B>Notes:</B>
                        Valid pause frames received.  */
      unsigned int   msmSystemRxPauseFramesCounter_1 : 16;    /* 1E.6055.F:0  ROS      Default = 0x0000 */
                     /* Rx pause frame counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxPauseFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx Too Long Errors Counter Register: 1E.6058 */
/*                  MSM System Rx Too Long Errors Counter Register: 1E.6058 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx Too Long Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6058.F:0 ROS MSM System Rx Too Long Errors Counter 0 [F:0]
                        AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD.u0.bits_0.msmSystemRxTooLongErrorsCounter_0

                        Default = 0x0000

                        Too-long errors counter bits 15:0

                 <B>Notes:</B>
                        Frame received exceeded the maximum length programmed with register FRM_LGTH.  */
      unsigned int   msmSystemRxTooLongErrorsCounter_0 : 16;    /* 1E.6058.F:0  ROS      Default = 0x0000 */
                     /* Too-long errors counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx Too Long Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6059.F:0 ROS MSM System Rx Too Long Errors Counter 1 [F:0]
                        AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD.u1.bits_1.msmSystemRxTooLongErrorsCounter_1

                        Default = 0x0000

                        Too-long errors counter bits 31:16

                 <B>Notes:</B>
                        Frame received exceeded the maximum length programmed with register FRM_LGTH.  */
      unsigned int   msmSystemRxTooLongErrorsCounter_1 : 16;    /* 1E.6059.F:0  ROS      Default = 0x0000 */
                     /* Too-long errors counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxTooLongErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx In Range Length Errors Counter Register: 1E.605C */
/*                  MSM System Rx In Range Length Errors Counter Register: 1E.605C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx In Range Length Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.605C.F:0 ROS MSM System Rx In Range Length Errors Counter 0 [F:0]
                        AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD.u0.bits_0.msmSystemRxInRangeLengthErrorsCounter_0

                        Default = 0x0000

                        In-range-length errors counter bits 15:0

                 <B>Notes:</B>
                        A count of frames with a length/type field value between 46 (VLAN: 42) and less than 0x0600, that does not match the number of payload data octets received. Should count also if length/type field is less than 46 (VLAN: 42) and the frame is longer than 64 bytes.  */
      unsigned int   msmSystemRxInRangeLengthErrorsCounter_0 : 16;    /* 1E.605C.F:0  ROS      Default = 0x0000 */
                     /* In-range-length errors counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx In Range Length Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.605D.F:0 ROS MSM System Rx In Range Length Errors Counter 1 [F:0]
                        AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD.u1.bits_1.msmSystemRxInRangeLengthErrorsCounter_1

                        Default = 0x0000

                        In-range-length errors counter bits 31:16

                 <B>Notes:</B>
                        A count of frames with a length/type field value between 46 (VLAN: 42) and less than 0x0600, that does not match the number of payload data octets received. Should count also if length/type field is less than 46 (VLAN: 42) and the frame is longer than 64 bytes.  */
      unsigned int   msmSystemRxInRangeLengthErrorsCounter_1 : 16;    /* 1E.605D.F:0  ROS      Default = 0x0000 */
                     /* In-range-length errors counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxInRangeLengthErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Tx VLAN Frames Counter Register: 1E.6060 */
/*                  MSM System Tx VLAN Frames Counter Register: 1E.6060 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Tx VLAN Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6060.F:0 ROS MSM System Tx VLAN Frames Counter 0 [F:0]
                        AQ_MsmSystemTxVlanFramesCounterRegister_HHD.u0.bits_0.msmSystemTxVlanFramesCounter_0

                        Default = 0x0000

                        Tx VLAN frames counter bits 15:0

                 <B>Notes:</B>
                        Valid VLAN tagged frames transmitted.  */
      unsigned int   msmSystemTxVlanFramesCounter_0 : 16;    /* 1E.6060.F:0  ROS      Default = 0x0000 */
                     /* Tx VLAN frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Tx VLAN Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6061.F:0 ROS MSM System Tx VLAN Frames Counter 1 [F:0]
                        AQ_MsmSystemTxVlanFramesCounterRegister_HHD.u1.bits_1.msmSystemTxVlanFramesCounter_1

                        Default = 0x0000

                        Tx VLAN frames counter bits 31:16

                 <B>Notes:</B>
                        Valid VLAN tagged frames transmitted.  */
      unsigned int   msmSystemTxVlanFramesCounter_1 : 16;    /* 1E.6061.F:0  ROS      Default = 0x0000 */
                     /* Tx VLAN frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemTxVlanFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx VLAN Frames Counter Register: 1E.6064 */
/*                  MSM System Rx VLAN Frames Counter Register: 1E.6064 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx VLAN Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6064.F:0 ROS MSM System Rx VLAN Frames Counter 0 [F:0]
                        AQ_MsmSystemRxVlanFramesCounterRegister_HHD.u0.bits_0.msmSystemRxVlanFramesCounter_0

                        Default = 0x0000

                        Rx VLAN frames counter bits 15:0

                 <B>Notes:</B>
                        Valid VLAN tagged frames received.  */
      unsigned int   msmSystemRxVlanFramesCounter_0 : 16;    /* 1E.6064.F:0  ROS      Default = 0x0000 */
                     /* Rx VLAN frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx VLAN Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6065.F:0 ROS MSM System Rx VLAN Frames Counter 1 [F:0]
                        AQ_MsmSystemRxVlanFramesCounterRegister_HHD.u1.bits_1.msmSystemRxVlanFramesCounter_1

                        Default = 0x0000

                        Rx VLAN frames counter bits 31:16

                 <B>Notes:</B>
                        Valid VLAN tagged frames received.  */
      unsigned int   msmSystemRxVlanFramesCounter_1 : 16;    /* 1E.6065.F:0  ROS      Default = 0x0000 */
                     /* Rx VLAN frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxVlanFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Tx Octets Counter Register: 1E.6068 */
/*                  MSM System Tx Octets Counter Register: 1E.6068 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Tx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6068.F:0 ROS MSM System Tx Octets Counter 0 [F:0]
                        AQ_MsmSystemTxOctetsCounterRegister_HHD.u0.bits_0.msmSystemTxOctetsCounter_0

                        Default = 0x0000

                        Tx octets counter bits 15:0

                 <B>Notes:</B>
                        All octets transmitted except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames transmitted.  */
      unsigned int   msmSystemTxOctetsCounter_0 : 16;    /* 1E.6068.F:0  ROS      Default = 0x0000 */
                     /* Tx octets counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Tx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6069.F:0 ROS MSM System Tx Octets Counter 1 [F:0]
                        AQ_MsmSystemTxOctetsCounterRegister_HHD.u1.bits_1.msmSystemTxOctetsCounter_1

                        Default = 0x0000

                        Tx octets counter bits 31:16

                 <B>Notes:</B>
                        All octets transmitted except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames transmitted.  */
      unsigned int   msmSystemTxOctetsCounter_1 : 16;    /* 1E.6069.F:0  ROS      Default = 0x0000 */
                     /* Tx octets counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of MSM System Tx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.606A.F:0 ROS MSM System Tx Octets Counter 2 [F:0]
                        AQ_MsmSystemTxOctetsCounterRegister_HHD.u2.bits_2.msmSystemTxOctetsCounter_2

                        Default = 0x0000

                        Tx octets counter bits 47:32

                 <B>Notes:</B>
                        All octets transmitted except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames transmitted.  */
      unsigned int   msmSystemTxOctetsCounter_2 : 16;    /* 1E.606A.F:0  ROS      Default = 0x0000 */
                     /* Tx octets counter bits 47:32  */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of MSM System Tx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.606B.F:0 ROS MSM System Tx Octets Counter 3 [F:0]
                        AQ_MsmSystemTxOctetsCounterRegister_HHD.u3.bits_3.msmSystemTxOctetsCounter_3

                        Default = 0x0000

                        Tx octets counter bits 63:48

                 <B>Notes:</B>
                        All octets transmitted except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames transmitted.  */
      unsigned int   msmSystemTxOctetsCounter_3 : 16;    /* 1E.606B.F:0  ROS      Default = 0x0000 */
                     /* Tx octets counter bits 63:48  */
    } bits_3;
    uint16_t word_3;
  } u3;
} AQ_MsmSystemTxOctetsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx Octets Counter Register: 1E.606C */
/*                  MSM System Rx Octets Counter Register: 1E.606C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.606C.F:0 ROS MSM System Rx Octets Counter 0 [F:0]
                        AQ_MsmSystemRxOctetsCounterRegister_HHD.u0.bits_0.msmSystemRxOctetsCounter_0

                        Default = 0x0000

                        Rx octets counter bits 15:0

                 <B>Notes:</B>
                        All octets received except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames received.  */
      unsigned int   msmSystemRxOctetsCounter_0 : 16;    /* 1E.606C.F:0  ROS      Default = 0x0000 */
                     /* Rx octets counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.606D.F:0 ROS MSM System Rx Octets Counter 1 [F:0]
                        AQ_MsmSystemRxOctetsCounterRegister_HHD.u1.bits_1.msmSystemRxOctetsCounter_1

                        Default = 0x0000

                        Rx octets counter bits 31:16

                 <B>Notes:</B>
                        All octets received except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames received.  */
      unsigned int   msmSystemRxOctetsCounter_1 : 16;    /* 1E.606D.F:0  ROS      Default = 0x0000 */
                     /* Rx octets counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxOctetsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx Unicast Frames Counter Register: 1E.6070 */
/*                  MSM System Rx Unicast Frames Counter Register: 1E.6070 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx Unicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6070.F:0 ROS MSM System Rx Unicast Frames Counter 0 [F:0]
                        AQ_MsmSystemRxUnicastFramesCounterRegister_HHD.u0.bits_0.msmSystemRxUnicastFramesCounter_0

                        Default = 0x0000

                        Rx unicast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface and bit 0 of the destination address was '0'.  */
      unsigned int   msmSystemRxUnicastFramesCounter_0 : 16;    /* 1E.6070.F:0  ROS      Default = 0x0000 */
                     /* Rx unicast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx Unicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6071.F:0 ROS MSM System Rx Unicast Frames Counter 1 [F:0]
                        AQ_MsmSystemRxUnicastFramesCounterRegister_HHD.u1.bits_1.msmSystemRxUnicastFramesCounter_1

                        Default = 0x0000

                        Rx unicast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface and bit 0 of the destination address was '0'.  */
      unsigned int   msmSystemRxUnicastFramesCounter_1 : 16;    /* 1E.6071.F:0  ROS      Default = 0x0000 */
                     /* Rx unicast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxUnicastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx Multicast Frames Counter Register: 1E.6074 */
/*                  MSM System Rx Multicast Frames Counter Register: 1E.6074 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx Multicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6074.F:0 ROS MSM System Rx Multicast Frames Counter 0 [F:0]
                        AQ_MsmSystemRxMulticastFramesCounterRegister_HHD.u0.bits_0.msmSystemRxMulticastFramesCounter_0

                        Default = 0x0000

                        Rx multicast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface and bit 0 of the destination address was '1' but not the broadcast address (all bits set '1' ). Pause frames are not counted.  */
      unsigned int   msmSystemRxMulticastFramesCounter_0 : 16;    /* 1E.6074.F:0  ROS      Default = 0x0000 */
                     /* Rx multicast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx Multicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6075.F:0 ROS MSM System Rx Multicast Frames Counter 1 [F:0]
                        AQ_MsmSystemRxMulticastFramesCounterRegister_HHD.u1.bits_1.msmSystemRxMulticastFramesCounter_1

                        Default = 0x0000

                        Rx multicast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface and bit 0 of the destination address was '1' but not the broadcast address (all bits set '1' ). Pause frames are not counted.  */
      unsigned int   msmSystemRxMulticastFramesCounter_1 : 16;    /* 1E.6075.F:0  ROS      Default = 0x0000 */
                     /* Rx multicast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxMulticastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx Broadcast Frames Counter Register: 1E.6078 */
/*                  MSM System Rx Broadcast Frames Counter Register: 1E.6078 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx Broadcast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6078.F:0 ROS MSM System Rx Broadcast Frames Counter 0 [F:0]
                        AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD.u0.bits_0.msmSystemRxBroadcastFramesCounter_0

                        Default = 0x0000

                        Rx broadcast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface (FIFO) and all bits of the destination address were set '1'.  */
      unsigned int   msmSystemRxBroadcastFramesCounter_0 : 16;    /* 1E.6078.F:0  ROS      Default = 0x0000 */
                     /* Rx broadcast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx Broadcast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6079.F:0 ROS MSM System Rx Broadcast Frames Counter 1 [F:0]
                        AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD.u1.bits_1.msmSystemRxBroadcastFramesCounter_1

                        Default = 0x0000

                        Rx broadcast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface (FIFO) and all bits of the destination address were set '1'.  */
      unsigned int   msmSystemRxBroadcastFramesCounter_1 : 16;    /* 1E.6079.F:0  ROS      Default = 0x0000 */
                     /* Rx broadcast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxBroadcastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Tx Errors Counter Register: 1E.607C */
/*                  MSM System Tx Errors Counter Register: 1E.607C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Tx Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.607C.F:0 ROS MSM System Tx Errors Counter 0 [F:0]
                        AQ_MsmSystemTxErrorsCounterRegister_HHD.u0.bits_0.msmSystemTxErrorsCounter_0

                        Default = 0x0000

                        Rx errors counter bits 15:0

                 <B>Notes:</B>
                        Number of frames transmitted with error: 
                        - FIFO Overflow Errors
                        - FIFO Underflow Errors  */
      unsigned int   msmSystemTxErrorsCounter_0 : 16;    /* 1E.607C.F:0  ROS      Default = 0x0000 */
                     /* Rx errors counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Tx Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.607D.F:0 ROS MSM System Tx Errors Counter 1 [F:0]
                        AQ_MsmSystemTxErrorsCounterRegister_HHD.u1.bits_1.msmSystemTxErrorsCounter_1

                        Default = 0x0000

                        Tx errors counter bits 31:16

                 <B>Notes:</B>
                        Number of frames transmitted with error: 
                        - FIFO Overflow Errors
                        - FIFO Underflow Errors  */
      unsigned int   msmSystemTxErrorsCounter_1 : 16;    /* 1E.607D.F:0  ROS      Default = 0x0000 */
                     /* Tx errors counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemTxErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Tx Unicast Frames Counter Register: 1E.6084 */
/*                  MSM System Tx Unicast Frames Counter Register: 1E.6084 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Tx Unicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6084.F:0 ROS MSM System Tx Unicast Frames Counter 0 [F:0]
                        AQ_MsmSystemTxUnicastFramesCounterRegister_HHD.u0.bits_0.msmSystemTxUnicastFramesCounter_0

                        Default = 0x0000

                        Tx unicast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and bit 0 of the destination address set to '0'.  */
      unsigned int   msmSystemTxUnicastFramesCounter_0 : 16;    /* 1E.6084.F:0  ROS      Default = 0x0000 */
                     /* Tx unicast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Tx Unicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6085.F:0 ROS MSM System Tx Unicast Frames Counter 1 [F:0]
                        AQ_MsmSystemTxUnicastFramesCounterRegister_HHD.u1.bits_1.msmSystemTxUnicastFramesCounter_1

                        Default = 0x0000

                        Tx unicast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and bit 0 of the destination address set to '0'.  */
      unsigned int   msmSystemTxUnicastFramesCounter_1 : 16;    /* 1E.6085.F:0  ROS      Default = 0x0000 */
                     /* Tx unicast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemTxUnicastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Tx Multicast Frames Counter Register: 1E.6088 */
/*                  MSM System Tx Multicast Frames Counter Register: 1E.6088 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Tx Multicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6088.F:0 ROS MSM System Tx Multicast Frames Counter 0 [F:0]
                        AQ_MsmSystemTxMulticastFramesCounterRegister_HHD.u0.bits_0.msmSystemTxMulticastFramesCounter_0

                        Default = 0x0000

                        Tx multicast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and bit 0 of the destination address set to '1' but not the broadcast address (all bits '1').  */
      unsigned int   msmSystemTxMulticastFramesCounter_0 : 16;    /* 1E.6088.F:0  ROS      Default = 0x0000 */
                     /* Tx multicast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Tx Multicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.6089.F:0 ROS MSM System Tx Multicast Frames Counter 1 [F:0]
                        AQ_MsmSystemTxMulticastFramesCounterRegister_HHD.u1.bits_1.msmSystemTxMulticastFramesCounter_1

                        Default = 0x0000

                        Tx multicast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and bit 0 of the destination address set to '1' but not the broadcast address (all bits '1').  */
      unsigned int   msmSystemTxMulticastFramesCounter_1 : 16;    /* 1E.6089.F:0  ROS      Default = 0x0000 */
                     /* Tx multicast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemTxMulticastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Tx Broadcast Frames Counter Register: 1E.608C */
/*                  MSM System Tx Broadcast Frames Counter Register: 1E.608C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Tx Broadcast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.608C.F:0 ROS MSM System Tx Broadcast Frames Counter 0 [F:0]
                        AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD.u0.bits_0.msmSystemTxBroadcastFramesCounter_0

                        Default = 0x0000

                        Tx broadcast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and all bits of the destination address set to '1'.  */
      unsigned int   msmSystemTxBroadcastFramesCounter_0 : 16;    /* 1E.608C.F:0  ROS      Default = 0x0000 */
                     /* Tx broadcast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Tx Broadcast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.608D.F:0 ROS MSM System Tx Broadcast Frames Counter 1 [F:0]
                        AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD.u1.bits_1.msmSystemTxBroadcastFramesCounter_1

                        Default = 0x0000

                        Tx broadcast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and all bits of the destination address set to '1'.  */
      unsigned int   msmSystemTxBroadcastFramesCounter_1 : 16;    /* 1E.608D.F:0  ROS      Default = 0x0000 */
                     /* Tx broadcast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemTxBroadcastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM System Rx Errors Counter Register: 1E.60C8 */
/*                  MSM System Rx Errors Counter Register: 1E.60C8 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM System Rx Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.60C8.F:0 ROS MSM System Rx Errors Counter 0 [F:0]
                        AQ_MsmSystemRxErrorsCounterRegister_HHD.u0.bits_0.msmSystemRxErrorsCounter_0

                        Default = 0x0000

                        Rx errors counter bits 15:0

                 <B>Notes:</B>
                        Number of frames received with error: 
                        - FIFO Overflow Errors
                        - CRC Errors
                        - Payload Length Errors
                        - Jabber and Oversized Errors
                        - Alignment Errors
                        - The dedicated Error Code (0xfe, not a code error) was received  */
      unsigned int   msmSystemRxErrorsCounter_0 : 16;    /* 1E.60C8.F:0  ROS      Default = 0x0000 */
                     /* Rx errors counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM System Rx Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.60C9.F:0 ROS MSM System Rx Errors Counter 1 [F:0]
                        AQ_MsmSystemRxErrorsCounterRegister_HHD.u1.bits_1.msmSystemRxErrorsCounter_1

                        Default = 0x0000

                        Rx errors counter bits 31:16

                 <B>Notes:</B>
                        Number of frames received with error: 
                        - FIFO Overflow Errors
                        - CRC Errors
                        - Payload Length Errors
                        - Jabber and Oversized Errors
                        - Alignment Errors
                        - The dedicated Error Code (0xfe, not a code error) was received  */
      unsigned int   msmSystemRxErrorsCounter_1 : 16;    /* 1E.60C9.F:0  ROS      Default = 0x0000 */
                     /* Rx errors counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmSystemRxErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress VLAN TPID 0 Register: 1E.8006 */
/*                  MSS Ingress VLAN TPID 0 Register: 1E.8006 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress VLAN TPID 0 Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8006.F:0 R/W MSS Ingress VLAN STag [F:0]
                        AQ_MssIngressVlanTpid_0Register_HHD.u0.bits_0.mssIngressVlanStag

                        Default = 0x0000

                        STag TPID
                        

                 <B>Notes:</B>
                        Service Tag Protocol Identifier (TPID) values to identify a VLAN tag. The " See SEC Egress VLAN CP Tag Parse STag " bit must be set to 1 for the incoming packet's TPID to be parsed.  */
      unsigned int   mssIngressVlanStag : 16;    /* 1E.8006.F:0  R/W      Default = 0x0000 */
                     /* STag TPID
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress VLAN TPID 0 Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressVlanTpid_0Register_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress VLAN TPID 1 Register: 1E.8008 */
/*                  MSS Ingress VLAN TPID 1 Register: 1E.8008 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress VLAN TPID 1 Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8008.F:0 R/W MSS Ingress VLAN QTag [F:0]
                        AQ_MssIngressVlanTpid_1Register_HHD.u0.bits_0.mssIngressVlanQtag

                        Default = 0x0000

                        QTag TPID
                        

                 <B>Notes:</B>
                        Customer Tag Protocol Identifier (TPID) values to identify a VLAN tag. The " See SEC Egress VLAN CP Tag Parse QTag " bit must be set to 1 for the incoming packet's TPID to be parsed.  */
      unsigned int   mssIngressVlanQtag : 16;    /* 1E.8008.F:0  R/W      Default = 0x0000 */
                     /* QTag TPID
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress VLAN TPID 1 Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressVlanTpid_1Register_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress VLAN Control Register: 1E.800A */
/*                  MSS Ingress VLAN Control Register: 1E.800A */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress VLAN Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.800A.F:0 R/W MSS Ingress VLAN UP Map Table LSW [F:0]
                        AQ_MssIngressVlanControlRegister_HHD.u0.bits_0.mssIngressVlanUpMapTableLSW

                        Default = 0x0000

                        Map table bits 15:0
                        

                 <B>Notes:</B>
                        If there is a customer TPID Tag match and no service TPID Tag match or the service TPID Tag match is disabled, the outer TAG's PCP is used to index into this map table to generate the packets user priority.
                        2:0 : UP value for customer Tag PCP 0x0
                        5:3: UP value for customer Tag PCP 0x0
                        8:6 : UP value for customer Tag PCP 0x0
                        11:9 : UP value for customer Tag PCP 0x0
                        14:12 : UP value for customer Tag PCP 0x0
                        17:15 : UP value for customer Tag PCP 0x0  */
      unsigned int   mssIngressVlanUpMapTableLSW : 16;    /* 1E.800A.F:0  R/W      Default = 0x0000 */
                     /* Map table bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress VLAN Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.800B.7:0 R/W MSS Ingress VLAN UP Map Table MSW [17:10]
                        AQ_MssIngressVlanControlRegister_HHD.u1.bits_1.mssIngressVlanUpMapTableMSW

                        Default = 0x00

                        UP Map table bits 23:16
                        

                 <B>Notes:</B>
                        If there is a customer TPID Tag match and no service TPID Tag match or the service TPID Tag match is disabled, the outer TAG's PCP is used to index into this map table to generate the packets user priority.
                        2:0 : UP value for customer Tag PCP 0x0
                        5:3: UP value for customer Tag PCP 0x0
                        8:6 : UP value for customer Tag PCP 0x0
                        11:9 : UP value for customer Tag PCP 0x0
                        14:12 : UP value for customer Tag PCP 0x0
                        17:15 : UP value for customer Tag PCP 0x0
                        20:18 : UP value for customer Tag PCP 0x0
                        23:21 : UP value for customer Tag PCP 0x0  */
      unsigned int   mssIngressVlanUpMapTableMSW : 8;    /* 1E.800B.7:0  R/W      Default = 0x00 */
                     /* UP Map table bits 23:16
                          */
                    /*! \brief 1E.800B.A:8 R/W MSS Ingress VLAN UP Default [2:0]
                        AQ_MssIngressVlanControlRegister_HHD.u1.bits_1.mssIngressVlanUpDefault

                        Default = 0x0

                        UP default
                        

                 <B>Notes:</B>
                        User priority default  */
      unsigned int   mssIngressVlanUpDefault : 3;    /* 1E.800B.A:8  R/W      Default = 0x0 */
                     /* UP default
                          */
                    /*! \brief 1E.800B.B R/W MSS Ingress VLAN STag UP Parse Enable
                        AQ_MssIngressVlanControlRegister_HHD.u1.bits_1.mssIngressVlanStagUpParseEnable

                        Default = 0x0

                        VLAN CP Tag STag UP enable
                        

                 <B>Notes:</B>
                        Enable controlled port service VLAN service Tag user priority field parsing.  */
      unsigned int   mssIngressVlanStagUpParseEnable : 1;    /* 1E.800B.B  R/W      Default = 0x0 */
                     /* VLAN CP Tag STag UP enable
                          */
                    /*! \brief 1E.800B.C R/W MSS Ingress VLAN QTag UP Parse Enable
                        AQ_MssIngressVlanControlRegister_HHD.u1.bits_1.mssIngressVlanQtagUpParseEnable

                        Default = 0x0

                        VLAN CP Tag QTag UP enable
                        

                 <B>Notes:</B>
                        Enable controlled port customer VLAN customer Tag user priority field parsing.  */
      unsigned int   mssIngressVlanQtagUpParseEnable : 1;    /* 1E.800B.C  R/W      Default = 0x0 */
                     /* VLAN CP Tag QTag UP enable
                          */
                    /*! \brief 1E.800B.D R/W MSS Ingress VLAN QinQ Parse Enable
                        AQ_MssIngressVlanControlRegister_HHD.u1.bits_1.mssIngressVlanQinqParseEnable

                        Default = 0x0

                        VLAN CP Tag Parse QinQ
                        

                 <B>Notes:</B>
                        Enable controlled port VLAN QinQ Tag parsing. When this bit is set to 1 both the outer and inner VLAN Tags will be parsed.  */
      unsigned int   mssIngressVlanQinqParseEnable : 1;    /* 1E.800B.D  R/W      Default = 0x0 */
                     /* VLAN CP Tag Parse QinQ
                          */
                    /*! \brief 1E.800B.E R/W MSS Ingress VLAN STag Parse Enable
                        AQ_MssIngressVlanControlRegister_HHD.u1.bits_1.mssIngressVlanStagParseEnable

                        Default = 0x0

                        1 = Enable VLAN STag parsing
                        

                 <B>Notes:</B>
                        Enable controlled port VLAN service Tag parsing. When this bit is set to 1, the incoming packets outer TPID will be compared with the configured " See MSS Ingress VLAN Stag [F:0] " for matching. If the " See SEC Egress VLAN CP Tag Parse QinQ " bit is set to1, this will also be used to compare the incoming packet's inner TPID.  */
      unsigned int   mssIngressVlanStagParseEnable : 1;    /* 1E.800B.E  R/W      Default = 0x0 */
                     /* 1 = Enable VLAN STag parsing
                          */
                    /*! \brief 1E.800B.F R/W MSS Ingress VLAN QTag Parse Enable
                        AQ_MssIngressVlanControlRegister_HHD.u1.bits_1.mssIngressVlanQtagParseEnable

                        Default = 0x0

                        1 = Enable VLAN QTag parsing
                        

                 <B>Notes:</B>
                        Enable controlled port VLAN customer Tag parsing. When this bit is set to 1, the incoming packet's outer TPID will be compared with the configured " See MSS Ingress VLAN QTag [F:0] " for matching. If the " See SEC Egress VLAN CP Tag Parse QinQ " bit is set to1, this will also be used to compare the incoming packet's inner TPID.  */
      unsigned int   mssIngressVlanQtagParseEnable : 1;    /* 1E.800B.F  R/W      Default = 0x0 */
                     /* 1 = Enable VLAN QTag parsing
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressVlanControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress MTU Size Control Register: 1E.800C */
/*                  MSS Ingress MTU Size Control Register: 1E.800C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress MTU Size Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.800C.F:0 R/W MSS Ingress Controlled Packet MTU Size [F:0]
                        AQ_MssIngressMtuSizeControlRegister_HHD.u0.bits_0.mssIngressControlledPacketMtuSize

                        Default = 0x05DC

                        Maximum transmission unit for controlled packet
                        

                 <B>Notes:</B>
                        Maximum transmission unit of controlled packet  */
      unsigned int   mssIngressControlledPacketMtuSize : 16;    /* 1E.800C.F:0  R/W      Default = 0x05DC */
                     /* Maximum transmission unit for controlled packet
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress MTU Size Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.800D.F:0 R/W MSS Ingress Uncontrolled Packet MTU Size [F:0]
                        AQ_MssIngressMtuSizeControlRegister_HHD.u1.bits_1.mssIngressUncontrolledPacketMtuSize

                        Default = 0x05DC

                        Maximum transmission unit for uncontrolled packet
                        

                 <B>Notes:</B>
                        Maximum transmission unit of uncontrolled packet  */
      unsigned int   mssIngressUncontrolledPacketMtuSize : 16;    /* 1E.800D.F:0  R/W      Default = 0x05DC */
                     /* Maximum transmission unit for uncontrolled packet
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressMtuSizeControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress Control Register: 1E.800E */
/*                  MSS Ingress Control Register: 1E.800E */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.800E.0 R/W MSS Ingress Soft Reset
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressSoftReset

                        Default = 0x0

                        1 = Soft reset
                        

                 <B>Notes:</B>
                        S/W reset  */
      unsigned int   mssIngressSoftReset : 1;    /* 1E.800E.0  R/W      Default = 0x0 */
                     /* 1 = Soft reset
                          */
                    /*! \brief 1E.800E.1 R/W MSS Ingress Operation Point To Point
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressOperationPointToPoint

                        Default = 0x0

                        1 = Enable the SCI for authorization default
                        

                 <B>Notes:</B>
                        The default SCI for authorization is configured in  See MSS Ingress SCI Default [F:0]   See MSS Ingress SCI Default [1F:10] , See MSS Ingress SCI Default [2F:20] , and  See MSS Ingress SCI Default [3F:30] .  */
      unsigned int   mssIngressOperationPointToPoint : 1;    /* 1E.800E.1  R/W      Default = 0x0 */
                     /* 1 = Enable the SCI for authorization default
                          */
                    /*! \brief 1E.800E.2 R/W MSS Ingress Create SCI
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressCreateSci

                        Default = 0x0

                        0 = SCI from IGPRC LUT
                        

                 <B>Notes:</B>
                        If the SCI is not in the packet and this bit is set to 0, the SCI will be taken from the IGPRC LUT.  */
      unsigned int   mssIngressCreateSci : 1;    /* 1E.800E.2  R/W      Default = 0x0 */
                     /* 0 = SCI from IGPRC LUT
                          */
                    /*! \brief 1E.800E.3 R/W MSS Ingress Mask Short Length Error
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressMaskShortLengthError

                        Default = 0x0

                        Unused
                        

                 <B>Notes:</B>
                        Unused  */
      unsigned int   mssIngressMaskShortLengthError : 1;    /* 1E.800E.3  R/W      Default = 0x0 */
                     /* Unused
                          */
                    /*! \brief 1E.800E.4 R/W MSS Ingress Drop Kay Packet
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressDropKayPacket

                        Default = 0x0

                        1 = Drop KaY packets
                        

                 <B>Notes:</B>
                        Decides whether KaY packets have to be dropped  */
      unsigned int   mssIngressDropKayPacket : 1;    /* 1E.800E.4  R/W      Default = 0x0 */
                     /* 1 = Drop KaY packets
                          */
                    /*! \brief 1E.800E.5 R/W MSS Ingress Drop IGPRC Miss
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressDropIgprcMiss

                        Default = 0x0

                        1 = Drop IGPRC miss packets
                        

                 <B>Notes:</B>
                        Decides whether Ingress Pre-Security Classification (IGPRC) LUT miss packets are to be dropped  */
      unsigned int   mssIngressDropIgprcMiss : 1;    /* 1E.800E.5  R/W      Default = 0x0 */
                     /* 1 = Drop IGPRC miss packets
                          */
                    /*! \brief 1E.800E.6 R/W MSS Ingress Check ICV
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressCheckIcv

                        Default = 0x0

                        Unused
                        

                 <B>Notes:</B>
                        Unused  */
      unsigned int   mssIngressCheckIcv : 1;    /* 1E.800E.6  R/W      Default = 0x0 */
                     /* Unused
                          */
                    /*! \brief 1E.800E.7 R/W MSS Ingress Clear Global Time
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressClearGlobalTime

                        Default = 0x0

                        1 = Clear global time
                        

                 <B>Notes:</B>
                        Clear global time  */
      unsigned int   mssIngressClearGlobalTime : 1;    /* 1E.800E.7  R/W      Default = 0x0 */
                     /* 1 = Clear global time
                          */
                    /*! \brief 1E.800E.8 R/W MSS Ingress Clear Count
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressClearCount

                        Default = 0x0

                        1 = Clear all MIB counters
                        

                 <B>Notes:</B>
                        If this bit is set to 1, all MIB counters will be cleared.  */
      unsigned int   mssIngressClearCount : 1;    /* 1E.800E.8  R/W      Default = 0x0 */
                     /* 1 = Clear all MIB counters
                          */
                    /*! \brief 1E.800E.9 R/W MSS Ingress High Priority
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressHighPriority

                        Default = 0x0

                        1 = MIB counter clear on read enable
                        

                 <B>Notes:</B>
                        If this bit is set to 1, read is given high priority and the MIB count value becomes 0 after read.  */
      unsigned int   mssIngressHighPriority : 1;    /* 1E.800E.9  R/W      Default = 0x0 */
                     /* 1 = MIB counter clear on read enable
                          */
                    /*! \brief 1E.800E.A R/W MSS Ingress Remove SECTag
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressRemoveSectag

                        Default = 0x0

                        1 = Enable removal of SECTag
                        

                 <B>Notes:</B>
                        If this bit is set and either of the following two conditions occurs, the SECTag will be removed.
                        Controlled packet and either the SA or SC is invalid.
                        IGPRC miss.  */
      unsigned int   mssIngressRemoveSectag : 1;    /* 1E.800E.A  R/W      Default = 0x0 */
                     /* 1 = Enable removal of SECTag
                          */
                    /*! \brief 1E.800E.C:B R/W MSS Ingress Global Validate Frames [1:0]
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressGlobalValidateFrames

                        Default = 0x0

                        Default validate frames configuration
                        

                 <B>Notes:</B>
                        If the SC is invalid or if an IGPRC miss packet condition occurs, this default will be used for the validate frames configuration instead of the validate frame entry in the Ingress SC Table (IGSCT).  */
      unsigned int   mssIngressGlobalValidateFrames : 2;    /* 1E.800E.C:B  R/W      Default = 0x0 */
                     /* Default validate frames configuration
                          */
                    /*! \brief 1E.800E.D R/W MSS Ingress ICV LSB 8 Bytes Enable
                        AQ_MssIngressControlRegister_HHD.u0.bits_0.mssIngressIcvLsb_8BytesEnable

                        Default = 0x0

                        1 = Use LSB
                        0 = Use MSB
                        
                        

                 <B>Notes:</B>
                        This bit selects MSB or LSB 8 bytes selection in the case where the ICV is 8 bytes.
                        0 = MSB is used.  */
      unsigned int   mssIngressIcvLsb_8BytesEnable : 1;    /* 1E.800E.D  R/W      Default = 0x0 */
                     /* 1 = Use LSB
                        0 = Use MSB
                        
                          */
      unsigned int   reserved0 : 2;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress Control Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress SA Control Register: 1E.8010 */
/*                  MSS Ingress SA Control Register: 1E.8010 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress SA Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8010.F:0 R/W MSS Ingress SA Threshold LSW [F:0]
                        AQ_MssIngressSaControlRegister_HHD.u0.bits_0.mssIngressSaThresholdLSW

                        Default = 0x0000

                        SA threshold bits 15:0
                        

                 <B>Notes:</B>
                        Ingress PN threshold to generate SA threshold interrupt.  */
      unsigned int   mssIngressSaThresholdLSW : 16;    /* 1E.8010.F:0  R/W      Default = 0x0000 */
                     /* SA threshold bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress SA Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8011.F:0 R/W MSS Ingress SA Threshold MSW [1F:10]
                        AQ_MssIngressSaControlRegister_HHD.u1.bits_1.mssIngressSaThresholdMSW

                        Default = 0x0000

                        SA threshold bits 31:16
                        

                 <B>Notes:</B>
                        Ingress PN threshold to generate SA threshold interrupt.  */
      unsigned int   mssIngressSaThresholdMSW : 16;    /* 1E.8011.F:0  R/W      Default = 0x0000 */
                     /* SA threshold bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressSaControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress Interrupt Status Register: 1E.802E */
/*                  MSS Ingress Interrupt Status Register: 1E.802E */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress Interrupt Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.802E.0 COW MSS Master Ingress Interrupt
                        AQ_MssIngressInterruptStatusRegister_HHD.u0.bits_0.mssMasterIngressInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when any one of the above interrupt and the corresponding interrupt enable are both set. The interrupt enable for this bit must also be set for this bit to be set.  */
      unsigned int   mssMasterIngressInterrupt : 1;    /* 1E.802E.0  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.802E.1 COW MSS Ingress SA Expired Interrupt
                        AQ_MssIngressInterruptStatusRegister_HHD.u0.bits_0.mssIngressSaExpiredInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when the SA PN reaches all ones saturation.  */
      unsigned int   mssIngressSaExpiredInterrupt : 1;    /* 1E.802E.1  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.802E.2 COW MSS Ingress SA Threshold Expired Interrupt
                        AQ_MssIngressInterruptStatusRegister_HHD.u0.bits_0.mssIngressSaThresholdExpiredInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when the SA PN reaches the  See SEC Egress PN Threshold [F:0] and  See SEC Egress PN Threshold [1F:10] .  */
      unsigned int   mssIngressSaThresholdExpiredInterrupt : 1;    /* 1E.802E.2  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.802E.3 COW MSS Ingress ICV Error Interrupt
                        AQ_MssIngressInterruptStatusRegister_HHD.u0.bits_0.mssIngressIcvErrorInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear.  */
      unsigned int   mssIngressIcvErrorInterrupt : 1;    /* 1E.802E.3  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.802E.4 COW MSS Ingress Replay Error Interrupt
                        AQ_MssIngressInterruptStatusRegister_HHD.u0.bits_0.mssIngressReplayErrorInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear.  */
      unsigned int   mssIngressReplayErrorInterrupt : 1;    /* 1E.802E.4  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.802E.5 COW MSS Ingress MIB Saturation Interrupt
                        AQ_MssIngressInterruptStatusRegister_HHD.u0.bits_0.mssIngressMibSaturationInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This bit is set when the MIB counters reaches all ones saturation.  */
      unsigned int   mssIngressMibSaturationInterrupt : 1;    /* 1E.802E.5  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.802E.6 COW MSS Ingress ECC Error Interrupt
                        AQ_MssIngressInterruptStatusRegister_HHD.u0.bits_0.mssIngressEccErrorInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear.  */
      unsigned int   mssIngressEccErrorInterrupt : 1;    /* 1E.802E.6  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.802E.7 COW MSS Ingress TCI E/C Error Interrupt
                        AQ_MssIngressInterruptStatusRegister_HHD.u0.bits_0.mssIngressTciE_cErrorInterrupt

                        Default = 0x0

                        1 = Interrupt
                        

                 <B>Notes:</B>
                        Write to 1 to clear. This error occurs when the TCI E bit is 1 and the TCI C bit is 0. The packet is not dropped, uncontrolled, or untagged.  */
      unsigned int   mssIngressTciE_cErrorInterrupt : 1;    /* 1E.802E.7  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
                    /*! \brief 1E.802E.8 COW MSS Ingress IGPOC Miss Interrupt
                        AQ_MssIngressInterruptStatusRegister_HHD.u0.bits_0.mssIngressIgpocMissInterrupt

                        Default = 0x0

                        1 = Interrupt
                        
  */
      unsigned int   mssIngressIgpocMissInterrupt : 1;    /* 1E.802E.8  COW      Default = 0x0 */
                     /* 1 = Interrupt
                          */
      unsigned int   reserved0 : 7;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress Interrupt Status Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressInterruptStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress Interrupt Mask Register: 1E.8030 */
/*                  MSS Ingress Interrupt Mask Register: 1E.8030 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress Interrupt Mask Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8030.0 R/W MSS Ingress Master Interrupt Enable
                        AQ_MssIngressInterruptMaskRegister_HHD.u0.bits_0.mssIngressMasterInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        
  */
      unsigned int   mssIngressMasterInterruptEnable : 1;    /* 1E.8030.0  R/W      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.8030.1 R/W MSS Ingress SA Expired Interrupt Enable
                        AQ_MssIngressInterruptMaskRegister_HHD.u0.bits_0.mssIngressSaExpiredInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        
  */
      unsigned int   mssIngressSaExpiredInterruptEnable : 1;    /* 1E.8030.1  R/W      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.8030.2 R/W MSS Ingress SA Threshold Expired Interrupt Enable
                        AQ_MssIngressInterruptMaskRegister_HHD.u0.bits_0.mssIngressSaThresholdExpiredInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        
  */
      unsigned int   mssIngressSaThresholdExpiredInterruptEnable : 1;    /* 1E.8030.2  R/W      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.8030.3 R/W MSS Ingress ICV Error Interrupt Enable
                        AQ_MssIngressInterruptMaskRegister_HHD.u0.bits_0.mssIngressIcvErrorInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        
  */
      unsigned int   mssIngressIcvErrorInterruptEnable : 1;    /* 1E.8030.3  R/W      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.8030.4 R/W MSS Ingress Replay Error Interrupt Enable
                        AQ_MssIngressInterruptMaskRegister_HHD.u0.bits_0.mssIngressReplayErrorInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        
  */
      unsigned int   mssIngressReplayErrorInterruptEnable : 1;    /* 1E.8030.4  R/W      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.8030.5 R/W MSS Ingress MIB Saturation Interrupt Enable
                        AQ_MssIngressInterruptMaskRegister_HHD.u0.bits_0.mssIngressMibSaturationInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        
  */
      unsigned int   mssIngressMibSaturationInterruptEnable : 1;    /* 1E.8030.5  R/W      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.8030.6 R/W MSS Ingress ECC Error Interrupt Enable
                        AQ_MssIngressInterruptMaskRegister_HHD.u0.bits_0.mssIngressEccErrorInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        
  */
      unsigned int   mssIngressEccErrorInterruptEnable : 1;    /* 1E.8030.6  R/W      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.8030.7 R/W MSS Ingress TCI E/C Error Interrupt Enable
                        AQ_MssIngressInterruptMaskRegister_HHD.u0.bits_0.mssIngressTciE_cErrorInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        
  */
      unsigned int   mssIngressTciE_cErrorInterruptEnable : 1;    /* 1E.8030.7  R/W      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
                    /*! \brief 1E.8030.8 R/W MSS Ingress IGPOC Miss Interrupt Enable
                        AQ_MssIngressInterruptMaskRegister_HHD.u0.bits_0.mssIngressIgpocMissInterruptEnable

                        Default = 0x0

                        1 = Interrupt enabled
                        
  */
      unsigned int   mssIngressIgpocMissInterruptEnable : 1;    /* 1E.8030.8  R/W      Default = 0x0 */
                     /* 1 = Interrupt enabled
                          */
      unsigned int   reserved0 : 7;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress Interrupt Mask Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressInterruptMaskRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress SA ICV Error Status Register: 1E.8032 */
/*                  MSS Ingress SA ICV Error Status Register: 1E.8032 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress SA ICV Error Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8032.F:0 COW MSS Ingress SA ICV Error LSW [F:0]
                        AQ_MssIngressSaIcvErrorStatusRegister_HHD.u0.bits_0.mssIngressSaIcvErrorLSW

                        Default = 0x0000

                        SA ICV error bits 15:0
                        

                 <B>Notes:</B>
                        When set, these bits identify the SA that has an ICV error. Write these bits to 1 to clear.  */
      unsigned int   mssIngressSaIcvErrorLSW : 16;    /* 1E.8032.F:0  COW      Default = 0x0000 */
                     /* SA ICV error bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress SA ICV Error Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8033.F:0 COW MSS Ingress SA ICV Error MSW [1F:10]
                        AQ_MssIngressSaIcvErrorStatusRegister_HHD.u1.bits_1.mssIngressSaIcvErrorMSW

                        Default = 0x0000

                        SA ICV error bits 31:16
                        

                 <B>Notes:</B>
                        When set, these bits identify the SA that has an ICV error. Write these bits to 1 to clear.  */
      unsigned int   mssIngressSaIcvErrorMSW : 16;    /* 1E.8033.F:0  COW      Default = 0x0000 */
                     /* SA ICV error bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressSaIcvErrorStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress SA Replay Error Status Register: 1E.8034 */
/*                  MSS Ingress SA Replay Error Status Register: 1E.8034 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress SA Replay Error Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8034.F:0 COW MSS Ingress SA Replay Error LSW [F:0]
                        AQ_MssIngressSaReplayErrorStatusRegister_HHD.u0.bits_0.mssIngressSaReplayErrorLSW

                        Default = 0x0000

                        SA replay error bits 15:0
                        

                 <B>Notes:</B>
                        When set, these bits identify the SA that has a replay error. Write these bits to 1 to clear.  */
      unsigned int   mssIngressSaReplayErrorLSW : 16;    /* 1E.8034.F:0  COW      Default = 0x0000 */
                     /* SA replay error bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress SA Replay Error Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8035.F:0 COW MSS Ingress SA Replay Error MSW [1F:10]
                        AQ_MssIngressSaReplayErrorStatusRegister_HHD.u1.bits_1.mssIngressSaReplayErrorMSW

                        Default = 0x0000

                        SA replay error bits 31:16
                        

                 <B>Notes:</B>
                        When set, these bits identify the SA that has a replay error. Write these bits to 1 to clear.  */
      unsigned int   mssIngressSaReplayErrorMSW : 16;    /* 1E.8035.F:0  COW      Default = 0x0000 */
                     /* SA replay error bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressSaReplayErrorStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress SA Expired Status Register: 1E.8036 */
/*                  MSS Ingress SA Expired Status Register: 1E.8036 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress SA Expired Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8036.F:0 ROS MSS Ingress SA Expired LSW [F:0]
                        AQ_MssIngressSaExpiredStatusRegister_HHD.u0.bits_0.mssIngressSaExpiredLSW

                        Default = 0x0000

                        SA expired bits 15:0
                        

                 <B>Notes:</B>
                        When set, these bits identify the SA that has expired when the SA PN reaches all-ones saturation. Write these bits to 1 to clear.  */
      unsigned int   mssIngressSaExpiredLSW : 16;    /* 1E.8036.F:0  ROS      Default = 0x0000 */
                     /* SA expired bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress SA Expired Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8037.F:0 ROS MSS Ingress SA Expired MSW [1F:10]
                        AQ_MssIngressSaExpiredStatusRegister_HHD.u1.bits_1.mssIngressSaExpiredMSW

                        Default = 0x0000

                        SA expired bits 31:16
                        

                 <B>Notes:</B>
                        When set, these bits identify the SA that has expired when the SA PN reaches all-ones saturation. Write these bits to 1 to clear.  */
      unsigned int   mssIngressSaExpiredMSW : 16;    /* 1E.8037.F:0  ROS      Default = 0x0000 */
                     /* SA expired bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressSaExpiredStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress SA Threshold Expired Status Register: 1E.8038 */
/*                  MSS Ingress SA Threshold Expired Status Register: 1E.8038 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress SA Threshold Expired Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8038.F:0 ROS MSS Ingress SA Threshold Expired LSW [F:0]
                        AQ_MssIngressSaThresholdExpiredStatusRegister_HHD.u0.bits_0.mssIngressSaThresholdExpiredLSW

                        Default = 0x0000

                        SA threshold expired bits 15:0
                        

                 <B>Notes:</B>
                        When set, these bits identify the SA that has expired when the SA PN has reached the configured threshold  See SEC Egress PN Threshold [F:0] and  See SEC Egress PN Threshold [1F:10] . Write these bits to 1 to clear.  */
      unsigned int   mssIngressSaThresholdExpiredLSW : 16;    /* 1E.8038.F:0  ROS      Default = 0x0000 */
                     /* SA threshold expired bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress SA Threshold Expired Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8039.F:0 ROS MSS Ingress SA Threshold Expired MSW [1F:10]
                        AQ_MssIngressSaThresholdExpiredStatusRegister_HHD.u1.bits_1.mssIngressSaThresholdExpiredMSW

                        Default = 0x0000

                        SA threshold expired bits 31:16
                        

                 <B>Notes:</B>
                        When set, these bits identify the SA that has expired when the SA PN has reached the configured threshold  See SEC Egress PN Threshold [F:0] and  See SEC Egress PN Threshold [1F:10] . Write these bits to 1 to clear.  */
      unsigned int   mssIngressSaThresholdExpiredMSW : 16;    /* 1E.8039.F:0  ROS      Default = 0x0000 */
                     /* SA threshold expired bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressSaThresholdExpiredStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress ECC Interrupt Status Register: 1E.803A */
/*                  MSS Ingress ECC Interrupt Status Register: 1E.803A */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress ECC Interrupt Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.803A.F:0 R/W MSS Ingress SA ECC Error Interrupt LSW [F:0]
                        AQ_MssIngressEccInterruptStatusRegister_HHD.u0.bits_0.mssIngressSaEccErrorInterruptLSW

                        Default = 0x0000

                        SA ECC error interrupt bits 15:0
                        

                 <B>Notes:</B>
                        When set to 1, indicates that an ECC error occured for the SA.  */
      unsigned int   mssIngressSaEccErrorInterruptLSW : 16;    /* 1E.803A.F:0  R/W      Default = 0x0000 */
                     /* SA ECC error interrupt bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress ECC Interrupt Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.803B.F:0 R/W MSS Ingress SA ECC Error Interrupt MSW [1F:10]
                        AQ_MssIngressEccInterruptStatusRegister_HHD.u1.bits_1.mssIngressSaEccErrorInterruptMSW

                        Default = 0x0000

                        SA ECC error interrupt bits 31:16
                        

                 <B>Notes:</B>
                        When set to 1, indicates that an ECC error occured for the SA.  */
      unsigned int   mssIngressSaEccErrorInterruptMSW : 16;    /* 1E.803B.F:0  R/W      Default = 0x0000 */
                     /* SA ECC error interrupt bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MssIngressEccInterruptStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress LUT Address Control Register: 1E.8080 */
/*                  MSS Ingress LUT Address Control Register: 1E.8080 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress LUT Address Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.8080.8:0 R/W MSS Ingress LUT Address [8:0]
                        AQ_MssIngressLutAddressControlRegister_HHD.u0.bits_0.mssIngressLutAddress

                        Default = 0x000

                        LUT address
                        
  */
      unsigned int   mssIngressLutAddress : 9;    /* 1E.8080.8:0  R/W      Default = 0x000 */
                     /* LUT address
                          */
      unsigned int   reserved0 : 3;
                    /*! \brief 1E.8080.F:C R/W MSS Ingress LUT Select [3:0]
                        AQ_MssIngressLutAddressControlRegister_HHD.u0.bits_0.mssIngressLutSelect

                        Default = 0x0

                        LUT select
                        

                 <B>Notes:</B>
                        0x0 : Ingress Pre-Security MAC Control FIlter (IGPRCTLF) LUT
                        0x1 : Ingress Pre-Security Classification LUT (IGPRC)
                        0x2 : Ingress Packet Format (IGPFMT) SAKey LUT
                        0x3 : Ingress Packet Format (IGPFMT) SC/SA LUT
                        0x4 : Ingress Post-Security Classification LUT (IGPOC)
                        0x5 : Ingress Post-Security MAC Control Filter (IGPOCTLF) LUT
                        0x6 : Ingress MIB (IGMIB)  */
      unsigned int   mssIngressLutSelect : 4;    /* 1E.8080.F:C  R/W      Default = 0x0 */
                     /* LUT select
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_MssIngressLutAddressControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress LUT Control Register: 1E.8081 */
/*                  MSS Ingress LUT Control Register: 1E.8081 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress LUT Control Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 14;
                    /*! \brief 1E.8081.E R/W MSS Ingress LUT Read
                        AQ_MssIngressLutControlRegister_HHD.u0.bits_0.mssIngressLutRead

                        Default = 0x0

                        1 = LUT read
                        

                 <B>Notes:</B>
                        Setting this bit to 1, will read the LUT. This bit will automatically clear to 0.  */
      unsigned int   mssIngressLutRead : 1;    /* 1E.8081.E  R/W      Default = 0x0 */
                     /* 1 = LUT read
                          */
                    /*! \brief 1E.8081.F R/W MSS Ingress LUT Write
                        AQ_MssIngressLutControlRegister_HHD.u0.bits_0.mssIngressLutWrite

                        Default = 0x0

                        1 = LUT write
                        

                 <B>Notes:</B>
                        Setting this bit to 1, will write the LUT. This bit will automatically clear to 0.  */
      unsigned int   mssIngressLutWrite : 1;    /* 1E.8081.F  R/W      Default = 0x0 */
                     /* 1 = LUT write
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_MssIngressLutControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSS Ingress LUT Data Control Register: 1E.80A0 */
/*                  MSS Ingress LUT Data Control Register: 1E.80A0 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A0.F:0 R/W MSS Ingress LUT Data 0 [F:0]
                        AQ_MssIngressLutDataControlRegister_HHD.u0.bits_0.mssIngressLutData_0

                        Default = 0x0000

                        LUT data bits 15:0
                        
  */
      unsigned int   mssIngressLutData_0 : 16;    /* 1E.80A0.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 15:0
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A1.F:0 R/W MSS Ingress LUT Data 1 [1F:10]
                        AQ_MssIngressLutDataControlRegister_HHD.u1.bits_1.mssIngressLutData_1

                        Default = 0x0000

                        LUT data bits 31:16
                        
  */
      unsigned int   mssIngressLutData_1 : 16;    /* 1E.80A1.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A2.F:0 R/W MSS Ingress LUT Data 2 [2F:20]
                        AQ_MssIngressLutDataControlRegister_HHD.u2.bits_2.mssIngressLutData_2

                        Default = 0x0000

                        LUT data bits 47:32
                        
  */
      unsigned int   mssIngressLutData_2 : 16;    /* 1E.80A2.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 47:32
                          */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A3.F:0 R/W MSS Ingress LUT Data 3 [3F:30]
                        AQ_MssIngressLutDataControlRegister_HHD.u3.bits_3.mssIngressLutData_3

                        Default = 0x0000

                        LUT data bits 63:48
                        
  */
      unsigned int   mssIngressLutData_3 : 16;    /* 1E.80A3.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 63:48
                          */
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A4.F:0 R/W MSS Ingress LUT Data 4 [4F:40]
                        AQ_MssIngressLutDataControlRegister_HHD.u4.bits_4.mssIngressLutData_4

                        Default = 0x0000

                        LUT data bits 79:64
                        
  */
      unsigned int   mssIngressLutData_4 : 16;    /* 1E.80A4.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 79:64
                          */
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A5.F:0 R/W MSS Ingress LUT Data 5 [5F:50]
                        AQ_MssIngressLutDataControlRegister_HHD.u5.bits_5.mssIngressLutData_5

                        Default = 0x0000

                        LUT data bits 95:80
                        
  */
      unsigned int   mssIngressLutData_5 : 16;    /* 1E.80A5.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 95:80
                          */
    } bits_5;
    uint16_t word_5;
  } u5;
  /*! \brief Union for bit and word level access of word 6 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A6.F:0 R/W MSS Ingress LUT Data 6 [6F:60]
                        AQ_MssIngressLutDataControlRegister_HHD.u6.bits_6.mssIngressLutData_6

                        Default = 0x0000

                        LUT data bits 111:96
                        
  */
      unsigned int   mssIngressLutData_6 : 16;    /* 1E.80A6.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 111:96
                          */
    } bits_6;
    uint16_t word_6;
  } u6;
  /*! \brief Union for bit and word level access of word 7 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A7.F:0 R/W MSS Ingress LUT Data 7 [7F:70]
                        AQ_MssIngressLutDataControlRegister_HHD.u7.bits_7.mssIngressLutData_7

                        Default = 0x0000

                        LUT data bits 127:112
                        
  */
      unsigned int   mssIngressLutData_7 : 16;    /* 1E.80A7.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 127:112
                          */
    } bits_7;
    uint16_t word_7;
  } u7;
  /*! \brief Union for bit and word level access of word 8 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A8.F:0 R/W MSS Ingress LUT Data 8 [8F:80]
                        AQ_MssIngressLutDataControlRegister_HHD.u8.bits_8.mssIngressLutData_8

                        Default = 0x0000

                        LUT data bits 143:128
                        
  */
      unsigned int   mssIngressLutData_8 : 16;    /* 1E.80A8.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 143:128
                          */
    } bits_8;
    uint16_t word_8;
  } u8;
  /*! \brief Union for bit and word level access of word 9 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80A9.F:0 R/W MSS Ingress LUT Data 9 [9F:90]
                        AQ_MssIngressLutDataControlRegister_HHD.u9.bits_9.mssIngressLutData_9

                        Default = 0x0000

                        LUT data bits 159:144
                        
  */
      unsigned int   mssIngressLutData_9 : 16;    /* 1E.80A9.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 159:144
                          */
    } bits_9;
    uint16_t word_9;
  } u9;
  /*! \brief Union for bit and word level access of word 10 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80AA.F:0 R/W MSS Ingress LUT Data 10 [AF:A0]
                        AQ_MssIngressLutDataControlRegister_HHD.u10.bits_10.mssIngressLutData_10

                        Default = 0x0000

                        LUT data bits 175:160
                        
  */
      unsigned int   mssIngressLutData_10 : 16;    /* 1E.80AA.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 175:160
                          */
    } bits_10;
    uint16_t word_10;
  } u10;
  /*! \brief Union for bit and word level access of word 11 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80AB.F:0 R/W MSS Ingress LUT Data 11 [BF:B0]
                        AQ_MssIngressLutDataControlRegister_HHD.u11.bits_11.mssIngressLutData_11

                        Default = 0x0000

                        LUT data bits 191:176
                        
  */
      unsigned int   mssIngressLutData_11 : 16;    /* 1E.80AB.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 191:176
                          */
    } bits_11;
    uint16_t word_11;
  } u11;
  /*! \brief Union for bit and word level access of word 12 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80AC.F:0 R/W MSS Ingress LUT Data 12 [CF:C0]
                        AQ_MssIngressLutDataControlRegister_HHD.u12.bits_12.mssIngressLutData_12

                        Default = 0x0000

                        LUT data bits 207:192
                        
  */
      unsigned int   mssIngressLutData_12 : 16;    /* 1E.80AC.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 207:192
                          */
    } bits_12;
    uint16_t word_12;
  } u12;
  /*! \brief Union for bit and word level access of word 13 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80AD.F:0 R/W MSS Ingress LUT Data 13 [DF:D0]
                        AQ_MssIngressLutDataControlRegister_HHD.u13.bits_13.mssIngressLutData_13

                        Default = 0x0000

                        LUT data bits 223:208
                        
  */
      unsigned int   mssIngressLutData_13 : 16;    /* 1E.80AD.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 223:208
                          */
    } bits_13;
    uint16_t word_13;
  } u13;
  /*! \brief Union for bit and word level access of word 14 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80AE.F:0 R/W MSS Ingress LUT Data 14 [EF:E0]
                        AQ_MssIngressLutDataControlRegister_HHD.u14.bits_14.mssIngressLutData_14

                        Default = 0x0000

                        LUT data bits 239:224
                        
  */
      unsigned int   mssIngressLutData_14 : 16;    /* 1E.80AE.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 239:224
                          */
    } bits_14;
    uint16_t word_14;
  } u14;
  /*! \brief Union for bit and word level access of word 15 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80AF.F:0 R/W MSS Ingress LUT Data 15 [FF:F0]
                        AQ_MssIngressLutDataControlRegister_HHD.u15.bits_15.mssIngressLutData_15

                        Default = 0x0000

                        LUT data bits 255:240
                        
  */
      unsigned int   mssIngressLutData_15 : 16;    /* 1E.80AF.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 255:240
                          */
    } bits_15;
    uint16_t word_15;
  } u15;
  /*! \brief Union for bit and word level access of word 16 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80B0.F:0 R/W MSS Ingress LUT Data 16 [10F:100]
                        AQ_MssIngressLutDataControlRegister_HHD.u16.bits_16.mssIngressLutData_16

                        Default = 0x0000

                        LUT data bits 271:256
                        
  */
      unsigned int   mssIngressLutData_16 : 16;    /* 1E.80B0.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 271:256
                          */
    } bits_16;
    uint16_t word_16;
  } u16;
  /*! \brief Union for bit and word level access of word 17 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80B1.F:0 R/W MSS Ingress LUT Data 17 [11F:110]
                        AQ_MssIngressLutDataControlRegister_HHD.u17.bits_17.mssIngressLutData_17

                        Default = 0x0000

                        LUT data bits 287:272
                        
  */
      unsigned int   mssIngressLutData_17 : 16;    /* 1E.80B1.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 287:272
                          */
    } bits_17;
    uint16_t word_17;
  } u17;
  /*! \brief Union for bit and word level access of word 18 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80B2.F:0 R/W MSS Ingress LUT Data 18 [12F:120]
                        AQ_MssIngressLutDataControlRegister_HHD.u18.bits_18.mssIngressLutData_18

                        Default = 0x0000

                        LUT data bits 303:288
                        
  */
      unsigned int   mssIngressLutData_18 : 16;    /* 1E.80B2.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 303:288
                          */
    } bits_18;
    uint16_t word_18;
  } u18;
  /*! \brief Union for bit and word level access of word 19 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80B3.F:0 R/W MSS Ingress LUT Data 19 [13F:130]
                        AQ_MssIngressLutDataControlRegister_HHD.u19.bits_19.mssIngressLutData_19

                        Default = 0x0000

                        LUT data bits 319:304
                        
  */
      unsigned int   mssIngressLutData_19 : 16;    /* 1E.80B3.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 319:304
                          */
    } bits_19;
    uint16_t word_19;
  } u19;
  /*! \brief Union for bit and word level access of word 20 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80B4.F:0 R/W MSS Ingress LUT Data 20 [14F:140]
                        AQ_MssIngressLutDataControlRegister_HHD.u20.bits_20.mssIngressLutData_20

                        Default = 0x0000

                        LUT data bits 335:320
                        
  */
      unsigned int   mssIngressLutData_20 : 16;    /* 1E.80B4.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 335:320
                          */
    } bits_20;
    uint16_t word_20;
  } u20;
  /*! \brief Union for bit and word level access of word 21 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80B5.F:0 R/W MSS Ingress LUT Data 21 [15F:150]
                        AQ_MssIngressLutDataControlRegister_HHD.u21.bits_21.mssIngressLutData_21

                        Default = 0x0000

                        LUT data bits 351:336
                        
  */
      unsigned int   mssIngressLutData_21 : 16;    /* 1E.80B5.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 351:336
                          */
    } bits_21;
    uint16_t word_21;
  } u21;
  /*! \brief Union for bit and word level access of word 22 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80B6.F:0 R/W MSS Ingress LUT Data 22 [16F:160]
                        AQ_MssIngressLutDataControlRegister_HHD.u22.bits_22.mssIngressLutData_22

                        Default = 0x0000

                        LUT data bits 367:352
                        
  */
      unsigned int   mssIngressLutData_22 : 16;    /* 1E.80B6.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 367:352
                          */
    } bits_22;
    uint16_t word_22;
  } u22;
  /*! \brief Union for bit and word level access of word 23 of MSS Ingress LUT Data Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.80B7.F:0 R/W MSS Ingress LUT Data 23 [17F:170]
                        AQ_MssIngressLutDataControlRegister_HHD.u23.bits_23.mssIngressLutData_23

                        Default = 0x0000

                        LUT data bits 383:368
                        
  */
      unsigned int   mssIngressLutData_23 : 16;    /* 1E.80B7.F:0  R/W      Default = 0x0000 */
                     /* LUT data bits 383:368
                          */
    } bits_23;
    uint16_t word_23;
  } u23;
} AQ_MssIngressLutDataControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line  General Control Register: 1E.9004 */
/*                  MSM Line  General Control Register: 1E.9004 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line  General Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9004.0 R/W MSM Line  Tx Enable
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineTxEnable

                        Default = 0x0

                        1 = Tx enable

                 <B>Notes:</B>
                        MAC Rx path enable. Should be set to 1 to enable the MAC Rx path. Should be set to 0 to disable the MAC Rx path.  */
      unsigned int   msmLineTxEnable : 1;    /* 1E.9004.0  R/W      Default = 0x0 */
                     /* 1 = Tx enable  */
                    /*! \brief 1E.9004.1 R/W MSM Line  Rx Enable
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineRxEnable

                        Default = 0x0

                        1 = Rx enable

                 <B>Notes:</B>
                        MAC Tx path enable. Should be set to 1 to enable the MAC Tx path. Should be set to 0 to disable the MAC Tx path.  */
      unsigned int   msmLineRxEnable : 1;    /* 1E.9004.1  R/W      Default = 0x0 */
                     /* 1 = Rx enable  */
      unsigned int   reserved0 : 1;
                    /*! \brief 1E.9004.3 R/W MSM Line  WAN Mode
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineWanMode

                        Default = 0x0

                        1 = WAN mode
                        0 = LAN mode
                        

                 <B>Notes:</B>
                        WAN mode enable. Sets WAN mode when set to 1 and LAN mode when set to 0. Note: When changing the mode, verifiy correct setting of the Tx IPG.  */
      unsigned int   msmLineWanMode : 1;    /* 1E.9004.3  R/W      Default = 0x0 */
                     /* 1 = WAN mode
                        0 = LAN mode
                          */
                    /*! \brief 1E.9004.4 R/W MSM Line  Promiscuous Mode
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLinePromiscuousMode

                        Default = 0x0

                        1 = Promiscuous mode
                        

                 <B>Notes:</B>
                        When set to 1, all frames are received without any MAC address filtering.  */
      unsigned int   msmLinePromiscuousMode : 1;    /* 1E.9004.4  R/W      Default = 0x0 */
                     /* 1 = Promiscuous mode
                          */
                    /*! \brief 1E.9004.5 R/W MSM Line  PAD Enable
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLinePadEnable

                        Default = 0x0

                        1 = Enable frame padding removal on Rx
                        

                 <B>Notes:</B>
                        When set to 1, enable frame padding removal on the Rx path. If enabled, padding is removed before the frame is transferred to the MAC client application. If disabled, no padding is removed on the Rx by the MAC.
                        Note : On Tx, the MAC always adds padding as required.  */
      unsigned int   msmLinePadEnable : 1;    /* 1E.9004.5  R/W      Default = 0x0 */
                     /* 1 = Enable frame padding removal on Rx
                          */
                    /*! \brief 1E.9004.6 R/W MSM Line  CRC Forward
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineCrcForward

                        Default = 0x0

                        1 = Enable CRC forwarding
                        

                 <B>Notes:</B>
                        When set to 1, the CRC field of the received frames is forwarded with the frame to the user application. If disabled, the CRC field is stripped from the frame.
                        Note : If padding is enabled ( See MAC PAD Enable  set to 1), this bit is ignored.  */
      unsigned int   msmLineCrcForward : 1;    /* 1E.9004.6  R/W      Default = 0x0 */
                     /* 1 = Enable CRC forwarding
                          */
                    /*! \brief 1E.9004.7 R/W MSM Line  Pause Forward
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLinePauseForward

                        Default = 0x0

                        1 = Enable Pause forwarding
                        

                 <B>Notes:</B>
                        Terminate or forward pause frames. If set to 1, pause frames are forwarded to the user application. In normal mode, when set to 0, pause frames are terminated and discarded within the MAC.  */
      unsigned int   msmLinePauseForward : 1;    /* 1E.9004.7  R/W      Default = 0x0 */
                     /* 1 = Enable Pause forwarding
                          */
                    /*! \brief 1E.9004.8 R/W MSM Line  Pause Ignore
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLinePauseIgnore

                        Default = 0x0

                        1 = Ignore pause frames
                        

                 <B>Notes:</B>
                        Ignore pause frame quanta. If set to 1, received pause frames are ignored by the MAC. When set to 0, the Tx is stopped for the amount of time specified in the pause quanta received within the pause frame.  */
      unsigned int   msmLinePauseIgnore : 1;    /* 1E.9004.8  R/W      Default = 0x0 */
                     /* 1 = Ignore pause frames
                          */
                    /*! \brief 1E.9004.9 R/W MSM Line  Tx Address Insert Enable
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineTxAddressInsertEnable

                        Default = 0x0

                        1 = Insert Tx MAC source address
                        

                 <B>Notes:</B>
                        Set the source MAC address on transmit. If set to 1, the MAC overwrites the source MAC address with the MAC programmed address in all transmitted frames. When set to 0, the source MAC address is transmitted unmodified from the MAC Tx client application.  */
      unsigned int   msmLineTxAddressInsertEnable : 1;    /* 1E.9004.9  R/W      Default = 0x0 */
                     /* 1 = Insert Tx MAC source address
                          */
                    /*! \brief 1E.9004.A R/W MSM Line  Tx CRC Append
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineTxCrcAppend

                        Default = 0x0

                        1 = Append Tx CRC
                        

                 <B>Notes:</B>
                        Permanently enable CRC append on transmit. If set to 1, the Tx will append a CRC to all transmitted frames. If set to 0, CRC append can be controlled on a per frame basis using the pin ff_tx_crc.
                        This configuration bit is OR'ed with the external ff_tx_crc pin to instruct the Tx to append a CRC to transmitted frames. The ff_tx_crc pin is tied to 0.  */
      unsigned int   msmLineTxCrcAppend : 1;    /* 1E.9004.A  R/W      Default = 0x0 */
                     /* 1 = Append Tx CRC
                          */
                    /*! \brief 1E.9004.B R/W MSM Line  Tx Pad Enable
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineTxPadEnable

                        Default = 0x1

                        1 = Enable Tx padding
                        

                 <B>Notes:</B>
                        When set to 1, enable padding of frames in the Tx direction. When set to 0, the MAC will not extend frames from the application to a minimum of 64 bytes, allowing to transmit short frames (violating the Ethernet mimimum size requirements). Must be set to 1 for normal operation.  */
      unsigned int   msmLineTxPadEnable : 1;    /* 1E.9004.B  R/W      Default = 0x1 */
                     /* 1 = Enable Tx padding
                          */
                    /*! \brief 1E.9004.C R/WSC MSM Line  Soft Reset
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineSoftReset

                        Default = 0x0

                        1 = Soft reset
                        

                 <B>Notes:</B>
                        Software reset. Self clearing bit. When set to 1, resets all statistic counters as well as the Tx and Rx FIFOs. It should be issued after all traffic has been stopped as a result of clearing the Rx/Tx enable bits ( See MAC Rx Enable  set to 0 and  See MAC Tx Enable  set to 0).
                        Note : Can lead to an Rx interface (ff_rx_xxx) violations to the application if the reset is issued in the middle of a receive frame transfer. Then the end of packet (assertion of ff_rx_eop) is lost and the application should be prepeared to handle this exception.  */
      unsigned int   msmLineSoftReset : 1;    /* 1E.9004.C  R/WSC      Default = 0x0 */
                     /* 1 = Soft reset
                          */
                    /*! \brief 1E.9004.D R/W MSM Line  Control Frame Enable
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineControlFrameEnable

                        Default = 0x0

                        1 = Control frame enabled
                        

                 <B>Notes:</B>
                        MAC control frame enable. When set to 1, the MAC control frames with any Opcode other than 0x0001 are accepted and forwarded to the client interface. When set to 0, MAC control frames with any opcode other than 0x0001 are silently discarded.  */
      unsigned int   msmLineControlFrameEnable : 1;    /* 1E.9004.D  R/W      Default = 0x0 */
                     /* 1 = Control frame enabled
                          */
                    /*! \brief 1E.9004.E R/W MSM Line  Rx Error Discard
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLineRxErrorDiscard

                        Default = 0x0

                        1 = Enable discard of received errored frames
                        

                 <B>Notes:</B>
                        Rx errored frame discard enable. When set to 1, any frame received with an error is discarded and not forwarded to the client interface. When set to 0, errored frames are forwarded to the client interface with ff_rx_err asserted.
                        Note : It is recommended to set this bit to 1 only when store and forward operation is enabled (RX_SECTION_FULL TBD).  */
      unsigned int   msmLineRxErrorDiscard : 1;    /* 1E.9004.E  R/W      Default = 0x0 */
                     /* 1 = Enable discard of received errored frames
                          */
                    /*! \brief 1E.9004.F R/W MSM Line  PHY Tx Enable
                        AQ_MsmLineGeneralControlRegister_HHD.u0.bits_0.msmLinePhyTxEnable

                        Default = 0x0

                        1 = Enable PHY Tx
                        

                 <B>Notes:</B>
                        Directly controls the phy_tx_ena pin.  */
      unsigned int   msmLinePhyTxEnable : 1;    /* 1E.9004.F  R/W      Default = 0x0 */
                     /* 1 = Enable PHY Tx
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line  General Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9005.0 R/W MSM Line  Force Send IDLE
                        AQ_MsmLineGeneralControlRegister_HHD.u1.bits_1.msmLineForceSendIdle

                        Default = 0x0

                        1 = Force send idle

                 <B>Notes:</B>
                        When set to 1, suppress any frame transmissions and forces IDLE n the Tx interface instead of frames. This control affects the MAC reconciliation layer (RS) which acts after all MAC datapath has processed the frame.
                        Note : Does not have an effect on fault handling (i.e. reception of local fault will still cause transmit of remote fault).
                        Must be 0 for normal operation.  */
      unsigned int   msmLineForceSendIdle : 1;    /* 1E.9005.0  R/W      Default = 0x0 */
                     /* 1 = Force send idle  */
                    /*! \brief 1E.9005.1 R/W MSM Line  Length Check Disable
                        AQ_MsmLineGeneralControlRegister_HHD.u1.bits_1.msmLineLengthCheckDisable

                        Default = 0x0

                        1 = Disable length check

                 <B>Notes:</B>
                        Payload length check disable. When set to 0, the MAC checks the frames payload length with the frame length/type field. When set to 1, the payload length check is disabled.  */
      unsigned int   msmLineLengthCheckDisable : 1;    /* 1E.9005.1  R/W      Default = 0x0 */
                     /* 1 = Disable length check  */
                    /*! \brief 1E.9005.2 R/W MSM Line  IDLE Column Count Extend
                        AQ_MsmLineGeneralControlRegister_HHD.u1.bits_1.msmLineIdleColumnCountExtend

                        Default = 0x0

                        1 = Extend IDLE column count

                 <B>Notes:</B>
                        When set to 1, extends the RS layer IDLE column counter by 2x. The IEEE 802.3ae defines the fault condition to be cleared after 128 columns of IDLE have been received. If the MAC operates together with a WAN mode PCS (WIS) it may may happen (depending on PCS) that this period is too short to bridge the IDLE stuffing occurring in this mode, leading to a toggling fault indication. In this case, extending the counter helps to aoivd toggling fault indications.  */
      unsigned int   msmLineIdleColumnCountExtend : 1;    /* 1E.9005.2  R/W      Default = 0x0 */
                     /* 1 = Extend IDLE column count  */
                    /*! \brief 1E.9005.3 R/W MSM Line  Priority Flow Control Enable
                        AQ_MsmLineGeneralControlRegister_HHD.u1.bits_1.msmLinePriorityFlowControlEnable

                        Default = 0x0

                        1 = Enable priority flow control
                        0 = Enable link flow control
                        

                 <B>Notes:</B>
                        Enable priority flow control (PFC) mode of operation. When set to 0, the MAC uses standard link pause frames. When set to 1, the MAC will transmit and accept PFC frames.  */
      unsigned int   msmLinePriorityFlowControlEnable : 1;    /* 1E.9005.3  R/W      Default = 0x0 */
                     /* 1 = Enable priority flow control
                        0 = Enable link flow control
                          */
      unsigned int   reserved2 : 1;
                    /*! \brief 1E.9005.5 R/W MSM Line  SFD Check Disable
                        AQ_MsmLineGeneralControlRegister_HHD.u1.bits_1.msmLineSfdCheckDisable

                        Default = 0x0

                        1 = Disable SFD check
                        

                 <B>Notes:</B>
                        Disable check of SFD (0xD5) character at frame start. When set to 1, the frame is accepted even if the SFD byte following the preamble is not 0xD5. When set to 0, a frame is accepted only if the SFD byte is found with the value 0xD5.  */
      unsigned int   msmLineSfdCheckDisable : 1;    /* 1E.9005.5  R/W      Default = 0x0 */
                     /* 1 = Disable SFD check
                          */
      unsigned int   reserved1 : 1;
                    /*! \brief 1E.9005.7 R/W MSM Line  Tx Low Power IDLE Enable
                        AQ_MsmLineGeneralControlRegister_HHD.u1.bits_1.msmLineTxLowPowerIdleEnable

                        Default = 0x0

                        1 = Transmit LPI enable
                        

                 <B>Notes:</B>
                        Transmit low power IDLE enable. When set to 1, the MAC completes the transmission of the current frame and generates low power IDLE sequences (LPI) to the XGMII/SGMII. When set to 0, the MAC operates in normal mode. This bit is OR'ed with the reg_lowp_ena pin.  */
      unsigned int   msmLineTxLowPowerIdleEnable : 1;    /* 1E.9005.7  R/W      Default = 0x0 */
                     /* 1 = Transmit LPI enable
                          */
      unsigned int   reserved0 : 8;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineGeneralControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line  FIFO Control Register: 1E.900E */
/*                  MSM Line  FIFO Control Register: 1E.900E */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line  FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.900E.7:0 R/W MSM Line  Rx FIFO Full Threshold [7:0]
                        AQ_MsmLineFifoControlRegister_HHD.u0.bits_0.msmLineRxFifoFullThreshold

                        Default = 0x08

                        Rx FIFO full threshold

                 <B>Notes:</B>
                        All threshold values are in steps of FIFO words.  */
      unsigned int   msmLineRxFifoFullThreshold : 8;    /* 1E.900E.7:0  R/W      Default = 0x08 */
                     /* Rx FIFO full threshold  */
      unsigned int   reserved0 : 8;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line  FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.900F.7:0 R/W MSM Line  Rx FIFO Empty Threshold [7:0]
                        AQ_MsmLineFifoControlRegister_HHD.u1.bits_1.msmLineRxFifoEmptyThreshold

                        Default = 0x00

                        Rx FIFO empty threshold

                 <B>Notes:</B>
                        All threshold values are in steps of FIFO words.  */
      unsigned int   msmLineRxFifoEmptyThreshold : 8;    /* 1E.900F.7:0  R/W      Default = 0x00 */
                     /* Rx FIFO empty threshold  */
      unsigned int   reserved0 : 8;
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of MSM Line  FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9010.5:0 R/W MSM Line  Tx FIFO Full Threshold [5:0]
                        AQ_MsmLineFifoControlRegister_HHD.u2.bits_2.msmLineTxFifoFullThreshold

                        Default = 0x08

                        Tx FIFO full threshold

                 <B>Notes:</B>
                        All threshold values are in steps of FIFO words.  */
      unsigned int   msmLineTxFifoFullThreshold : 6;    /* 1E.9010.5:0  R/W      Default = 0x08 */
                     /* Tx FIFO full threshold  */
      unsigned int   reserved0 : 10;
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of MSM Line  FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9011.5:0 R/W MSM Line  Tx FIFO Empty Threshold [5:0]
                        AQ_MsmLineFifoControlRegister_HHD.u3.bits_3.msmLineTxFifoEmptyThreshold

                        Default = 0x00

                        Tx FIFO empty threshold

                 <B>Notes:</B>
                        All threshold values are in steps of FIFO words.  */
      unsigned int   msmLineTxFifoEmptyThreshold : 6;    /* 1E.9011.5:0  R/W      Default = 0x00 */
                     /* Tx FIFO empty threshold  */
      unsigned int   reserved0 : 10;
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of MSM Line  FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9012.7:0 ROS MSM Line  Rx FIFO Almost Full Threshold [7:0]
                        AQ_MsmLineFifoControlRegister_HHD.u4.bits_4.msmLineRxFifoAlmostFullThreshold

                        Default = 0x00

                        Rx FIFO almost full threshold

                 <B>Notes:</B>
                        Unused.  */
      unsigned int   msmLineRxFifoAlmostFullThreshold : 8;    /* 1E.9012.7:0  ROS      Default = 0x00 */
                     /* Rx FIFO almost full threshold  */
      unsigned int   reserved0 : 8;
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of MSM Line  FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9013.7:0 ROS MSM Line  Rx FIFO Almost Empty Threshold [7:0]
                        AQ_MsmLineFifoControlRegister_HHD.u5.bits_5.msmLineRxFifoAlmostEmptyThreshold

                        Default = 0x00

                        Rx FIFO almost empty threshold

                 <B>Notes:</B>
                        Unused.  */
      unsigned int   msmLineRxFifoAlmostEmptyThreshold : 8;    /* 1E.9013.7:0  ROS      Default = 0x00 */
                     /* Rx FIFO almost empty threshold  */
      unsigned int   reserved0 : 8;
    } bits_5;
    uint16_t word_5;
  } u5;
  /*! \brief Union for bit and word level access of word 6 of MSM Line  FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9014.7:0 ROS MSM Line  Tx FIFO Almost Full Threshold [7:0]
                        AQ_MsmLineFifoControlRegister_HHD.u6.bits_6.msmLineTxFifoAlmostFullThreshold

                        Default = 0x00

                        Tx FIFO almost full threshold

                 <B>Notes:</B>
                        Unused.  */
      unsigned int   msmLineTxFifoAlmostFullThreshold : 8;    /* 1E.9014.7:0  ROS      Default = 0x00 */
                     /* Tx FIFO almost full threshold  */
      unsigned int   reserved0 : 8;
    } bits_6;
    uint16_t word_6;
  } u6;
  /*! \brief Union for bit and word level access of word 7 of MSM Line  FIFO Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9015.7:0 ROS MSM Line  Tx FIFO Almost Empty Threshold [7:0]
                        AQ_MsmLineFifoControlRegister_HHD.u7.bits_7.msmLineTxFifoAlmostEmptyThreshold

                        Default = 0x00

                        Tx FIFO almost empty threshold

                 <B>Notes:</B>
                        Unused.  */
      unsigned int   msmLineTxFifoAlmostEmptyThreshold : 8;    /* 1E.9015.7:0  ROS      Default = 0x00 */
                     /* Tx FIFO almost empty threshold  */
      unsigned int   reserved0 : 8;
    } bits_7;
    uint16_t word_7;
  } u7;
} AQ_MsmLineFifoControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line  General Status Register: 1E.9020 */
/*                  MSM Line  General Status Register: 1E.9020 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line  General Status Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9020.0 BLH MSM Line  Rx Local Fault
                        AQ_MsmLineGeneralStatusRegister_HHD.u0.bits_0.msmLineRxLocalFault

                        

                        Rx local fault detected

                 <B>Notes:</B>
                        Latch high local fault status. Set to 1, whent he MAC detects Rx local fault sequences on the Rx interface. Reset to 0 after read and after reset.  */
      unsigned int   msmLineRxLocalFault : 1;    /* 1E.9020.0  BLH       */
                     /* Rx local fault detected  */
                    /*! \brief 1E.9020.1 BLH MSM Line  Rx Remote Fault
                        AQ_MsmLineGeneralStatusRegister_HHD.u0.bits_0.msmLineRxRemoteFault

                        

                        Rx remote fault detected

                 <B>Notes:</B>
                        Latch high local fault status. Set to 1, whent he MAC detects Rx local fault sequences on the Rx interface. Reset to 0 after read and after reset.  */
      unsigned int   msmLineRxRemoteFault : 1;    /* 1E.9020.1  BLH       */
                     /* Rx remote fault detected  */
                    /*! \brief 1E.9020.2 RO MSM Line  PHY Loss of Signal
                        AQ_MsmLineGeneralStatusRegister_HHD.u0.bits_0.msmLinePhyLossOfSignal

                        

                        PHY loss of signal

                 <B>Notes:</B>
                        PHY indicates loss of signal. This is the value of pin phy_los which is tied to 0.  */
      unsigned int   msmLinePhyLossOfSignal : 1;    /* 1E.9020.2  RO       */
                     /* PHY loss of signal  */
                    /*! \brief 1E.9020.3 R/W MSM Line  Timestamp Available
                        AQ_MsmLineGeneralStatusRegister_HHD.u0.bits_0.msmLineTimestampAvailable

                        Default = 0x0

                        Timestamp available

                 <B>Notes:</B>
                        Transmit timestamp available. Indicates that the timestamp of the last transmitted event frame (which had ff_tx_ts_frm=1) is available in the register  See MAC Time Stamp Status 0 [F:0]  and  See MAC Time Stamp Status 1 [F:0] . To clear this bit, the bit must be written with a 1.
                          */
      unsigned int   msmLineTimestampAvailable : 1;    /* 1E.9020.3  R/W      Default = 0x0 */
                     /* Timestamp available  */
                    /*! \brief 1E.9020.4 RO MSM Line  Rx Low Power IDLE
                        AQ_MsmLineGeneralStatusRegister_HHD.u0.bits_0.msmLineRxLowPowerIdle

                        

                        Rx LPI detected

                 <B>Notes:</B>
                        Receive low power IDLE (LPI). Set to 1 when LPI is currently detected on the MAC Rx interface. Set to 0, when the MAC currently operates in normal mode.  */
      unsigned int   msmLineRxLowPowerIdle : 1;    /* 1E.9020.4  RO       */
                     /* Rx LPI detected  */
                    /*! \brief 1E.9020.5 RO MSM Line  Tx FIFO Empty
                        AQ_MsmLineGeneralStatusRegister_HHD.u0.bits_0.msmLineTxFifoEmpty

                        

                        Tx FIFO empty

                 <B>Notes:</B>
                        When set to 1, indicates the Tx FIFO is empty. When set to 0, Tx FIFO is non-empty.  */
      unsigned int   msmLineTxFifoEmpty : 1;    /* 1E.9020.5  RO       */
                     /* Tx FIFO empty  */
      unsigned int   reserved0 : 10;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line  General Status Register */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineGeneralStatusRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line  Tx IPG Control Register: 1E.9022 */
/*                  MSM Line  Tx IPG Control Register: 1E.9022 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line  Tx IPG Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9022.5:0 R/W MSM Line  Tx IPG Length [5:0]
                        AQ_MsmLineTxIpgControlRegister_HHD.u0.bits_0.msmLineTxIpgLength

                        Default = 0x0C

                        Tx IPG length

                 <B>Notes:</B>
                        Tx inter-packet gap (IPG) value. Depending on LAN or WAN mode of operation.
                        LAN Mode : Number of octets in steps of 4. Valid values are 8, 12, 16,..., 100. DIC is supported for any setting > 8. A default of 12 must be set to conform to IEEE802.3ae.
                        WAN Mode : Stretch factor. Valid values are 4 ... 15. The stretch factor is calculated as (value+1)*8. A default of 12 must be set to conform to IEEE802.3ae (i.e. 13*8=104). A larger value shrinks the IPG (increasing bandwidth).
                        The reset value of 12 leads to IEEE802.3ae conformant behavior in both modes.
                        Note : WAN mode is only available in 10G mode of operation.  */
      unsigned int   msmLineTxIpgLength : 6;    /* 1E.9022.5:0  R/W      Default = 0x0C */
                     /* Tx IPG length  */
      unsigned int   reserved0 : 10;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line  Tx IPG Control Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9023.F:0  MSM Line Tx IPG Reserved
                        AQ_MsmLineTxIpgControlRegister_HHD.u1.bits_1.msmLineTxIpgReserved

                        

                        Value always 0, writes ignored
  */
      unsigned int   msmLineTxIpgReserved : 16;    /* 1E.9023.F:0         */
                     /* Value always 0, writes ignored  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineTxIpgControlRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Tx Good Frames Counter Register: 1E.9040 */
/*                  MSM Line Tx Good Frames Counter Register: 1E.9040 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Tx Good Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9040.F:0 ROS MSM Line Tx Good Frames Counter 0 [F:0]
                        AQ_MsmLineTxGoodFramesCounterRegister_HHD.u0.bits_0.msmLineTxGoodFramesCounter_0

                        Default = 0x0000

                        Tx good frame counter bits 15:0

                 <B>Notes:</B>
                        Count of frames transmitted without error (Including pause frames).  */
      unsigned int   msmLineTxGoodFramesCounter_0 : 16;    /* 1E.9040.F:0  ROS      Default = 0x0000 */
                     /* Tx good frame counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Tx Good Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9041.F:0 ROS MSM Line Tx Good Frames Counter 1 [F:0]
                        AQ_MsmLineTxGoodFramesCounterRegister_HHD.u1.bits_1.msmLineTxGoodFramesCounter_1

                        Default = 0x0000

                        Tx good frame counter bits 31:16
                        

                 <B>Notes:</B>
                        Count of frames transmitted without error (Including pause frames).  */
      unsigned int   msmLineTxGoodFramesCounter_1 : 16;    /* 1E.9041.F:0  ROS      Default = 0x0000 */
                     /* Tx good frame counter bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineTxGoodFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx Good Frames Counter Register: 1E.9044 */
/*                  MSM Line Rx Good Frames Counter Register: 1E.9044 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx Good Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9044.F:0 ROS MSM Line Rx Good Frames Counter 0 [F:0]
                        AQ_MsmLineRxGoodFramesCounterRegister_HHD.u0.bits_0.msmLineRxGoodFramesCounter_0

                        Default = 0x0000

                        Rx good frame counter bits 15:0

                 <B>Notes:</B>
                        Count of frames received without error (Including pause frames).  */
      unsigned int   msmLineRxGoodFramesCounter_0 : 16;    /* 1E.9044.F:0  ROS      Default = 0x0000 */
                     /* Rx good frame counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx Good Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9045.F:0 ROS MSM Line Rx Good Frames Counter 1 [F:0]
                        AQ_MsmLineRxGoodFramesCounterRegister_HHD.u1.bits_1.msmLineRxGoodFramesCounter_1

                        Default = 0x0000

                        Rx good frame counter bits 31:16

                 <B>Notes:</B>
                        Count of frames received without error (Including pause frames).  */
      unsigned int   msmLineRxGoodFramesCounter_1 : 16;    /* 1E.9045.F:0  ROS      Default = 0x0000 */
                     /* Rx good frame counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxGoodFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx FCS Errors Counter Register: 1E.9048 */
/*                  MSM Line Rx FCS Errors Counter Register: 1E.9048 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx FCS Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9048.F:0 ROS MSM Line FCS Error Counter 0 [F:0]
                        AQ_MsmLineRxFcsErrorsCounterRegister_HHD.u0.bits_0.msmLineFcsErrorCounter_0

                        Default = 0x0000

                        Frame check sequence error counter bits 15:0

                 <B>Notes:</B>
                        Count of frames for which a CRC-32 Error is detected but the frame is otherwise of correct length.  */
      unsigned int   msmLineFcsErrorCounter_0 : 16;    /* 1E.9048.F:0  ROS      Default = 0x0000 */
                     /* Frame check sequence error counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx FCS Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9049.F:0 ROS MSM Line FCS Error Counter 1 [F:0]
                        AQ_MsmLineRxFcsErrorsCounterRegister_HHD.u1.bits_1.msmLineFcsErrorCounter_1

                        Default = 0x0000

                        Frame check sequence error counter bits 31:16

                 <B>Notes:</B>
                        Count of frames for which a CRC-32 Error is detected but the frame is otherwise of correct length.  */
      unsigned int   msmLineFcsErrorCounter_1 : 16;    /* 1E.9049.F:0  ROS      Default = 0x0000 */
                     /* Frame check sequence error counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxFcsErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx Alignment Errors Counter Register: 1E.904C */
/*                  MSM Line Rx Alignment Errors Counter Register: 1E.904C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx Alignment Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.904C.F:0 ROS MSM Line Alignment Error Counter 0 [F:0]
                        AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD.u0.bits_0.msmLineAlignmentErrorCounter_0

                        Default = 0x0000

                        Alignment error counter bits 15:0

                 <B>Notes:</B>
                        Count of frames received with an alignment error.  */
      unsigned int   msmLineAlignmentErrorCounter_0 : 16;    /* 1E.904C.F:0  ROS      Default = 0x0000 */
                     /* Alignment error counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx Alignment Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.904D.F:0 ROS MSM Line Alignment Error Counter 1 [F:0]
                        AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD.u1.bits_1.msmLineAlignmentErrorCounter_1

                        Default = 0x0000

                        Alignment error counter bits 31:16

                 <B>Notes:</B>
                        Count of frames received with an alignment error.  */
      unsigned int   msmLineAlignmentErrorCounter_1 : 16;    /* 1E.904D.F:0  ROS      Default = 0x0000 */
                     /* Alignment error counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxAlignmentErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Tx Pause Frames Counter Register: 1E.9050 */
/*                  MSM Line Tx Pause Frames Counter Register: 1E.9050 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Tx Pause Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9050.F:0 ROS MSM Line Tx Pause Frames Counter 0 [F:0]
                        AQ_MsmLineTxPauseFramesCounterRegister_HHD.u0.bits_0.msmLineTxPauseFramesCounter_0

                        Default = 0x0000

                        Tx pause frame counter bits 15:0

                 <B>Notes:</B>
                        Valid pause frames transmitted.  */
      unsigned int   msmLineTxPauseFramesCounter_0 : 16;    /* 1E.9050.F:0  ROS      Default = 0x0000 */
                     /* Tx pause frame counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Tx Pause Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9051.F:0 ROS MSM Line Tx Pause Frames Counter 1 [F:0]
                        AQ_MsmLineTxPauseFramesCounterRegister_HHD.u1.bits_1.msmLineTxPauseFramesCounter_1

                        Default = 0x0000

                        Tx pause frame counter bits 31:16
                        

                 <B>Notes:</B>
                        Valid pause frames transmitted.  */
      unsigned int   msmLineTxPauseFramesCounter_1 : 16;    /* 1E.9051.F:0  ROS      Default = 0x0000 */
                     /* Tx pause frame counter bits 31:16
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineTxPauseFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx Pause Frames Counter Register: 1E.9054 */
/*                  MSM Line Rx Pause Frames Counter Register: 1E.9054 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx Pause Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9054.F:0 ROS MSM Line Rx Pause Frames Counter 0 [F:0]
                        AQ_MsmLineRxPauseFramesCounterRegister_HHD.u0.bits_0.msmLineRxPauseFramesCounter_0

                        Default = 0x0000

                        Rx pause frame counter bits 15:0

                 <B>Notes:</B>
                        Valid pause frames received.  */
      unsigned int   msmLineRxPauseFramesCounter_0 : 16;    /* 1E.9054.F:0  ROS      Default = 0x0000 */
                     /* Rx pause frame counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx Pause Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9055.F:0 ROS MSM Line Rx Pause Frames Counter 1 [F:0]
                        AQ_MsmLineRxPauseFramesCounterRegister_HHD.u1.bits_1.msmLineRxPauseFramesCounter_1

                        Default = 0x0000

                        Rx pause frame counter bits 31:16

                 <B>Notes:</B>
                        Valid pause frames received.  */
      unsigned int   msmLineRxPauseFramesCounter_1 : 16;    /* 1E.9055.F:0  ROS      Default = 0x0000 */
                     /* Rx pause frame counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxPauseFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx Too Long Errors Counter Register: 1E.9058 */
/*                  MSM Line Rx Too Long Errors Counter Register: 1E.9058 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx Too Long Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9058.F:0 ROS MSM Line Rx Too Long Errors Counter 0 [F:0]
                        AQ_MsmLineRxTooLongErrorsCounterRegister_HHD.u0.bits_0.msmLineRxTooLongErrorsCounter_0

                        Default = 0x0000

                        Too-long errors counter bits 15:0

                 <B>Notes:</B>
                        Frame received exceeded the maximum length programmed with register FRM_LGTH.  */
      unsigned int   msmLineRxTooLongErrorsCounter_0 : 16;    /* 1E.9058.F:0  ROS      Default = 0x0000 */
                     /* Too-long errors counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx Too Long Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9059.F:0 ROS MSM Line Rx Too Long Errors Counter 1 [F:0]
                        AQ_MsmLineRxTooLongErrorsCounterRegister_HHD.u1.bits_1.msmLineRxTooLongErrorsCounter_1

                        Default = 0x0000

                        Too-long errors counter bits 31:16

                 <B>Notes:</B>
                        Frame received exceeded the maximum length programmed with register FRM_LGTH.  */
      unsigned int   msmLineRxTooLongErrorsCounter_1 : 16;    /* 1E.9059.F:0  ROS      Default = 0x0000 */
                     /* Too-long errors counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxTooLongErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx In Range Length Errors Counter Register: 1E.905C */
/*                  MSM Line Rx In Range Length Errors Counter Register: 1E.905C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx In Range Length Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.905C.F:0 ROS MSM Line Rx In Range Length Errors Counter 0 [F:0]
                        AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD.u0.bits_0.msmLineRxInRangeLengthErrorsCounter_0

                        Default = 0x0000

                        In-range-length errors counter bits 15:0

                 <B>Notes:</B>
                        A count of frames with a length/type field value between 46 (VLAN: 42) and less than 0x0600, that does not match the number of payload data octets received. Should count also if length/type field is less than 46 (VLAN: 42) and the frame is longer than 64 bytes.  */
      unsigned int   msmLineRxInRangeLengthErrorsCounter_0 : 16;    /* 1E.905C.F:0  ROS      Default = 0x0000 */
                     /* In-range-length errors counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx In Range Length Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.905D.F:0 ROS MSM Line Rx In Range Length Errors Counter 1 [F:0]
                        AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD.u1.bits_1.msmLineRxInRangeLengthErrorsCounter_1

                        Default = 0x0000

                        In-range-length errors counter bits 31:16

                 <B>Notes:</B>
                        A count of frames with a length/type field value between 46 (VLAN: 42) and less than 0x0600, that does not match the number of payload data octets received. Should count also if length/type field is less than 46 (VLAN: 42) and the frame is longer than 64 bytes.  */
      unsigned int   msmLineRxInRangeLengthErrorsCounter_1 : 16;    /* 1E.905D.F:0  ROS      Default = 0x0000 */
                     /* In-range-length errors counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxInRangeLengthErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Tx VLAN Frames Counter Register: 1E.9060 */
/*                  MSM Line Tx VLAN Frames Counter Register: 1E.9060 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Tx VLAN Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9060.F:0 ROS MSM Line Tx VLAN Frames Counter 0 [F:0]
                        AQ_MsmLineTxVlanFramesCounterRegister_HHD.u0.bits_0.msmLineTxVlanFramesCounter_0

                        Default = 0x0000

                        Tx VLAN frames counter bits 15:0

                 <B>Notes:</B>
                        Valid VLAN tagged frames transmitted.  */
      unsigned int   msmLineTxVlanFramesCounter_0 : 16;    /* 1E.9060.F:0  ROS      Default = 0x0000 */
                     /* Tx VLAN frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Tx VLAN Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9061.F:0 ROS MSM Line Tx VLAN Frames Counter 1 [F:0]
                        AQ_MsmLineTxVlanFramesCounterRegister_HHD.u1.bits_1.msmLineTxVlanFramesCounter_1

                        Default = 0x0000

                        Tx VLAN frames counter bits 31:16

                 <B>Notes:</B>
                        Valid VLAN tagged frames transmitted.  */
      unsigned int   msmLineTxVlanFramesCounter_1 : 16;    /* 1E.9061.F:0  ROS      Default = 0x0000 */
                     /* Tx VLAN frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineTxVlanFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx VLAN Frames Counter Register: 1E.9064 */
/*                  MSM Line Rx VLAN Frames Counter Register: 1E.9064 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx VLAN Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9064.F:0 ROS MSM Line Rx VLAN Frames Counter 0 [F:0]
                        AQ_MsmLineRxVlanFramesCounterRegister_HHD.u0.bits_0.msmLineRxVlanFramesCounter_0

                        Default = 0x0000

                        Rx VLAN frames counter bits 15:0

                 <B>Notes:</B>
                        Valid VLAN tagged frames received.  */
      unsigned int   msmLineRxVlanFramesCounter_0 : 16;    /* 1E.9064.F:0  ROS      Default = 0x0000 */
                     /* Rx VLAN frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx VLAN Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9065.F:0 ROS MSM Line Rx VLAN Frames Counter 1 [F:0]
                        AQ_MsmLineRxVlanFramesCounterRegister_HHD.u1.bits_1.msmLineRxVlanFramesCounter_1

                        Default = 0x0000

                        Rx VLAN frames counter bits 31:16

                 <B>Notes:</B>
                        Valid VLAN tagged frames received.  */
      unsigned int   msmLineRxVlanFramesCounter_1 : 16;    /* 1E.9065.F:0  ROS      Default = 0x0000 */
                     /* Rx VLAN frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxVlanFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Tx Octets Counter Register: 1E.9068 */
/*                  MSM Line Tx Octets Counter Register: 1E.9068 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Tx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9068.F:0 ROS MSM Line Tx Octets Counter 0 [F:0]
                        AQ_MsmLineTxOctetsCounterRegister_HHD.u0.bits_0.msmLineTxOctetsCounter_0

                        Default = 0x0000

                        Tx octets counter bits 15:0

                 <B>Notes:</B>
                        All octets transmitted except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames transmitted.  */
      unsigned int   msmLineTxOctetsCounter_0 : 16;    /* 1E.9068.F:0  ROS      Default = 0x0000 */
                     /* Tx octets counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Tx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9069.F:0 ROS MSM Line Tx Octets Counter 1 [F:0]
                        AQ_MsmLineTxOctetsCounterRegister_HHD.u1.bits_1.msmLineTxOctetsCounter_1

                        Default = 0x0000

                        Tx octets counter bits 31:16

                 <B>Notes:</B>
                        All octets transmitted except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames transmitted.  */
      unsigned int   msmLineTxOctetsCounter_1 : 16;    /* 1E.9069.F:0  ROS      Default = 0x0000 */
                     /* Tx octets counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of MSM Line Tx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.906A.F:0 ROS MSM Line Tx Octets Counter 2 [F:0]
                        AQ_MsmLineTxOctetsCounterRegister_HHD.u2.bits_2.msmLineTxOctetsCounter_2

                        Default = 0x0000

                        Tx octets counter bits 47:32

                 <B>Notes:</B>
                        All octets transmitted except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames transmitted.  */
      unsigned int   msmLineTxOctetsCounter_2 : 16;    /* 1E.906A.F:0  ROS      Default = 0x0000 */
                     /* Tx octets counter bits 47:32  */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of MSM Line Tx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.906B.F:0 ROS MSM Line Tx Octets Counter 3 [F:0]
                        AQ_MsmLineTxOctetsCounterRegister_HHD.u3.bits_3.msmLineTxOctetsCounter_3

                        Default = 0x0000

                        Tx octets counter bits 63:48

                 <B>Notes:</B>
                        All octets transmitted except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames transmitted.  */
      unsigned int   msmLineTxOctetsCounter_3 : 16;    /* 1E.906B.F:0  ROS      Default = 0x0000 */
                     /* Tx octets counter bits 63:48  */
    } bits_3;
    uint16_t word_3;
  } u3;
} AQ_MsmLineTxOctetsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx Octets Counter Register: 1E.906C */
/*                  MSM Line Rx Octets Counter Register: 1E.906C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.906C.F:0 ROS MSM Line Rx Octets Counter 0 [F:0]
                        AQ_MsmLineRxOctetsCounterRegister_HHD.u0.bits_0.msmLineRxOctetsCounter_0

                        Default = 0x0000

                        Rx octets counter bits 15:0

                 <B>Notes:</B>
                        All octets received except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames received.  */
      unsigned int   msmLineRxOctetsCounter_0 : 16;    /* 1E.906C.F:0  ROS      Default = 0x0000 */
                     /* Rx octets counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx Octets Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.906D.F:0 ROS MSM Line Rx Octets Counter 1 [F:0]
                        AQ_MsmLineRxOctetsCounterRegister_HHD.u1.bits_1.msmLineRxOctetsCounter_1

                        Default = 0x0000

                        Rx octets counter bits 31:16

                 <B>Notes:</B>
                        All octets received except preamble (i.e. Header, Payload, Pad and FCS) for all valid frames and valid pause frames received.  */
      unsigned int   msmLineRxOctetsCounter_1 : 16;    /* 1E.906D.F:0  ROS      Default = 0x0000 */
                     /* Rx octets counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxOctetsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx Unicast Frames Counter Register: 1E.9070 */
/*                  MSM Line Rx Unicast Frames Counter Register: 1E.9070 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx Unicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9070.F:0 ROS MSM Line Rx Unicast Frames Counter 0 [F:0]
                        AQ_MsmLineRxUnicastFramesCounterRegister_HHD.u0.bits_0.msmLineRxUnicastFramesCounter_0

                        Default = 0x0000

                        Rx unicast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface and bit 0 of the destination address was '0'.  */
      unsigned int   msmLineRxUnicastFramesCounter_0 : 16;    /* 1E.9070.F:0  ROS      Default = 0x0000 */
                     /* Rx unicast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx Unicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9071.F:0 ROS MSM Line Rx Unicast Frames Counter 1 [F:0]
                        AQ_MsmLineRxUnicastFramesCounterRegister_HHD.u1.bits_1.msmLineRxUnicastFramesCounter_1

                        Default = 0x0000

                        Rx unicast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface and bit 0 of the destination address was '0'.  */
      unsigned int   msmLineRxUnicastFramesCounter_1 : 16;    /* 1E.9071.F:0  ROS      Default = 0x0000 */
                     /* Rx unicast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxUnicastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx Multicast Frames Counter Register: 1E.9074 */
/*                  MSM Line Rx Multicast Frames Counter Register: 1E.9074 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx Multicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9074.F:0 ROS MSM Line Rx Multicast Frames Counter 0 [F:0]
                        AQ_MsmLineRxMulticastFramesCounterRegister_HHD.u0.bits_0.msmLineRxMulticastFramesCounter_0

                        Default = 0x0000

                        Rx multicast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface and bit 0 of the destination address was '1' but not the broadcast address (all bits set '1' ). Pause frames are not counted.  */
      unsigned int   msmLineRxMulticastFramesCounter_0 : 16;    /* 1E.9074.F:0  ROS      Default = 0x0000 */
                     /* Rx multicast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx Multicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9075.F:0 ROS MSM Line Rx Multicast Frames Counter 1 [F:0]
                        AQ_MsmLineRxMulticastFramesCounterRegister_HHD.u1.bits_1.msmLineRxMulticastFramesCounter_1

                        Default = 0x0000

                        Rx multicast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface and bit 0 of the destination address was '1' but not the broadcast address (all bits set '1' ). Pause frames are not counted.  */
      unsigned int   msmLineRxMulticastFramesCounter_1 : 16;    /* 1E.9075.F:0  ROS      Default = 0x0000 */
                     /* Rx multicast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxMulticastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx Broadcast Frames Counter Register: 1E.9078 */
/*                  MSM Line Rx Broadcast Frames Counter Register: 1E.9078 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx Broadcast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9078.F:0 ROS MSM Line Rx Broadcast Frames Counter 0 [F:0]
                        AQ_MsmLineRxBroadcastFramesCounterRegister_HHD.u0.bits_0.msmLineRxBroadcastFramesCounter_0

                        Default = 0x0000

                        Rx broadcast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface (FIFO) and all bits of the destination address were set '1'.  */
      unsigned int   msmLineRxBroadcastFramesCounter_0 : 16;    /* 1E.9078.F:0  ROS      Default = 0x0000 */
                     /* Rx broadcast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx Broadcast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9079.F:0 ROS MSM Line Rx Broadcast Frames Counter 1 [F:0]
                        AQ_MsmLineRxBroadcastFramesCounterRegister_HHD.u1.bits_1.msmLineRxBroadcastFramesCounter_1

                        Default = 0x0000

                        Rx broadcast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each valid frame received on the receive FIFO interface (FIFO) and all bits of the destination address were set '1'.  */
      unsigned int   msmLineRxBroadcastFramesCounter_1 : 16;    /* 1E.9079.F:0  ROS      Default = 0x0000 */
                     /* Rx broadcast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxBroadcastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Tx Errors Counter Register: 1E.907C */
/*                  MSM Line Tx Errors Counter Register: 1E.907C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Tx Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.907C.F:0 ROS MSM Line Tx Errors Counter 0 [F:0]
                        AQ_MsmLineTxErrorsCounterRegister_HHD.u0.bits_0.msmLineTxErrorsCounter_0

                        Default = 0x0000

                        Rx errors counter bits 15:0

                 <B>Notes:</B>
                        Number of frames transmitted with error: 
                        - FIFO Overflow Errors
                        - FIFO Underflow Errors  */
      unsigned int   msmLineTxErrorsCounter_0 : 16;    /* 1E.907C.F:0  ROS      Default = 0x0000 */
                     /* Rx errors counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Tx Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.907D.F:0 ROS MSM Line Tx Errors Counter 1 [F:0]
                        AQ_MsmLineTxErrorsCounterRegister_HHD.u1.bits_1.msmLineTxErrorsCounter_1

                        Default = 0x0000

                        Tx errors counter bits 31:16

                 <B>Notes:</B>
                        Number of frames transmitted with error: 
                        - FIFO Overflow Errors
                        - FIFO Underflow Errors  */
      unsigned int   msmLineTxErrorsCounter_1 : 16;    /* 1E.907D.F:0  ROS      Default = 0x0000 */
                     /* Tx errors counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineTxErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Tx Unicast Frames Counter Register: 1E.9084 */
/*                  MSM Line Tx Unicast Frames Counter Register: 1E.9084 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Tx Unicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9084.F:0 ROS MSM Line Tx Unicast Frames Counter 0 [F:0]
                        AQ_MsmLineTxUnicastFramesCounterRegister_HHD.u0.bits_0.msmLineTxUnicastFramesCounter_0

                        Default = 0x0000

                        Tx unicast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and bit 0 of the destination address set to '0'.  */
      unsigned int   msmLineTxUnicastFramesCounter_0 : 16;    /* 1E.9084.F:0  ROS      Default = 0x0000 */
                     /* Tx unicast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Tx Unicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9085.F:0 ROS MSM Line Tx Unicast Frames Counter 1 [F:0]
                        AQ_MsmLineTxUnicastFramesCounterRegister_HHD.u1.bits_1.msmLineTxUnicastFramesCounter_1

                        Default = 0x0000

                        Tx unicast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and bit 0 of the destination address set to '0'.  */
      unsigned int   msmLineTxUnicastFramesCounter_1 : 16;    /* 1E.9085.F:0  ROS      Default = 0x0000 */
                     /* Tx unicast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineTxUnicastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Tx Multicast Frames Counter Register: 1E.9088 */
/*                  MSM Line Tx Multicast Frames Counter Register: 1E.9088 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Tx Multicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9088.F:0 ROS MSM Line Tx Multicast Frames Counter 0 [F:0]
                        AQ_MsmLineTxMulticastFramesCounterRegister_HHD.u0.bits_0.msmLineTxMulticastFramesCounter_0

                        Default = 0x0000

                        Tx multicast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and bit 0 of the destination address set to '1' but not the broadcast address (all bits '1').  */
      unsigned int   msmLineTxMulticastFramesCounter_0 : 16;    /* 1E.9088.F:0  ROS      Default = 0x0000 */
                     /* Tx multicast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Tx Multicast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.9089.F:0 ROS MSM Line Tx Multicast Frames Counter 1 [F:0]
                        AQ_MsmLineTxMulticastFramesCounterRegister_HHD.u1.bits_1.msmLineTxMulticastFramesCounter_1

                        Default = 0x0000

                        Tx multicast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and bit 0 of the destination address set to '1' but not the broadcast address (all bits '1').  */
      unsigned int   msmLineTxMulticastFramesCounter_1 : 16;    /* 1E.9089.F:0  ROS      Default = 0x0000 */
                     /* Tx multicast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineTxMulticastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Tx Broadcast Frames Counter Register: 1E.908C */
/*                  MSM Line Tx Broadcast Frames Counter Register: 1E.908C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Tx Broadcast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.908C.F:0 ROS MSM Line Tx Broadcast Frames Counter 0 [F:0]
                        AQ_MsmLineTxBroadcastFramesCounterRegister_HHD.u0.bits_0.msmLineTxBroadcastFramesCounter_0

                        Default = 0x0000

                        Tx broadcast frames counter bits 15:0

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and all bits of the destination address set to '1'.  */
      unsigned int   msmLineTxBroadcastFramesCounter_0 : 16;    /* 1E.908C.F:0  ROS      Default = 0x0000 */
                     /* Tx broadcast frames counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Tx Broadcast Frames Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.908D.F:0 ROS MSM Line Tx Broadcast Frames Counter 1 [F:0]
                        AQ_MsmLineTxBroadcastFramesCounterRegister_HHD.u1.bits_1.msmLineTxBroadcastFramesCounter_1

                        Default = 0x0000

                        Tx broadcast frames counter bits 31:16

                 <B>Notes:</B>
                        Incremented with each frame written to the FIFO interface and all bits of the destination address set to '1'.  */
      unsigned int   msmLineTxBroadcastFramesCounter_1 : 16;    /* 1E.908D.F:0  ROS      Default = 0x0000 */
                     /* Tx broadcast frames counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineTxBroadcastFramesCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                MSM Line Rx Errors Counter Register: 1E.90C8 */
/*                  MSM Line Rx Errors Counter Register: 1E.90C8 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of MSM Line Rx Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.90C8.F:0 ROS MSM Line Rx Errors Counter 0 [F:0]
                        AQ_MsmLineRxErrorsCounterRegister_HHD.u0.bits_0.msmLineRxErrorsCounter_0

                        Default = 0x0000

                        Rx errors counter bits 15:0

                 <B>Notes:</B>
                        Number of frames received with error: 
                        - FIFO Overflow Errors
                        - CRC Errors
                        - Payload Length Errors
                        - Jabber and Oversized Errors
                        - Alignment Errors
                        - The dedicated Error Code (0xfe, not a code error) was received  */
      unsigned int   msmLineRxErrorsCounter_0 : 16;    /* 1E.90C8.F:0  ROS      Default = 0x0000 */
                     /* Rx errors counter bits 15:0  */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of MSM Line Rx Errors Counter Register */
  union
  {
    struct
    {
                    /*! \brief 1E.90C9.F:0 ROS MSM Line Rx Errors Counter 1 [F:0]
                        AQ_MsmLineRxErrorsCounterRegister_HHD.u1.bits_1.msmLineRxErrorsCounter_1

                        Default = 0x0000

                        Rx errors counter bits 31:16

                 <B>Notes:</B>
                        Number of frames received with error: 
                        - FIFO Overflow Errors
                        - CRC Errors
                        - Payload Length Errors
                        - Jabber and Oversized Errors
                        - Alignment Errors
                        - The dedicated Error Code (0xfe, not a code error) was received  */
      unsigned int   msmLineRxErrorsCounter_1 : 16;    /* 1E.90C9.F:0  ROS      Default = 0x0000 */
                     /* Rx errors counter bits 31:16  */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_MsmLineRxErrorsCounterRegister_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Control: 1E.C000 */
/*                  Global Control: 1E.C000 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Control */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Control */
  union
  {
    struct
    {
                    /*! \brief 1E.C001.0 R/W uP Run Stall
                        AQ_GlobalControl_HHD.u1.bits_1.upRunStall

                        Default = 0x0

                        1 = uP Run Stall
                        0 = uP normal mode
                        

                 <B>Notes:</B>
                        Deactivates the uP.  */
      unsigned int   upRunStall : 1;    /* 1E.C001.0  R/W      Default = 0x0 */
                     /* 1 = uP Run Stall
                        0 = uP normal mode
                          */
      unsigned int   reserved1 : 5;
                    /*! \brief 1E.C001.6 R/W uP Run Stall Override
                        AQ_GlobalControl_HHD.u1.bits_1.upRunStallOverride

                        Default = 0x0

                        0 = uP Run Stall from "MDIO Boot Load" pin.
                        1 = uP Run Stall from  See MCP Run Stall bit
                        
                        

                 <B>Notes:</B>
                        This bit selects the uP Run Stall from either the "MDIO Boot Load" pin or the  See MCP Run Stall bit. Pin no longer brought out as deprecated.  */
      unsigned int   upRunStallOverride : 1;    /* 1E.C001.6  R/W      Default = 0x0 */
                     /* 0 = uP Run Stall from "MDIO Boot Load" pin.
                        1 = uP Run Stall from  See MCP Run Stall bit
                        
                          */
      unsigned int   reserved0 : 8;
                    /*! \brief 1E.C001.F R/W uP Reset
                        AQ_GlobalControl_HHD.u1.bits_1.upReset

                        Default = 0x0

                        1 = Reset
                        

                 <B>Notes:</B>
                        Resets the uP and the PIF master and slave bus. Will be active for a minimum of 100 microseconds.  */
      unsigned int   upReset : 1;    /* 1E.C001.F  R/W      Default = 0x0 */
                     /* 1 = Reset
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_GlobalControl_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Reset Control: 1E.C006 */
/*                  Global Reset Control: 1E.C006 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Reset Control */
  union
  {
    struct
    {
      unsigned int   reserved1 : 14;
                    /*! \brief 1E.C006.E R/WPD Global MMD Reset Disable
                        AQ_GlobalResetControl_HHD.u0.bits_0.globalMMD_ResetDisable

                        Provisionable Default = 0x0

                        1 = Disable the S/W reset to the Global MMD registers
                        0 = Enable the S/W reset to the Global MMD registers
                        

                 <B>Notes:</B>
                        Setting this bit prevents a Global S/W reset or Global S/W reset from resetting the Global MMD registers  */
      unsigned int   globalMMD_ResetDisable : 1;    /* 1E.C006.E  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Disable the S/W reset to the Global MMD registers
                        0 = Enable the S/W reset to the Global MMD registers
                          */
      unsigned int   reserved0 : 1;
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalResetControl_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Diagnostic Provisioning: 1E.C400 */
/*                  Global Diagnostic Provisioning: 1E.C400 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Diagnostic Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 15;
                    /*! \brief 1E.C400.F R/WPD Enable Diagnostics
                        AQ_GlobalDiagnosticProvisioning_HHD.u0.bits_0.enableDiagnostics

                        Provisionable Default = 0x1

                        1 = Chip performs diagnostics on power-up
  */
      unsigned int   enableDiagnostics : 1;    /* 1E.C400.F  R/WPD      Provisionable Default = 0x1 */
                     /* 1 = Chip performs diagnostics on power-up  */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalDiagnosticProvisioning_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Thermal Provisioning: 1E.C420 */
/*                  Global Thermal Provisioning: 1E.C420 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Thermal Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C420.F:0 R/WPD Reserved 0 [F:0]
                        AQ_GlobalThermalProvisioning_HHD.u0.bits_0.reserved_0

                        Provisionable Default = 0x0000

                        Internal reserved - do not modify
                        
  */
      unsigned int   reserved_0 : 16;    /* 1E.C420.F:0  R/WPD      Provisionable Default = 0x0000 */
                     /* Internal reserved - do not modify
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Thermal Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C421.F:0 R/WPD High Temp Failure Threshold [F:0]
                        AQ_GlobalThermalProvisioning_HHD.u1.bits_1.highTempFailureThreshold

                        Provisionable Default = 0x4600

                        [F:0] of high temperature failure threshold

                 <B>Notes:</B>
                        2's complement value with the LSB representing 1/256 of a degree Celsius. This corresponds to -40 degreesC = 0xD800. Default is 70 degreesC. 
                        
                        In XENPAK mode, F/W will use the XENPAK register 1.A000 - 1.A001: instead of this register.
                        
                        NOTE! All Thresholds are orthogonal and can be set to any value regardless the value of the other thresholds. i.e. High-Temperature-Warning (1E.C423) could be higher than High-Temperature-Failure (1E.C421).  */
      unsigned int   highTempFailureThreshold : 16;    /* 1E.C421.F:0  R/WPD      Provisionable Default = 0x4600 */
                     /* [F:0] of high temperature failure threshold  */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global Thermal Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C422.F:0 R/WPD Low Temp Failure Threshold [F:0]
                        AQ_GlobalThermalProvisioning_HHD.u2.bits_2.lowTempFailureThreshold

                        Provisionable Default = 0x0000

                        [F:0] of low temperature failure threshold

                 <B>Notes:</B>
                        2's complement value with the LSB representing 1/256 of a degree Celsius. This corresponds to -40 degreesC = 0xD800. Default is 0 degreesC. 
                        
                        In XENPAK mode, F/W will use the XENPAK register 1.A002 - 1.A003: instead of this register.
                        
                        NOTE! All Thresholds are orthogonal and can be set to any value regardless the value of the other thresholds. i.e. High-Temperature-Warning (1E.C423) could be higher than High-Temperature-Failure (1E.C421).  */
      unsigned int   lowTempFailureThreshold : 16;    /* 1E.C422.F:0  R/WPD      Provisionable Default = 0x0000 */
                     /* [F:0] of low temperature failure threshold  */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global Thermal Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C423.F:0 R/WPD High Temp Warning Threshold [F:0]
                        AQ_GlobalThermalProvisioning_HHD.u3.bits_3.highTempWarningThreshold

                        Provisionable Default = 0x3C00

                        [F:0] of high temperature warning threshold

                 <B>Notes:</B>
                        2's complement value with the LSB representing 1/256 of a degree Celsius. This corresponds to -40 degreesC = 0xD008. Default is 60 degreesC. 
                        
                        In XENPAK mode, F/W will use the XENPAK register 1.A004 - 1.A005: instead of this register.
                        
                        NOTE! All Thresholds are orthogonal and can be set to any value regardless the value of the other thresholds. i.e. High-Temperature-Warning (1E.C423) could be higher than High-Temperature-Failure (1E.C421).  */
      unsigned int   highTempWarningThreshold : 16;    /* 1E.C423.F:0  R/WPD      Provisionable Default = 0x3C00 */
                     /* [F:0] of high temperature warning threshold  */
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of Global Thermal Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C424.F:0 R/WPD Low Temp Warning Threshold [F:0]
                        AQ_GlobalThermalProvisioning_HHD.u4.bits_4.lowTempWarningThreshold

                        Provisionable Default = 0x0A00

                        [F:0] of low temperature warning threshold

                 <B>Notes:</B>
                        2's complement value with the LSB representing 1/256 of a degree Celsius. This corresponds to -40 degreesC = 0xD800. Default is 10 degreesC. 
                        
                        In XENPAK mode, F/W will use the XENPAK register 1.A006 - 1.A007: instead of this register.
                        
                        NOTE! All Thresholds are orthogonal and can be set to any value regardless the value of the other thresholds. i.e. High-Temperature-Warning (1E.C423) could be higher than High-Temperature-Failure (1E.C421).  */
      unsigned int   lowTempWarningThreshold : 16;    /* 1E.C424.F:0  R/WPD      Provisionable Default = 0x0A00 */
                     /* [F:0] of low temperature warning threshold  */
    } bits_4;
    uint16_t word_4;
  } u4;
} AQ_GlobalThermalProvisioning_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global LED Provisioning: 1E.C430 */
/*                  Global LED Provisioning: 1E.C430 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global LED Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C430.1:0 R/WPD LED #0 Activity Stretch [1:0]
                        AQ_GlobalLedProvisioning_HHD.u0.bits_0.led_0ActivityStretch

                        Provisionable Default = 0x3

                        [1:0]
                        0x3 = stretch activity by 100 ms
                        0x2 = stretch activity by 60 ms
                        0x1 = stretch activity by 28 ms
                        0x0 = no stretching
                        
  */
      unsigned int   led_0ActivityStretch : 2;    /* 1E.C430.1:0  R/WPD      Provisionable Default = 0x3 */
                     /* [1:0]
                        0x3 = stretch activity by 100 ms
                        0x2 = stretch activity by 60 ms
                        0x1 = stretch activity by 28 ms
                        0x0 = no stretching
                          */
                    /*! \brief 1E.C430.2 R/WPD LED #0 Transmit Activity
                        AQ_GlobalLedProvisioning_HHD.u0.bits_0.led_0TransmitActivity

                        Provisionable Default = 0x0

                        1 = LED toggles on transmit activity
                        
  */
      unsigned int   led_0TransmitActivity : 1;    /* 1E.C430.2  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED toggles on transmit activity
                          */
                    /*! \brief 1E.C430.3 R/WPD LED #0 Receive Activity
                        AQ_GlobalLedProvisioning_HHD.u0.bits_0.led_0ReceiveActivity

                        Provisionable Default = 0x0

                        1 = LED toggles on receive activity
                        
  */
      unsigned int   led_0ReceiveActivity : 1;    /* 1E.C430.3  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED toggles on receive activity
                          */
                    /*! \brief 1E.C430.4 R/WPD LED #0 Connecting
                        AQ_GlobalLedProvisioning_HHD.u0.bits_0.led_0Connecting

                        Provisionable Default = 0x0

                        1 = LED is on when attempting to connect.
                        
  */
      unsigned int   led_0Connecting : 1;    /* 1E.C430.4  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when attempting to connect.
                          */
                    /*! \brief 1E.C430.5 R/WPD LED #0 100 Mb/s Link Established
                        AQ_GlobalLedProvisioning_HHD.u0.bits_0.led_0_100Mb_sLinkEstablished

                        Provisionable Default = 0x0

                        1 = LED is on when link connects at 100 Mb/s.
                        
  */
      unsigned int   led_0_100Mb_sLinkEstablished : 1;    /* 1E.C430.5  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when link connects at 100 Mb/s.
                          */
                    /*! \brief 1E.C430.6 R/WPD LED #0 1 Gb/s Link Established
                        AQ_GlobalLedProvisioning_HHD.u0.bits_0.led_0_1Gb_sLinkEstablished

                        Provisionable Default = 0x0

                        1 = LED is on when link connects at 1 Gb/s
                        
  */
      unsigned int   led_0_1Gb_sLinkEstablished : 1;    /* 1E.C430.6  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when link connects at 1 Gb/s
                          */
                    /*! \brief 1E.C430.7 R/WPD LED #0 10 Gb/s Link Established
                        AQ_GlobalLedProvisioning_HHD.u0.bits_0.led_0_10Gb_sLinkEstablished

                        Provisionable Default = 0x0

                        1 = LED is on when link connects at 10 Gb/s
                        
  */
      unsigned int   led_0_10Gb_sLinkEstablished : 1;    /* 1E.C430.7  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when link connects at 10 Gb/s
                          */
                    /*! \brief 1E.C430.8 R/WPD LED #0 Manual Set
                        AQ_GlobalLedProvisioning_HHD.u0.bits_0.led_0ManualSet

                        Provisionable Default = 0x0

                        1 = LED On
                        
  */
      unsigned int   led_0ManualSet : 1;    /* 1E.C430.8  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED On
                          */
                    /*! \brief 1E.C430.D:9 R/WPD Reserved Provisioning C430 [4:0]
                        AQ_GlobalLedProvisioning_HHD.u0.bits_0.reservedProvisioningC430

                        Provisionable Default = 0x00

                        Reserved for future use
  */
      unsigned int   reservedProvisioningC430 : 5;    /* 1E.C430.D:9  R/WPD      Provisionable Default = 0x00 */
                     /* Reserved for future use  */
      unsigned int   reserved0 : 2;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global LED Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C431.1:0 R/WPD LED #1 Activity Stretch [1:0]
                        AQ_GlobalLedProvisioning_HHD.u1.bits_1.led_1ActivityStretch

                        Provisionable Default = 0x3

                        [1:0]
                        0x3 = stretch activity by 100 ms
                        0x2 = stretch activity by 60 ms
                        0x1 = stretch activity by 28 ms
                        0x0 = no stretching
                        
  */
      unsigned int   led_1ActivityStretch : 2;    /* 1E.C431.1:0  R/WPD      Provisionable Default = 0x3 */
                     /* [1:0]
                        0x3 = stretch activity by 100 ms
                        0x2 = stretch activity by 60 ms
                        0x1 = stretch activity by 28 ms
                        0x0 = no stretching
                          */
                    /*! \brief 1E.C431.2 R/WPD LED #1 Transmit Activity
                        AQ_GlobalLedProvisioning_HHD.u1.bits_1.led_1TransmitActivity

                        Provisionable Default = 0x0

                        1 = LED toggles on transmit activity
                        
  */
      unsigned int   led_1TransmitActivity : 1;    /* 1E.C431.2  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED toggles on transmit activity
                          */
                    /*! \brief 1E.C431.3 R/WPD LED #1 Receive Activity
                        AQ_GlobalLedProvisioning_HHD.u1.bits_1.led_1ReceiveActivity

                        Provisionable Default = 0x0

                        1 = LED toggles on receive activity
                        
  */
      unsigned int   led_1ReceiveActivity : 1;    /* 1E.C431.3  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED toggles on receive activity
                          */
                    /*! \brief 1E.C431.4 R/WPD LED #1 Connecting
                        AQ_GlobalLedProvisioning_HHD.u1.bits_1.led_1Connecting

                        Provisionable Default = 0x0

                        1 = LED is on when attempting to connect.
                        
  */
      unsigned int   led_1Connecting : 1;    /* 1E.C431.4  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when attempting to connect.
                          */
                    /*! \brief 1E.C431.5 R/WPD LED #1 100 Mb/s Link Established
                        AQ_GlobalLedProvisioning_HHD.u1.bits_1.led_1_100Mb_sLinkEstablished

                        Provisionable Default = 0x0

                        1 = LED is on when link connects at 100 Mb/s.
                        
  */
      unsigned int   led_1_100Mb_sLinkEstablished : 1;    /* 1E.C431.5  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when link connects at 100 Mb/s.
                          */
                    /*! \brief 1E.C431.6 R/WPD LED #1 1 Gb/s Link Established
                        AQ_GlobalLedProvisioning_HHD.u1.bits_1.led_1_1Gb_sLinkEstablished

                        Provisionable Default = 0x0

                        1 = LED is on when link connects at 1 Gb/s
                        
  */
      unsigned int   led_1_1Gb_sLinkEstablished : 1;    /* 1E.C431.6  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when link connects at 1 Gb/s
                          */
                    /*! \brief 1E.C431.7 R/WPD LED #1 10 Gb/s Link Established
                        AQ_GlobalLedProvisioning_HHD.u1.bits_1.led_1_10Gb_sLinkEstablished

                        Provisionable Default = 0x0

                        1 = LED is on when link connects at 10 Gb/s
                        
  */
      unsigned int   led_1_10Gb_sLinkEstablished : 1;    /* 1E.C431.7  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when link connects at 10 Gb/s
                          */
                    /*! \brief 1E.C431.8 R/WPD LED #1 Manual Set
                        AQ_GlobalLedProvisioning_HHD.u1.bits_1.led_1ManualSet

                        Provisionable Default = 0x0

                        1 = LED On
                        
  */
      unsigned int   led_1ManualSet : 1;    /* 1E.C431.8  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED On
                          */
                    /*! \brief 1E.C431.D:9 R/WPD Reserved Provisioning C431 [4:0]
                        AQ_GlobalLedProvisioning_HHD.u1.bits_1.reservedProvisioningC431

                        Provisionable Default = 0x00

                        Reserved for future use
  */
      unsigned int   reservedProvisioningC431 : 5;    /* 1E.C431.D:9  R/WPD      Provisionable Default = 0x00 */
                     /* Reserved for future use  */
      unsigned int   reserved0 : 2;
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global LED Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C432.1:0 R/WPD LED #2 Activity Stretch [1:0]
                        AQ_GlobalLedProvisioning_HHD.u2.bits_2.led_2ActivityStretch

                        Provisionable Default = 0x3

                        [1:0]
                        0x3 = stretch activity by 100 ms
                        0x2 = stretch activity by 60 ms
                        0x1 = stretch activity by 28 ms
                        0x0 = no stretching
                        
  */
      unsigned int   led_2ActivityStretch : 2;    /* 1E.C432.1:0  R/WPD      Provisionable Default = 0x3 */
                     /* [1:0]
                        0x3 = stretch activity by 100 ms
                        0x2 = stretch activity by 60 ms
                        0x1 = stretch activity by 28 ms
                        0x0 = no stretching
                          */
                    /*! \brief 1E.C432.2 R/WPD LED #2 Transmit Activity
                        AQ_GlobalLedProvisioning_HHD.u2.bits_2.led_2TransmitActivity

                        Provisionable Default = 0x0

                        1 = LED toggles on transmit activity
                        
  */
      unsigned int   led_2TransmitActivity : 1;    /* 1E.C432.2  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED toggles on transmit activity
                          */
                    /*! \brief 1E.C432.3 R/WPD LED #2 Receive Activity
                        AQ_GlobalLedProvisioning_HHD.u2.bits_2.led_2ReceiveActivity

                        Provisionable Default = 0x0

                        1 = LED toggles on receive activity
                        
  */
      unsigned int   led_2ReceiveActivity : 1;    /* 1E.C432.3  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED toggles on receive activity
                          */
                    /*! \brief 1E.C432.4 R/WPD LED #2 Connecting
                        AQ_GlobalLedProvisioning_HHD.u2.bits_2.led_2Connecting

                        Provisionable Default = 0x0

                        1 = LED is on when attempting to connect.
                        
  */
      unsigned int   led_2Connecting : 1;    /* 1E.C432.4  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when attempting to connect.
                          */
                    /*! \brief 1E.C432.5 R/WPD LED #2 100 Mb/s Link Established
                        AQ_GlobalLedProvisioning_HHD.u2.bits_2.led_2_100Mb_sLinkEstablished

                        Provisionable Default = 0x0

                        1 = LED is on when link connects at 100 Mb/s.
  */
      unsigned int   led_2_100Mb_sLinkEstablished : 1;    /* 1E.C432.5  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when link connects at 100 Mb/s.  */
                    /*! \brief 1E.C432.6 R/WPD LED #2 1 Gb/s Link Established
                        AQ_GlobalLedProvisioning_HHD.u2.bits_2.led_2_1Gb_sLinkEstablished

                        Provisionable Default = 0x0

                        1 = LED is on when link connects at 1 Gb/s
                        
  */
      unsigned int   led_2_1Gb_sLinkEstablished : 1;    /* 1E.C432.6  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when link connects at 1 Gb/s
                          */
                    /*! \brief 1E.C432.7 R/WPD LED #2 10 Gb/s Link Established
                        AQ_GlobalLedProvisioning_HHD.u2.bits_2.led_2_10Gb_sLinkEstablished

                        Provisionable Default = 0x0

                        1 = LED is on when link connects at 10 Gb/s
                        
  */
      unsigned int   led_2_10Gb_sLinkEstablished : 1;    /* 1E.C432.7  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED is on when link connects at 10 Gb/s
                          */
                    /*! \brief 1E.C432.8 R/WPD LED #2 Manual Set
                        AQ_GlobalLedProvisioning_HHD.u2.bits_2.led_2ManualSet

                        Provisionable Default = 0x0

                        1 = LED On
                        
  */
      unsigned int   led_2ManualSet : 1;    /* 1E.C432.8  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED On
                          */
                    /*! \brief 1E.C432.D:9 R/WPD Reserved Provisioning C432 [4:0]
                        AQ_GlobalLedProvisioning_HHD.u2.bits_2.reservedProvisioningC432

                        Provisionable Default = 0x00

                        Reserved for future use
  */
      unsigned int   reservedProvisioningC432 : 5;    /* 1E.C432.D:9  R/WPD      Provisionable Default = 0x00 */
                     /* Reserved for future use  */
      unsigned int   reserved0 : 2;
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_5;
    uint16_t word_5;
  } u5;
  /*! \brief Union for bit and word level access of word 6 of Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_6;
    uint16_t word_6;
  } u6;
  /*! \brief Union for bit and word level access of word 7 of Global LED Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C437.0 R/WPD LED Operation Mode
                        AQ_GlobalLedProvisioning_HHD.u7.bits_7.ledOperationMode

                        Provisionable Default = 0x0

                        1 = LED link activity in Mode #2
                        0 = LED link activity in Aquantia classic mode
                        

                 <B>Notes:</B>
                        When set to 1, the LED blinking rate is based on Mode #2 algorithm. When set to 0, the LED blinking rate is based on the classic Aquantia algorithm.  */
      unsigned int   ledOperationMode : 1;    /* 1E.C437.0  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = LED link activity in Mode #2
                        0 = LED link activity in Aquantia classic mode
                          */
      unsigned int   reserved0 : 15;
    } bits_7;
    uint16_t word_7;
  } u7;
  /*! \brief Dummy union to fill space in the structure Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved : 16;
    } bits_8;
    uint16_t word_8;
  } u8;
  /*! \brief Dummy union to fill space in the structure Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved : 16;
    } bits_9;
    uint16_t word_9;
  } u9;
  /*! \brief Dummy union to fill space in the structure Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved : 16;
    } bits_10;
    uint16_t word_10;
  } u10;
  /*! \brief Dummy union to fill space in the structure Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved : 16;
    } bits_11;
    uint16_t word_11;
  } u11;
  /*! \brief Dummy union to fill space in the structure Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved : 16;
    } bits_12;
    uint16_t word_12;
  } u12;
  /*! \brief Dummy union to fill space in the structure Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved : 16;
    } bits_13;
    uint16_t word_13;
  } u13;
  /*! \brief Union for bit and word level access of word 14 of Global LED Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_14;
    uint16_t word_14;
  } u14;
} AQ_GlobalLedProvisioning_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global General Provisioning: 1E.C440 */
/*                  Global General Provisioning: 1E.C440 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global General Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global General Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved3 : 2;
      unsigned int   reserved2 : 1;
                    /*! \brief 1E.C441.3 R/WPD MDIO Preamble Detection Disable
                        AQ_GlobalGeneralProvisioning_HHD.u1.bits_1.mdioPreambleDetectionDisable

                        Provisionable Default = 0x0

                        1 = Suppress preamble detection on MDIO
                        0 = Enable preamble detection on MDIO
                        
  */
      unsigned int   mdioPreambleDetectionDisable : 1;    /* 1E.C441.3  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Suppress preamble detection on MDIO
                        0 = Enable preamble detection on MDIO
                          */
                    /*! \brief 1E.C441.4 R/WPD MDIO Drive Configuration
                        AQ_GlobalGeneralProvisioning_HHD.u1.bits_1.mdioDriveConfiguration

                        Provisionable Default = 0x0

                        0 = MDIO driver is in normal mode
                        1 = MDIO driver is in open drain mode
                        

                 <B>Notes:</B>
                        When the MDIO driver is in open drain mode during a read cycle, "0" data will be actively driven out of the MDIO, "1" data will set the MDIO driver in high impedance state and an external pullup will set the MDIO line to "1". The Turn-Around "0" will also be actively driven out of the MDIO, therefore in open drain mode, the Turn-Around is still "Z0".  */
      unsigned int   mdioDriveConfiguration : 1;    /* 1E.C441.4  R/WPD      Provisionable Default = 0x0 */
                     /* 0 = MDIO driver is in normal mode
                        1 = MDIO driver is in open drain mode
                          */
      unsigned int   reserved1 : 8;
                    /*! \brief 1E.C441.D R/WPD MDIO Read MSW First Enable
                        AQ_GlobalGeneralProvisioning_HHD.u1.bits_1.mdioReadMSW_FirstEnable

                        Provisionable Default = 0x0

                        1 = MSW of counter must be read first
                        0 = LSW of counter must be read first
                        

                 <B>Notes:</B>
                        This bit configures whether the MSW or LSW must be read first for counters greater than 16 bits.  */
      unsigned int   mdioReadMSW_FirstEnable : 1;    /* 1E.C441.D  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = MSW of counter must be read first
                        0 = LSW of counter must be read first
                          */
                    /*! \brief 1E.C441.E R/WPD MDIO Broadcast Mode Enable
                        AQ_GlobalGeneralProvisioning_HHD.u1.bits_1.mdioBroadcastModeEnable

                        Provisionable Default = 0x0

                        1 = Enable broadcast on address set in 1E.C446
                        0 = Disable broadcast on n address set in 1E.C446
                        

                 <B>Notes:</B>
                        When enabled, writes and load MMD address opcodes are supported. Read opcodes are ignored.  */
      unsigned int   mdioBroadcastModeEnable : 1;    /* 1E.C441.E  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable broadcast on address set in 1E.C446
                        0 = Disable broadcast on n address set in 1E.C446
                          */
      unsigned int   reserved0 : 1;
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global General Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C442.0 R/W Daisy Chain Reset
                        AQ_GlobalGeneralProvisioning_HHD.u2.bits_2.daisyChainReset

                        Default = 0x0

                        1 = Reset the daisy chain
                        

                 <B>Notes:</B>
                        Toggling this bit from 0 to 1 will reload the IRAM and DRAM and reset the uP. The uP will be in uP run stall during the reload process. After the reload process, uP run stall will be de-asserted and the uP reset will be asserted. Note that before setting this bit, the  See Soft Reset bit needs to be de-asserted.  */
      unsigned int   daisyChainReset : 1;    /* 1E.C442.0  R/W      Default = 0x0 */
                     /* 1 = Reset the daisy chain
                          */
      unsigned int   reserved0 : 15;
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global General Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of Global General Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of Global General Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_5;
    uint16_t word_5;
  } u5;
  /*! \brief Union for bit and word level access of word 6 of Global General Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_6;
    uint16_t word_6;
  } u6;
  /*! \brief Union for bit and word level access of word 7 of Global General Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C447.4:0 R/WPD MDIO Broadcast Address Configuration [4:0]
                        AQ_GlobalGeneralProvisioning_HHD.u7.bits_7.mdioBroadcastAddressConfiguration

                        Provisionable Default = 0x1F

                        Broadcast address
                        

                 <B>Notes:</B>
                        Allows setting the broadcast address. By default this is set to 0x1F  */
      unsigned int   mdioBroadcastAddressConfiguration : 5;    /* 1E.C447.4:0  R/WPD      Provisionable Default = 0x1F */
                     /* Broadcast address
                          */
      unsigned int   reserved0 : 11;
    } bits_7;
    uint16_t word_7;
  } u7;
  /*! \brief Union for bit and word level access of word 8 of Global General Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved0 : 16;
    } bits_8;
    uint16_t word_8;
  } u8;
  /*! \brief Union for bit and word level access of word 9 of Global General Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C449.6:0 R/W MDIO Preamble Length [6:0]
                        AQ_GlobalGeneralProvisioning_HHD.u9.bits_9.mdioPreambleLength

                        Default = 0x02

                        MDIO Preamble Length
                        
  */
      unsigned int   mdioPreambleLength : 7;    /* 1E.C449.6:0  R/W      Default = 0x02 */
                     /* MDIO Preamble Length
                          */
      unsigned int   reserved0 : 9;
    } bits_9;
    uint16_t word_9;
  } u9;
} AQ_GlobalGeneralProvisioning_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global NVR Provisioning: 1E.C450 */
/*                  Global NVR Provisioning: 1E.C450 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global NVR Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C450.1:0 R/WPD NVR Address Length [1:0]
                        AQ_GlobalNvrProvisioning_HHD.u0.bits_0.nvrAddressLength

                        Provisionable Default = 0x2

                        NVR address length ranges from 0 bytes up to 3 bytes
                        

                 <B>Notes:</B>
                        This sets the length of the address field used in read and write operations. Use of this field is enabled via Bit 8 of  See Global NVR Provisioning 2: Address 1E.C451 .
                          */
      unsigned int   nvrAddressLength : 2;    /* 1E.C450.1:0  R/WPD      Provisionable Default = 0x2 */
                     /* NVR address length ranges from 0 bytes up to 3 bytes
                          */
      unsigned int   reserved2 : 2;
                    /*! \brief 1E.C450.6:4 R/WPD NVR Dummy Length [2:0]
                        AQ_GlobalNvrProvisioning_HHD.u0.bits_0.nvrDummyLength

                        Provisionable Default = 0x0

                        NVR dummy length ranges from 0 bytes to 4 bytes
                        

                 <B>Notes:</B>
                        This sets the length of the dummy field used in some manufacturer's read status and write status operations.
                          */
      unsigned int   nvrDummyLength : 3;    /* 1E.C450.6:4  R/WPD      Provisionable Default = 0x0 */
                     /* NVR dummy length ranges from 0 bytes to 4 bytes
                          */
      unsigned int   reserved1 : 1;
                    /*! \brief 1E.C450.A:8 R/WPD NVR Data Length [2:0]
                        AQ_GlobalNvrProvisioning_HHD.u0.bits_0.nvrDataLength

                        Provisionable Default = 0x4

                        NVR data length ranges from 0 bytes to 4 bytes
                        

                 <B>Notes:</B>
                        This sets the length of the data burst used in read and write operations.
                          */
      unsigned int   nvrDataLength : 3;    /* 1E.C450.A:8  R/WPD      Provisionable Default = 0x4 */
                     /* NVR data length ranges from 0 bytes to 4 bytes
                          */
      unsigned int   reserved0 : 5;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global NVR Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C451.7:0 R/WPD NVR Clock Divide [7:0]
                        AQ_GlobalNvrProvisioning_HHD.u1.bits_1.nvrClockDivide

                        Provisionable Default = 0xA0

                        NVR clock divide. Clock frequency is divided by the NVR clock divide + 1
                        
  */
      unsigned int   nvrClockDivide : 8;    /* 1E.C451.7:0  R/WPD      Provisionable Default = 0xA0 */
                     /* NVR clock divide. Clock frequency is divided by the NVR clock divide + 1
                          */
                    /*! \brief 1E.C451.8 R/WPD NVR Address Length Override
                        AQ_GlobalNvrProvisioning_HHD.u1.bits_1.nvrAddressLengthOverride

                        Provisionable Default = 0x0

                        0 = NVR address length is based on the "NVR_SIZE" pin.
                        1 = NVR address length is based on the  See NVR Address Length [1:0]  register
                        

                 <B>Notes:</B>
                        When this bit = 0 and NVR_SIZE pin = 0, the NVR address length is 2 bytes. When this bit = 0 and the NVR_SIZE pin = 1, the NVR address length is 3 bytes. When this bit = 1 the NVR address length is from the  See NVR Address Length [1:0]   */
      unsigned int   nvrAddressLengthOverride : 1;    /* 1E.C451.8  R/WPD      Provisionable Default = 0x0 */
                     /* 0 = NVR address length is based on the "NVR_SIZE" pin.
                        1 = NVR address length is based on the  See NVR Address Length [1:0]  register
                          */
      unsigned int   reserved0 : 7;
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global NVR Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C452.0 R/W NVR Daisy Chain Disable
                        AQ_GlobalNvrProvisioning_HHD.u2.bits_2.nvrDaisyChainDisable

                        Default = 0x0

                        1 = Disable the Daisy Chain
                        

                 <B>Notes:</B>
                        When in daisy chain master mode, the daisy chain and MDIO can both access the SPI. Setting this bit to 1 will disable the daisy chain from accessing the SPI and force it into a reset state.  */
      unsigned int   nvrDaisyChainDisable : 1;    /* 1E.C452.0  R/W      Default = 0x0 */
                     /* 1 = Disable the Daisy Chain
                          */
                    /*! \brief 1E.C452.1 R/W NVR Daisy Chain Clock Divide Override
                        AQ_GlobalNvrProvisioning_HHD.u2.bits_2.nvrDaisyChainClockDivideOverride

                        Default = 0x0

                        1 = Override NVR clock divide when in daisy chain master mode
                        

                 <B>Notes:</B>
                        When in daisy chain master mode, the clock divide configuration is received from the FLASH. This bit will override the clock divide configuration from the FLASH with the  See NVR Clock Divide [7:0] .  */
      unsigned int   nvrDaisyChainClockDivideOverride : 1;    /* 1E.C452.1  R/W      Default = 0x0 */
                     /* 1 = Override NVR clock divide when in daisy chain master mode
                          */
      unsigned int   reserved0 : 14;
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global NVR Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved1 : 4;
                    /*! \brief 1E.C453.4 R/W NVR Reset
                        AQ_GlobalNvrProvisioning_HHD.u3.bits_3.nvrReset

                        Default = 0x0

                        1 = Reset SPI
                        
  */
      unsigned int   nvrReset : 1;    /* 1E.C453.4  R/W      Default = 0x0 */
                     /* 1 = Reset SPI
                          */
      unsigned int   reserved0 : 11;
    } bits_3;
    uint16_t word_3;
  } u3;
} AQ_GlobalNvrProvisioning_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Reserved Provisioning: 1E.C470 */
/*                  Global Reserved Provisioning: 1E.C470 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Reserved Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved2 : 4;
                    /*! \brief 1E.C470.4 R/WSC Initiate Cable Diagnostics
                        AQ_GlobalReservedProvisioning_HHD.u0.bits_0.initiateCableDiagnostics

                        Default = 0x0

                        1 = Perform cable diagnostics
                        

                 <B>Notes:</B>
                        Perform cable diagnostics regardless of link state. If link is up, setting this bit will cause the link to drop while diagnostics are performed. This bit is self-clearing upon completion of the cable diagnostics.
                        
                        NOTE!! This is a processor intensive operation. Completion of this operation can also be monitored via 1E.C831.F  */
      unsigned int   initiateCableDiagnostics : 1;    /* 1E.C470.4  R/WSC      Default = 0x0 */
                     /* 1 = Perform cable diagnostics
                          */
      unsigned int   reserved1 : 3;
      unsigned int   reserved0 : 5;
                    /*! \brief 1E.C470.E:D R/WPD Extended MDI Diagnostics Select [1:0]
                        AQ_GlobalReservedProvisioning_HHD.u0.bits_0.extendedMdiDiagnosticsSelect

                        Provisionable Default = 0x0

                        0x0 = TDR Data
                        0x1 = RFI Channel PSD
                        0x2 = Noise PSD while the local Tx is Off
                        0x3 = Noise PSD while the local Tx is On
                        

                 <B>Notes:</B>
                        These bits select what sort of cable diagnostics to perform. For regular cable diagnostics, Bit F is set to zero, and the diagnostics are triggered by setting Bit 4. For extended diagnostics, Bit F is set to 1, and the desired extended diagnostics are selected by Bits E:D. The routine is then triggered by setting Bit 4. Each of the extended diagnostic routines present data for all for MDI pairs (A, B, C, D) consecutively, and after the data for each channel is gathered Bits F:D are reset. To get the data for the next pair, Bits F:D must be set back to the desired value (which must be the same as the initial channel). This continues until the data for all channels has been gathered. The address in memory where the data is stored is given in 1E.C802 and 1E.C804.
                        
                        For the case of PSD, the structure is as follows:
                        Int32      info
                        Int16      data[Len]
                        Info = Len << 16 | TxEnable << 8 | Pair (0 = A, etc.)
                        
                        For TDR:
                        Int32      info
                        Int16      tdr_A[Len]
                        Int16      tdr_B[Len]
                        Int16      tdr_C[Len]
                        Int16      tdr_D[Len]
                        
                        Info = Len << 16 | Channel
                        
                        TDR data is from the current pair to all other pairs.
                        
                        At the end of retrieving extended MDI diag data, the part will be reset. Conversely the only way to exit this routine once it starts is to issue a PMA reset.  */
      unsigned int   extendedMdiDiagnosticsSelect : 2;    /* 1E.C470.E:D  R/WPD      Provisionable Default = 0x0 */
                     /* 0x0 = TDR Data
                        0x1 = RFI Channel PSD
                        0x2 = Noise PSD while the local Tx is Off
                        0x3 = Noise PSD while the local Tx is On
                          */
                    /*! \brief 1E.C470.F R/WPD Diagnostics Select
                        AQ_GlobalReservedProvisioning_HHD.u0.bits_0.diagnosticsSelect

                        Provisionable Default = 0x0

                        1 = Provide Extended MDI Diagnostics Information.
                        0 = Provide normal cable diagnostics
                        

                 <B>Notes:</B>
                        These bits select what sort of cable diagnostics to perform. For regular cable diagnostics, Bit F is set to zero, and the diagnostics are triggered by setting Bit 4. For extended diagnostics, Bit F is set to 1, and the desired extended diagnostics are selected by Bits E:D. The routine is then triggered by setting Bit 4. Each of the extended diagnostic routines present data for all for MDI pairs (A, B, C, D) consecutively, and after the data for each channel is gathered Bits F:D are reset. To get the data for the next pair, Bits F:D must be set back to the desired value (which must be the same as the initial channel). This continues until the data for all channels has been gathered. The address in memory where the data is stored is given in 1E.C802 and 1E.C804.
                        
                        For the case of PSD, the structure is as follows:
                        Int32      info
                        Int16      data[Len]
                        Info = Len << 16 | TxEnable << 8 | Pair (0 = A, etc.)
                        
                        For TDR:
                        Int32      info
                        Int16      tdr_A[Len]
                        Int16      tdr_B[Len]
                        Int16      tdr_C[Len]
                        Int16      tdr_D[Len]
                        
                        Info = Len << 16 | Channel
                        
                        TDR data is from the current pair to all other pairs.
                        
                        At the end of retrieving extended MDI diag data, the part will be reset. Conversely the only way to exit this routine once it starts is to issue a PMA reset.  */
      unsigned int   diagnosticsSelect : 1;    /* 1E.C470.F  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Provide Extended MDI Diagnostics Information.
                        0 = Provide normal cable diagnostics
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Reserved Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C471.5:0 R/WuP Daisy-Chain Hop-Count Override Value [5:0]
                        AQ_GlobalReservedProvisioning_HHD.u1.bits_1.daisy_chainHop_countOverrideValue

                        Default = 0x00

                        The value to use for the PHY's daisy-chain hop-count. Valid values are from 0 -> 47
                        

                 <B>Notes:</B>
                        Daisy-Chain Hop-Count Override should be used during MDIO boot-load operation, as the daisy-chain hop-count does not function when the daisy-chain is disabled (1E.C452.0). Setting this bit tells the processor where in the daisy-chain it is, so that the provisioning operation will function correctly.  */
      unsigned int   daisy_chainHop_countOverrideValue : 6;    /* 1E.C471.5:0  R/WuP      Default = 0x00 */
                     /* The value to use for the PHY's daisy-chain hop-count. Valid values are from 0 -> 47
                          */
                    /*! \brief 1E.C471.6 R/WuP Enable Daisy-Chain Hop-Count Override
                        AQ_GlobalReservedProvisioning_HHD.u1.bits_1.enableDaisy_chainHop_countOverride

                        Default = 0x0

                        1 = Hop-count is set by Bits 5:0
                        0 = Hop-count is determined by the daisy-chain
                        

                 <B>Notes:</B>
                        Daisy-Chain Hop-Count Override should be used during MDIO boot-load operation, as the daisy-chain hop-count does not function when the daisy-chain is disabled (1E.C452.0). Setting this bit tells the processor where in the daisy-chain it is, so that the provisioning operation will function correctly.  */
      unsigned int   enableDaisy_chainHop_countOverride : 1;    /* 1E.C471.6  R/WuP      Default = 0x0 */
                     /* 1 = Hop-count is set by Bits 5:0
                        0 = Hop-count is determined by the daisy-chain
                          */
      unsigned int   reserved0 : 9;
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global Reserved Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C472.0 R/WPDuP Enable 5th Channel RFI Cancellation
                        AQ_GlobalReservedProvisioning_HHD.u2.bits_2.enable_5thChannelRfiCancellation

                        Provisionable Default = 0x0

                        1 = 5th channel and RFI cancellers operation enabled
                        0 = 5th channel AFE is powered down, 5th channel digital is clock gated, RFI cancellers are disabled
                        

                 <B>Notes:</B>
                        Note: The value of this bit at the time of Autonegotiation sets the local PHY behavior until the next time Autonegotiation occurs.  */
      unsigned int   enable_5thChannelRfiCancellation : 1;    /* 1E.C472.0  R/WPDuP      Provisionable Default = 0x0 */
                     /* 1 = 5th channel and RFI cancellers operation enabled
                        0 = 5th channel AFE is powered down, 5th channel digital is clock gated, RFI cancellers are disabled
                          */
                    /*! \brief 1E.C472.1 R/WPDuP Enable XENPAK Register Space
                        AQ_GlobalReservedProvisioning_HHD.u2.bits_2.enableXenpakRegisterSpace

                        Provisionable Default = 0x0

                        1 = XENPAK register space enabled
                        0 = XENPAK register space disabled
                        
  */
      unsigned int   enableXenpakRegisterSpace : 1;    /* 1E.C472.1  R/WPDuP      Provisionable Default = 0x0 */
                     /* 1 = XENPAK register space enabled
                        0 = XENPAK register space disabled
                          */
                    /*! \brief 1E.C472.5:2 R/WPD External VDD Change Request [3:0]
                        AQ_GlobalReservedProvisioning_HHD.u2.bits_2.externalVddChangeRequest

                        Provisionable Default = 0x0

                        The amount of VDD change requested by firmware, in mV (2's complement value).
                        
  */
      unsigned int   externalVddChangeRequest : 4;    /* 1E.C472.5:2  R/WPD      Provisionable Default = 0x0 */
                     /* The amount of VDD change requested by firmware, in mV (2's complement value).
                          */
                    /*! \brief 1E.C472.6 R/WPD Tunable External VDD Power Supply Present
                        AQ_GlobalReservedProvisioning_HHD.u2.bits_2.tunableExternalVddPowerSupplyPresent

                        Provisionable Default = 0x0

                        1 = Tunable external VDD power supply present
                        0 = No tunable external VDD power supply present
                        

                 <B>Notes:</B>
                        This bit must be set if tuning of external power supply is desired.  */
      unsigned int   tunableExternalVddPowerSupplyPresent : 1;    /* 1E.C472.6  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Tunable external VDD power supply present
                        0 = No tunable external VDD power supply present
                          */
      unsigned int   reserved1 : 7;
                    /*! \brief 1E.C472.E R/WPD Enable VDD Power Supply Tuning
                        AQ_GlobalReservedProvisioning_HHD.u2.bits_2.enableVddPowerSupplyTuning

                        Provisionable Default = 0x0

                        1 = Enable external VDD power supply tuning
                        0 = Disable external VDD power supply tuning is disabled
                        

                 <B>Notes:</B>
                        This bit controls whether the PHY attempts to tune the external VDD power supply via the SMBus. This bit is only operational if the external supply is present. (See 1E.C472.6)  */
      unsigned int   enableVddPowerSupplyTuning : 1;    /* 1E.C472.E  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable external VDD power supply tuning
                        0 = Disable external VDD power supply tuning is disabled
                          */
      unsigned int   reserved0 : 1;
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global Reserved Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C473.7:0 R/WPD Training SNR [7:0]
                        AQ_GlobalReservedProvisioning_HHD.u3.bits_3.trainingSNR

                        Provisionable Default = 0x00

                        SNR during 10G training on the worst channel. SNR is in steps of 0.1dB
                        

                 <B>Notes:</B>
                        The SNR margin that is enjoyed by the worst channel, over and above the minimum SNR required to operate at a BER of 10-12. It is reported with 0.1 dB of resolution to an accuracy of 0.5 dB within the range of -12.7 dB to 12.7 dB. The number is in offset binary, with 0.0 dB represented by 0x8000.  */
      unsigned int   trainingSNR : 8;    /* 1E.C473.7:0  R/WPD      Provisionable Default = 0x00 */
                     /* SNR during 10G training on the worst channel. SNR is in steps of 0.1dB
                          */
                    /*! \brief 1E.C473.A:8 R/WPD Rate Transition Request [2:0]
                        AQ_GlobalReservedProvisioning_HHD.u3.bits_3.rateTransitionRequest

                        Provisionable Default = 0x0

                        0 = No Transition
                        1 = Reserved
                        2 = Reserved
                        3 = Retrain at 10G
                        4 = Retrain at 5G
                        5 = Retrain at 2.5G
                        6 = Retrain at 1G
                        7 = Reserved
                        
  */
      unsigned int   rateTransitionRequest : 3;    /* 1E.C473.A:8  R/WPD      Provisionable Default = 0x0 */
                     /* 0 = No Transition
                        1 = Reserved
                        2 = Reserved
                        3 = Retrain at 10G
                        4 = Retrain at 5G
                        5 = Retrain at 2.5G
                        6 = Retrain at 1G
                        7 = Reserved
                          */
      unsigned int   reserved0 : 5;
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of Global Reserved Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C474.0 R/W NVR Daisy Chain Kickstart
                        AQ_GlobalReservedProvisioning_HHD.u4.bits_4.nvrDaisyChainKickstart

                        Default = 0x0

                        1 = Kickstart the Daisy Chain
                        

                 <B>Notes:</B>
                        When in daisy chain master mode, the PHY0 can kickstart the daisy chain. The kickstart will not reload the IRAM/DRAM or reset the uP for PHY0. It will just read the FLASH and transfer the FLASH data to the daisy chain.  */
      unsigned int   nvrDaisyChainKickstart : 1;    /* 1E.C474.0  R/W      Default = 0x0 */
                     /* 1 = Kickstart the Daisy Chain
                          */
                    /*! \brief 1E.C474.F:1 R/WPD Reserved Provisioning 5 [F:1]
                        AQ_GlobalReservedProvisioning_HHD.u4.bits_4.reservedProvisioning_5

                        Provisionable Default = 0x0000

                        Reserved for future use
                        
  */
      unsigned int   reservedProvisioning_5 : 15;    /* 1E.C474.F:1  R/WPD      Provisionable Default = 0x0000 */
                     /* Reserved for future use
                          */
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of Global Reserved Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved1 : 2;
                    /*! \brief 1E.C475.2 R/WPD Smart Power-Down Enable
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.smartPower_downEnable

                        Provisionable Default = 0x0

                        1 = Enable smart power down mode
                        0 = Smart power-down mode disabled
                        

                 <B>Notes:</B>
                        Smart power down (SPD) is the lowest power mode at which PHY is able to autonegotiate. SPD can be enabled with bit 1E.C475.2  */
      unsigned int   smartPower_downEnable : 1;    /* 1E.C475.2  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable smart power down mode
                        0 = Smart power-down mode disabled
                          */
                    /*! \brief 1E.C475.3 R/WPD Deadlock Avoidance Enable
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.deadlockAvoidanceEnable

                        Provisionable Default = 0x0

                        1 = SPD with deadlock avoidance: PHY transmits autonegotiation pulses (FLPs) at a slower rate (~ 1 FLP/ 100ms) than specified by autonegotiation standard (~1 FLP / 8.25ms). Receiver is active and able to detect the pulses.
                        0 = SPD without deadlock avoidance: PHY transmitter is shut down, no autonegotiation pulses are sent on the line but the receiver is active and able to detect the pulses
                        
  */
      unsigned int   deadlockAvoidanceEnable : 1;    /* 1E.C475.3  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = SPD with deadlock avoidance: PHY transmits autonegotiation pulses (FLPs) at a slower rate (~ 1 FLP/ 100ms) than specified by autonegotiation standard (~1 FLP / 8.25ms). Receiver is active and able to detect the pulses.
                        0 = SPD without deadlock avoidance: PHY transmitter is shut down, no autonegotiation pulses are sent on the line but the receiver is active and able to detect the pulses
                          */
                    /*! \brief 1E.C475.4 R/WPD CFR Support
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.cfrSupport

                        Provisionable Default = 0x0

                        1 = Local PHY supports Cisco Fast Retrain
                        0 = Local PHY does support Cisco Fast Retrain
                        
  */
      unsigned int   cfrSupport : 1;    /* 1E.C475.4  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Local PHY supports Cisco Fast Retrain
                        0 = Local PHY does support Cisco Fast Retrain
                          */
                    /*! \brief 1E.C475.5 R/WPD CFR THP
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.cfrTHP

                        Provisionable Default = 0x0

                        1 = Local PHY requires local PHY to enable THP
                        0 = Local PHY does not require local PHY to enable THP
                        
  */
      unsigned int   cfrTHP : 1;    /* 1E.C475.5  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Local PHY requires local PHY to enable THP
                        0 = Local PHY does not require local PHY to enable THP
                          */
                    /*! \brief 1E.C475.6 R/WPD CFR Extended Maxwait
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.cfrExtendedMaxwait

                        Provisionable Default = 0x0

                        1 = Local PHY requires extended maxwait
                        0 = Local PHY does not require extended maxwait
                        
  */
      unsigned int   cfrExtendedMaxwait : 1;    /* 1E.C475.6  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Local PHY requires extended maxwait
                        0 = Local PHY does not require extended maxwait
                          */
                    /*! \brief 1E.C475.7 R/WPD CFR Disable Timer
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.cfrDisableTimer

                        Provisionable Default = 0x0

                        1 = Local PHY requires cfr_disable timer
                        0 = Local PHY does not require cfr_disable timer
                        
  */
      unsigned int   cfrDisableTimer : 1;    /* 1E.C475.7  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Local PHY requires cfr_disable timer
                        0 = Local PHY does not require cfr_disable timer
                          */
                    /*! \brief 1E.C475.8 R/WPD CFR LP Support
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.cfrLpSupport

                        Provisionable Default = 0x0

                        1 = Link partner supports Cisco Fast Retrain
                        0 = Link partner does support Cisco Fast Retrain
                        
  */
      unsigned int   cfrLpSupport : 1;    /* 1E.C475.8  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Link partner supports Cisco Fast Retrain
                        0 = Link partner does support Cisco Fast Retrain
                          */
                    /*! \brief 1E.C475.9 R/WPD CFR LP THP
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.cfrLpTHP

                        Provisionable Default = 0x0

                        1 = Link partner requires local PHY to enable THP
                        0 = Link partner does not require local PHY to enable THP
                        
  */
      unsigned int   cfrLpTHP : 1;    /* 1E.C475.9  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Link partner requires local PHY to enable THP
                        0 = Link partner does not require local PHY to enable THP
                          */
                    /*! \brief 1E.C475.A R/WPD CFR LP Extended Maxwait
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.cfrLpExtendedMaxwait

                        Provisionable Default = 0x0

                        1 = Link partner requires extended maxwait
                        0 = Link partner does not require extended maxwait
                        
  */
      unsigned int   cfrLpExtendedMaxwait : 1;    /* 1E.C475.A  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Link partner requires extended maxwait
                        0 = Link partner does not require extended maxwait
                          */
                    /*! \brief 1E.C475.B R/WPD CFR LP Disable Timer
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.cfrLpDisableTimer

                        Provisionable Default = 0x0

                        1 = Link partner requires cfr_disable timer
                        0 = Link partner does not require cfr_disable timer
                        
  */
      unsigned int   cfrLpDisableTimer : 1;    /* 1E.C475.B  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Link partner requires cfr_disable timer
                        0 = Link partner does not require cfr_disable timer
                          */
                    /*! \brief 1E.C475.C R/WPD Reserved Provisioning 6
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.reservedProvisioning_6

                        Provisionable Default = 0x0

                        Internal reserved - do not modify
                        
  */
      unsigned int   reservedProvisioning_6 : 1;    /* 1E.C475.C  R/WPD      Provisionable Default = 0x0 */
                     /* Internal reserved - do not modify
                          */
                    /*! \brief 1E.C475.D R/WPD Smart Power-Down Status
                        AQ_GlobalReservedProvisioning_HHD.u5.bits_5.smartPower_downStatus

                        Provisionable Default = 0x0

                        1 = Smart Power-Down Active
                        0 = Smart Power-Down Inactive
                        
  */
      unsigned int   smartPower_downStatus : 1;    /* 1E.C475.D  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Smart Power-Down Active
                        0 = Smart Power-Down Inactive
                          */
      unsigned int   reserved0 : 2;
    } bits_5;
    uint16_t word_5;
  } u5;
  /*! \brief Dummy union to fill space in the structure Global Reserved Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved : 16;
    } bits_6;
    uint16_t word_6;
  } u6;
  /*! \brief Dummy union to fill space in the structure Global Reserved Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved : 16;
    } bits_7;
    uint16_t word_7;
  } u7;
  /*! \brief Union for bit and word level access of word 8 of Global Reserved Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C478.A:0 R/WPD Reserved Provisioning 9 [A:0]
                        AQ_GlobalReservedProvisioning_HHD.u8.bits_8.reservedProvisioning_9

                        Provisionable Default = 0x000

                        Reserved for future use
                        
  */
      unsigned int   reservedProvisioning_9 : 11;    /* 1E.C478.A:0  R/WPD      Provisionable Default = 0x000 */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.C478.E:B R/WPD DTE Drop Reporting Timer [3:0]
                        AQ_GlobalReservedProvisioning_HHD.u8.bits_8.dteDropReportingTimer

                        Provisionable Default = 0x0

                        Number of seconds between loss of link partner filter and assertion of no-power-needed state, in 5 second increments (e.g. 0x4 = 20 seconds).
                        

                 <B>Notes:</B>
                        These bits are used to set how long the PHY waits after it no longer detects the link partner filter before declaring that power is not needed.  */
      unsigned int   dteDropReportingTimer : 4;    /* 1E.C478.E:B  R/WPD      Provisionable Default = 0x0 */
                     /* Number of seconds between loss of link partner filter and assertion of no-power-needed state, in 5 second increments (e.g. 0x4 = 20 seconds).
                          */
                    /*! \brief 1E.C478.F R/WPD DTE Enable
                        AQ_GlobalReservedProvisioning_HHD.u8.bits_8.dteEnable

                        Provisionable Default = 0x0

                        1 = Enable DTE
                        0 = Disable DTE
                        
  */
      unsigned int   dteEnable : 1;    /* 1E.C478.F  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable DTE
                        0 = Disable DTE
                          */
    } bits_8;
    uint16_t word_8;
  } u8;
  /*! \brief Union for bit and word level access of word 9 of Global Reserved Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C479.E:0 R/WPD Reserved Provisioning 10 [E:0]
                        AQ_GlobalReservedProvisioning_HHD.u9.bits_9.reservedProvisioning_10

                        Provisionable Default = 0x0000

                        Reserved for future use
                        
  */
      unsigned int   reservedProvisioning_10 : 15;    /* 1E.C479.E:0  R/WPD      Provisionable Default = 0x0000 */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.C479.F R/WPD Power Up Stall
                        AQ_GlobalReservedProvisioning_HHD.u9.bits_9.powerUpStall

                        Provisionable Default = 0x0

                        1 = Stall FW at Power Up
                        0 = Unstall the FW
                        

                 <B>Notes:</B>
                        This bit needs to be provisioned in Power Up Init for firmware to stall.  */
      unsigned int   powerUpStall : 1;    /* 1E.C479.F  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Stall FW at Power Up
                        0 = Unstall the FW
                          */
    } bits_9;
    uint16_t word_9;
  } u9;
  /*! \brief Union for bit and word level access of word 10 of Global Reserved Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C47A.1:0 R/WPD Rate [1:0]
                        AQ_GlobalReservedProvisioning_HHD.u10.bits_10.rate

                        Provisionable Default = 0x0

                        0x3 = 10G
                        0x2 = 1G
                        0x1 = 100M
                        0x0 = reserved
                        

                 <B>Notes:</B>
                        These bits select the rate for the loopback and packet generation. SERDES configuration, as well autonegotiation is controlled accordingly when a loopback is selected. For instance, if 100M system loopback on the network interface is selected, SGMII on the system interface is enabled to connect at 100M, and if passthrough is enabled 100BASE-TX will be the only advertised rate and will force a re-autonegotiation if not already connected at 100M.
                        
                        NOTE!! This is a processor intensive operation. Completion of this operation can be monitored via 1E.C831.F
                        
                        The controls in this register are identical to, and mirrored with, the controls in 4.C444.  */
      unsigned int   rate : 2;    /* 1E.C47A.1:0  R/WPD      Provisionable Default = 0x0 */
                     /* 0x3 = 10G
                        0x2 = 1G
                        0x1 = 100M
                        0x0 = reserved
                          */
                    /*! \brief 1E.C47A.2 R/WPD Reserved Provisioning 11a
                        AQ_GlobalReservedProvisioning_HHD.u10.bits_10.reservedProvisioning_11a

                        Provisionable Default = 0x0

                        Reserved for future use
                        
  */
      unsigned int   reservedProvisioning_11a : 1;    /* 1E.C47A.2  R/WPD      Provisionable Default = 0x0 */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.C47A.3 R/WPD System I/F Packet Generation
                        AQ_GlobalReservedProvisioning_HHD.u10.bits_10.systemI_fPacketGeneration

                        Provisionable Default = 0x0

                        1 = CRPAT packet generation out 10G system interface
                        0 = No CRPAT packet generation out 10G system interface
                        

                 <B>Notes:</B>
                        Selecting this mode of operation causes the CRPAT packet generator in the PHY to output CRPAT packets on the selected 10G system interface (4.C441.F:E)
                        
                        NOTE!! This is a processor intensive operation. Completion of this operation can be monitored via 1E.C831.F
                        
                        The controls in this register are identical to, and mirrored with, the controls in 4.C444.  */
      unsigned int   systemI_fPacketGeneration : 1;    /* 1E.C47A.3  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = CRPAT packet generation out 10G system interface
                        0 = No CRPAT packet generation out 10G system interface
                          */
                    /*! \brief 1E.C47A.4 R/WPD Look-Aside Port Packet Generation
                        AQ_GlobalReservedProvisioning_HHD.u10.bits_10.look_asidePortPacketGeneration

                        Provisionable Default = 0x0

                        1 = CRPAT packet generation out 10G look-aside interface (KR0)
                        0 = No CRPAT packet generation out 10G look-aside interface (KR0)
                        

                 <B>Notes:</B>
                        Selecting this mode of operation causes the CRPAT packet generator in the PHY to output on KR0.
                        
                        NOTE!! This only functions if KR1 (SERDES2) is selected as the system interface in (4.C441.F:E).
                        
                        NOTE!! This is a processor intensive operation. Completion of this operation can be monitored via 1E.C831.F
                        
                        The controls in this register are identical to, and mirrored with, the controls in 4.C444.  */
      unsigned int   look_asidePortPacketGeneration : 1;    /* 1E.C47A.4  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = CRPAT packet generation out 10G look-aside interface (KR0)
                        0 = No CRPAT packet generation out 10G look-aside interface (KR0)
                          */
                    /*! \brief 1E.C47A.5 R/WPD MDI Packet Generation
                        AQ_GlobalReservedProvisioning_HHD.u10.bits_10.mdiPacketGeneration

                        Provisionable Default = 0x0

                        1 = CRPAT packet generation out MDI interface
                        0 = No CRPAT packet generation out MDI interface
                        

                 <B>Notes:</B>
                        Selecting this mode of operation causes the CRPAT packet generator in the PHY to output on the MDI interface at the selected rate.
                        
                        NOTE!! This is a processor intensive operation. Completion of this operation can be monitored via 1E.C831.F
                        
                        The controls in this register are identical to, and mirrored with, the controls in 4.C444.  */
      unsigned int   mdiPacketGeneration : 1;    /* 1E.C47A.5  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = CRPAT packet generation out MDI interface
                        0 = No CRPAT packet generation out MDI interface
                          */
                    /*! \brief 1E.C47A.A:6 R/WPD Reserved Provisioning 11 [4:0]
                        AQ_GlobalReservedProvisioning_HHD.u10.bits_10.reservedProvisioning_11

                        Provisionable Default = 0x00

                        Reserved for future use
                        
  */
      unsigned int   reservedProvisioning_11 : 5;    /* 1E.C47A.A:6  R/WPD      Provisionable Default = 0x00 */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.C47A.F:B R/WPD Loopback Control [4:0]
                        AQ_GlobalReservedProvisioning_HHD.u10.bits_10.loopbackControl

                        Provisionable Default = 0x00

                        0x00 = No loopback
                        0x01 = System Interface - System Loopback
                        0x02 = System Interface - System Loopback with Passthrough
                        0x03 = System Interface - Network Loopback
                        0x04 = System Interface - Network Loopback with Passthrough
                        0x05 = System Interface - Network Loopback with Passthrough and Merge
                        0x06 = System Interface - Peer-to-peer loopback
                        0x07 - 0x08 = Reserved
                        0x09 = Network Interface - System Loopback
                        0x0A = Network Interface - System Loopback with Passthrough
                        0x0B = Network Interface - Network Loopback
                        0x0C = Network Interface - Network Loopback with Passthrough
                        0x0D = Network Interface - Peer-to-peer loopback
                        0x0E - 0x0F = Reserved
                        0x10 = Cross-connect System Loopback
                        0x11 = Cross-connect Network Loopback
                        0x12 - 0x13 = Reserved
                        0x14 = Network Interface - System Loopback via Loopback Plug
                        0x15 - 0x1F = Reserved
                        

                 <B>Notes:</B>
                        These bits, in conjunction with the chip configuration and the rate (Bits 1:0), select the loopback to configure for the chip. Setting one of these loopbacks provisions the chip for the specified loopback. Upon clearing the loopback, the chip returns to it's configuration prior to entering loopback (irregardless of whether other loopbacks were selected after the initial loopback).
                        
                        NOTE!! This is a processor intensive operation. Completion of this operation can be monitored via 1E.C831.F.
                        
                        The controls in this register are identical to, and mirrored with, the controls in 4.C444.
                          */
      unsigned int   loopbackControl : 5;    /* 1E.C47A.F:B  R/WPD      Provisionable Default = 0x00 */
                     /* 0x00 = No loopback
                        0x01 = System Interface - System Loopback
                        0x02 = System Interface - System Loopback with Passthrough
                        0x03 = System Interface - Network Loopback
                        0x04 = System Interface - Network Loopback with Passthrough
                        0x05 = System Interface - Network Loopback with Passthrough and Merge
                        0x06 = System Interface - Peer-to-peer loopback
                        0x07 - 0x08 = Reserved
                        0x09 = Network Interface - System Loopback
                        0x0A = Network Interface - System Loopback with Passthrough
                        0x0B = Network Interface - Network Loopback
                        0x0C = Network Interface - Network Loopback with Passthrough
                        0x0D = Network Interface - Peer-to-peer loopback
                        0x0E - 0x0F = Reserved
                        0x10 = Cross-connect System Loopback
                        0x11 = Cross-connect Network Loopback
                        0x12 - 0x13 = Reserved
                        0x14 = Network Interface - System Loopback via Loopback Plug
                        0x15 - 0x1F = Reserved
                          */
    } bits_10;
    uint16_t word_10;
  } u10;
  /*! \brief Union for bit and word level access of word 11 of Global Reserved Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C47B.0 R/WPD Enable PTP
                        AQ_GlobalReservedProvisioning_HHD.u11.bits_11.enablePtp

                        Provisionable Default = 0x0

                        1 = PTP functionality is enabled
                        0 = PTP functionality is disabled
                        

                 <B>Notes:</B>
                        If this bit is 1, the PTP/SEC block will be included in the data path, regardless of operating mode.  */
      unsigned int   enablePtp : 1;    /* 1E.C47B.0  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = PTP functionality is enabled
                        0 = PTP functionality is disabled
                          */
                    /*! \brief 1E.C47B.1 R/WPD Enable MACSec
                        AQ_GlobalReservedProvisioning_HHD.u11.bits_11.enableMacsec

                        Provisionable Default = 0x0

                        1 = MACSec functionality is enabled
                        0 = MACSec functionality is disabled
                        

                 <B>Notes:</B>
                        If this bit is 1, the PTP/SEC block will be included in the data path, regardless of operating mode.  */
      unsigned int   enableMacsec : 1;    /* 1E.C47B.1  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = MACSec functionality is enabled
                        0 = MACSec functionality is disabled
                          */
                    /*! \brief 1E.C47B.F:2 R/WPD Reserved Provisioning 12 [D:0]
                        AQ_GlobalReservedProvisioning_HHD.u11.bits_11.reservedProvisioning_12

                        Provisionable Default = 0x0000

                        Reserved for future use
                        
  */
      unsigned int   reservedProvisioning_12 : 14;    /* 1E.C47B.F:2  R/WPD      Provisionable Default = 0x0000 */
                     /* Reserved for future use
                          */
    } bits_11;
    uint16_t word_11;
  } u11;
} AQ_GlobalReservedProvisioning_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                PIF Mailbox Control: 1E.C47C */
/*                  PIF Mailbox Control: 1E.C47C */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of PIF Mailbox Control */
  union
  {
    struct
    {
                    /*! \brief 1E.C47C.F:0 R/WPDuP PIF Mailbox Address [F:0]
                        AQ_PifMailboxControl_HHD.u0.bits_0.pifMailboxAddress

                        Provisionable Default = 0x0000

                        The least 16 bits of the PIF address to read or write.
                        
  */
      unsigned int   pifMailboxAddress : 16;    /* 1E.C47C.F:0  R/WPDuP      Provisionable Default = 0x0000 */
                     /* The least 16 bits of the PIF address to read or write.
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of PIF Mailbox Control */
  union
  {
    struct
    {
                    /*! \brief 1E.C47D.F:0 R/WPDuP PIF Mailbox Data [F:0]
                        AQ_PifMailboxControl_HHD.u1.bits_1.pifMailboxData

                        Provisionable Default = 0x0000

                        The data to be written, or that had been read.
                        
  */
      unsigned int   pifMailboxData : 16;    /* 1E.C47D.F:0  R/WPDuP      Provisionable Default = 0x0000 */
                     /* The data to be written, or that had been read.
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of PIF Mailbox Control */
  union
  {
    struct
    {
                    /*! \brief 1E.C47E.7:0 R/WPDuP PIF Mailbox MMD [7:0]
                        AQ_PifMailboxControl_HHD.u2.bits_2.pifMailboxMMD

                        Provisionable Default = 0x00

                        MMD (upper 8 bits) of the PID address to read or write.
                        
  */
      unsigned int   pifMailboxMMD : 8;    /* 1E.C47E.7:0  R/WPDuP      Provisionable Default = 0x00 */
                     /* MMD (upper 8 bits) of the PID address to read or write.
                          */
                    /*! \brief 1E.C47E.B:8 R/WPDuP PIF Mailbox Command Type [3:0]
                        AQ_PifMailboxControl_HHD.u2.bits_2.pifMailboxCommandType

                        Provisionable Default = 0x0

                        0 = No Action
                        1 = Read
                        2 = Write
                        

                 <B>Notes:</B>
                        System SW writes non-zero value to start a PIF command.  */
      unsigned int   pifMailboxCommandType : 4;    /* 1E.C47E.B:8  R/WPDuP      Provisionable Default = 0x0 */
                     /* 0 = No Action
                        1 = Read
                        2 = Write
                          */
                    /*! \brief 1E.C47E.F:C R/WPD Reserved PIF Mailbox Control 3 [3:0]
                        AQ_PifMailboxControl_HHD.u2.bits_2.reservedPifMailboxControl_3

                        Provisionable Default = 0x0

                        Reserved for future use
                        
  */
      unsigned int   reservedPifMailboxControl_3 : 4;    /* 1E.C47E.F:C  R/WPD      Provisionable Default = 0x0 */
                     /* Reserved for future use
                          */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of PIF Mailbox Control */
  union
  {
    struct
    {
                    /*! \brief 1E.C47F.3:0 R/WPDuP PIF Mailbox Command Status [3:0]
                        AQ_PifMailboxControl_HHD.u3.bits_3.pifMailboxCommandStatus

                        Provisionable Default = 0x0

                        0 = Idle
                        1 = Command completed
                        2 = Command did not complete
                        

                 <B>Notes:</B>
                        System SW should write 0 before writing Command Type to clear completion status  */
      unsigned int   pifMailboxCommandStatus : 4;    /* 1E.C47F.3:0  R/WPDuP      Provisionable Default = 0x0 */
                     /* 0 = Idle
                        1 = Command completed
                        2 = Command did not complete
                          */
                    /*! \brief 1E.C47F.F:4 R/WPD Reserved PIF Mailbox Control 4 [B:0]
                        AQ_PifMailboxControl_HHD.u3.bits_3.reservedPifMailboxControl_4

                        Provisionable Default = 0x000

                        Reserved for future use
                        
  */
      unsigned int   reservedPifMailboxControl_4 : 12;    /* 1E.C47F.F:4  R/WPD      Provisionable Default = 0x000 */
                     /* Reserved for future use
                          */
    } bits_3;
    uint16_t word_3;
  } u3;
} AQ_PifMailboxControl_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global SMBus 0 Provisioning: 1E.C485 */
/*                  Global SMBus 0 Provisioning: 1E.C485 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global SMBus 0 Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved1 : 1;
                    /*! \brief 1E.C485.7:1 R/W SMB 0 Slave Address [7:1]
                        AQ_GlobalSmbus_0Provisioning_HHD.u0.bits_0.smb_0SlaveAddress

                        Default = 0x00

                        SMB slave address configuration
                        
  */
      unsigned int   smb_0SlaveAddress : 7;    /* 1E.C485.7:1  R/W      Default = 0x00 */
                     /* SMB slave address configuration
                          */
      unsigned int   reserved0 : 8;
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalSmbus_0Provisioning_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global SMBus 1 Provisioning: 1E.C495 */
/*                  Global SMBus 1 Provisioning: 1E.C495 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global SMBus 1 Provisioning */
  union
  {
    struct
    {
      unsigned int   reserved1 : 1;
                    /*! \brief 1E.C495.7:1 R/W SMB 1 Slave Address [7:1]
                        AQ_GlobalSmbus_1Provisioning_HHD.u0.bits_0.smb_1SlaveAddress

                        Default = 0x00

                        SMB slave address configuration
                        
  */
      unsigned int   smb_1SlaveAddress : 7;    /* 1E.C495.7:1  R/W      Default = 0x00 */
                     /* SMB slave address configuration
                          */
      unsigned int   reserved0 : 8;
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalSmbus_1Provisioning_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global EEE Provisioning: 1E.C4A0 */
/*                  Global EEE Provisioning: 1E.C4A0 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global EEE Provisioning */
  union
  {
    struct
    {
                    /*! \brief 1E.C4A0.0 R/WPD EEE Mode
                        AQ_GlobalEeeProvisioning_HHD.u0.bits_0.eeeMode

                        Provisionable Default = 0x0

                        1 = EEE mode of operation
                        

                 <B>Notes:</B>
                        EEE mode of operation (0=disable, 1=enable, default:0)  */
      unsigned int   eeeMode : 1;    /* 1E.C4A0.0  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = EEE mode of operation
                          */
      unsigned int   reserved0 : 15;
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalEeeProvisioning_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Cable Diagnostic Status: 1E.C800 */
/*                  Global Cable Diagnostic Status: 1E.C800 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Cable Diagnostic Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C800.2:0 RO Pair D Status [2:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u0.bits_0.pairDStatus

                        

                        [6:4]
                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        011= Connected to Pair C
                        010= Connected to Pair B
                        001= Connected to Pair A
                        000= OK

                 <B>Notes:</B>
                        This register summarizes the worst impairment on Pair D.  */
      unsigned int   pairDStatus : 3;    /* 1E.C800.2:0  RO       */
                     /* [6:4]
                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        011= Connected to Pair C
                        010= Connected to Pair B
                        001= Connected to Pair A
                        000= OK  */
      unsigned int   reserved3 : 1;
                    /*! \brief 1E.C800.6:4 RO Pair C Status [2:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u0.bits_0.pairCStatus

                        

                        [9:7]
                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        011= Connected to Pair B
                        010= Connected to Pair A
                        001= Connected to Pair D
                        000= OK

                 <B>Notes:</B>
                        This register summarizes the worst impairment on Pair C.  */
      unsigned int   pairCStatus : 3;    /* 1E.C800.6:4  RO       */
                     /* [9:7]
                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        011= Connected to Pair B
                        010= Connected to Pair A
                        001= Connected to Pair D
                        000= OK  */
      unsigned int   reserved2 : 1;
                    /*! \brief 1E.C800.A:8 RO Pair B Status [2:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u0.bits_0.pairBStatus

                        

                        [C:A]
                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        011= Connected to Pair A
                        010= Connected to Pair D
                        001= Connected to Pair C
                        000= OK

                 <B>Notes:</B>
                        This register summarizes the worst impairment on Pair B.  */
      unsigned int   pairBStatus : 3;    /* 1E.C800.A:8  RO       */
                     /* [C:A]
                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        011= Connected to Pair A
                        010= Connected to Pair D
                        001= Connected to Pair C
                        000= OK  */
      unsigned int   reserved1 : 1;
                    /*! \brief 1E.C800.E:C RO Pair A Status [2:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u0.bits_0.pairAStatus

                        

                        [F:D]
                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        011= Connected to Pair D
                        010= Connected to Pair C
                        001= Connected to Pair B
                        000= OK

                 <B>Notes:</B>
                        This register summarizes the worst impairment on Pair A.  */
      unsigned int   pairAStatus : 3;    /* 1E.C800.E:C  RO       */
                     /* [F:D]
                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        011= Connected to Pair D
                        010= Connected to Pair C
                        001= Connected to Pair B
                        000= OK  */
      unsigned int   reserved0 : 1;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Cable Diagnostic Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C801.7:0 RO Pair A Reflection #2 [7:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u1.bits_1.pairAReflection_2

                        

                        The distance in meters, accurate to 1m, of the second of the four worst reflections seen by the PHY on Pair A

                 <B>Notes:</B>
                        The distance to this reflection is given in  See Global Reserved Status 1: Address 1E.C870 . A value of zero indicates that this reflection does not exist or was not computed.  */
      unsigned int   pairAReflection_2 : 8;    /* 1E.C801.7:0  RO       */
                     /* The distance in meters, accurate to 1m, of the second of the four worst reflections seen by the PHY on Pair A  */
                    /*! \brief 1E.C801.F:8 RO Pair A Reflection #1 [7:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u1.bits_1.pairAReflection_1

                        

                        The distance in meters, accurate to 1m, of the first of the four worst reflections seen by the PHY on Pair A

                 <B>Notes:</B>
                        The distance to this reflection is given in  See Global Reserved Status 1: Address 1E.C870 . A value of zero indicates that this reflection does not exist or was not computed.  */
      unsigned int   pairAReflection_1 : 8;    /* 1E.C801.F:8  RO       */
                     /* The distance in meters, accurate to 1m, of the first of the four worst reflections seen by the PHY on Pair A  */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global Cable Diagnostic Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C802.F:0 RO Impulse Response MSW [F:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u2.bits_2.impulseResponseMSW

                        

                        The MSW of the memory location that contains the start of the impulse response data for the Extended Diagnostic type in 1E.C470.E:D

                 <B>Notes:</B>
                        See 1E.C470 for more information  */
      unsigned int   impulseResponseMSW : 16;    /* 1E.C802.F:0  RO       */
                     /* The MSW of the memory location that contains the start of the impulse response data for the Extended Diagnostic type in 1E.C470.E:D  */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global Cable Diagnostic Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C803.7:0 RO Pair B Reflection #2 [7:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u3.bits_3.pairBReflection_2

                        

                        The distance in meters, accurate to 1m, of the second of the four worst reflections seen by the PHY on Pair B

                 <B>Notes:</B>
                        The distance to this reflection is given in  See Global Reserved Status 2: Address 1E.C871 . A value of zero indicates that this reflection does not exist or was not computed.  */
      unsigned int   pairBReflection_2 : 8;    /* 1E.C803.7:0  RO       */
                     /* The distance in meters, accurate to 1m, of the second of the four worst reflections seen by the PHY on Pair B  */
                    /*! \brief 1E.C803.F:8 RO Pair B Reflection #1 [7:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u3.bits_3.pairBReflection_1

                        

                        The distance in meters, accurate to 1m, of the first of the four worst reflections seen by the PHY on Pair B

                 <B>Notes:</B>
                        The distance to this reflection is given in  See Global Reserved Status 2: Address 1E.C871 . A value of zero indicates that this reflection does not exist or was not computed.  */
      unsigned int   pairBReflection_1 : 8;    /* 1E.C803.F:8  RO       */
                     /* The distance in meters, accurate to 1m, of the first of the four worst reflections seen by the PHY on Pair B  */
    } bits_3;
    uint16_t word_3;
  } u3;
  /*! \brief Union for bit and word level access of word 4 of Global Cable Diagnostic Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C804.F:0 RO Impulse Response LSW [F:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u4.bits_4.impulseResponseLSW

                        

                        The LSW of the memory location that contains the start of the impulse response data for the Extended Diagnostic type specified in 1E.C470.E:D

                 <B>Notes:</B>
                        See 1E.C470 for more information  */
      unsigned int   impulseResponseLSW : 16;    /* 1E.C804.F:0  RO       */
                     /* The LSW of the memory location that contains the start of the impulse response data for the Extended Diagnostic type specified in 1E.C470.E:D  */
    } bits_4;
    uint16_t word_4;
  } u4;
  /*! \brief Union for bit and word level access of word 5 of Global Cable Diagnostic Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C805.7:0 RO Pair C Reflection #2 [7:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u5.bits_5.pairCReflection_2

                        

                        The distance in meters, accurate to 1m, of the second of the four worst reflections seen by the PHY on Pair C

                 <B>Notes:</B>
                        The distance to this reflection is given in  See Global Reserved Status 3: Address 1E.C872 . A value of zero indicates that this reflection does not exist or was not computed.  */
      unsigned int   pairCReflection_2 : 8;    /* 1E.C805.7:0  RO       */
                     /* The distance in meters, accurate to 1m, of the second of the four worst reflections seen by the PHY on Pair C  */
                    /*! \brief 1E.C805.F:8 RO Pair C Reflection #1 [7:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u5.bits_5.pairCReflection_1

                        

                        The distance in meters, accurate to 1m, of the first of the four worst reflections seen by the PHY on Pair C

                 <B>Notes:</B>
                        The distance to this reflection is given in  See Global Reserved Status 3: Address 1E.C872 . A value of zero indicates that this reflection does not exist or was not computed.  */
      unsigned int   pairCReflection_1 : 8;    /* 1E.C805.F:8  RO       */
                     /* The distance in meters, accurate to 1m, of the first of the four worst reflections seen by the PHY on Pair C  */
    } bits_5;
    uint16_t word_5;
  } u5;
  /*! \brief Union for bit and word level access of word 6 of Global Cable Diagnostic Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C806.F:0 RO Reserved 1 [F:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u6.bits_6.reserved_1

                        

                        Reserved for future use
  */
      unsigned int   reserved_1 : 16;    /* 1E.C806.F:0  RO       */
                     /* Reserved for future use  */
    } bits_6;
    uint16_t word_6;
  } u6;
  /*! \brief Union for bit and word level access of word 7 of Global Cable Diagnostic Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C807.7:0 RO Pair D Reflection #2 [7:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u7.bits_7.pairDReflection_2

                        

                        The distance in meters, accurate to 1m, of the second of the four worst reflections seen by the PHY on Pair D

                 <B>Notes:</B>
                        The distance to this reflection is given in  See Global Reserved Status 4: Address 1E.C873 . A value of zero indicates that this reflection does not exist or was not computed.  */
      unsigned int   pairDReflection_2 : 8;    /* 1E.C807.7:0  RO       */
                     /* The distance in meters, accurate to 1m, of the second of the four worst reflections seen by the PHY on Pair D  */
                    /*! \brief 1E.C807.F:8 RO Pair D Reflection #1 [7:0]
                        AQ_GlobalCableDiagnosticStatus_HHD.u7.bits_7.pairDReflection_1

                        

                        The distance in meters, accurate to 1m, of the first of the four worst reflections seen by the PHY on Pair D

                 <B>Notes:</B>
                        The distance to this reflection is given in  See Global Reserved Status 4: Address 1E.C873 . A value of zero indicates that this reflection does not exist or was not computed.  */
      unsigned int   pairDReflection_1 : 8;    /* 1E.C807.F:8  RO       */
                     /* The distance in meters, accurate to 1m, of the first of the four worst reflections seen by the PHY on Pair D  */
    } bits_7;
    uint16_t word_7;
  } u7;
} AQ_GlobalCableDiagnosticStatus_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Thermal Status: 1E.C820 */
/*                  Global Thermal Status: 1E.C820 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Thermal Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C820.F:0 RO Temperature [F:0]
                        AQ_GlobalThermalStatus_HHD.u0.bits_0.temperature

                        

                        [F:0] of temperature
                        

                 <B>Notes:</B>
                        2's complement value with the LSB representing 1/256 of a degree Celsius. This corresponds to -40 degreesC = 0xD800. Default is 70 degreesC. This is a mirror of the XENPAK register 1.A060 - 1.A061. The mirror is performed in H/W.  */
      unsigned int   temperature : 16;    /* 1E.C820.F:0  RO       */
                     /* [F:0] of temperature
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Thermal Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C821.0 RO Temperature Ready
                        AQ_GlobalThermalStatus_HHD.u1.bits_1.temperatureReady

                        

                        1 = Temperature measurement is valid
                        

                 <B>Notes:</B>
                        This is a mirror of the XENPAK register 1.A06E.  */
      unsigned int   temperatureReady : 1;    /* 1E.C821.0  RO       */
                     /* 1 = Temperature measurement is valid
                          */
      unsigned int   reserved0 : 15;
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_GlobalThermalStatus_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global General Status: 1E.C830 */
/*                  Global General Status: 1E.C830 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global General Status */
  union
  {
    struct
    {
      unsigned int   reserved1 : 11;
                    /*! \brief 1E.C830.B RO Low Temperature Warning State
                        AQ_GlobalGeneralStatus_HHD.u0.bits_0.lowTemperatureWarningState

                        

                        1 = Low temperature warning threshold has been exceeded

                 <B>Notes:</B>
                        In XENPAK mode, F/W will copy this register to the 1.A074.6 register.
                        
                          */
      unsigned int   lowTemperatureWarningState : 1;    /* 1E.C830.B  RO       */
                     /* 1 = Low temperature warning threshold has been exceeded  */
                    /*! \brief 1E.C830.C RO High Temperature Warning State
                        AQ_GlobalGeneralStatus_HHD.u0.bits_0.highTemperatureWarningState

                        

                        1 = High temperature warning threshold has been exceeded

                 <B>Notes:</B>
                        In XENPAK mode, F/W will copy this register to the 1.A074.7 register.
                        
                          */
      unsigned int   highTemperatureWarningState : 1;    /* 1E.C830.C  RO       */
                     /* 1 = High temperature warning threshold has been exceeded  */
                    /*! \brief 1E.C830.D RO Low Temperature Failure State
                        AQ_GlobalGeneralStatus_HHD.u0.bits_0.lowTemperatureFailureState

                        

                        1 = Low temperature failure threshold has been exceeded

                 <B>Notes:</B>
                        In XENPAK mode, F/W will copy this register to the 1.A070.6 register.
                        
                          */
      unsigned int   lowTemperatureFailureState : 1;    /* 1E.C830.D  RO       */
                     /* 1 = Low temperature failure threshold has been exceeded  */
                    /*! \brief 1E.C830.E RO High Temperature Failure State
                        AQ_GlobalGeneralStatus_HHD.u0.bits_0.highTemperatureFailureState

                        

                        1 = High temperature failure threshold has been exceeded

                 <B>Notes:</B>
                        In XENPAK mode, F/W will copy this register to the 1.A070.7 register.
                        
                          */
      unsigned int   highTemperatureFailureState : 1;    /* 1E.C830.E  RO       */
                     /* 1 = High temperature failure threshold has been exceeded  */
      unsigned int   reserved0 : 1;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global General Status */
  union
  {
    struct
    {
      unsigned int   reserved0 : 15;
                    /*! \brief 1E.C831.F RO Processor Intensive MDIO Operation In- Progress
                        AQ_GlobalGeneralStatus_HHD.u1.bits_1.processorIntensiveMdioOperationIn_Progress

                        

                        1 = PHY microprocessor is busy with a processor-intensive MDIO operation
                        0 = Processor-intensive MDIO operation completed
                        

                 <B>Notes:</B>
                        This bit should may be used with certain processor-intensive MDIO commands (such as Loopbacks, Test Modes, Low power modes, Tx-Disable, Restart autonegotiation, Cable Diagnostics, etc.) that take longer than an MDIO cycle to complete. Upon receiving an MDIO command that involves the PHY's microprocessor, this bit is set, and when the command is completed, this bit is cleared.
                        
                        NOTE!!! This bit should be checked only after 1 ms of issuing a processor-intensive MDIO operation.
                        
                        The list of operations that set this bit are as follows:
                        
                        1.0.0, PMA Loopback
                        1.0.B, Low power mode
                        1.9.4:0, Tx Disable
                        1.84, 10G Test modes
                        1.8000.5, XENPAK Control
                        1.9000, XENPAK Rx Fault Enable
                        1.9002, XENPAK Alarm Enable
                        1.E400.F, External loopback
                        3.0.B, Low power mode
                        3.0.E, System PCS loopback
                        3.C471.5, PRBS Test
                        3.C471.6, PRBS Test
                        3.E471.5, PRBS Test 
                        3.E471.6, PRBS Test
                        4.0.B, Low power mode 
                        4.0.E, PHY-XS network loopback
                        4.C440, Output clock control, Load SERDES parameters
                        4.F802.E, System loopback 
                        4.C444.F:B, Loopback Control
                        4.C444.4:2, Packet generation
                        4.C445.C, SERDES calibration
                        7.0.9, Restart autonegotiation
                        1D.C280, 1G/100M Network loopback
                        1D.C500, 1G System loopback 
                        1D.C501, 1G / 100M Test modes
                        1E.C470.4, Cable diagnostics
                        1E.C47A.F:B, Loopback Control
                        1E.C47A.4:2, Packet generation  */
      unsigned int   processorIntensiveMdioOperationIn_Progress : 1;    /* 1E.C831.F  RO       */
                     /* 1 = PHY microprocessor is busy with a processor-intensive MDIO operation
                        0 = Processor-intensive MDIO operation completed
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
} AQ_GlobalGeneralStatus_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Pin Status: 1E.C840 */
/*                  Global Pin Status: 1E.C840 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Pin Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C840.5:0 RO LED Pullup State [5:0]
                        AQ_GlobalPinStatus_HHD.u0.bits_0.ledPullupState

                        

                        1 = LED output pin is pulled high
                        0 = LED output pin is pulled low
                        
  */
      unsigned int   ledPullupState : 6;    /* 1E.C840.5:0  RO       */
                     /* 1 = LED output pin is pulled high
                        0 = LED output pin is pulled low
                          */
      unsigned int   reserved4 : 1;
                    /*! \brief 1E.C840.7 RO Tx Enable
                        AQ_GlobalPinStatus_HHD.u0.bits_0.txEnable

                        

                        Current Value of Tx Enable pin
                        

                 <B>Notes:</B>
                        0 = Disable Transmitter  */
      unsigned int   txEnable : 1;    /* 1E.C840.7  RO       */
                     /* Current Value of Tx Enable pin
                          */
      unsigned int   reserved3 : 1;
                    /*! \brief 1E.C840.9 RO Package Connectivity
                        AQ_GlobalPinStatus_HHD.u0.bits_0.packageConnectivity

                        

                        Value of the package connection pin
                        
  */
      unsigned int   packageConnectivity : 1;    /* 1E.C840.9  RO       */
                     /* Value of the package connection pin
                          */
      unsigned int   reserved2 : 3;
                    /*! \brief 1E.C840.D RO DC_MASTER_N
                        AQ_GlobalPinStatus_HHD.u0.bits_0.dcMasterN

                        

                        Value of DC_MASTER_N pin:
                        
                        0x1 = PHY Slave Daisy Chain Boot
                        0x0 = PHY Master Daisy Chain Boot from FLASH
  */
      unsigned int   dcMasterN : 1;    /* 1E.C840.D  RO       */
                     /* Value of DC_MASTER_N pin:
                        
                        0x1 = PHY Slave Daisy Chain Boot
                        0x0 = PHY Master Daisy Chain Boot from FLASH  */
      unsigned int   reserved1 : 1;
      unsigned int   reserved0 : 1;
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalPinStatus_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Daisy Chain Status: 1E.C842 */
/*                  Global Daisy Chain Status: 1E.C842 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Daisy Chain Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C842.F:0 RO Rx Daisy Chain Calculated CRC [F:0]
                        AQ_GlobalDaisyChainStatus_HHD.u0.bits_0.rxDaisyChainCalculatedCrc

                        

                        Rx Daisy Chain Calculated CRC
                        

                 <B>Notes:</B>
                        This is the calculated daisy chain CRC.  */
      unsigned int   rxDaisyChainCalculatedCrc : 16;    /* 1E.C842.F:0  RO       */
                     /* Rx Daisy Chain Calculated CRC
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalDaisyChainStatus_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Fault Message: 1E.C850 */
/*                  Global Fault Message: 1E.C850 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Fault Message */
  union
  {
    struct
    {
                    /*! \brief 1E.C850.F:0 RO Message [F:0]
                        AQ_GlobalFaultMessage_HHD.u0.bits_0.message

                        

                        Error code describing fault

                 <B>Notes:</B>
                        Code 0x8001: Firmware not compatible with chip architecture. This fault occurs when firmware compiled for a different microprocessor core is loaded.
                        Code 0x8002: VCO calibration failed. This occurs when the main PLLs on chip fail to lock: this is not possible to trigger.
                        Code 0x8003: XAUI calibration failed. This occurs when the XAUI PLLs fail to lock: this is not possible to trigger.
                        Code 0x8005: Unexpected device ID. This occurs if the device ID programmed into the internal E-Fuse registers in not valid: this is not possible to trigger.
                        Code 0x8006: Computed checksum does not match expected checksum. This occurs when the FLASH checksum check performed at boot time fails. This only occurs when the system boots from FLASH.
                        Code 0x8007: Detected a bit error in static memory. To trigger, corrupt one of the static regions.
                        Code 0xC001: Illegal Instruction exception. This occurs when the processor attempts to execute an illegal instruction. To trigger this, write an illegal instruction to program memory. It's possible that the bit error check will trigger before the illegal instruction is executed.
                        Code 0xC002 Instruction Fetch Error. Internal physical address or a data error during instruction fetch: this is not possible to trigger.
                        Code 0xC003 Load Store Error. Internal physical address or data error during load store operation: this is not possible to trigger..
                        Code 0xC004 Privileged Instruction. Attempt to execute a privileged operation without sufficient privilege: this is not possible to trigger.
                        Code 0xC005 Unaligned Load or Store. Attempt to load or store data at an address which cannot be handled due to alignment: this is not possible to trigger.
                        Code 0xC006 Instruction fetch from prohibited space: this is not possible to trigger.
                        Code 0xC007 Data load from prohibited space: this is not possible to trigger.
                        Code 0xC008 Data store into prohibited space: this is not possible to trigger.  */
      unsigned int   message : 16;    /* 1E.C850.F:0  RO       */
                     /* Error code describing fault  */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalFaultMessage_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Cable Diagnostic Impedance: 1E.C880 */
/*                  Global Cable Diagnostic Impedance: 1E.C880 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Cable Diagnostic Impedance */
  union
  {
    struct
    {
                    /*! \brief 1E.C880.2:0 RO Pair A Reflection #4 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u0.bits_0.pairAReflection_4

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the fourth worst reflection on Pair A. The corresponding length of this reflection from the PHY is given in  See Global Power Control - Address 1E.21   */
      unsigned int   pairAReflection_4 : 3;    /* 1E.C880.2:0  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C880.3 RO Reserved 4
                        AQ_GlobalCableDiagnosticImpedance_HHD.u0.bits_0.reserved_4

                        

                        Reserved
                        
  */
      unsigned int   reserved_4 : 1;    /* 1E.C880.3  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C880.6:4 RO Pair A Reflection #3 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u0.bits_0.pairAReflection_3

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the third worst reflection on Pair A. The corresponding length of this reflection from the PHY is given in  See Global Power Control - Address 1E.21   */
      unsigned int   pairAReflection_3 : 3;    /* 1E.C880.6:4  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C880.7 RO Reserved 3
                        AQ_GlobalCableDiagnosticImpedance_HHD.u0.bits_0.reserved_3

                        

                        Reserved
                        
  */
      unsigned int   reserved_3 : 1;    /* 1E.C880.7  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C880.A:8 RO Pair A Reflection #2 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u0.bits_0.pairAReflection_2

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the second worst reflection on Pair A. The corresponding length of this reflection from the PHY is given in  See Global Power Control - Address 1E.21   */
      unsigned int   pairAReflection_2 : 3;    /* 1E.C880.A:8  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C880.B RO Reserved 2
                        AQ_GlobalCableDiagnosticImpedance_HHD.u0.bits_0.reserved_2

                        

                        Reserved
                        
  */
      unsigned int   reserved_2 : 1;    /* 1E.C880.B  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C880.E:C RO Pair A Reflection #1 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u0.bits_0.pairAReflection_1

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the first worst reflection on Pair A. The corresponding length of this reflection from the PHY is given in  See Global Power Control - Address 1E.21   */
      unsigned int   pairAReflection_1 : 3;    /* 1E.C880.E:C  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C880.F RO Reserved 1
                        AQ_GlobalCableDiagnosticImpedance_HHD.u0.bits_0.reserved_1

                        

                        Reserved
                        
  */
      unsigned int   reserved_1 : 1;    /* 1E.C880.F  RO       */
                     /* Reserved
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Cable Diagnostic Impedance */
  union
  {
    struct
    {
                    /*! \brief 1E.C881.2:0 RO Pair B Reflection #4 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u1.bits_1.pairBReflection_4

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the fourth worst reflection on Pair B. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 2 - Address 1E.32 - 1E.33   */
      unsigned int   pairBReflection_4 : 3;    /* 1E.C881.2:0  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C881.3 RO Reserved 8
                        AQ_GlobalCableDiagnosticImpedance_HHD.u1.bits_1.reserved_8

                        

                        Reserved
                        
  */
      unsigned int   reserved_8 : 1;    /* 1E.C881.3  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C881.6:4 RO Pair B Reflection #3 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u1.bits_1.pairBReflection_3

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the third worst reflection on Pair B. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 2 - Address 1E.32 - 1E.33   */
      unsigned int   pairBReflection_3 : 3;    /* 1E.C881.6:4  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C881.7 RO Reserved 7
                        AQ_GlobalCableDiagnosticImpedance_HHD.u1.bits_1.reserved_7

                        

                        Reserved
                        
  */
      unsigned int   reserved_7 : 1;    /* 1E.C881.7  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C881.A:8 RO Pair B Reflection #2 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u1.bits_1.pairBReflection_2

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the second worst reflection on Pair B. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 2 - Address 1E.32 - 1E.33   */
      unsigned int   pairBReflection_2 : 3;    /* 1E.C881.A:8  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C881.B RO Reserved 6
                        AQ_GlobalCableDiagnosticImpedance_HHD.u1.bits_1.reserved_6

                        

                        Reserved
                        
  */
      unsigned int   reserved_6 : 1;    /* 1E.C881.B  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C881.E:C RO Pair B Reflection #1 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u1.bits_1.pairBReflection_1

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the first worst reflection on Pair B. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 2 - Address 1E.32 - 1E.33   */
      unsigned int   pairBReflection_1 : 3;    /* 1E.C881.E:C  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C881.F RO Reserved 5
                        AQ_GlobalCableDiagnosticImpedance_HHD.u1.bits_1.reserved_5

                        

                        Reserved
                        
  */
      unsigned int   reserved_5 : 1;    /* 1E.C881.F  RO       */
                     /* Reserved
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global Cable Diagnostic Impedance */
  union
  {
    struct
    {
                    /*! \brief 1E.C882.2:0 RO Pair C Reflection #4 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u2.bits_2.pairCReflection_4

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the fourth worst reflection on Pair C. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 3 - Address 1E.33   */
      unsigned int   pairCReflection_4 : 3;    /* 1E.C882.2:0  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C882.3 RO Reserved 12
                        AQ_GlobalCableDiagnosticImpedance_HHD.u2.bits_2.reserved_12

                        

                        Reserved
                        
  */
      unsigned int   reserved_12 : 1;    /* 1E.C882.3  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C882.6:4 RO Pair C Reflection #3 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u2.bits_2.pairCReflection_3

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the third worst reflection on Pair C. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 3 - Address 1E.33   */
      unsigned int   pairCReflection_3 : 3;    /* 1E.C882.6:4  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C882.7 RO Reserved 11
                        AQ_GlobalCableDiagnosticImpedance_HHD.u2.bits_2.reserved_11

                        

                        Reserved
                        
  */
      unsigned int   reserved_11 : 1;    /* 1E.C882.7  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C882.A:8 RO Pair C Reflection #2 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u2.bits_2.pairCReflection_2

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the second worst reflection on Pair C. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 3 - Address 1E.33   */
      unsigned int   pairCReflection_2 : 3;    /* 1E.C882.A:8  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C882.B RO Reserved 10
                        AQ_GlobalCableDiagnosticImpedance_HHD.u2.bits_2.reserved_10

                        

                        Reserved
                        
  */
      unsigned int   reserved_10 : 1;    /* 1E.C882.B  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C882.E:C RO Pair C Reflection #1 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u2.bits_2.pairCReflection_1

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the first worst reflection on Pair C. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 3 - Address 1E.33   */
      unsigned int   pairCReflection_1 : 3;    /* 1E.C882.E:C  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C882.F RO Reserved 9
                        AQ_GlobalCableDiagnosticImpedance_HHD.u2.bits_2.reserved_9

                        

                        Reserved
                        
  */
      unsigned int   reserved_9 : 1;    /* 1E.C882.F  RO       */
                     /* Reserved
                          */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global Cable Diagnostic Impedance */
  union
  {
    struct
    {
                    /*! \brief 1E.C883.2:0 RO Pair D Reflection #4 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u3.bits_3.pairDReflection_4

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the fourth worst reflection on Pair D. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 3 - Address 1E.34 - 1E.35   */
      unsigned int   pairDReflection_4 : 3;    /* 1E.C883.2:0  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C883.3 RO Reserved 16
                        AQ_GlobalCableDiagnosticImpedance_HHD.u3.bits_3.reserved_16

                        

                        Reserved
                        
  */
      unsigned int   reserved_16 : 1;    /* 1E.C883.3  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C883.6:4 RO Pair D Reflection #3 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u3.bits_3.pairDReflection_3

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the third worst reflection on Pair D. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 3 - Address 1E.34 - 1E.35   */
      unsigned int   pairDReflection_3 : 3;    /* 1E.C883.6:4  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C883.7 RO Reserved 15
                        AQ_GlobalCableDiagnosticImpedance_HHD.u3.bits_3.reserved_15

                        

                        Reserved
                        
  */
      unsigned int   reserved_15 : 1;    /* 1E.C883.7  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C883.A:8 RO Pair D Reflection #2 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u3.bits_3.pairDReflection_2

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the second worst reflection on Pair D. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 3 - Address 1E.34 - 1E.35   */
      unsigned int   pairDReflection_2 : 3;    /* 1E.C883.A:8  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C883.B RO Reserved 14
                        AQ_GlobalCableDiagnosticImpedance_HHD.u3.bits_3.reserved_14

                        

                        Reserved
                        
  */
      unsigned int   reserved_14 : 1;    /* 1E.C883.B  RO       */
                     /* Reserved
                          */
                    /*! \brief 1E.C883.E:C RO Pair D Reflection #1 [2:0]
                        AQ_GlobalCableDiagnosticImpedance_HHD.u3.bits_3.pairDReflection_1

                        

                        111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                        

                 <B>Notes:</B>
                        The impedance of the first worst reflection on Pair D. The corresponding length of this reflection from the PHY is given in  See Global Cable Diagnostic Status 3 - Address 1E.34 - 1E.35   */
      unsigned int   pairDReflection_1 : 3;    /* 1E.C883.E:C  RO       */
                     /* 111 = Open Circuit (> 300W)
                        110 = High Mismatch (> 115W)
                        101 = Low Mismatch (< 85W)
                        100 = Short Circuit (< 30W)
                        0xx= No information available
                          */
                    /*! \brief 1E.C883.F RO Reserved 13
                        AQ_GlobalCableDiagnosticImpedance_HHD.u3.bits_3.reserved_13

                        

                        Reserved
                        
  */
      unsigned int   reserved_13 : 1;    /* 1E.C883.F  RO       */
                     /* Reserved
                          */
    } bits_3;
    uint16_t word_3;
  } u3;
} AQ_GlobalCableDiagnosticImpedance_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Status: 1E.C884 */
/*                  Global Status: 1E.C884 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C884.7:0 RO Cable Length [7:0]
                        AQ_GlobalStatus_HHD.u0.bits_0.cableLength

                        

                        The estimated length of the cable in meters
                        

                 <B>Notes:</B>
                        The length of the cable shown here is estimated from the cable diagnostic engine and should be accurate to +/-1m.  */
      unsigned int   cableLength : 8;    /* 1E.C884.7:0  RO       */
                     /* The estimated length of the cable in meters
                          */
                    /*! \brief 1E.C884.F:8 RO Reserved Status 0 [7:0]
                        AQ_GlobalStatus_HHD.u0.bits_0.reservedStatus_0

                        

                        Reserved
                        
  */
      unsigned int   reservedStatus_0 : 8;    /* 1E.C884.F:8  RO       */
                     /* Reserved
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalStatus_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Reserved Status: 1E.C885 */
/*                  Global Reserved Status: 1E.C885 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Reserved Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C885.3:0 ROSPD Provisioning ID [3:0]
                        AQ_GlobalReservedStatus_HHD.u0.bits_0.provisioningID

                        Provisionable Default = 0x0

                        Provisioning ID
                        

                 <B>Notes:</B>
                        Customers may receive multiple ROM images that differ only in their provisioning. This field is used to differentiate those images. This field is used in conjunction with the firmware major and minor revision numbers to uniquely identify ROM images.  */
      unsigned int   provisioningID : 4;    /* 1E.C885.3:0  ROSPD      Provisionable Default = 0x0 */
                     /* Provisioning ID
                          */
                    /*! \brief 1E.C885.7:4 ROSPD Firmware Build ID [3:0]
                        AQ_GlobalReservedStatus_HHD.u0.bits_0.firmwareBuildID

                        Provisionable Default = 0x0

                        Firmware Build ID
                        

                 <B>Notes:</B>
                        Customers may receive multiple ROM images that differ only in their provisioning. This field is used to differentiate those images. This field is used in conjunction with the firmware major and minor revision numbers to uniquely identify ROM images.  */
      unsigned int   firmwareBuildID : 4;    /* 1E.C885.7:4  ROSPD      Provisionable Default = 0x0 */
                     /* Firmware Build ID
                          */
                    /*! \brief 1E.C885.9:8 ROSPD XENPAK NVR Status [1:0]
                        AQ_GlobalReservedStatus_HHD.u0.bits_0.xenpakNvrStatus

                        Provisionable Default = 0x0

                        Status of XENPAK NVR:
                        0: NVR not enabled
                        1: Last NVR operation succeeded
                        2: Last NVR operation failed
                        3: Reserved
                        

                 <B>Notes:</B>
                        XENPAK register space is mirrored in NVR (SPI ROM). This register indicates the status of the last NVR operation.  */
      unsigned int   xenpakNvrStatus : 2;    /* 1E.C885.9:8  ROSPD      Provisionable Default = 0x0 */
                     /* Status of XENPAK NVR:
                        0: NVR not enabled
                        1: Last NVR operation succeeded
                        2: Last NVR operation failed
                        3: Reserved
                          */
                    /*! \brief 1E.C885.F:A RO Nearly Seconds MSW[5:0]
                        AQ_GlobalReservedStatus_HHD.u0.bits_0.nearlySecondsMSW

                        

                        Bits 16 to 21 of the 22 bit "Nearly Seconds" uptime counter.
                        

                 <B>Notes:</B>
                        The "Nearly Seconds" counter is incremented every 1024 milliseconds.  */
      unsigned int   nearlySecondsMSW : 6;    /* 1E.C885.F:A  RO       */
                     /* Bits 16 to 21 of the 22 bit "Nearly Seconds" uptime counter.
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Reserved Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C886.F:0 RO Nearly Seconds LSW [F:0]
                        AQ_GlobalReservedStatus_HHD.u1.bits_1.nearlySecondsLSW

                        

                        Bits 0 to 15 of the 22 bit "Nearly Seconds" uptime counter
                        

                 <B>Notes:</B>
                        The "Nearly Seconds" counter is incremented every 1024 milliseconds.  */
      unsigned int   nearlySecondsLSW : 16;    /* 1E.C886.F:0  RO       */
                     /* Bits 0 to 15 of the 22 bit "Nearly Seconds" uptime counter
                          */
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global Reserved Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C887.D:0 RO Reserved Status 3 [D:0]
                        AQ_GlobalReservedStatus_HHD.u2.bits_2.reservedStatus_3

                        

                        Reserved for future use
                        
  */
      unsigned int   reservedStatus_3 : 14;    /* 1E.C887.D:0  RO       */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.C887.E ROS Power Up Stall Status
                        AQ_GlobalReservedStatus_HHD.u2.bits_2.powerUpStallStatus

                        Default = 0x0

                        1 = FW is stalled at power up
                        0 = Firmware is unstalled
                        
  */
      unsigned int   powerUpStallStatus : 1;    /* 1E.C887.E  ROS      Default = 0x0 */
                     /* 1 = FW is stalled at power up
                        0 = Firmware is unstalled
                          */
                    /*! \brief 1E.C887.F ROS DTE Status
                        AQ_GlobalReservedStatus_HHD.u2.bits_2.dteStatus

                        Default = 0x0

                        1 = Need power
                        0 = Don't need power
                        
  */
      unsigned int   dteStatus : 1;    /* 1E.C887.F  ROS      Default = 0x0 */
                     /* 1 = Need power
                        0 = Don't need power
                          */
    } bits_2;
    uint16_t word_2;
  } u2;
  /*! \brief Union for bit and word level access of word 3 of Global Reserved Status */
  union
  {
    struct
    {
                    /*! \brief 1E.C888.1:0 RO Rate [1:0]
                        AQ_GlobalReservedStatus_HHD.u3.bits_3.rate

                        Default = 0x0

                        0x3 = 10G
                        0x2 = 1G
                        0x1 = 100M
                        0x0 = invalid
                        

                 <B>Notes:</B>
                        These bits report the selected rate for the loopback and packet generation.  */
      unsigned int   rate : 2;    /* 1E.C888.1:0  RO      Default = 0x0 */
                     /* 0x3 = 10G
                        0x2 = 1G
                        0x1 = 100M
                        0x0 = invalid
                          */
                    /*! \brief 1E.C888.2 RO Reserved Status 4a
                        AQ_GlobalReservedStatus_HHD.u3.bits_3.reservedStatus_4a

                        Default = 0x0

                        Reserved for future use
                        
  */
      unsigned int   reservedStatus_4a : 1;    /* 1E.C888.2  RO      Default = 0x0 */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.C888.3 RO System I/F Packet Generation Status
                        AQ_GlobalReservedStatus_HHD.u3.bits_3.systemI_fPacketGenerationStatus

                        Default = 0x0

                        1 = CRPAT packet generation out 10G system interface
                        0 = No CRPAT packet generation out 10G system interface
                        

                 <B>Notes:</B>
                        Reports whether the CRPAT packet generator in the PHY outputs on the selected system interface at the selected rate.  */
      unsigned int   systemI_fPacketGenerationStatus : 1;    /* 1E.C888.3  RO      Default = 0x0 */
                     /* 1 = CRPAT packet generation out 10G system interface
                        0 = No CRPAT packet generation out 10G system interface
                          */
                    /*! \brief 1E.C888.4 RO Look-Aside Port Packet Generation Status
                        AQ_GlobalReservedStatus_HHD.u3.bits_3.look_asidePortPacketGenerationStatus

                        Default = 0x0

                        1 = CRPAT packet generation out 10G look-aside interface (KR0)
                        0 = No CRPAT packet generation out 10G look-aside interface (KR0)
                        

                 <B>Notes:</B>
                        Reports whether the CRPAT packet generator in the PHY outputs on the KR0 interface at the selected rate.  */
      unsigned int   look_asidePortPacketGenerationStatus : 1;    /* 1E.C888.4  RO      Default = 0x0 */
                     /* 1 = CRPAT packet generation out 10G look-aside interface (KR0)
                        0 = No CRPAT packet generation out 10G look-aside interface (KR0)
                          */
                    /*! \brief 1E.C888.5 RO MDI Packet Generation Status
                        AQ_GlobalReservedStatus_HHD.u3.bits_3.mdiPacketGenerationStatus

                        Default = 0x0

                        1 = CRPAT packet generation out MDI interface
                        0 = No CRPAT packet generation out MDI interface
                        

                 <B>Notes:</B>
                        Reports whether the CRPAT packet generator in the PHY outputs on the MDI interface at the selected rate.  */
      unsigned int   mdiPacketGenerationStatus : 1;    /* 1E.C888.5  RO      Default = 0x0 */
                     /* 1 = CRPAT packet generation out MDI interface
                        0 = No CRPAT packet generation out MDI interface
                          */
                    /*! \brief 1E.C888.A:6 RO Reserved Status 4 [4:0]
                        AQ_GlobalReservedStatus_HHD.u3.bits_3.reservedStatus_4

                        Default = 0x00

                        Reserved for future use
                        
  */
      unsigned int   reservedStatus_4 : 5;    /* 1E.C888.A:6  RO      Default = 0x00 */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.C888.F:B RO Loopback Status [4:0]
                        AQ_GlobalReservedStatus_HHD.u3.bits_3.loopbackStatus

                        Default = 0x00

                        0x00 = No loopback
                        0x01 = System Interface - System Loopback
                        0x02 = System Interface - System Loopback with Passthrough
                        0x03 = System Interface - Network Loopback
                        0x04 = System Interface - Network Loopback with Passthrough
                        0x05 = System Interface - Network Loopback with Passthrough and Merge
                        0x06 = System Interface - Peer-to-peer loopback
                        0x07 - 0x08 = Reserved
                        0x09 = Network Interface - System Loopback
                        0x0A = Network Interface - System Loopback with Passthrough
                        0x0B = Network Interface - Network Loopback
                        0x0C = Network Interface - Network Loopback with Passthrough
                        0x0D = Network Interface - Peer-to-peer loopback
                        0x0E - 0x0F = Reserved
                        0x10 = Cross-connect System Loopback
                        0x11 = Cross-connect Network Loopback
                        0x12 - 0x13 = Reserved
                        0x14 = Network Interface - System Loopback via Loopback Plug
                        0x15 - 0x1F = Reserved
                        

                 <B>Notes:</B>
                        These bits, in conjunction with the chip configuration and the rate (Bits 1:0), report the selected loopback.
                        
                          */
      unsigned int   loopbackStatus : 5;    /* 1E.C888.F:B  RO      Default = 0x00 */
                     /* 0x00 = No loopback
                        0x01 = System Interface - System Loopback
                        0x02 = System Interface - System Loopback with Passthrough
                        0x03 = System Interface - Network Loopback
                        0x04 = System Interface - Network Loopback with Passthrough
                        0x05 = System Interface - Network Loopback with Passthrough and Merge
                        0x06 = System Interface - Peer-to-peer loopback
                        0x07 - 0x08 = Reserved
                        0x09 = Network Interface - System Loopback
                        0x0A = Network Interface - System Loopback with Passthrough
                        0x0B = Network Interface - Network Loopback
                        0x0C = Network Interface - Network Loopback with Passthrough
                        0x0D = Network Interface - Peer-to-peer loopback
                        0x0E - 0x0F = Reserved
                        0x10 = Cross-connect System Loopback
                        0x11 = Cross-connect Network Loopback
                        0x12 - 0x13 = Reserved
                        0x14 = Network Interface - System Loopback via Loopback Plug
                        0x15 - 0x1F = Reserved
                          */
    } bits_3;
    uint16_t word_3;
  } u3;
} AQ_GlobalReservedStatus_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Alarms: 1E.CC00 */
/*                  Global Alarms: 1E.CC00 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Alarms */
  union
  {
    struct
    {
                    /*! \brief 1E.CC00.0 LH Reserved Alarm D
                        AQ_GlobalAlarms_HHD.u0.bits_0.reservedAlarmD

                        

                        Reserved for future use
                        
  */
      unsigned int   reservedAlarmD : 1;    /* 1E.CC00.0  LH       */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.CC00.1 LH Reserved Alarm C
                        AQ_GlobalAlarms_HHD.u0.bits_0.reservedAlarmC

                        

                        Reserved for future use
                        
  */
      unsigned int   reservedAlarmC : 1;    /* 1E.CC00.1  LH       */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.CC00.2 LH Reserved Alarm B
                        AQ_GlobalAlarms_HHD.u0.bits_0.reservedAlarmB

                        

                        Reserved for future use
                        
  */
      unsigned int   reservedAlarmB : 1;    /* 1E.CC00.2  LH       */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.CC00.3 LH Reserved Alarm A
                        AQ_GlobalAlarms_HHD.u0.bits_0.reservedAlarmA

                        

                        Reserved for future use
                        
  */
      unsigned int   reservedAlarmA : 1;    /* 1E.CC00.3  LH       */
                     /* Reserved for future use
                          */
                    /*! \brief 1E.CC00.4 LH Device Fault
                        AQ_GlobalAlarms_HHD.u0.bits_0.deviceFault

                        

                        1 = Fault

                 <B>Notes:</B>
                        When set, a fault has been detected by the uP and the associated 16 bit error code is visible in  See Global Configuration Fault Message: Address 1E.C850   */
      unsigned int   deviceFault : 1;    /* 1E.CC00.4  LH       */
                     /* 1 = Fault  */
      unsigned int   reserved2 : 1;
                    /*! \brief 1E.CC00.6 LH Reset completed
                        AQ_GlobalAlarms_HHD.u0.bits_0.resetCompleted

                        

                        1 = Chip wide reset completed

                 <B>Notes:</B>
                        This bit is set by the microprocessor when it has completed it's initialization sequence. This bit is mirrored in 1.CC02.0  */
      unsigned int   resetCompleted : 1;    /* 1E.CC00.6  LH       */
                     /* 1 = Chip wide reset completed  */
      unsigned int   reserved1 : 4;
                    /*! \brief 1E.CC00.B LH Low Temperature Warning
                        AQ_GlobalAlarms_HHD.u0.bits_0.lowTemperatureWarning

                        

                        1 = Low temperature warning threshold has been exceeded
                        

                 <B>Notes:</B>
                        
                        
                        
                        
                        These bits mirror the matching bit in 1.A070 and 1.A074. These bits are driven by Bits E:B in  See Global General Status 1: Address 1E.C830 .  */
      unsigned int   lowTemperatureWarning : 1;    /* 1E.CC00.B  LH       */
                     /* 1 = Low temperature warning threshold has been exceeded
                          */
                    /*! \brief 1E.CC00.C LH High Temperature Warning
                        AQ_GlobalAlarms_HHD.u0.bits_0.highTemperatureWarning

                        

                        1 = High temperature warning threshold has been exceeded
                        

                 <B>Notes:</B>
                        
                        
                        
                        
                        These bits mirror the matching bit in 1.A070 and 1.A074. These bits are driven by Bits E:B in  See Global General Status 1: Address 1E.C830 .  */
      unsigned int   highTemperatureWarning : 1;    /* 1E.CC00.C  LH       */
                     /* 1 = High temperature warning threshold has been exceeded
                          */
                    /*! \brief 1E.CC00.D LH Low Temperature Failure
                        AQ_GlobalAlarms_HHD.u0.bits_0.lowTemperatureFailure

                        

                        1 = Low temperature failure threshold has been exceeded
                        

                 <B>Notes:</B>
                        
                        
                        
                        
                        These bits mirror the matching bit in 1.A070 and 1.A074. These bits are driven by Bits E:B in  See Global General Status 1: Address 1E.C830 .  */
      unsigned int   lowTemperatureFailure : 1;    /* 1E.CC00.D  LH       */
                     /* 1 = Low temperature failure threshold has been exceeded
                          */
                    /*! \brief 1E.CC00.E LH High Temperature Failure
                        AQ_GlobalAlarms_HHD.u0.bits_0.highTemperatureFailure

                        

                        1 = High temperature failure threshold has been exceeded
                        

                 <B>Notes:</B>
                        
                        
                        
                        
                        These bits mirror the matching bit in 1.A070 and 1.A074. These bits are driven by Bits E:B in  See Global General Status 1: Address 1E.C830 .  */
      unsigned int   highTemperatureFailure : 1;    /* 1E.CC00.E  LH       */
                     /* 1 = High temperature failure threshold has been exceeded
                          */
      unsigned int   reserved0 : 1;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Alarms */
  union
  {
    struct
    {
      unsigned int   reserved2 : 1;
      unsigned int   reserved1 : 6;
                    /*! \brief 1E.CC01.7 LH MDIO Command Handling Overflow
                        AQ_GlobalAlarms_HHD.u1.bits_1.mdioCommandHandlingOverflow

                        

                        1 = PHY was issued more MDIO requests than it could service in it's request buffer
                        

                 <B>Notes:</B>
                        Assertion of this bit means that more MDIO commands were issued than FW could handle.  */
      unsigned int   mdioCommandHandlingOverflow : 1;    /* 1E.CC01.7  LH       */
                     /* 1 = PHY was issued more MDIO requests than it could service in it's request buffer
                          */
                    /*! \brief 1E.CC01.A:8 LH Reserved Alarms [2:0]
                        AQ_GlobalAlarms_HHD.u1.bits_1.reservedAlarms

                        

                        Reserved
                        
                        
  */
      unsigned int   reservedAlarms : 3;    /* 1E.CC01.A:8  LH       */
                     /* Reserved
                        
                          */
                    /*! \brief 1E.CC01.B LH DTE Status Change
                        AQ_GlobalAlarms_HHD.u1.bits_1.dteStatusChange

                        

                        1 = DTE status change
                        

                 <B>Notes:</B>
                        Change in 1E.C887[F].  */
      unsigned int   dteStatusChange : 1;    /* 1E.CC01.B  LH       */
                     /* 1 = DTE status change
                          */
                    /*! \brief 1E.CC01.C LH IP Phone Detect
                        AQ_GlobalAlarms_HHD.u1.bits_1.ipPhoneDetect

                        

                        1 = IP Phone Detect
                        

                 <B>Notes:</B>
                        Assertion of this bit means that the presence of an IP Phone has been detected.  */
      unsigned int   ipPhoneDetect : 1;    /* 1E.CC01.C  LH       */
                     /* 1 = IP Phone Detect
                          */
                    /*! \brief 1E.CC01.D RO XENPAK Alarm
                        AQ_GlobalAlarms_HHD.u1.bits_1.xenpakAlarm

                        

                        1 = XENPAK Alarm
                        
  */
      unsigned int   xenpakAlarm : 1;    /* 1E.CC01.D  RO       */
                     /* 1 = XENPAK Alarm
                          */
                    /*! \brief 1E.CC01.E LH Smart Power-Down Entered
                        AQ_GlobalAlarms_HHD.u1.bits_1.smartPower_downEntered

                        

                        1 = Smart Power-Down State Entered
                        

                 <B>Notes:</B>
                        When this bit is set, it indicates that the Smart Power-Down state was entered  */
      unsigned int   smartPower_downEntered : 1;    /* 1E.CC01.E  LH       */
                     /* 1 = Smart Power-Down State Entered
                          */
      unsigned int   reserved0 : 1;
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global Alarms */
  union
  {
    struct
    {
                    /*! \brief 1E.CC02.0 LH Watchdog Timer Alarm
                        AQ_GlobalAlarms_HHD.u2.bits_2.watchdogTimerAlarm

                        

                        1 = Watchdog timer alarm
                        
  */
      unsigned int   watchdogTimerAlarm : 1;    /* 1E.CC02.0  LH       */
                     /* 1 = Watchdog timer alarm
                          */
                    /*! \brief 1E.CC02.1 LH MDIO Timeout Error
                        AQ_GlobalAlarms_HHD.u2.bits_2.mdioTimeoutError

                        

                        1 = MDIO timeout detected
                        
  */
      unsigned int   mdioTimeoutError : 1;    /* 1E.CC02.1  LH       */
                     /* 1 = MDIO timeout detected
                          */
                    /*! \brief 1E.CC02.2 LH MDIO MMD Error
                        AQ_GlobalAlarms_HHD.u2.bits_2.mdioMMD_Error

                        

                        1 = Invalid MMD address detected
                        
  */
      unsigned int   mdioMMD_Error : 1;    /* 1E.CC02.2  LH       */
                     /* 1 = Invalid MMD address detected
                          */
      unsigned int   reserved2 : 2;
                    /*! \brief 1E.CC02.5 LRF Tx Enable State Change
                        AQ_GlobalAlarms_HHD.u2.bits_2.txEnableStateChange

                        

                        1 = TX_EN pin has changed state
                        
  */
      unsigned int   txEnableStateChange : 1;    /* 1E.CC02.5  LRF       */
                     /* 1 = TX_EN pin has changed state
                          */
      unsigned int   reserved1 : 2;
                    /*! \brief 1E.CC02.9:8 LH uP IRAM Parity Error [1:0]
                        AQ_GlobalAlarms_HHD.u2.bits_2.upIramParityError

                        

                        1 = Parity error detected in the uP IRAM
                        

                 <B>Notes:</B>
                        Bit 0 indicates a parity error was detected in the uP IRAM but was corrected.
                        Bit 1 indicates a multiple parity errors were detected in the uP IRAM and could not be corrected.
                        The uP IRAM is protected with ECC.  */
      unsigned int   upIramParityError : 2;    /* 1E.CC02.9:8  LH       */
                     /* 1 = Parity error detected in the uP IRAM
                          */
                    /*! \brief 1E.CC02.A LH uP DRAM Parity Error
                        AQ_GlobalAlarms_HHD.u2.bits_2.upDramParityError

                        

                        1 = Parity error detected in the uP DRAM
                        
  */
      unsigned int   upDramParityError : 1;    /* 1E.CC02.A  LH       */
                     /* 1 = Parity error detected in the uP DRAM
                          */
      unsigned int   reserved0 : 3;
                    /*! \brief 1E.CC02.E LH Mailbox Operation: Complete
                        AQ_GlobalAlarms_HHD.u2.bits_2.mailboxOperation_Complete

                        

                        1 = Mailbox operation is complete
                        

                 <B>Notes:</B>
                        Mailbox interface is ready interrupt for registers  See Global Vendor Specific Control - Address 1E.C000  -  See Global Vendor Specific Provisioning 5 - Address 1E.C404   */
      unsigned int   mailboxOperation_Complete : 1;    /* 1E.CC02.E  LH       */
                     /* 1 = Mailbox operation is complete
                          */
                    /*! \brief 1E.CC02.F LH NVR Operation Complete
                        AQ_GlobalAlarms_HHD.u2.bits_2.nvrOperationComplete

                        

                        1 = NVR operation is complete
                        

                 <B>Notes:</B>
                        NVR interface is ready interrupt for registers  See Global NVR Interface 1: Address 1E.100  -  See Global NVR Provisioning Data MSW - Address 1E.17 .  */
      unsigned int   nvrOperationComplete : 1;    /* 1E.CC02.F  LH       */
                     /* 1 = NVR operation is complete
                          */
    } bits_2;
    uint16_t word_2;
  } u2;
} AQ_GlobalAlarms_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Interrupt Mask: 1E.D400 */
/*                  Global Interrupt Mask: 1E.D400 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Interrupt Mask */
  union
  {
    struct
    {
                    /*! \brief 1E.D400.0 R/WPD Reserved Alarm D Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.reservedAlarmDMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   reservedAlarmDMask : 1;    /* 1E.D400.0  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
                    /*! \brief 1E.D400.1 R/WPD Reserved Alarm C Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.reservedAlarmCMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   reservedAlarmCMask : 1;    /* 1E.D400.1  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
                    /*! \brief 1E.D400.2 R/WPD Reserved Alarm B Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.reservedAlarmBMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   reservedAlarmBMask : 1;    /* 1E.D400.2  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
                    /*! \brief 1E.D400.3 R/WPD Reserved Alarm A Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.reservedAlarmAMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   reservedAlarmAMask : 1;    /* 1E.D400.3  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
                    /*! \brief 1E.D400.4 R/WPD Device Fault Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.deviceFaultMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   deviceFaultMask : 1;    /* 1E.D400.4  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
      unsigned int   reserved2 : 1;
                    /*! \brief 1E.D400.6 R/WPD Reset completed Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.resetCompletedMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   resetCompletedMask : 1;    /* 1E.D400.6  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
      unsigned int   reserved1 : 4;
                    /*! \brief 1E.D400.B R/WPD Low Temperature Warning Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.lowTemperatureWarningMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   lowTemperatureWarningMask : 1;    /* 1E.D400.B  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.D400.C R/WPD High Temperature Warning Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.highTemperatureWarningMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   highTemperatureWarningMask : 1;    /* 1E.D400.C  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.D400.D R/WPD Low Temperature Failure Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.lowTemperatureFailureMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   lowTemperatureFailureMask : 1;    /* 1E.D400.D  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.D400.E R/WPD High Temperature Failure Mask
                        AQ_GlobalInterruptMask_HHD.u0.bits_0.highTemperatureFailureMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   highTemperatureFailureMask : 1;    /* 1E.D400.E  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
      unsigned int   reserved0 : 1;
    } bits_0;
    uint16_t word_0;
  } u0;
  /*! \brief Union for bit and word level access of word 1 of Global Interrupt Mask */
  union
  {
    struct
    {
                    /*! \brief 1E.D401.0 R/WPD Diagnostic Alarm Mask
                        AQ_GlobalInterruptMask_HHD.u1.bits_1.diagnosticAlarmMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   diagnosticAlarmMask : 1;    /* 1E.D401.0  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
      unsigned int   reserved1 : 6;
                    /*! \brief 1E.D401.7 R/WPD MDIO Command Handling Overflow Mask
                        AQ_GlobalInterruptMask_HHD.u1.bits_1.mdioCommandHandlingOverflowMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   mdioCommandHandlingOverflowMask : 1;    /* 1E.D401.7  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
                    /*! \brief 1E.D401.A:8 R/WPD Reserved Alarms Mask [2:0]
                        AQ_GlobalInterruptMask_HHD.u1.bits_1.reservedAlarmsMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   reservedAlarmsMask : 3;    /* 1E.D401.A:8  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
                    /*! \brief 1E.D401.B R/WPD DTE Status Change Mask
                        AQ_GlobalInterruptMask_HHD.u1.bits_1.dteStatusChangeMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   dteStatusChangeMask : 1;    /* 1E.D401.B  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
                    /*! \brief 1E.D401.C R/WPD IP Phone Detect Mask
                        AQ_GlobalInterruptMask_HHD.u1.bits_1.ipPhoneDetectMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   ipPhoneDetectMask : 1;    /* 1E.D401.C  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
                    /*! \brief 1E.D401.D R/WPD XENPAK Alarm Mask
                        AQ_GlobalInterruptMask_HHD.u1.bits_1.xenpakAlarmMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   xenpakAlarmMask : 1;    /* 1E.D401.D  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.D401.E R/WPD Smart Power-Down Entered Mask
                        AQ_GlobalInterruptMask_HHD.u1.bits_1.smartPower_downEnteredMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   smartPower_downEnteredMask : 1;    /* 1E.D401.E  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
      unsigned int   reserved0 : 1;
    } bits_1;
    uint16_t word_1;
  } u1;
  /*! \brief Union for bit and word level access of word 2 of Global Interrupt Mask */
  union
  {
    struct
    {
                    /*! \brief 1E.D402.0 R/WPD Watchdog Timer Alarm Mask
                        AQ_GlobalInterruptMask_HHD.u2.bits_2.watchdogTimerAlarmMask

                        Provisionable Default = 0x1

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   watchdogTimerAlarmMask : 1;    /* 1E.D402.0  R/WPD      Provisionable Default = 0x1 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.D402.1 R/WPD MDIO Timeout Error Mask
                        AQ_GlobalInterruptMask_HHD.u2.bits_2.mdioTimeoutErrorMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   mdioTimeoutErrorMask : 1;    /* 1E.D402.1  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.D402.2 R/WPD MDIO MMD Error Mask
                        AQ_GlobalInterruptMask_HHD.u2.bits_2.mdioMMD_ErrorMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   mdioMMD_ErrorMask : 1;    /* 1E.D402.2  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
      unsigned int   reserved2 : 2;
                    /*! \brief 1E.D402.5 R/WPD Tx Enable State Change Mask
                        AQ_GlobalInterruptMask_HHD.u2.bits_2.txEnableStateChangeMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   txEnableStateChangeMask : 1;    /* 1E.D402.5  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
      unsigned int   reserved1 : 2;
                    /*! \brief 1E.D402.9:8 R/WPD uP IRAM Parity Error Mask [1:0]
                        AQ_GlobalInterruptMask_HHD.u2.bits_2.upIramParityErrorMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   upIramParityErrorMask : 2;    /* 1E.D402.9:8  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
                    /*! \brief 1E.D402.A R/WPD uP DRAM Parity Error Mask
                        AQ_GlobalInterruptMask_HHD.u2.bits_2.upDramParityErrorMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
                        
  */
      unsigned int   upDramParityErrorMask : 1;    /* 1E.D402.A  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation
                          */
      unsigned int   reserved0 : 3;
                    /*! \brief 1E.D402.E R/WPD Mailbox Operation Complete Mask
                        AQ_GlobalInterruptMask_HHD.u2.bits_2.mailboxOperationCompleteMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation

                 <B>Notes:</B>
                        Mailbox interface is ready interrupt for registers  See Global Vendor Specific Control - Address 1E.C000  -  See Global Vendor Specific Provisioning 5 - Address 1E.C404   */
      unsigned int   mailboxOperationCompleteMask : 1;    /* 1E.D402.E  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.D402.F R/WPD NVR Operation Complete Mask
                        AQ_GlobalInterruptMask_HHD.u2.bits_2.nvrOperationCompleteMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation

                 <B>Notes:</B>
                        NVR interface is ready interrupt for registers  See Global NVR Interface 1: Address 1E.100  -  See Global NVR Provisioning Data MSW - Address 1E.17   */
      unsigned int   nvrOperationCompleteMask : 1;    /* 1E.D402.F  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
    } bits_2;
    uint16_t word_2;
  } u2;
} AQ_GlobalInterruptMask_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Chip-Wide Standard Interrupt Flags: 1E.FC00 */
/*                  Global Chip-Wide Standard Interrupt Flags: 1E.FC00 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Chip-Wide Standard Interrupt Flags */
  union
  {
    struct
    {
                    /*! \brief 1E.FC00.0 RO All Vendor Alarms Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.allVendorAlarmsInterrupt

                        

                        1 = Interrupt in all vendor alarms
                        

                 <B>Notes:</B>
                        An interrupt was generated from status register ( See Global Chip-Wide LASI Vendor Interrupt Flags: Address 1E.FC01 ) and the corresponding mask register. ( See Global Interrupt LASI Mask: Address 1E.FF01 )  */
      unsigned int   allVendorAlarmsInterrupt : 1;    /* 1E.FC00.0  RO       */
                     /* 1 = Interrupt in all vendor alarms
                          */
      unsigned int   reserved0 : 5;
                    /*! \brief 1E.FC00.6 RO GbE Standard Alarms Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.gbeStandardAlarmsInterrupt

                        

                        1 = Interrupt in GbE standard alarms
                        

                 <B>Notes:</B>
                        An interrupt was generated from the TGE core.  */
      unsigned int   gbeStandardAlarmsInterrupt : 1;    /* 1E.FC00.6  RO       */
                     /* 1 = Interrupt in GbE standard alarms
                          */
                    /*! \brief 1E.FC00.7 RO Autonegotiation Standard Alarms 2 Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.autonegotiationStandardAlarms_2Interrupt

                        

                        1 = Interrupt in Autonegotiation standard alarms 2
                        

                 <B>Notes:</B>
                        An interrupt was generated from status register ( See Autonegotiation 10GBASE-T Status Register - Address 7.21 ) and the corresponding mask register. ( See PHY XS Standard Transmit XAUI Rx Interrupt Mask 8 - Address 4.A008 )  */
      unsigned int   autonegotiationStandardAlarms_2Interrupt : 1;    /* 1E.FC00.7  RO       */
                     /* 1 = Interrupt in Autonegotiation standard alarms 2
                          */
                    /*! \brief 1E.FC00.8 RO Autonegotiation Standard Alarms 1 Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.autonegotiationStandardAlarms_1Interrupt

                        

                        1 = Interrupt in Autonegotiation standard alarms 1
                        

                 <B>Notes:</B>
                        An interrupt was generated from status register ( See PHY XS Standard Status 1 - Address 4.1 ) and the corresponding mask register. ( See Autonegotiation Standard LASI Interrupt Mask 1: Address 7.D000 )  */
      unsigned int   autonegotiationStandardAlarms_1Interrupt : 1;    /* 1E.FC00.8  RO       */
                     /* 1 = Interrupt in Autonegotiation standard alarms 1
                          */
                    /*! \brief 1E.FC00.9 RO PHY XS Standard Alarms 2 Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.phyXS_StandardAlarms_2Interrupt

                        

                        1 = Interrupt in PHY XS standard alarms 2
                        

                 <B>Notes:</B>
                        An interrupt was generated from the status register ( See PHY XS Standard Vendor Devices in Package - Address 4.8 ) and the corresponding mask register. ( See PHY XS Standard Transmit XAUI Rx Interrupt Mask 8 - Address 4.A008 )  */
      unsigned int   phyXS_StandardAlarms_2Interrupt : 1;    /* 1E.FC00.9  RO       */
                     /* 1 = Interrupt in PHY XS standard alarms 2
                          */
                    /*! \brief 1E.FC00.A RO PHY XS Standard Alarms 1 Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.phyXS_StandardAlarms_1Interrupt

                        

                        1 = Interrupt in PHY XS standard alarms 1
                        

                 <B>Notes:</B>
                        An interrupt was generated from the status register ( See PHY XS Standard Status 1 - Address 4.1 ) and the corresponding mask register. ( See PHY XS Standard Transmit XAUI Rx Interrupt Mask 2 - Address 4.A001 )  */
      unsigned int   phyXS_StandardAlarms_1Interrupt : 1;    /* 1E.FC00.A  RO       */
                     /* 1 = Interrupt in PHY XS standard alarms 1
                          */
                    /*! \brief 1E.FC00.B RO PCS Standard Alarm 3 Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.pcsStandardAlarm_3Interrupt

                        

                        1 = Interrupt in PCS standard alarms 3
                        

                 <B>Notes:</B>
                        An interrupt was generated from status register ( See PCS 10GBASE-T Status 2 - Address 3.21 ) and the corresponding mask register. ( See PCS Standard Interrupt Mask 1 - Address 3.E021 )  */
      unsigned int   pcsStandardAlarm_3Interrupt : 1;    /* 1E.FC00.B  RO       */
                     /* 1 = Interrupt in PCS standard alarms 3
                          */
                    /*! \brief 1E.FC00.C RO PCS Standard Alarm 2 Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.pcsStandardAlarm_2Interrupt

                        

                        1 = Interrupt in PCS standard alarms 2
                        

                 <B>Notes:</B>
                        An interrupt was generated from status register ( See PHY XS Standard Vendor Devices in Package - Address 4.8 ) and the corresponding mask register. ( See PHY XS Standard Transmit XAUI Rx Interrupt Mask 8 - Address 4.A008 )  */
      unsigned int   pcsStandardAlarm_2Interrupt : 1;    /* 1E.FC00.C  RO       */
                     /* 1 = Interrupt in PCS standard alarms 2
                          */
                    /*! \brief 1E.FC00.D RO PCS Standard Alarm 1 Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.pcsStandardAlarm_1Interrupt

                        

                        1 = Interrupt in PCS standard alarms 1
                        

                 <B>Notes:</B>
                        An interrupt was generated from status register ( See PHY XS Standard Status 1 - Address 4.1 ) and the corresponding mask register. ( See PHY XS Standard Transmit XAUI Rx Interrupt Mask 2 - Address 4.A001 )  */
      unsigned int   pcsStandardAlarm_1Interrupt : 1;    /* 1E.FC00.D  RO       */
                     /* 1 = Interrupt in PCS standard alarms 1
                          */
                    /*! \brief 1E.FC00.E RO PMA Standard Alarm 2 Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.pmaStandardAlarm_2Interrupt

                        

                        1 = Interrupt in PMA standard alarms 2
                        

                 <B>Notes:</B>
                        An interrupt was generated from either bit 1.8.B or 1.8.A.
                        An interrupt was generated from status register ( See PHY XS Standard Vendor Devices in Package - Address 4.8 ) and the corresponding mask register. ( See PHY XS Standard Transmit XAUI Rx Interrupt Mask 8 - Address 4.A008 )  */
      unsigned int   pmaStandardAlarm_2Interrupt : 1;    /* 1E.FC00.E  RO       */
                     /* 1 = Interrupt in PMA standard alarms 2
                          */
                    /*! \brief 1E.FC00.F RO PMA Standard Alarm 1 Interrupt
                        AQ_GlobalChip_wideStandardInterruptFlags_HHD.u0.bits_0.pmaStandardAlarm_1Interrupt

                        

                        1 = Interrupt in PMA standard alarms 1
                        

                 <B>Notes:</B>
                        An interrupt was generated from bit 1.1.2.
                        An interrupt was generated from status register ( See PHY XS Standard Status 1 - Address 4.1 ) and the corresponding mask register. ( See PHY XS Standard Transmit XAUI Rx Interrupt Mask 2 - Address 4.A001 )  */
      unsigned int   pmaStandardAlarm_1Interrupt : 1;    /* 1E.FC00.F  RO       */
                     /* 1 = Interrupt in PMA standard alarms 1
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalChip_wideStandardInterruptFlags_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Chip-Wide Vendor Interrupt Flags: 1E.FC01 */
/*                  Global Chip-Wide Vendor Interrupt Flags: 1E.FC01 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Chip-Wide Vendor Interrupt Flags */
  union
  {
    struct
    {
                    /*! \brief 1E.FC01.0 RO Global Alarms 3 Interrupt
                        AQ_GlobalChip_wideVendorInterruptFlags_HHD.u0.bits_0.globalAlarms_3Interrupt

                        

                        1 = Interrupt in Global alarms 3
                        

                 <B>Notes:</B>
                        An interrupt was generated from status register ( See Global Vendor Alarms 2: Address 1E.CC01 ) and the corresponding mask register. ( See Global LASI Interrupt Mask 2: Address 1E.D401 )  */
      unsigned int   globalAlarms_3Interrupt : 1;    /* 1E.FC01.0  RO       */
                     /* 1 = Interrupt in Global alarms 3
                          */
                    /*! \brief 1E.FC01.1 RO Global Alarms 2 Interrupt
                        AQ_GlobalChip_wideVendorInterruptFlags_HHD.u0.bits_0.globalAlarms_2Interrupt

                        

                        1 = Interrupt in Global alarms 2
                        

                 <B>Notes:</B>
                        An interrupt was generated from status register ( See Global Alarms 2: Address 1E.CC01 ) and the corresponding mask register. ( See Global LASI Interrupt Mask 2: Address 1E.D401 )  */
      unsigned int   globalAlarms_2Interrupt : 1;    /* 1E.FC01.1  RO       */
                     /* 1 = Interrupt in Global alarms 2
                          */
                    /*! \brief 1E.FC01.2 RO Global Alarms 1 Interrupt
                        AQ_GlobalChip_wideVendorInterruptFlags_HHD.u0.bits_0.globalAlarms_1Interrupt

                        

                        1 = Interrupt in Global alarms 1
                        

                 <B>Notes:</B>
                        An interrupt was generated from status register ( See Global Vendor Alarms 1 - Address 1E.CC00 ) and the corresponding mask register. ( See Global Vendor Interrupt Mask - Address 1E.D400 )  */
      unsigned int   globalAlarms_1Interrupt : 1;    /* 1E.FC01.2  RO       */
                     /* 1 = Interrupt in Global alarms 1
                          */
      unsigned int   reserved0 : 8;
                    /*! \brief 1E.FC01.B RO GbE Vendor Alarm Interrupt
                        AQ_GlobalChip_wideVendorInterruptFlags_HHD.u0.bits_0.gbeVendorAlarmInterrupt

                        

                        1 = Interrupt in GbE vendor specific alarm
                        

                 <B>Notes:</B>
                        A GbE alarm was generated. ( See GbE PHY Vendor Global LASI Interrupt Flags 1: Address 1D.FC00 )  */
      unsigned int   gbeVendorAlarmInterrupt : 1;    /* 1E.FC01.B  RO       */
                     /* 1 = Interrupt in GbE vendor specific alarm
                          */
                    /*! \brief 1E.FC01.C RO Autonegotiation Vendor Alarm Interrupt
                        AQ_GlobalChip_wideVendorInterruptFlags_HHD.u0.bits_0.autonegotiationVendorAlarmInterrupt

                        

                        1 = Interrupt in Autonegotiation vendor specific alarm
                        

                 <B>Notes:</B>
                        An Autonegotiation alarm was generated. ( See Autonegotiation Vendor Global LASI Interrupt Flags 1: Address 7.FC00 )  */
      unsigned int   autonegotiationVendorAlarmInterrupt : 1;    /* 1E.FC01.C  RO       */
                     /* 1 = Interrupt in Autonegotiation vendor specific alarm
                          */
                    /*! \brief 1E.FC01.D RO PHY XS Vendor Alarm Interrupt
                        AQ_GlobalChip_wideVendorInterruptFlags_HHD.u0.bits_0.phyXS_VendorAlarmInterrupt

                        

                        1 = Interrupt in PHY XS vendor specific alarm
                        

                 <B>Notes:</B>
                        A PHY XS alarm was generated. ( See PHY XS Vendor Global LASI Interrupt Flags 1: Address 4.FC00 )  */
      unsigned int   phyXS_VendorAlarmInterrupt : 1;    /* 1E.FC01.D  RO       */
                     /* 1 = Interrupt in PHY XS vendor specific alarm
                          */
                    /*! \brief 1E.FC01.E RO PCS Vendor Alarm Interrupt
                        AQ_GlobalChip_wideVendorInterruptFlags_HHD.u0.bits_0.pcsVendorAlarmInterrupt

                        

                        1 = Interrupt in PCS vendor specific alarm
                        

                 <B>Notes:</B>
                        A PCS alarm was generated. ( See PHY XS Vendor Global Interrupt Flags 1- Address 4.F800 )  */
      unsigned int   pcsVendorAlarmInterrupt : 1;    /* 1E.FC01.E  RO       */
                     /* 1 = Interrupt in PCS vendor specific alarm
                          */
                    /*! \brief 1E.FC01.F RO PMA Vendor Alarm Interrupt
                        AQ_GlobalChip_wideVendorInterruptFlags_HHD.u0.bits_0.pmaVendorAlarmInterrupt

                        

                        1 = Interrupt in PMA vendor specific alarm
                        

                 <B>Notes:</B>
                        A PMA alarm was generated. ( See PHY XS Vendor Global Interrupt Flags 1- Address 4.F800 )  */
      unsigned int   pmaVendorAlarmInterrupt : 1;    /* 1E.FC01.F  RO       */
                     /* 1 = Interrupt in PMA vendor specific alarm
                          */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalChip_wideVendorInterruptFlags_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Interrupt Chip-Wide Standard Mask: 1E.FF00 */
/*                  Global Interrupt Chip-Wide Standard Mask: 1E.FF00 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Interrupt Chip-Wide Standard Mask */
  union
  {
    struct
    {
                    /*! \brief 1E.FF00.0 R/WPD All Vendor Alarms Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.allVendorAlarmsInterruptMask

                        Provisionable Default = 0x1

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   allVendorAlarmsInterruptMask : 1;    /* 1E.FF00.0  R/WPD      Provisionable Default = 0x1 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
      unsigned int   reserved0 : 5;
                    /*! \brief 1E.FF00.6 R/WPD Gbe Standard Alarms Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.gbeStandardAlarmsInterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   gbeStandardAlarmsInterruptMask : 1;    /* 1E.FF00.6  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF00.7 R/WPD Autonegotiation Standard Alarms 2 Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.autonegotiationStandardAlarms_2InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   autonegotiationStandardAlarms_2InterruptMask : 1;    /* 1E.FF00.7  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF00.8 R/WPD Autonegotiation Standard Alarms 1 Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.autonegotiationStandardAlarms_1InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   autonegotiationStandardAlarms_1InterruptMask : 1;    /* 1E.FF00.8  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF00.9 R/WPD PHY XS Standard Alarms 2 Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.phyXS_StandardAlarms_2InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   phyXS_StandardAlarms_2InterruptMask : 1;    /* 1E.FF00.9  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF00.A R/WPD PHY XS Standard Alarms 1 Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.phyXS_StandardAlarms_1InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   phyXS_StandardAlarms_1InterruptMask : 1;    /* 1E.FF00.A  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF00.B R/WPD PCS Standard Alarm 3 Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.pcsStandardAlarm_3InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   pcsStandardAlarm_3InterruptMask : 1;    /* 1E.FF00.B  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF00.C R/WPD PCS Standard Alarm 2 Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.pcsStandardAlarm_2InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   pcsStandardAlarm_2InterruptMask : 1;    /* 1E.FF00.C  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF00.D R/WPD PCS Standard Alarm 1 Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.pcsStandardAlarm_1InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   pcsStandardAlarm_1InterruptMask : 1;    /* 1E.FF00.D  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF00.E R/WPD PMA Standard Alarm 2 Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.pmaStandardAlarm_2InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   pmaStandardAlarm_2InterruptMask : 1;    /* 1E.FF00.E  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF00.F R/WPD PMA Standard Alarm 1 Interrupt Mask
                        AQ_GlobalInterruptChip_wideStandardMask_HHD.u0.bits_0.pmaStandardAlarm_1InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   pmaStandardAlarm_1InterruptMask : 1;    /* 1E.FF00.F  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalInterruptChip_wideStandardMask_HHD;


/*---------------------------------------------------------------------------------*/
/*! \brief                Global Interrupt Chip-Wide Vendor Mask: 1E.FF01 */
/*                  Global Interrupt Chip-Wide Vendor Mask: 1E.FF01 */
/*---------------------------------------------------------------------------------*/
typedef struct 
{
  /*! \brief Union for bit and word level access of word 0 of Global Interrupt Chip-Wide Vendor Mask */
  union
  {
    struct
    {
                    /*! \brief 1E.FF01.0 R/WPD Global Alarms 3 Interrupt Mask
                        AQ_GlobalInterruptChip_wideVendorMask_HHD.u0.bits_0.globalAlarms_3InterruptMask

                        Provisionable Default = 0x1

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   globalAlarms_3InterruptMask : 1;    /* 1E.FF01.0  R/WPD      Provisionable Default = 0x1 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF01.1 R/WPD Global Alarms 2 Interrupt Mask
                        AQ_GlobalInterruptChip_wideVendorMask_HHD.u0.bits_0.globalAlarms_2InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   globalAlarms_2InterruptMask : 1;    /* 1E.FF01.1  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF01.2 R/WPD Global Alarms 1 Interrupt Mask
                        AQ_GlobalInterruptChip_wideVendorMask_HHD.u0.bits_0.globalAlarms_1InterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   globalAlarms_1InterruptMask : 1;    /* 1E.FF01.2  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
      unsigned int   reserved0 : 8;
                    /*! \brief 1E.FF01.B R/WPD GbE Vendor Alarm Interrupt Mask
                        AQ_GlobalInterruptChip_wideVendorMask_HHD.u0.bits_0.gbeVendorAlarmInterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   gbeVendorAlarmInterruptMask : 1;    /* 1E.FF01.B  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF01.C R/WPD Autonegotiation Vendor Alarm Interrupt Mask
                        AQ_GlobalInterruptChip_wideVendorMask_HHD.u0.bits_0.autonegotiationVendorAlarmInterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   autonegotiationVendorAlarmInterruptMask : 1;    /* 1E.FF01.C  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF01.D R/WPD PHY XS Vendor Alarm Interrupt Mask
                        AQ_GlobalInterruptChip_wideVendorMask_HHD.u0.bits_0.phyXS_VendorAlarmInterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   phyXS_VendorAlarmInterruptMask : 1;    /* 1E.FF01.D  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF01.E R/WPD PCS Vendor Alarm Interrupt Mask
                        AQ_GlobalInterruptChip_wideVendorMask_HHD.u0.bits_0.pcsVendorAlarmInterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   pcsVendorAlarmInterruptMask : 1;    /* 1E.FF01.E  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
                    /*! \brief 1E.FF01.F R/WPD PMA Vendor Alarm Interrupt Mask
                        AQ_GlobalInterruptChip_wideVendorMask_HHD.u0.bits_0.pmaVendorAlarmInterruptMask

                        Provisionable Default = 0x0

                        1 = Enable interrupt generation
                        0 = Disable interrupt generation
  */
      unsigned int   pmaVendorAlarmInterruptMask : 1;    /* 1E.FF01.F  R/WPD      Provisionable Default = 0x0 */
                     /* 1 = Enable interrupt generation
                        0 = Disable interrupt generation  */
    } bits_0;
    uint16_t word_0;
  } u0;
} AQ_GlobalInterruptChip_wideVendorMask_HHD;

#endif
/*@}*/
/*@}*/
