/*
 This file is provided under a dual BSD/GPLv2 license.  When using or
 redistributing this file, you may do so under either license.

 GPL LICENSE SUMMARY

 Copyright(c) 2016 - 2017 Intel Corporation.

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 Contact Information:
  Intel Corporation
  2200 Mission College Blvd.
  Santa Clara, CA  97052

 BSD LICENSE

 Copyright(c) 2016 - 2017 Intel Corporation.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.
  * Neither the name of Intel Corporation nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef _GSW_TYPES_H_
#define _GSW_TYPES_H_
/** \file gsw_types.h GSW Base Types */

/** \brief MAC Address Field Size.
    Number of bytes used to store MAC address information. */
#define GSW_MAC_ADDR_LEN 6
/** \brief Instantiated tables entries name  string length.
    The user can supply a name and get in return an id from Switch API. */
#define GSW_NAME_LEN	32
/** \brief This is the unsigned 64-bit datatype. */
typedef unsigned long long    u64;
/** \brief This is the unsigned 32-bit datatype. */
typedef unsigned int    u32;
/** \brief This is the unsigned 8-bit datatype. */
typedef unsigned char   u8;
/** \brief This is the unsigned 16-bit datatype. */
typedef unsigned short  u16;
/** \brief This is the signed 16-bit datatype. */
typedef short  i16;
/** \brief This is the signed 8-bit datatype. */
typedef char  i8;
/** \brief This is the signed 32-bit datatype. */
typedef long  i32;
/** \brief This is the unsigned 32-bit datatype for register access. */
typedef unsigned int ur;
/** \brief This enumeration type defines two boolean states: False and True. */
typedef enum {
	/** Boolean False. */
	LTQ_FALSE		= 0,
	/** Boolean True. */
	LTQ_TRUE		= 1
} ltq_bool_t;

/** \brief This is a union to describe the IPv4
	and IPv6 Address in numeric representation.
	Used by multiple Structures and APIs. The member selection
	would be based upon \ref GSW_IP_Select_t */
typedef union {
	/** Describe the IPv4 address.
	    Only used if the IPv4 address should be read or configured.
	    Cannot be used together with the IPv6 address fields. */
	u32	nIPv4;
	/** Describe the IPv6 address.
	    Only used if the IPv6 address should be read or configured.
	    Cannot be used together with the IPv4 address fields. */
	u16	nIPv6[8];
} GSW_IP_t;

/** \brief Selection to use IPv4 or IPv6.
    Used  along with \ref GSW_IP_t to denote which union member to be accessed.
*/
typedef enum {
	/** IPv4 Type */
	GSW_IP_SELECT_IPV4	= 0,
	/** IPv6 Type */
	GSW_IP_SELECT_IPV6	= 1
} GSW_IP_Select_t;
#endif
