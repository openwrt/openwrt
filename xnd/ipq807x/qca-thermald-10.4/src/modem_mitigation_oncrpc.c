/*===========================================================================

  modem_mitigation_oncrpc.c

  DESCRIPTION
  Modem mitigation action over ONCRPC.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  modem_communication_init() should be called before the modem_request().

  Copyright (c) 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <errno.h>
#include "thermal.h"

#include "oncrpc.h"
#include "thermal_mitigation.h"
#include "thermal_mitigation_rpc.h"

#ifdef ENABLE_FUSION_MODEM
#include "thermal_mitigation_fusion.h"
#include "thermal_mitigation_fusion_rpc.h"
#endif

/* Modem mitigative action */

#define MITIGATION_DEVICE "modem"
#define MAX_MODEM_MITIGATION_LEVEL  (3)
#define MODEM_DEVICE	0
#define FUSION_DEVICE	1

static int modem_exists;
static int fusion_modem_exists;
static unsigned int modem_oncrpc_handle;
static unsigned int fusion_oncrpc_handle;
static int modem_req[SENSOR_IDX_MAX];
static int fusion_modem_req[SENSOR_IDX_MAX];
static pthread_mutex_t modem_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t fusion_modem_mtx = PTHREAD_MUTEX_INITIALIZER;
static int modem_register(void);
static int fusion_modem_register(void);

/*===========================================================================
FUNCTION modem_thermal_state_cb

Callback function when modem does its own thermal mitigation.
Notifies the interested through the socket.

ARGUMENTS
	None.

RETURN VALUE
	None.
===========================================================================*/
static int modem_thermal_state_cb(const char *device, unsigned int state)
{
	char tempBuf[60];

	dbgmsg("Callback from modem (%s) with state %u", device, state);

	snprintf(tempBuf, sizeof(tempBuf) -1, "%s\n%u", device, state);
	write_to_local_socket(UI_LOCALSOCKET_NAME, tempBuf, strlen(tempBuf));

	return 0;
}

/*===========================================================================
FUNCTION fusion_thermal_state_cb

Callback function when fusion modem does its own thermal mitigation.
Notifies the interested through the socket.

ARGUMENTS
	None.

RETURN VALUE
	None.
===========================================================================*/
static int fusion_thermal_state_cb(const char *device, unsigned int state)
{
	char tempBuf[60];

	dbgmsg("Callback from fusion (%s) with state %u", device, state);

	snprintf(tempBuf, sizeof(tempBuf) -1, "%s\n%u", device, state);
	write_to_local_socket(UI_LOCALSOCKET_NAME, tempBuf, strlen(tempBuf));

	return 0;
}

/*===========================================================================
FUNCTION thermald_oncrpc_cleanup_cb

Callback function for ONCRPC clean up.

ARGUMENTS
	handle : Passed from ONCRPC
	data: Userdata passed during callback register.

RETURN VALUE
	None.
===========================================================================*/
static void thermald_oncrpc_cleanup_cb(void *handle, void *data)
{
	int type = (int)data;

	switch (type) {
	case MODEM_DEVICE:
		info("Modem ONCRPC communication terminated.\n");
		modem_exists = 0;
		break;
#ifdef ENABLE_FUSION_MODEM
	case FUSION_DEVICE:
		info("Fusion modem ONCRPC communication terminated.\n");
		fusion_modem_exists = 0;
		break;
#endif
	default:
		msg("Unknown device in %s\n", __func__);
		break;
	}

	oncrpc_cleanup_done(handle);
	dbgmsg("ONCRPC clean up done for %d.\n", type);
}

/*===========================================================================
FUNCTION thermald_oncrpc_restart_cb

Callback for modem/fusion modem restart.

ARGUMENTS
	handle : Passed from ONCRPC
	data: Userdata passed during callback register.

RETURN VALUE
	None.
===========================================================================*/
static void thermald_oncrpc_restart_cb(void *handle, void *data)
{
	int type = (int)data;

	switch (type) {
	case MODEM_DEVICE:
		info("Modem restart detected.\n");
		modem_exists = modem_register();
		break;
#ifdef ENABLE_FUSION_MODEM
	case FUSION_DEVICE:
		info("Fusion modem restart detected.\n");
		fusion_modem_exists = fusion_modem_register();
		break;
#endif
	default:
		msg("Unknown device in %s\n", __func__);
		break;
	}
	dbgmsg("ONCRPC restart re-registered for %d.\n", type);
}

/*===========================================================================
FUNCTION modem_register

Register modem callbacks and server cleanup/restart callbacks

ARGUMENTS
	None.

RETURN VALUE
	Returns if the server is available.
	1 if server is available, 0 if not.
===========================================================================*/
static int modem_register(void)
{
	int ret = 0;

	pthread_mutex_lock(&modem_mtx);
	ret = thermal_mitigation_null();
	if (ret) {
		info("Modem thermal mitigation available.\n");
		ret = thermal_mitigation_register_notify_cb(MITIGATION_DEVICE,
				&modem_oncrpc_handle, modem_thermal_state_cb);
		if (ret) {
			msg("Modem callback not registered\n");
		}
		oncrpc_register_server_exit_notification_cb(
				THERMAL_MITIGATIONPROG,
				THERMAL_MITIGATIONVERS,
				thermald_oncrpc_cleanup_cb, (void *)MODEM_DEVICE);
		oncrpc_register_server_restart_notification_cb(
				THERMAL_MITIGATIONPROG,
				THERMAL_MITIGATIONVERS,
				thermald_oncrpc_restart_cb, (void *)MODEM_DEVICE);
		/* We are still ok to continue as only the callback failed */
		dbgmsg("Modem thermal mitigation callbacks registered.\n");
		ret = 1;
	} else {
		msg("Modem thermal mitigation not available.\n");
		ret = 0;
	}
	pthread_mutex_unlock(&modem_mtx);

	return ret;
}

/*===========================================================================
FUNCTION fusion_modem_register

Register fusion modem callbacks and server cleanup/restart callbacks

ARGUMENTS
	None.

RETURN VALUE
	Returns if the server is available.
	1 if server is available, 0 if not.
===========================================================================*/
static int fusion_modem_register(void)
{
	int ret = 0;

#ifdef ENABLE_FUSION_MODEM
	pthread_mutex_lock(&fusion_modem_mtx);
	ret = thermal_mitigation_fusion_null();
	if (ret) {
		info("Fusion modem thermal mitigation available.\n");
		ret = thermal_mitigation_register_notify_cb_fusion(MITIGATION_DEVICE,
				&fusion_oncrpc_handle, fusion_thermal_state_cb);
		if (ret) {
			msg("Fusion modem callback not registered\n");
		}
		oncrpc_register_server_exit_notification_cb(
				THERMAL_MITIGATION_FUSIONPROG,
				THERMAL_MITIGATION_FUSIONVERS,
				thermald_oncrpc_cleanup_cb, (void *)FUSION_DEVICE);
		oncrpc_register_server_restart_notification_cb(
				THERMAL_MITIGATION_FUSIONPROG,
				THERMAL_MITIGATION_FUSIONVERS,
				thermald_oncrpc_restart_cb, (void *)FUSION_DEVICE);
		/* We are still ok to continue as only the callback failed */
		dbgmsg("Fusion modem thermal mitigation callbacks registered.\n");
		ret = 1;
	} else {
		msg("Fusion modem thermal mitigation not available.\n");
		ret = 0;
	}
	pthread_mutex_unlock(&fusion_modem_mtx);
#endif
	return ret;
}

/*===========================================================================
FUNCTION modem_request

Action function to throttle modem functionality.

ARGUMENTS
	requester - requesting sensor enum
	temperature - temperature reached
	level - 0-3 throttling level for modem mitigation

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int modem_request(int requester, int temperature, int level)
{
	int ret = -1;
	int i;
	static int prev;

	if (!modem_exists)
		return ret;

	/* Modem level: 0 - No action
	 * 		1 - Mitigation level 1
	 * 		2 - Mitigation level 2
	 * 		3 - Emergency
	 */

	if (level < 0)
		level = 0;

	if (level > MAX_MODEM_MITIGATION_LEVEL)
		level = MAX_MODEM_MITIGATION_LEVEL;

	pthread_mutex_lock(&modem_mtx);

	/* Aggregate modem mitigation level for all sensors */
	modem_req[requester] = level;
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if (modem_req[i] > level)
			level = modem_req[i];
	}

	if (level != prev) {
		ret = thermal_mitigation_set(MITIGATION_DEVICE, level);
		if (ret) {
			msg("Modem mitigation failed with %d for level %d\n", ret, level);
		} else {
			prev = level;
			info("ACTION: MODEM - "
			    "Modem mitigation succeeded for level %d.\n", level);
		}
	} else {
		dbgmsg("Modem already at mitigation state %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&modem_mtx);

	return ret;
}

/*===========================================================================
FUNCTION fusion_modem_request

Action function to throttle fusion modem functionality.

ARGUMENTS
	requester - requesting sensor enum
	temperature - temperature reached
	level - 0-3 throttling level for modem mitigation

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int fusion_modem_request(int requester, int temperature, int level)
{
	int ret = -1;
	int i;
	static int prev;

	if (!fusion_modem_exists)
		return ret;

	/* Modem level: 0 - No action
	 * 		1 - Mitigation level 1
	 * 		2 - Mitigation level 2
	 * 		3 - Emergency
	 */

	if (level < 0)
		level = 0;

	if (level > MAX_MODEM_MITIGATION_LEVEL)
		level = MAX_MODEM_MITIGATION_LEVEL;

	pthread_mutex_lock(&fusion_modem_mtx);

	/* Aggregate modem mitigation level for all sensors */
	fusion_modem_req[requester] = level;
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if (fusion_modem_req[i] > level)
			level = fusion_modem_req[i];
	}

	if (level != prev) {
#ifdef ENABLE_FUSION_MODEM
		ret = thermal_mitigation_set_fusion(MITIGATION_DEVICE, level);
#endif
		if (ret) {
			msg("Fusion modem mitigation failed with %d for level %d\n", ret, level);
		} else {
			prev = level;
			info("ACTION: FUSION - "
			    "Fusion modem mitigation succeeded for level %d.\n", level);
		}
	} else {
		dbgmsg("Fusion modem already at mitigation state %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&fusion_modem_mtx);

	return ret;
}

/*===========================================================================
FUNCTION modem_communication_init

Initialization function for modem communication functionality.
Needs to be called before modem_request() or fusion_modem_request()
can be called.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int modem_communication_init(void)
{
	oncrpc_init();
	oncrpc_task_start();
	info("Establishing ONCRPC communication.\n");

	thermal_mitigationcb_app_init();
	modem_exists = modem_register();

#ifdef ENABLE_FUSION_MODEM
	thermal_mitigation_fusioncb_app_init();
	fusion_modem_exists = fusion_modem_register();
#endif

	return 0;
}

/*===========================================================================
FUNCTION compare_fn

Comparator function for oncrpc

ARGUMENTS
	d1, d2: void pointers for comparison

RETURN VALUE
	1 if they are equal, 0 if they are not.
===========================================================================*/
static boolean compare_fn(void *d1, void *d2)
{
	return (d1 == d2);
}

/*===========================================================================
FUNCTION modem_communication_release

Release function for modem communication to clean up resources.
Called after use of client handle is complete.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int modem_communication_release(void)
{
	int ret = 0;
	int rc = 0;

	if (modem_exists) {
		ret = thermal_mitigation_deregister_notify_cb(modem_oncrpc_handle);
		if (ret) {
			msg("Unable to de-register modem cb handle (%d).\n", ret);
		}
		oncrpc_unregister_server_exit_notification_cb(
				THERMAL_MITIGATIONPROG,
				THERMAL_MITIGATIONVERS,
				thermald_oncrpc_cleanup_cb, (void *)MODEM_DEVICE, compare_fn);
		oncrpc_unregister_server_restart_notification_cb(
				THERMAL_MITIGATIONPROG,
				THERMAL_MITIGATIONVERS,
				thermald_oncrpc_restart_cb, (void *)MODEM_DEVICE, compare_fn);
	}

#ifdef ENABLE_FUSION_MODEM
	if (fusion_modem_exists) {
		rc = thermal_mitigation_deregister_notify_cb_fusion(fusion_oncrpc_handle);
		if (rc) {
			msg("Unable to de-register fusion cb handle (%d).\n", rc);
		}
		oncrpc_unregister_server_exit_notification_cb(
				THERMAL_MITIGATION_FUSIONPROG,
				THERMAL_MITIGATION_FUSIONVERS,
				thermald_oncrpc_cleanup_cb, (void *)FUSION_DEVICE, compare_fn);
		oncrpc_unregister_server_restart_notification_cb(
				THERMAL_MITIGATION_FUSIONPROG,
				THERMAL_MITIGATION_FUSIONVERS,
				thermald_oncrpc_restart_cb, (void *)FUSION_DEVICE, compare_fn);
	}
#endif
	oncrpc_task_stop();
	oncrpc_deinit();
	info("ONCRPC communication terminated.\n");

	return (ret || rc) ? -1 : 0;
}
