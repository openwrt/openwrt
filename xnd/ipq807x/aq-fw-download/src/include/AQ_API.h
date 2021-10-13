/*
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
*/

/*! \file
    This file contains the AQ_API function and datatype declarations. */


#ifndef AQ_API_TOKEN
#define AQ_API_TOKEN

#include <stdint.h>

#include "AQ_User.h"
#include "AQ_ReturnCodes.h"

/*******************************************************************
                         General
*******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! This typedef defines the bool datatype which takes the values 
true and false.*/
typedef enum {False = 0, True = 1} AQ_boolean;


/*@}*/


/*******************************************************************
                        Device Identity
*******************************************************************/

/*! \defgroup deviceIdentity Device Identity
All AQ_API functions accept a parameter identifying the target PHY that
should be acted upon. */
/*@{*/


/*! This enumeration is used to describe the different types of
    Aquantia PHY.*/
typedef enum    
{    
  /*! 1/2/4-port package, 40nm architechture.*/
 AQ_DEVICE_APPIA,   
  /*! 1/2/4-port package, 28nm architechture.*/
 AQ_DEVICE_HHD   
}  AQ_API_Device;  

/*! This structure is used to specify a particular Aquantia PHY port
    within the system.*/
typedef struct    
{    
  /*! The type of Aquantia PHY*/
 AQ_API_Device  device; 
  /*! Uniquely identifies the port within the system. AQ_Port must be
      defined to whatever data type is suitable for the platform.
      AQ_API functions will never do anything with PHY_ID other than
      pass it down to the platform's PHY register read/write
      functions.*/
 AQ_Port  PHY_ID; 
} AQ_API_Port;    

/*@}*/


/*! This function boot-loads the instruction and data memory (IRAM and
    DRAM) of a set of Aquantia PHYs from a .cld format image file (the
    same image file used to burn the FLASH). During boot-load of each
    Aquantia PHY, the processor is halted, and after programming is
    complete the processor is released. Note that calling this
    function leaves the daisy-chain disabled to prevent RAM over-
    write. To exit MDIO boot-load mode, use the function
    AQ_API_EnableDaisyChain.
    Unlike most of the other functions in this API, this function can
    operate on a group of PHYs simultaneously. This is referred to as
    gang-loading. To facilitate this, this function takes as
    parameters 3 parallel arrays: PHY_IDs, provisioningAddresses, and
    resultCodes. The length of these arrays must be identical, and is
    specified by the num_PHY_IDs parameter.
    In order to check the integrity of the boot-load operation, a
    CRC-16 value is calculated over the IRAM and DRAM. After the image
    has been loaded, this value is directly compared against each
    PHY's Mailbox CRC-16 in 1E.0201.
    The value of register 1E.C441 must be the same for all the boot-
    loaded PHYs. This will be checked before the boot-load is
    performed, and if a non-uniform value is read from any of the
    PHYs, the function will fail before any writes are performed.
    A separate result code is returned for each of the boot-loaded
    PHYs, in the OUT parameter, resultCodes.
    Individual Port Return codes:
    AQ_RET_BOOTLOAD_PROVADDR_OOR: The specified provisioning address
    was outside of the permitted range.
    AQ_RET_BOOTLOAD_NONUNIFORM_REGVALS: The values of the register(s)
    that must be uniform across the ports being bootloaded were not
    uniform.
    AQ_RET_BOOTLOAD_CRC_MISMATCH: The image was completely loaded into
    memory, but the after the port exited bootload the running
    checksum that was read from the uP memory mailbox was not the
    expected value. This indicates that the memory has potentially
    been corrupted, and the PHY should be reset before trying the
    bootload again.
    Overall Return codes (the return value from the function call):
    AQ_RET_OK: all ports were successfully bootloaded.
    AQ_RET_ERROR: One or more ports were not successfully bootloaded.
    */
AQ_Retcode   AQ_API_WriteBootLoadImage 
(    
  /*! An array identifying the target PHY ports.*/
 AQ_API_Port**  ports, 
  /*! The length of the arrays ports, provisioningAddresses, and
      resultCodes. These are parallel arrays, and must all be of the
      same length.*/
 unsigned int  numPorts, 
  /*! The provisioning addresses of each of the PHYs specified in
      ports. This can range from 0 through 47, and is also known as
      the daisy-chain address or the hop-count. If the PHYs are
      connected to a FLASH using the daisy-chain, this is the distance
      from the PHY to the FLASH, and is used to identify customized
      provisioning for each PHY from the provisioning data within the
      image. Otherwise, it is an arbitrary number. The length of this
      array must match the length of ports.*/
 unsigned int*  provisioningAddresses, 
  /*! OUT: The result code indicating success or failure of boot-
      loading each of the PHYs specified in ports.*/
 AQ_Retcode*  resultCodes, 
  /*! A pointer to the size of the image (in bytes) that is being
      loaded into the Aquantia PHY.*/
 uint32_t*  imageSizePointer, 
  /*! The image being loaded into the Aquantia PHY. This is the same
      regardless of whether the target is internal RAM or FLASH.*/
 uint8_t*  image, 
  /*! The 5-bit address to be used during the gang-loading operation.
      During the boot-loading process, each of the PHYs specified in
      ports will be changed such that they are addressed on the MDIO
      bus at gangloadAddress. This allows all the PHYs to be loaded
      simultaneously. Before returning, each PHY will be moved back to
      its original MDIO address. If ports contains only a single
      element, callers will probably want to use the PHY's original
      MDIO address for this parameter.*/
 uint8_t  gangload_MDIO_address, 
  /*! The address of the PHYs while in gangload mode.  This is
      ultimately some combination of the system address and the
      gangload MDIO address, specified by gangload_MDIO_address.  For
      most platforms, gangload_MDIO_address and gangload_PHY_ID should
      have the same value.*/
 AQ_API_Port*  gangloadPort 
);    

/*! This function boot-loads the instruction and data memory (IRAM and
    DRAM) of a set of Aquantia PHYs from a .cld format image file (the
    same image file used to burn the FLASH), as well as a separately
    provided provisioning table image file.The provisioning table
    image allows additional provisioning to be provided, beyond what
    is built in to the .cld image. If provTableSizePointer or
    provTableImage are NULL, this function behaves like
    AQ_API_WriteBootLoadImage.
    Aside from the additional provisioing table, this function behaves
    exactly the same as AQ_API_WriteBootLoadImage. For additional
    documentation and information on return codes, refer to
    AQ_API_WriteBootLoadImage.
    Individual Port Return codes (same as AQ_API_WriteBootLoadImage,
    plus):
    AQ_RET_BOOTLOAD_PROVTABLE_TOO_LARGE: The supplied provisioning
    table image does not fit within the alloted space.*/
AQ_Retcode   AQ_API_WriteBootLoadImageWithProvTable 
(    
  /*! An array identifying the target PHY ports.*/
 AQ_API_Port**  ports, 
  /*! The length of the arrays ports, provisioningAddresses, and
      resultCodes. These are parallel arrays, and must all be of the
      same length.*/
 unsigned int  numPorts, 
  /*! The provisioning addresses of each of the PHYs specified in
      ports. This can range from 0 through 47, and is also known as
      the daisy-chain address or the hop-count. If the PHYs are
      connected to a FLASH using the daisy-chain, this is the distance
      from the PHY to the FLASH, and is used to identify customized
      provisioning for each PHY from the provisioning data within the
      image. Otherwise, it is an arbitrary number. The length of this
      array must match the length of ports.*/
 unsigned int*  provisioningAddresses, 
  /*! OUT: The result code indicating success or failure of boot-
      loading each of the PHYs specified in ports.*/
 AQ_Retcode*  resultCodes, 
  /*! A pointer to the size of the image (in bytes) that is being
      loaded into the Aquantia PHY.*/
 uint32_t*  imageSizePointer, 
  /*! The image being loaded into the Aquantia PHY. This is the same
      regardless of whether the target is internal RAM or FLASH.*/
 uint8_t*  image, 
  /*! The 5-bit address to be used during the gang-loading operation.
      During the boot-loading process, each of the PHYs specified in
      ports will be changed such that they are addressed on the MDIO
      bus at gangloadAddress. This allows all the PHYs to be loaded
      simultaneously. Before returning, each PHY will be moved back to
      its original MDIO address. If ports contains only a single
      element, callers will probably want to use the PHY's original
      MDIO address for this parameter.*/
 uint8_t  gangload_MDIO_address, 
  /*! The address of the PHYs while in gangload mode.  This is
      ultimately some combination of the system address and the
      gangload MDIO address, specified by gangload_MDIO_address.  For
      most platforms, gangload_MDIO_address and gangload_PHY_ID should
      have the same value.*/
 AQ_API_Port*  gangloadPort, 
  /*! A pointer to the size of the provTableImage (in bytes) that is
      being loaded into the Aquantia PHY.*/
 uint32_t*  provTableSizePointer, 
  /*! The additional provisioning table image being loaded into the
      Aquantia PHY.*/
 uint8_t*  provTableImage 
);    

/*! Calling this function disables boot-loading and enables the daisy-
    chain. This would typically be called after using MDIO boot-
    loading on a daisy-chain enabled PHY. Re-enabling the daisy-chain
    after performing an MDIO bootload will cause the PHY to reboot
    from FLASH.*/
AQ_Retcode   AQ_API_EnableDaisyChain 
(    
  /*! The target PHY port.*/
 AQ_API_Port*  port 
);    


#ifdef __cplusplus
}
#endif

#endif
