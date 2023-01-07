#ifndef _HOSTDL_PACKET_H_
#define _HOSTDL_PACKET_H_

/*==================================================================
 *
 * FILE:        hostdl_packet.h
 *
 * SERVICES:    None
 *
 * DESCRIPTION:
 *   This header file contains externalized definitions from the packet layer.
 *   It is used by the stream buffering layer.
 *
 * Copyright (c) 2008-2010, 2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

/* Maximum data length. */
/* Currently QPST tool supports a maximum packet length of 4096. This 
 * value can be tuned to use bigger packet sizes for better performance 
 * if the QPST tool supports bigger size packets */
#define UNFRAMED_MAX_DATA_LENGTH 1024

/* Older versions of QPST only supported packet sizes up to 1024 bytes in
 * length. This value is used in the hello packet to support older versions
 * of QPST
 */
#define FRAMED_MAX_DATA_LENGTH 1024

/* Number of packets. NUMBER_OF_PACKETS * UNFRAMED_MAX_DATA_LENGTH will be our
   maximum window size. */
#define NUMBER_OF_PACKETS 2

/* ----------------------------------------------------------------------
 * Defines used to calculate size of reply buffer to QPST and also
 * number of Flash part sectors that will fit into that reply buffer.
 * Code in packet layer will cause a fatal error if the size of the
 * buffer is exceeded at run time.  Code in NOR flash layer will cause
 * a fatal error at compile time if number of sectors is exceeded.
  ----------------------------------------------------------------------*/
#define HOST_REPLY_BUFFER_SIZE  1024

/* Fixed size elements of Parameter Request Reply packet */
#define PACKET_OVERHEAD_SIZE    7
#define CMD_SIZE                1
#define MAGIC_SIZE              32
#define VERSION_SIZE            1
#define COMPAT_VERSION_SIZE     1
#define BLOCK_SIZE_SIZE         4
#define FLASH_BASE_SIZE         4
#define FLASH_ID_LEN_SIZE       1
#define WINDOW_SIZE_SIZE        2
#define NUM_SECTORS_SIZE        2
#define FEATURE_BITS_SIZE       4

/* Variable size element of Parameter Request Reply packet.  The length
 * of the Flash ID string is indeterminate, but currently the largest is
 * 16 bytes, so allow double this size for growth. */
#define FLASH_ID_STRING_SIZE    32


/* Add up all the parts except sectors */
#define REPLY_FIXED_SIZE (PACKET_OVERHEAD_SIZE+CMD_SIZE+MAGIC_SIZE+ \
  VERSION_SIZE+COMPAT_VERSION_SIZE+BLOCK_SIZE_SIZE+FLASH_BASE_SIZE+ \
  FLASH_ID_LEN_SIZE+WINDOW_SIZE_SIZE+NUM_SECTORS_SIZE+ \
  FEATURE_BITS_SIZE+FLASH_ID_STRING_SIZE)


#define REPLY_BUFFER_SIZE   HOST_REPLY_BUFFER_SIZE

/* Calculate amount of 4 byte sector sizes which fit in remaining
 * portion of parameter request reply
 */
#define MAX_SECTORS      ((REPLY_BUFFER_SIZE - REPLY_FIXED_SIZE) / 4)


#define DEVICE_UNKNOWN  0xFF

/* Maximum packet size.  1 for packet type.  4 for length.  2 for CRC. 
 * 9 for any other overhead */
#define MAX_PACKET_SIZE (UNFRAMED_MAX_DATA_LENGTH + 1 + 4 + 2 + 9)


#define STREAM_DLOAD_MAX_VER         0x04
#define STREAM_DLOAD_MIN_VER         0x02

#define UNFRAMED_DLOAD_MIN_VER       0x04

/* UART does not support protocol versions > 3 because it does not have 
 * hardware error checking and CRC as required by the unframed stream write
 * command
 */
#define UART_DLOAD_MAX_VER           0x03

#if 0
#if defined(USE_UART_ONLY) && (STREAM_DLOAD_MAX_VER > UART_DLOAD_MAX_VER)
  #warning UART does not support protocol versions beyond UART_DLOAD_MAX_VER. \
    Reverting to an earlier protocol version.
  #undef STREAM_DLOAD_MAX_VER
  #define STREAM_DLOAD_MAX_VER UART_DLOAD_MAX_VER
#endif
#endif

#define FEATURE_UNCOMPRESSED_DLOAD   0x00000001

/* We only support these on NAND targets */
#define FEATURE_NAND_PRIMARY_IMAGE      0x00000002
#define FEATURE_NAND_BOOTLOADER_IMAGE   0x00000004
#define FEATURE_NAND_MULTI_IMAGE        0x00000008

/* This version of the downloader does not support the old NAND two
 * image mode, only Multi-Image
 */
#define SUPPORTED_FEATURES \
  (FEATURE_UNCOMPRESSED_DLOAD | FEATURE_NAND_MULTI_IMAGE)

#define READ_LEN                     7



/* Command/Rsp codes, DUMMY_RSP should alwaus be the last one */
#define HELLO_REQ                    0x01
#define HELLO_RSP                    0x02
#define READ_RSP                     0x04
#define WRITE_RSP                    0x06
#define STREAM_WRITE_RSP             0x08
#define NOP_RSP                      0x0A
#define RESET_RSP                    0x0C
#define ERROR_RSP                    0x0D
#define CMD_LOG                      0x0E
#define UNLOCK_RSP                   0x10
#define PWRDOWN_RSP                  0x12
#define OPEN_RSP                     0x14
#define CLOSE_RSP                    0x16
#define SECURITY_MODE_RSP            0x18
#define PARTITION_TABLE_RSP          0x1A
#define OPEN_MULTI_IMAGE_RSP         0x1C
#define ERASE_RSP                    0x1E

/* Gap here for commands implemented in QPST but not phone side */
#define UNFRAMED_STREAM_WRITE_CMD    0x30 /* Needed for command parsing */
#define UNFRAMED_STREAM_WRITE_RSP    0x31
#define DUMMY_RSP                    0x32

/* Only dispatch commands if they fall in the valid command range. */
#define FIRST_COMMAND 0x01
#define LAST_COMMAND  0x31

/* Check to make sure that LAST_COMMAND is in agreement */
#if (DUMMY_RSP != (LAST_COMMAND + 1))
#error LAST_COMMAND and DUMMY_RSP mismatch. Bailing out!
#endif


/* Length of buffer for size message back to host.  Increase this
 * value if you change the size of size_msg_fixed.  Used by the
 * function log_size_msg().
 */
#define SIZE_MSG_LEN  64

/* Byte offset defines for Hello packet response */
#define HELLO_CMD_OFFSET             0
#define HELLO_MAGIC_NUM_OFFSET       1
#define HELLO_MAX_VER_OFFSET        33
#define HELLO_MIN_VER_OFFSET        34
#define HELLO_MAX_DATA_SZ_1_OFFSET  35
#define HELLO_MAX_DATA_SZ_2_OFFSET  36
#define HELLO_MAX_DATA_SZ_3_OFFSET  37
#define HELLO_MAX_DATA_SZ_4_OFFSET  38

#endif /* _HOSTDL_PACKET_H_ */
