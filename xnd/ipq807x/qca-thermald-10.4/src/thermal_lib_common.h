/*===========================================================================

 Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#ifndef __THERMAL_LIB_COMMON_H__
#define __THERMAL_LIB_COMMON_H__

#define THERMAL_SEND_CLIENT_SOCKET   "/dev/socket/thermal-send-client"
#define THERMAL_RECV_CLIENT_SOCKET   "/dev/socket/thermal-recv-client"
#define NUM_LISTEN_QUEUE   (20)
#define LEVEL_MAX          (3)
#define CLIENT_HANDLE_MAX  (32)
#define CLIENT_NAME_MAX    (12)

/* Thermal client data */
struct thermal_cdata {
	int client_cb_handle;
        char *client_name;
        void *callback;
        void *user_data;
	void *data_reserved;
        struct thermal_cdata *next;
};

/* Thermal socket message data type */
struct thermal_msg_data {
	int msg_type;
	char client_name[CLIENT_NAME_MAX];
	int req_data;
};

int add_to_list(char *name,  void *callback, void *data);
struct thermal_cdata *get_callback_node_from_list(struct thermal_cdata *list, char *name);
int remove_from_list(int handle);

#endif  /* __THERMAL_LIB_COMMON_H__ */
