/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
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
 * $Id: //depot/sw/releases/olca2.0-GPL/host/include/dset_api.h#1 $
 *
 * Host-side DataSet API.
 *
 */

#ifndef _DSET_API_H_
#define _DSET_API_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Host-side DataSet support is optional, and is not
 * currently required for correct operation.  To disable
 * Host-side DataSet support, set this to 0.
 */
#ifndef CONFIG_HOST_DSET_SUPPORT
#define CONFIG_HOST_DSET_SUPPORT 1
#endif

/* Called to send a DataSet Open Reply back to the Target. */
A_STATUS wmi_dset_open_reply(struct wmi_t *wmip,
                             A_UINT32 status,
                             A_UINT32 access_cookie,
                             A_UINT32 size,
                             A_UINT32 version,
                             A_UINT32 targ_handle,
                             A_UINT32 targ_reply_fn,
                             A_UINT32 targ_reply_arg);

/* Called to send a DataSet Data Reply back to the Target. */
A_STATUS wmi_dset_data_reply(struct wmi_t *wmip,
                             A_UINT32 status,
                             A_UINT8 *host_buf,
                             A_UINT32 length,
                             A_UINT32 targ_buf,
                             A_UINT32 targ_reply_fn,
                             A_UINT32 targ_reply_arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _DSET_API_H_ */
