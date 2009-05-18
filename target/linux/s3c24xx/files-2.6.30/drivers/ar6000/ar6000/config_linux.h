/*
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  Software distributed under the License is distributed on an "AS
 *  IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  rights and limitations under the License.
 *
 *
 *
 */

#ifndef _CONFIG_LINUX_H_
#define _CONFIG_LINUX_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Host-side GPIO support is optional.
 * If run-time access to GPIO pins is not required, then
 * this should be changed to #undef.
 */
#define CONFIG_HOST_GPIO_SUPPORT

/*
 * Host side Test Command support
 */
#define CONFIG_HOST_TCMD_SUPPORT

#define USE_4BYTE_REGISTER_ACCESS

#ifdef __cplusplus
}
#endif

#endif
