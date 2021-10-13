/*===========================================================================

 Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#ifndef __THERMAL_CLIENT_H__
#define __THERMAL_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

int thermal_client_register_callback(char *client_name, int (*callback)(int , void *, void *), void *data);
int thermal_client_request(char *client_name, int req_data);
void thermal_client_unregister_callback(int client_cb_handle);

#ifdef __cplusplus
}
#endif

#endif /* __THERMAL_CLIENT_H__ */
