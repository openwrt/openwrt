/*AQ_API.c*/

/************************************************************************************
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
* Description:
*
*   This file contains the code for all of the API functions defined in AQ_API.h
*
************************************************************************************/


/*! \file
*   This file contains the code for all of the API functions defined in AQ_API.h
 */

#include <string.h>
#include <stdlib.h>

#include "AQ_API.h"
#include "AQ_User.h"
#include "AQ_RegMacro.h"
#include "AQ_PlatformRoutines.h"
#include "AQ_RegMaps.h"
#include "AQ_ReturnCodes.h"

#ifdef AQ_VERBOSE
  #include <stdio.h>
  #include <assert.h>
#endif


#ifndef AQ_TIME_T_EXISTS
  #ifndef AQ_MDIO_READS_PER_SECOND
    #error AQ_MDIO_READS_PER_SECOND in AQ_User.h must be defined, as AQ_TIME_T_EXISTS is currently undefined!
  #endif
#endif

#ifdef AQ_TIME_T_EXISTS
  #include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AQ_ENABLE_UP_BUSY_CHECKS
  #ifdef AQ_VERBOSE
    #define AQ_API_UP_BUSY_PRINT_STATEMENT printf("uP-busy check timed out.\n");
  #else
    #define AQ_API_UP_BUSY_PRINT_STATEMENT /* nothing */ 
  #endif
  
  #ifdef AQ_TIME_T_EXISTS
    #define AQ_API_UP_BUSY_TIMEOUT (CLOCKS_PER_SEC / 10)
  
    #define AQ_API_DECLARE_UP_BUSY_VARS AQ_API_Variable(AQ_GlobalGeneralStatus) \
                                        uint16_t uPbusy; \
                                        clock_t startTime; \
                                        AQ_boolean uPBusyTimeoutOccurred = False;
      
    #define AQ_API_CHECK_UP_NOT_BUSY AQ_API_Wait(1, port); \
                                     startTime = clock(); \
                                     do \
                                     { \
                                       AQ_API_Get(port->PHY_ID, AQ_GlobalGeneralStatus, processorIntensiveMdioOperationIn_Progress, uPbusy); \
                                       if ((clock() - startTime) > AQ_API_UP_BUSY_TIMEOUT) \
                                       { \
                                         AQ_API_UP_BUSY_PRINT_STATEMENT \
                                         uPBusyTimeoutOccurred = True; \
                                         break; \
                                       } \
                                     } while (uPbusy != 0);
  #else
    #define AQ_API_UP_BUSY_MAX_CHECKS (AQ_MDIO_READS_PER_SECOND * 5)
  
    #define AQ_API_DECLARE_UP_BUSY_VARS AQ_API_Variable(AQ_GlobalGeneralStatus) \
                                        uint16_t uPbusy; \
                                        uint32_t numChecks; \
                                        AQ_boolean uPBusyTimeoutOccurred = False;
  
    #define AQ_API_CHECK_UP_NOT_BUSY AQ_API_Wait(1, port); \
                                     numChecks = 0; \
                                     do \
                                     { \
                                       AQ_API_Get(port->PHY_ID, AQ_GlobalGeneralStatus, processorIntensiveMdioOperationIn_Progress, uPbusy); \
                                       if (numChecks++ > AQ_API_UP_BUSY_MAX_CHECKS) \
                                       { \
                                         AQ_API_UP_BUSY_PRINT_STATEMENT \
                                         uPBusyTimeoutOccurred = True; \
                                         break; \
                                       } \
                                     } while (uPbusy != 0);
  #endif

  /* If a uP busy timeout occurred, return the corresponding return code; otherwise, return
   * retval.  retval should be a return code defined in AQ_ReturnCodes. */
  #define AQ_API_RETURN_UP_BUSY(retval) return (uPBusyTimeoutOccurred ? AQ_RET_UP_BUSY_TIMEOUT : retval);

#else
  #define AQ_API_DECLARE_UP_BUSY_VARS /* nothing */
  #define AQ_API_CHECK_UP_NOT_BUSY /* nothing */
  #define AQ_API_RETURN_UP_BUSY(retval) return retval;
#endif


/*! FW image version string maximum length. */
#define AQ_VERSION_STRING_SIZE 0x40

/*! The byte offset from top of DRAM to the FW image version string. */
#define AQ_VERSION_STRING_BLOCK_OFFSET 0x0200

/*! The byte address, in processor memory, of the start of the IRAM segment. */
#define AQ_IRAM_BASE_ADDRESS 0x40000000

/*! The byte address, in processor memory, of the start of the DRAM segment. */
#define AQ_DRAM_BASE_ADDRESS 0x3FFE0000

/*! The byte offset from the top of the PHY image to the header content (HHD devices). */
#define AQ_PHY_IMAGE_HEADER_CONTENT_OFFSET_HHD 0x300

/*! The byte offset from the top of the PHY image to the header content (APPIA devices). */
#define AQ_PHY_IMAGE_HEADER_CONTENT_OFFSET_APPIA 0

/*! The offset, from the start of DRAM, where the provisioning block begins. */
#define AQ_PHY_IMAGE_PROVTABLE_OFFSET 0x680

/*! The offset, from the start of DRAM, where the provisioning block's ending address is recorded. */
#define AQ_PHY_IMAGE_PROVTABLE_TERM_OFFSET 0x028C

/*! The size of the space alloted within the PHY image for the provisioning table. */
#define AQ_PHY_IMAGE_PROVTABLE_MAXSIZE 0x800

/*! The maximum number of polling cycles ever required before the FLASH interface is ready. */
#define AQ_FLASH_INTERFACE_MAX_POLL_COUNT 20

/*! The maximum number of ports that can be MDIO bootloaded at once. */
#define AQ_MAX_NUM_PHY_IDS 48

/*! The maximum allowed number of times to poll for debug-trace-freeze acknowledgement. */
#define AQ_MAX_FREEZE_CHECKS 2000

/*! The maximum size of the debug trace buffer. */
#define AQ_MAX_TRACE_BUFFER_LENGTH 8192

/*! The maximum allowed number of times to poll for SERDES Rx eye measurement done. */
#define AQ_SERDESEYE_MAX_DONE_CHECKS 50

/*! The maximum allowed number of times to poll for PIF mailbox status. */
#define MAX_NUM_COMMAND_STATUS_POLLS 700

/* REGDOC_START */


/**********************************************************************************************************************
*                                               MDIO Boot Load
**********************************************************************************************************************/

const uint16_t AQ_CRC16Table[256] = {0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
                                         0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
                                         0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
                                         0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
                                         0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
                                         0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
                                         0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
                                         0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
                                         0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
                                         0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
                                         0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
                                         0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
                                         0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
                                         0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
                                         0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
                                         0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
                                         0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
                                         0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
                                         0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
                                         0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
                                         0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
                                         0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
                                         0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
                                         0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
                                         0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
                                         0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
                                         0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
                                         0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
                                         0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
                                         0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
                                         0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
                                         0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

/*! \addtogroup writingImages
  @{
*/

/*! Prepare the specified port for MDIO bootloading.  Disables the daisy-chain,
 * and explicitly sets the port's provisioningAddress. */
void AQ_API_EnableMDIO_BootLoadMode 
(
    /*! The target PHY port.*/
  AQ_API_Port* port,
    /*! The provisioning address to use when the FW starts and applies the 
     * bootloaded image's provisioned values. */
  unsigned int provisioningAddress
)
{
  AQ_API_Variable(AQ_GlobalNvrProvisioning)

  AQ_API_DeclareLocalStruct(AQ_GlobalReservedProvisioning, globalReservedProvisioning)


  /* disable the daisy-chain */
  /* REGDOC: Read-Modify-Write bitfield (HHD/APPIA: 1E.C452.0) */
  AQ_API_Set(port->PHY_ID, AQ_GlobalNvrProvisioning, nvrDaisyChainDisable, 1);

  /* override the hop-count */
  AQ_API_AssignWordOfLocalStruct(globalReservedProvisioning, 1, 
      /* REGDOC: Read register (HHD/APPIA: 1E.C470 + 1) */
      AQ_API_ReadRegister(port->PHY_ID, AQ_GlobalReservedProvisioning, 1));
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C471.5:0) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalReservedProvisioning, globalReservedProvisioning, 
      daisy_chainHop_countOverrideValue, provisioningAddress);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C471.6) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalReservedProvisioning, globalReservedProvisioning, 
      enableDaisy_chainHop_countOverride, 1);
  /* REGDOC: Write register (HHD/APPIA: 1E.C470 + 1) */
  AQ_API_WriteRegister(port->PHY_ID, AQ_GlobalReservedProvisioning, 1, 
      AQ_API_WordOfLocalStruct(globalReservedProvisioning, 1));
  
  return;
}


/*! Prepare the specified port for MDIO bootloading, and set the temporary MDIO
 * address to be used during the bootload process.  Disables the daisy-chain,
 * and explicitly sets the port's provisioningAddress. */
void AQ_API_EnableGangLoadMode 
(
    /*! The target PHY port.*/
  AQ_API_Port* port,
    /*! The provisioning address to use when the FW starts and applies the 
     * bootloaded image's provisioned values. */
  unsigned int provisioningAddress, 
    /*! The PHY's MDIO address will be changed to this value during the 
     * bootload process. */
  unsigned int gangLoadAddress
)
{
  /*AQ_API_Variable_DeviceRestricted(APPIA, AQ_GlobalGeneralProvisioning)*/
  AQ_API_Variable(AQ_GlobalGeneralProvisioning)


  /* Get ready for MDIO bootloading. */
  AQ_API_EnableMDIO_BootLoadMode(port, provisioningAddress);

  /* Enable gangload mode.  After doing this, the PHY will be 
   * addressable at the MDIO address indicated by gangLoadAddress.
   * Now that the PHY is in gangload mode, MDIO reads are prohibited
   * until AQ_API_DisableGangLoadMode is called. */
  if (AQ_DEVICE_APPIA == port->device)
  {
    /* REGDOC: Read-Modify-Write bitfield (APPIA: 1E.C440.8:4) */
    AQ_API_Set_DeviceRestricted(APPIA, port->PHY_ID, AQ_GlobalGeneralProvisioning, 
        gangLoadMdioAddress, gangLoadAddress);
  }
  else if (AQ_DEVICE_HHD == port->device)
  {
    /* REGDOC: Read-Modify-Write bitfield (HHD: 1E.C447.4:0) */
    AQ_API_Set_DeviceRestricted(HHD, port->PHY_ID, AQ_GlobalGeneralProvisioning, 
        mdioBroadcastAddressConfiguration, gangLoadAddress);
    /* REGDOC: Read-Modify-Write bitfield (HHD: 1E.C441.E) */
    AQ_API_Set_DeviceRestricted(HHD, port->PHY_ID, AQ_GlobalGeneralProvisioning, 
        mdioBroadcastModeEnable, 1);
  }
  
  return;
}


/*! Restore the PHY's MDIO address to the pin-specified value.  Should be 
 * called when MDIO bootloading is complete, to return to normal MDIO 
 * addressing.
 * <b>This is a gang-load function, hence write-only!</b> */
void AQ_API_DisableGangLoadMode 
(
    /*! The target PHY port.*/
  AQ_API_Port* port,
    /*! The value to write to of AQ_GlobalGeneralProvisioning.u1.word_1. */
  uint16_t origVal_GGP1
)
{
  AQ_API_DeclareLocalStruct(AQ_GlobalGeneralProvisioning, globalGeneralProvisioning)


  /* Restore the original value of globalGeneralProvisioning.u1, and set 
   * the MDIO address reset bit.  This will cause the MDIO address to be
   * reset to the value indicated by the pins. */
  AQ_API_AssignWordOfLocalStruct(globalGeneralProvisioning, 1, origVal_GGP1);
  if (AQ_DEVICE_APPIA == port->device)
  {
    /* REGDOC: Assign to local representation of bitfield (APPIA: 1E.C441.2) */
    AQ_API_AssignBitfieldOfLocalStruct_DeviceRestricted(APPIA, AQ_GlobalGeneralProvisioning, 
        globalGeneralProvisioning, mdioAddressReset, 1);
  }
  else if (AQ_DEVICE_HHD == port->device)
  {
    /* REGDOC: Assign to local representation of bitfield (HHD: 1E.C441.E) */
    AQ_API_AssignBitfieldOfLocalStruct_DeviceRestricted(HHD, AQ_GlobalGeneralProvisioning, 
        globalGeneralProvisioning, mdioBroadcastModeEnable, 0);
  }  
  /* REGDOC: Write register (HHD/APPIA: 1E.C440 + 1) */
  AQ_API_WriteRegister(port->PHY_ID, AQ_GlobalGeneralProvisioning, 1, 
      AQ_API_WordOfLocalStruct(globalGeneralProvisioning, 1));

  /* The PHY has now exited gang-load mode. */
  return;
}


AQ_Retcode AQ_API_WriteBootLoadImageWithProvTable
(
  AQ_API_Port** ports,
  unsigned int numPorts,
  unsigned int* provisioningAddresses,
  AQ_Retcode* resultCodes,
  uint32_t* imageSizePointer,
  uint8_t* image,
  uint8_t gangload_MDIO_address,
  AQ_API_Port* gangloadPort,
  uint32_t* provTableSizePointer,
  uint8_t* provTableImage
)
{
  /*------------------------------------- NOTE!!!!!!!!!! ----------------------------------------------------------*/
  /* This function uses word level writes here as in gang-load mode we cannot do a read in a read-modify-write     */
  /* operation                                                                                                     */
  /*---------------------------------------------------------------------------------------------------------------*/

  AQ_API_Variable_DeviceRestricted(APPIA, AQ_GlobalPinStatus)

  AQ_API_DeclareLocalStruct(AQ_GlobalControl, globalControl)
  AQ_API_DeclareLocalStruct(AQ_GlobalMailboxInterface, globalMailboxInterface)

  AQ_API_Port* port;
  uint32_t primaryHeaderPtr = 0x00000000;
  uint32_t primaryIramPtr = 0x00000000;
  uint32_t primaryDramPtr = 0x00000000;
  uint32_t primaryIramSize = 0x00000000;
  uint32_t primaryDramSize = 0x00000000;
  uint32_t terminatorPtr = 0x00000000;
  uint32_t phyImageHeaderContentOffset;
  uint32_t i; 
  uint32_t j;
  uint32_t imageSize;
  uint32_t provTableImageSize = 0;
  uint32_t bytePointer;
  uint32_t byteSize; 
  uint32_t dWordSize;
#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE 
  uint32_t countPendingOps;                          /* A count of block MDIO operation pending... necessary to keep a count
                                                        in order to ensure we don't exceed the maximum pending operations. */
#endif
  uint16_t msw; 
  uint16_t lsw;
  uint16_t crc16Calculated; 
  uint16_t provTableCrc16Calculated; 
  uint16_t fileCRC; 
  uint16_t provTableFileCRC; 
  uint16_t mailboxCRC;
  uint16_t mailboxWrite;
  uint16_t bootLoadMode;
  uint16_t recordedGGP1Values[AQ_MAX_NUM_PHY_IDS];      /* When entering/exiting gangload mode, we record and restore
                                                        the AQ_GlobalGeneralProvisioning.u1 register values. */


  /* store the CRC-16 for the image, which is the last two bytes */
  imageSize = *imageSizePointer;
  fileCRC = image[imageSize-2] << 8 | image[imageSize-1];

  /*------------------------------------- Check the image integrity ------------------------------------------------*/
  crc16Calculated = 0x0000;
  for (i = 0; i < imageSize-2; i++)
  {
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ image[i]];
  }

  if (crc16Calculated != fileCRC)
  {
  #ifdef AQ_VERBOSE
    printf ("CRC check failed on image file (expected 0x%X, found 0x%X)\n",
        fileCRC, crc16Calculated);
  #endif
    for (j = 0; j < numPorts; j++) 
    {
      /* Before returning, set ALL result codes to indicate "bad image". */
      resultCodes[j] = AQ_RET_FLASH_IMAGE_CORRUPT;
    }
    return AQ_RET_ERROR;
  }
#ifdef AQ_VERBOSE
  else
  {
    printf ("CRC check good on image file (0x%04X)\n", crc16Calculated);
  }
#endif

  /*-------------------------------- Check the provisioning table image integrity ----------------------------------*/
  if (provTableSizePointer != NULL && provTableImage != NULL)
  {
    provTableImageSize = (*provTableSizePointer) - 2;
    provTableFileCRC = provTableImage[provTableImageSize + 1] << 8 | 
        provTableImage[provTableImageSize];

    provTableCrc16Calculated = 0x0000;
    for (i = 0; i < provTableImageSize; i++)
    {
      provTableCrc16Calculated = ((provTableCrc16Calculated & 0xFF) << 8) ^ 
          AQ_CRC16Table[(provTableCrc16Calculated >> 8) ^ provTableImage[i]];
    }

    if (provTableCrc16Calculated != provTableFileCRC)
    {
    #ifdef AQ_VERBOSE
      printf ("CRC check failed on provisioning table file (expected 0x%X, found 0x%X)\n",
          provTableFileCRC, provTableCrc16Calculated);
    #endif
      for (j = 0; j < numPorts; j++) 
      {
        /* Before returning, set ALL result codes to indicate "bad image". */
        resultCodes[j] = AQ_RET_FLASH_IMAGE_CORRUPT;
      }
      return AQ_RET_ERROR;
    }
  #ifdef AQ_VERBOSE
    else
    {
      printf ("CRC check good on provisioning table file (0x%04X)\n", 
          provTableCrc16Calculated);
    }
  #endif
  }

  /*------------------------ Check that all provisioning addresses are in the proper range. ------------------------*/
  for (j = 0; j < numPorts; j++) 
  {
    if (provisioningAddresses[j] > 47)
    {
    #ifdef AQ_VERBOSE
      printf ("Provisioning address out of range 0-47 (index %u: %d)\n", j, provisioningAddresses[j]);
    #endif
      for (j = 0; j < numPorts; j++) 
      {
        /* Before returning, set ALL result codes to indicate "provisioning address out-of-range". */
        resultCodes[j] = AQ_RET_BOOTLOAD_PROVADDR_OOR;
      }
      return AQ_RET_ERROR;
    }
  }

  /*--------------------------- Store 1E.C441 values for later use.  Enforce uniformity. ---------------------------*/
  for (j = 0; j < numPorts; j++) 
  {
    /* Record the original value of AQ_GlobalGeneralProvisioning.u1.word_1, 
     * so that we can restore it later after exiting gangload mode. */
    port = ports[j];
                            /* REGDOC: Read register (HHD/APPIA: 1E.C440 + 1) */
    recordedGGP1Values[j] = AQ_API_ReadRegister(port->PHY_ID, AQ_GlobalGeneralProvisioning, 1);

    /* If any of the PHYs' GGP1 values don't match the others, set the appropriate
     * error code and return. */
    if (j > 0 && recordedGGP1Values[j] != recordedGGP1Values[0])
    {
    #ifdef AQ_VERBOSE
      printf ("Non-uniform value of 1E.C441 found (expected 0x%X, found 0x%X)\n", 
          recordedGGP1Values[0], recordedGGP1Values[j]);
    #endif
      for (j = 0; j < numPorts; j++) 
      {
        /* Before returning, set ALL result codes to indicate "non-uniform GGP1 values". */
        resultCodes[j] = AQ_RET_BOOTLOAD_NONUNIFORM_REGVALS;
      }
      return AQ_RET_ERROR;
    }
  }

  /*--------------------------- Put each PHY into gangload mode at the specified address ---------------------------*/
  for (j = 0; j < numPorts; j++) 
  {
    AQ_API_EnableGangLoadMode(ports[j], provisioningAddresses[j], gangload_MDIO_address);
  }
  /* Set up the port context for using device-restricted macros while in
   * gangload mode. */
  port = gangloadPort;

  /*------------------------------------- Stall the uP  ------------------------------------------------------------*/
  AQ_API_AssignWordOfLocalStruct(globalControl, 1, 0x0000);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.6) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStallOverride, 1);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.0) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStall, 1);
  /* REGDOC: Write register (HHD/APPIA: 1E.C000 + 1) */
  AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalControl, 1, 
      AQ_API_WordOfLocalStruct(globalControl, 1));

  /*------------------------------------- Initialize the mailbox write command -------------------------------------*/
  AQ_API_AssignWordOfLocalStruct(globalMailboxInterface, 0, 0x0000);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.0200.E) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalMailboxInterface, globalMailboxInterface, upMailboxWriteMode, 1);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.0200.F) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalMailboxInterface, globalMailboxInterface, upMailboxExecuteOperation, 1);
  mailboxWrite = AQ_API_WordOfLocalStruct(globalMailboxInterface, 0);

  /*------------------------------------- Read the segment addresses and sizes -------------------------------------*/
  primaryHeaderPtr = (((image[0x9] & 0x0F) << 8) | image[0x8]) << 12;

  if (AQ_DEVICE_APPIA == port->device)
    phyImageHeaderContentOffset = AQ_PHY_IMAGE_HEADER_CONTENT_OFFSET_APPIA;
  else /* HHD */
    phyImageHeaderContentOffset = AQ_PHY_IMAGE_HEADER_CONTENT_OFFSET_HHD;

  primaryIramPtr =  (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x4 + 2] << 16) |
                    (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x4 + 1] << 8)  |
                     image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x4];
  primaryIramSize = (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x7 + 2] << 16) |
                    (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x7 + 1] << 8)  |
                     image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x7];
  primaryDramPtr =  (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xA + 2] << 16) |
                    (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xA + 1] << 8)  |
                     image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xA];
  primaryDramSize = (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xD + 2] << 16) |
                    (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xD + 1] << 8)  |
                     image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xD];

  if (AQ_DEVICE_HHD == port->device)
  {
    primaryIramPtr += primaryHeaderPtr;
    primaryDramPtr += primaryHeaderPtr;
  }

#ifdef AQ_VERBOSE
  printf ("\nSegment Addresses and Sizes as read from the PHY ROM image header:\n\n");
  printf ("Primary Iram Address: 0x%x\n", primaryIramPtr);
  printf ("Primary Iram Size: 0x%x\n", primaryIramSize);
  printf ("Primary Dram Address: 0x%x\n", primaryDramPtr);
  printf ("Primary Dram Size: 0x%x\n\n", primaryDramSize);
#endif

  /*----------------------------- Merge the provisioning table into the main image ---------------------------------*/
  if (provTableSizePointer != NULL && provTableImage != NULL)
  {
    /* Locate the terminator of the built-in provisioning table */
    terminatorPtr = primaryDramPtr + 
        ((image[primaryDramPtr + AQ_PHY_IMAGE_PROVTABLE_TERM_OFFSET + 1] << 8) | 
        image[primaryDramPtr + AQ_PHY_IMAGE_PROVTABLE_TERM_OFFSET]);

  #ifdef AQ_VERBOSE
    printf("Supplied Provisioning Table At Address: 0x%x\n\n", terminatorPtr);
  #endif

    /* Check that the supplied provisioning table will fit within the alloted
     * space. */
    if (terminatorPtr - (primaryDramPtr + AQ_PHY_IMAGE_PROVTABLE_OFFSET) + 
        provTableImageSize > AQ_PHY_IMAGE_PROVTABLE_MAXSIZE)
    {
      for (j = 0; j < numPorts; j++) 
      {
        /* Before returning, set ALL result codes to indicate "provisioning 
         * table too large". */
        resultCodes[j] = AQ_RET_BOOTLOAD_PROVTABLE_TOO_LARGE;
      }
      return AQ_RET_ERROR;
    }

    /* Write the supplied provisioning table into the image, starting at the 
     * terminator address. */
    for (i = 0; i < provTableImageSize; i++)
    {
      image[terminatorPtr + i] = provTableImage[i];
    }
  }

  /*------------------------------------- Load IRAM and DRAM -------------------------------------------------------*/
  /* clear the mailbox CRC */
  AQ_API_AssignWordOfLocalStruct(globalMailboxInterface, 0, 0x0000);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.0200.C) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalMailboxInterface, globalMailboxInterface, resetUpMailboxCrc, 1);
  /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
  AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, 
      AQ_API_WordOfLocalStruct(globalMailboxInterface, 0));
  /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
  AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, 0x0000);
  
  crc16Calculated = 0;         /* This is to calculate what was written through the mailbox */

  /* load the IRAM */
#ifdef AQ_VERBOSE
  printf ("\nLoading IRAM:\n\n");
#endif
  
  /* dWord align the address: note the image addressing is byte based, but is properly aligned on dWord
     boundaries, so the 2 LSbits of the block start are always zero. */
  msw = (uint16_t) (AQ_IRAM_BASE_ADDRESS >> 16);
  AQ_API_AssignWordOfLocalStruct(globalMailboxInterface, 3, 0x0000);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.0203.1:0) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalMailboxInterface, globalMailboxInterface, 
      upMailboxAddressLSW , (AQ_IRAM_BASE_ADDRESS & 0xFFFF) >> 2);
  lsw = AQ_API_WordOfLocalStruct(globalMailboxInterface, 3);
  /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 2) */
  AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 2, msw);              /* MSW */
  /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 3) */
  AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 3, lsw);              /* LSW */


  /* set block size so that there are from 0-3 bytes remaining */
  byteSize = primaryIramSize;
  dWordSize = byteSize >> 2;

  bytePointer = primaryIramPtr;
#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE 
  countPendingOps = 0;
#endif
  for (i = 0; i < dWordSize; i++)
  {
    /* write 4 bytes of data */
    lsw = (image[bytePointer+1] << 8) | image[bytePointer];
    bytePointer += 2;
    msw = (image[bytePointer+1] << 8) | image[bytePointer];
    bytePointer += 2;

  #ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE 
    AQ_API_BlockWriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
    AQ_API_BlockWriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);
    AQ_API_BlockWriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
          
    countPendingOps += 3;
    /* Check if we've filled our output buffer, and if so, flush. */
    if (countPendingOps >= AQ_API_MDIO_MaxBlockOperations() - 3 )
    {
      AQ_API_MDIO_BlockOperationExecute (gangloadPort->PHY_ID); 
      countPendingOps = 0;
    }
  #else
    /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 4) */
    AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
    /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 5) */
    AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);
    /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
    AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
  #endif
  
    /* update the calculated CRC */
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw >> 8)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw & 0xFF)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw >> 8)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw & 0xFF)];
  
  #ifdef AQ_VERBOSE
    if (i && ((i % 512) == 0)) printf("    Byte: %X:\n", i << 2);
  #endif
  }
  
#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE 
  /* flush the output buffer one last time. */
  AQ_API_MDIO_BlockOperationExecute(gangloadPort->PHY_ID); 
  countPendingOps = 0;
#endif
  
  /* Note: this final write right-justifies non-dWord data in the final dWord */
  switch (byteSize & 0x3)
  {
    case 0x1:
      /* write 1 byte of data */
      lsw = image[bytePointer++];
      msw = 0x0000;
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 4) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 5) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);

      /* no polling */
      /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
      break;

    case 0x2:
      /* write 2 bytes of data */
      lsw = (image[bytePointer+1] << 8) | image[bytePointer];
      bytePointer += 2;
      msw = 0x0000;
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 4) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 5) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);

      /* no polling */
      /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
      break;

    case 0x3:
      /* write 3 bytes of data */
      lsw = (image[bytePointer+1] << 8) | image[bytePointer];
      bytePointer += 2;
      msw = image[bytePointer++];
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 4) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 5) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);

      /* no polling */
      /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
      break;
  }
  
  if (byteSize & 0x3)
  {
   /* update the calculated CRC */
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw >> 8)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw & 0xFF)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw >> 8)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw & 0xFF)];    
  }
  
  /* load the DRAM */
#ifdef AQ_VERBOSE
  printf ("\nCRC-16 after loading IRAM: 0x%X\n", crc16Calculated);
  printf ("\nLoading DRAM:\n\n");
#endif
  
  /* dWord align the address: note the image addressing is byte based, but is properly aligned on dWord
     boundaries, so the 2 LSbits of the block start are always zero. */
  msw = (uint16_t) (AQ_DRAM_BASE_ADDRESS >> 16);
  AQ_API_AssignWordOfLocalStruct(globalMailboxInterface, 3, 0x0000);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.0203.1:0) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalMailboxInterface, globalMailboxInterface, 
      upMailboxAddressLSW, (AQ_DRAM_BASE_ADDRESS & 0xFFFF) >> 2);
  lsw = AQ_API_WordOfLocalStruct(globalMailboxInterface, 3);
  /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 2) */
  AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 2, msw);              /* MSW */
  /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 3) */
  AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 3, lsw);              /* LSW */


  /* set block size so that there are from 0-3 bytes remaining */
  byteSize = primaryDramSize;
  dWordSize = byteSize >> 2;

  bytePointer = primaryDramPtr;
  for (i = 0; i < dWordSize; i++)
  {
    /* write 4 bytes of data */
    lsw = (image[bytePointer+1] << 8) | image[bytePointer];
    bytePointer += 2;
    msw = (image[bytePointer+1] << 8) | image[bytePointer];
    bytePointer += 2;

  #ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE 
    AQ_API_BlockWriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
    AQ_API_BlockWriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);
    AQ_API_BlockWriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
          
    countPendingOps += 3;
    /* Check if we've filled our output buffer, and if so, flush. */
    if (countPendingOps >= AQ_API_MDIO_MaxBlockOperations() - 3 )
    {
      AQ_API_MDIO_BlockOperationExecute (gangloadPort->PHY_ID); 
      countPendingOps = 0;
    }
  #else
    /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 4) */
    AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
    /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 5) */
    AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);
    /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
    AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
  #endif
  
    /* update the calculated CRC */
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw >> 8)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw & 0xFF)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw >> 8)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw & 0xFF)];
  
  #ifdef AQ_VERBOSE
    if (i && ((i % 512) == 0)) printf("    Byte: %X:\n", i << 2);
  #endif
  }
  
#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE 
  /* flush the output buffer one last time. */
  AQ_API_MDIO_BlockOperationExecute(gangloadPort->PHY_ID); 
  countPendingOps = 0;
#endif
  
  /* Note: this final write right-justifies non-dWord data in the final dWord */
  switch (byteSize & 0x3)
  {
    case 0x1:
      /* write 1 byte of data */
      lsw = image[bytePointer++];
      msw = 0x0000;
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 4) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 5) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);

      /* no polling */
      /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
      break;

    case 0x2:
      /* write 2 bytes of data */
      lsw = (image[bytePointer+1] << 8) | image[bytePointer];
      bytePointer += 2;
      msw = 0x0000;
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 4) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 5) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);

      /* no polling */
      /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
      break;

    case 0x3:
      /* write 3 bytes of data */
      lsw = (image[bytePointer+1] << 8) | image[bytePointer];
      bytePointer += 2;
      msw = image[bytePointer++];
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 4) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 4, msw);
      /* REGDOC: Write register (HHD/APPIA: 1E.0200 + 5) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 5, lsw);

      /* no polling */
      /* REGDOC: Write register (HHD/APPIA: 1E.0200) */
      AQ_API_WriteRegister(gangloadPort->PHY_ID, AQ_GlobalMailboxInterface, 0, mailboxWrite);
      break;
  }
  
  if (byteSize & 0x3)
  {
   /* update the calculated CRC */
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw >> 8)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw & 0xFF)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw >> 8)];
    crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw & 0xFF)];    
  }

  /*------------------------------------- Exit gangload mode -------------------------------------------------------*/
  AQ_API_DisableGangLoadMode(gangloadPort, recordedGGP1Values[0]);

  /*------------------------------------- Check mailbox CRCs -------------------------------------------------------*/
  /* check to make sure the mailbox CRC matches the calculated CRC */
  /*foundMailboxCRCMismatch = False;*/
  for (j = 0; j < numPorts; j++) 
  {
                 /* REGDOC: Read register (HHD/APPIA: 1E.0200 + 1) */
    mailboxCRC = AQ_API_ReadRegister(ports[j]->PHY_ID,AQ_GlobalMailboxInterface, 1);
    if (mailboxCRC != crc16Calculated)
    {
    #ifdef AQ_VERBOSE
      printf("\n%uth port: Mailbox CRC-16 (0x%X) does not match calculated CRC-16 (0x%X)\n", 
          j, mailboxCRC, crc16Calculated);
    #endif
      /* Note that we can't just return here, because we still need to 
       * release the uPs for the other PHYs that might have been
       * bootloaded successfully. */
      resultCodes[j] = AQ_RET_BOOTLOAD_CRC_MISMATCH;
    }
  #ifdef AQ_VERBOSE
    else
    {
      printf("\n%uth port: Image load good - mailbox CRC-16 matches (0x%X)\n", 
          j, mailboxCRC);
      resultCodes[j] = AQ_RET_OK;
    }
  #endif
  }

  /*------------------------------------- Clear any resets ---------------------------------------------------------*/
  for (j = 0; j < numPorts; j++) 
  {
    /* REGDOC: Write register (HHD/APPIA: 1E.0000) */
    AQ_API_WriteRegister(ports[j]->PHY_ID,AQ_GlobalStandardControl_1, 0, 0x0000);
  }

  /*------------------------------------- Release the uP -----------------------------------------------------------*/
  AQ_API_AssignWordOfLocalStruct(globalControl, 1, 0x0000);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.6) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStallOverride, 1);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.0) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStall, 1);
  for (j = 0; j < numPorts; j++) 
  {
    /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.F) */
    AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upReset, 0);
    /* REGDOC: Write register (HHD/APPIA: 1E.C000 + 1) */
    AQ_API_WriteRegister(ports[j]->PHY_ID,AQ_GlobalControl, 1, 
        AQ_API_WordOfLocalStruct(globalControl, 1));
    /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.F) */
    AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upReset, 1);
    /* REGDOC: Write register (HHD/APPIA: 1E.C000 + 1) */
    AQ_API_WriteRegister(ports[j]->PHY_ID,AQ_GlobalControl, 1, 
        AQ_API_WordOfLocalStruct(globalControl, 1));
  }
  
  /* Need to wait at least 100us. */
  AQ_API_Wait(1, ports[0]);
  
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.F) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upReset, 0);
  /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.0) */
  AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStall, 0);
  for (j = 0; j < numPorts; j++) 
  {
    if (AQ_DEVICE_APPIA == port->device)
    {
      /* If the BOOT_LD pins are set to MDIO boot load mode, we can't clear the
       * uP run stall override bit.  If we did, the uP would stall. */
      /* REGDOC: Read bitfield (APPIA: 1E.C840.E:D) */
      AQ_API_Get_DeviceRestricted(APPIA, j, AQ_GlobalPinStatus, mdioBootLoad, bootLoadMode);
      if (bootLoadMode == 0x1)
      {
        /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.6) */
        AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStallOverride, 1);
      }
      else
      {
        /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.6) */
        AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStallOverride, 0);
      }
    }
    else
    {
      /* For post-APPIA devices, always set the uP stall override bit to 
       * smooth over any packaging differences WRT the boot load pin. */
      /* REGDOC: Assign to local representation of bitfield (HHD/APPIA: 1E.C001.6) */
      AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStallOverride, 1);
    }

    /* REGDOC: Write register (HHD/APPIA: 1E.C000 + 1) */
    AQ_API_WriteRegister(ports[j]->PHY_ID,AQ_GlobalControl, 1, 
        AQ_API_WordOfLocalStruct(globalControl, 1));
  }
  
  /* NOTE!!! We can't re-enable the daisy-chain here, as this will overwrite the IRAM and DRAM with the FLASH contents*/

  /* If any of the ports was not bootloaded successfully, return AQ_RET_ERROR */
  for (j = 0; j < numPorts; j++) 
  {
    if (resultCodes[j] != AQ_RET_OK)
      return AQ_RET_ERROR;
  }

  /* All ports were bootloaded successfully. */
  return AQ_RET_OK;
}


AQ_Retcode AQ_API_WriteBootLoadImage
(
  AQ_API_Port** ports,
  unsigned int numPorts,
  unsigned int* provisioningAddresses,
  AQ_Retcode* resultCodes,
  uint32_t* imageSizePointer,
  uint8_t* image,
  uint8_t gangload_MDIO_address,
  AQ_API_Port* gangloadPort
)
{
  return AQ_API_WriteBootLoadImageWithProvTable(ports, numPorts, 
      provisioningAddresses, resultCodes, imageSizePointer, image, 
      gangload_MDIO_address, gangloadPort, NULL, NULL);
}


AQ_Retcode AQ_API_EnableDaisyChain
(
    /*! The target PHY port.*/
  AQ_API_Port* port
)
{
  
  /* declare local variables */
  AQ_API_Variable(AQ_GlobalNvrProvisioning)
  AQ_API_Variable(AQ_GlobalReservedProvisioning)

  /* disable the hop-count override */
  /* REGDOC: Read-Modify-Write bitfield (HHD/APPIA: 1E.C471.6) */
  AQ_API_Set(port->PHY_ID, AQ_GlobalReservedProvisioning, enableDaisy_chainHop_countOverride, 0);

  /* enable the daisy-chain */
  /* REGDOC: Read-Modify-Write bitfield (HHD/APPIA: 1E.C452.0) */
  AQ_API_Set(port->PHY_ID, AQ_GlobalNvrProvisioning, nvrDaisyChainDisable, 0);
  
  return AQ_RET_OK;
}

/*@}*/


#ifdef __cplusplus
}
#endif
