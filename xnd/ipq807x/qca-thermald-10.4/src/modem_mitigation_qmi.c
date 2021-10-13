/*===========================================================================

  modem_mitigation_qmi.c

  DESCRIPTION
  Modem mitigation action over QMI.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  modem_communication_init() should be called before the modem_request().

  Copyright (c) 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>

#include "thermal.h"

#include "thermal_mitigation_device_service_v01.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_client_instance_defs.h"

#define QMI_MITIGATION_DEVICE_MODEM "pa"
#define MAX_MODEM_MITIGATION_LEVEL  (3)
static void * modem_clnt;
static void * fusion_modem_clnt;
static int modem_req[SENSOR_IDX_MAX];
static int fusion_modem_req[SENSOR_IDX_MAX];
static int prev_modem;
static int prev_fusion;
static pthread_mutex_t modem_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t fusion_modem_mtx = PTHREAD_MUTEX_INITIALIZER;

/*===========================================================================
LOCAL FUNCTION modem_verify_tmd_device

Helper function to verify QMI_MITIGATION_DEVICE_MODEM thermal mitigation
device on remote QMI TMD service.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
static int modem_verify_tmd_device(void *clnt)
{
	int rc = -1;
	int ret = -1;
	unsigned int i;
	tmd_get_mitigation_device_list_resp_msg_v01 data_resp;

	if (clnt == NULL) {
		return -1;
	}

	memset(&data_resp, 0, sizeof(data_resp));
	rc = qmi_client_send_msg_sync(clnt,
				      QMI_TMD_GET_MITIGATION_DEVICE_LIST_REQ_V01,
				      NULL, 0,
				      &data_resp, sizeof(data_resp), 0);
	if (rc == QMI_NO_ERR) {
		for (i = 0; i < data_resp.mitigation_device_list_len; i++) {
			if (0 == strncasecmp(QMI_MITIGATION_DEVICE_MODEM,
					     data_resp.mitigation_device_list[i].mitigation_dev_id.mitigation_dev_id,
					     strlen(QMI_MITIGATION_DEVICE_MODEM) + 1)) {
				/* found matching device name */
				ret = 0;
				break;
			}
		}
	} else {
		msg("%s: QMI send_msg_sync failed with error %d", __func__, rc);
	}

	return (!rc && !ret) ? 0 : -1;
}

static qmi_idl_service_object_type tmd_service_object;
static pthread_t qmi_register_thread;
static pthread_t qmi_register_fusion_thread;

void modem_clnt_error_cb(qmi_client_type clnt,
			 qmi_client_error_type error,
			 void *error_cb_data);

/*===========================================================================
LOCAL FUNCTION modem_request_common_qmi

Common qmi modem request function.

ARGUMENTS
	clnt - client on which to request throttling
	mitigation_dev_id - name of mitigation_dev_id
	level - 0-3 throttling level for modem mitigation

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
static int modem_request_common_qmi(void *clnt, char *mitigation_dev_id,
				    int level)
{
	int ret = -1;
	tmd_set_mitigation_level_req_msg_v01 data_req;
	tmd_set_mitigation_level_resp_msg_v01 data_resp;

	if (!clnt || !mitigation_dev_id)
		return ret;

	strlcpy(data_req.mitigation_dev_id.mitigation_dev_id, mitigation_dev_id,
		QMI_TMD_MITIGATION_DEV_ID_LENGTH_MAX_V01);
	data_req.mitigation_level = level;
	ret = qmi_client_send_msg_sync((qmi_client_type) clnt,
				       QMI_TMD_SET_MITIGATION_LEVEL_REQ_V01,
				       &data_req, sizeof(data_req),
				       &data_resp, sizeof(data_resp), 0);
	return ret;
}

/*===========================================================================
FUNCTION qmi_register

Helper function to initialize qmi connection to modem service.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
static void *qmi_register(void *data)
{
	int rc = -1;
	int ret = 0;
	int level = 0;
	int i;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	qmi_client_type notifier = NULL;
	void * modem_clnt_local = NULL;

	/* release any old handles for modem_clnt */
	if (modem_clnt) {
		qmi_client_release(modem_clnt);
		modem_clnt = NULL;
	}

	rc = qmi_client_notifier_init(tmd_service_object, &os_params, &notifier);
	if (rc != QMI_NO_ERR) {
		msg("qmi: qmi_client_notifier_init failed.\n");
		ret = -1;
		goto handle_error;
	}

	/* TODO: Assuming modem index is 0, depends on QMI implementation */
	while (1)
	{
		QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
		rc = qmi_client_get_service_instance(tmd_service_object, 0, &info);
		if(rc == QMI_NO_ERR)
			break;
		/* wait for server to come up */
		QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
	};

	rc = qmi_client_init(&info, tmd_service_object, NULL, NULL, NULL, (qmi_client_type *) (&modem_clnt_local));
	if (rc != QMI_NO_ERR) {
		msg("Modem thermal mitigation not available.\n");
		ret = -1;
		goto handle_error;
	}
	/* Verify modem mitigation device present on modem */
	rc = modem_verify_tmd_device(modem_clnt_local);
	if (rc != 0) {
		qmi_client_release(modem_clnt_local);
		modem_clnt_local = NULL;
		ret = -1;
		goto handle_error;
	}

	/* best effort register for error */
	qmi_client_register_error_cb(modem_clnt_local, modem_clnt_error_cb, NULL);
	modem_clnt = modem_clnt_local;
	info("Modem thermal mitigation available.\n");

	/*Setting highest pending mitigation level if any */
	pthread_mutex_lock(&modem_mtx);
	/* Aggregate modem mitigation level for all sensors */
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if (modem_req[i] > level)
			level = modem_req[i];
	}

	if (level) {
		ret = modem_request_common_qmi(modem_clnt,
					       QMI_MITIGATION_DEVICE_MODEM,
					       level);
		if (ret) {
			msg("Modem mitigation failed with %d for level %d\n",
			    ret, level);
		} else {
			prev_modem = level;
			info("ACTION: MODEM - "
			    "Pending request:Modem mitigation succeeded for "
			    "level %d.\n", level);
		}
	} else {
		dbgmsg("No pending request for Modem mitigation\n");
	}

	pthread_mutex_unlock(&modem_mtx);

handle_error:
	if (notifier != NULL) {
		qmi_client_release(notifier);
	}

	return NULL;
}

/*===========================================================================
FUNCTION qmi_register_fusion

Helper function to initialize qmi connection to fusion service.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
static void *qmi_register_fusion(void *data)
{
	int rc = -1;
	int ret = 0;
	int level = 0;
	int i;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	qmi_client_type notifier = NULL;
	void * fusion_clnt_local = NULL;
	qmi_service_instance instance_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0;

	/* release any old handles for fusion_modem_clnt */
	if (fusion_modem_clnt) {
		qmi_client_release(fusion_modem_clnt);
		fusion_modem_clnt = NULL;
	}

	rc = qmi_client_notifier_init(tmd_service_object, &os_params, &notifier);
	if (rc != QMI_NO_ERR) {
		msg("qmi: qmi_client_notifier_init failed.\n");
		ret = -1;
		goto handle_error_fusion;
	}

	info("qmi: Instance id %d for fusion TMD", instance_id);
	while (1)
	{
		QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
		rc = qmi_client_get_service_instance(tmd_service_object,
						     instance_id, &info);
		if(rc == QMI_NO_ERR)
			break;
		/* wait for server to come up */
		QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
	};

	rc = qmi_client_init(&info, tmd_service_object, NULL, NULL, NULL, (qmi_client_type *) (&fusion_clnt_local));
	if (rc != QMI_NO_ERR) {
		msg("Fusion modem thermal mitigation not available.\n");
		ret = -1;
		goto handle_error_fusion;
	}
	rc = modem_verify_tmd_device(fusion_clnt_local);
	if (rc != 0) {
		qmi_client_release(fusion_clnt_local);
		fusion_clnt_local = NULL;
		ret = -1;
		goto handle_error_fusion;
	}

	/* best effort register for error */
	qmi_client_register_error_cb(fusion_clnt_local, modem_clnt_error_cb, NULL);
	fusion_modem_clnt = fusion_clnt_local;
	info("Fusion modem thermal mitigation available.\n");

	/*Setting highest pending mitigation level if any */
	pthread_mutex_lock(&fusion_modem_mtx);

	/* Aggregate modem mitigation level for all sensors */
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if (fusion_modem_req[i] > level)
			level = fusion_modem_req[i];
	}

	if (level) {
		ret = modem_request_common_qmi(fusion_modem_clnt,
					       QMI_MITIGATION_DEVICE_MODEM,
					       level);
		if (ret) {
			msg("Fusion modem mitigation failed with %d for "
			    "level %d\n", ret, level);
		} else {
			prev_fusion = level;
			info("ACTION: FUSION - "
			    "Pending request:Fusion modem mitigation succeeded "
			    "for level %d.\n", level);
		}
	} else {
		dbgmsg("No pending request for Fusion modem mitigation\n");
	}

	pthread_mutex_unlock(&fusion_modem_mtx);

handle_error_fusion:
	if (notifier != NULL) {
		qmi_client_release(notifier);
	}

	return NULL;
}

/*===========================================================================
FUNCTION modem_clnt_error_cb

Callback function called by the QCCI infrastructure when it receives a
REMOVE SERVER message from the modem.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
void modem_clnt_error_cb(qmi_client_type clnt,
			 qmi_client_error_type error,
			 void *error_cb_data)
{
	info("%s: with %d called for clnt %p\n", __func__, error, (void *)clnt);
	if (clnt == NULL) {
		return;
	}
	if (clnt == modem_clnt) {
		pthread_join(qmi_register_thread, NULL);
		pthread_create(&qmi_register_thread, NULL,
			       qmi_register, NULL);
	} else if (clnt == fusion_modem_clnt) {
		pthread_join(qmi_register_fusion_thread, NULL);
		pthread_create(&qmi_register_fusion_thread, NULL,
			       qmi_register_fusion, NULL);
	}
}

/*===========================================================================
FUNCTION modem_communication_init

Helper function to initialize qmi communication to modem.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int modem_communication_init(void)
{
	/* Get the service object for the tmd API */
	tmd_service_object = tmd_get_service_object_v01();
	if (!tmd_service_object) {
		msg("qmi: tmd_get_service_object failed.\n");
		return -1;
	}

	/* start thread to register with QMI services */
	pthread_create(&qmi_register_thread, NULL,
		       qmi_register, NULL);

	/* start thread to register with QMI services */
	pthread_create(&qmi_register_fusion_thread, NULL,
		       qmi_register_fusion, NULL);

	return 0;
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

	if (!modem_clnt) {
		info("Modem: Requested level is recorded and waiting for"
		    "completing QMI registration");
		ret = 0;
		goto handle_return;
	}

	if (level != prev_modem) {
		ret = modem_request_common_qmi(modem_clnt,
					       QMI_MITIGATION_DEVICE_MODEM,
					       level);
		if (ret) {
			msg("Modem mitigation failed with %d for level %d\n",
			    ret, level);
		} else {
			prev_modem = level;
			info("ACTION: MODEM - "
			    "Modem mitigation succeeded for level %d.\n",
			    level);
		}
	} else {
		dbgmsg("Modem already at mitigation state %d\n", level);
		ret = 0;
	}

handle_return:
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

	if (!fusion_modem_clnt) {
		info("Fusion: Requested level is recorded and waiting for"
		    "completing QMI registration");
		ret = 0;
		goto handle_return;
	}

	if (level != prev_fusion) {
		ret = modem_request_common_qmi(fusion_modem_clnt,
					       QMI_MITIGATION_DEVICE_MODEM,
					       level);
		if (ret) {
			msg("Fusion modem mitigation failed with %d for "
			    "level %d\n", ret, level);
		} else {
			prev_fusion = level;
			info("ACTION: FUSION - "
			    "Fusion modem mitigation succeeded for "
			    "level %d.\n", level);
		}
	} else {
		dbgmsg("Fusion modem already at mitigation state %d\n", level);
		ret = 0;
	}

handle_return:
	pthread_mutex_unlock(&fusion_modem_mtx);

	return ret;
}

/*===========================================================================
FUNCTION modem_communication_release

Release function for modem communication to clean up resources.
Called after use of client handles is complete.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int modem_communication_release(void)
{
	int rc = 0;

	pthread_join(qmi_register_thread, NULL);
	pthread_join(qmi_register_fusion_thread, NULL);

	if (modem_clnt) {
		rc = qmi_client_release(modem_clnt);
		if (rc)
			msg("qmi: qmi_client_release modem clnt failed.\n");
		modem_clnt = 0;
	}
	if (fusion_modem_clnt) {
		rc = qmi_client_release(fusion_modem_clnt);
		if (rc)
			msg("qmi: qmi_client_release fusion clnt failed.\n");
		fusion_modem_clnt = 0;
	}

	return rc;
}
