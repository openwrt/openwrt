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

#ifndef _LTQ_GSW_KERNEL_API_H_
#define _LTQ_GSW_KERNEL_API_H_

#include "f2x_swapi.h"
#include "f2x_swapi_flow.h"

/* Group definitions for Doxygen */
/** \defgroup ETHSW_KERNELAPI Ethernet Switch Linux Kernel Interface
    This chapter describes the entire interface to access and
    configure the various services of the Ethernet switch. */

/*@{*/

/** Definition of the device handle that is retrieved during
    the \ref gsw_api_kopen call. This handle is used to access the switch
    device while calling \ref gsw_api_kioctl. */
typedef unsigned int GSW_API_HANDLE;

/**
   Request a device handle for a dedicated Ethernet switch device. The switch
   device is identified by the given device name (e.g. "/dev/switch/1").
   The device handle is the return value of this function. This handle is
   used to access the switch parameter and features while
   calling \ref gsw_api_kioctl. Please call the function
   \ref gsw_api_kclose to release a device handle that is not needed anymore.

	\param name Pointer to the device name of the
			requested Ethernet switch device.

   \remarks The client kernel module should check the function return value.
   A returned zero indicates that the resource allocation failed.

   \return Return the device handle in case the requested device is available.
   It returns a zero in case the device does not exist or is blocked
   by another application.
*/
GSW_API_HANDLE gsw_api_kopen(char *name);
/**
   Calls the switch API driver implementation with the given command and the
   parameter argument. The called Ethernet switch device is identified by the
   given device handle. This handle was previously requested by
   calling gsw_api_kopen function.

   \param handle Ethernet switch device handle, given by \ref gsw_api_kopen.
   \param command Switch API command to perform.
   \param arg Command arguments. This argument is basically a reference to
   the command parameter structure.

   \remarks The commands and arguments are the same as normally used over
   the Linux ioctl interface from user space.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurred.
*/
GSW_return_t gsw_api_kioctl(GSW_API_HANDLE handle,
                            unsigned int cmd, unsigned int arg);
/**
   Releases an Ethernet switch device handle which was previously
   allocated by \ref gsw_api_kopen.

   \param handle Ethernet switch device handle, given by \ref gsw_api_kopen.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurred.
*/
GSW_return_t gsw_api_kclose(GSW_API_HANDLE handle);
/*@}*/ /* GSWIP_ROUTE_IOCTL */

#endif /* _LTQ_GSW_KERNEL_API_H_ */

