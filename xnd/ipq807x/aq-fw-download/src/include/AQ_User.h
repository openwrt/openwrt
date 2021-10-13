/*AQ_User.h*/

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
*   This file contains preprocessor symbol definitions and type definitions 
*   for the platform-integrator controlled compile-time AQ_API options.
*
************************************************************************************/

/*! \file
  This file contains preprocessor symbol definitions and type definitions 
  for the platform-integrator controlled compile-time AQ_API options.
*/

#ifndef AQ_USER_TOKEN
#define AQ_USER_TOKEN

/*! \defgroup User User Definitions
This module contains the definitions used to configure AQ_API behavior as desired. */
/*@{*/


/*! Specify the proper data type for AQ_Port. This will depend on the
 * platform-specific implementation of the PHY register read/write functions.*/
typedef unsigned int AQ_Port;


/*! If defined, AQ_API functions will print various error and info messages 
 * to stdout.  If not, nothing will be printed and AQ_API.c will NOT include
 * stdio.h. */
#define AQ_VERBOSE


/*! If defined, the PHY interface supports block (asynchronous) read/write 
 * operation. If AQ_PHY_SUPPORTS_BLOCK_READ_WRITE is defined, then 
 * the API will call the block-operation functions and so implementations 
 * for each must be provided. If AQ_PHY_SUPPORTS_BLOCK_READ_WRITE is not 
 * defined, they will not be called, and need not be implemented. */
#undef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE


/*! If defined, time.h exists, and so the associated functions wil be used to 
 * compute the elapsed time spent in a polling loop, to ensure that the 
 * maximum time-out period will not be exceeded.  If not defined, then 
 * AQ_MDIO_READS_PER_SECOND will be used to calculate the minimum possible 
 * elapsed time. */
#define AQ_TIME_T_EXISTS


/*! The maximum number of synchronous PHY register reads that can be performed 
 * per second. A worst case number can be derived as follows: 
 *
 * AQ_MDIO_READS_PER_SECOND = MDIO Clock Frequency / 64 
 *
 * If using MDIO preamble suppression, multiply this number by 2 
 *
 * For instance, if a 5MHz MDIO clock is being used without preamble supression 
 * AQ_MDIO_READS_PER_SECOND = 78125 
 *
 * If AQ_TIME_T_EXISTS is defined, this will be ignored and need not be 
 * defined.  If AQ_TIME_T_EXISTS is not defined, this must be defined. */
#define AQ_MDIO_READS_PER_SECOND 78125


/*! If defined, after writing to one of the registers that can trigger a 
 * processor-intensive MDIO operation, AQ_API functions will poll the 
 * the "processor intensive MDIO operation in progress" bit and wait for it
 * to be zero before proceeding. */
#define AQ_ENABLE_UP_BUSY_CHECKS


/*! If defined, the register map header files containing reverse-packed
 * structs will be included.  If not, the register map header files containing 
 * non-reverse-packed structs will be included.  The proper choice is typically
 * a function of the endianness of the platform; on big-endian systems the 
 * reverse-packed structs should be used, and on little-endian systems the 
 * non-reverse-packed structs should be used. */
/*#define AQ_REVERSED_BITFIELD_ORDERING*/

/*@}*/
#endif

