/*===========================================================================

  modem_sensor_qmi.c

  DESCRIPTION
  Modem sensor action over QMI.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  modem_ts_qmi_init() should be called before the modem_ts_temp_request().

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "thermal.h"
#include "qmi-ts-sensor.h"

#include "thermal_sensor_service_v01.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_client_instance_defs.h"

#define QMI_SENSOR_MODEM "pa"
static void *fusion_modem_clnt;

static pthread_mutex_t fusion_modem_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  fusion_modem_cond = PTHREAD_COND_INITIALIZER;
static pthread_t fusion_qmi_register_thread;
static int fusion_modem_qmi_ts_ready;

static ts_temp_report_ind_msg_v01  ind_struct;

static qmi_idl_service_object_type ts_service_object;

static void modem_clnt_error_cb(qmi_client_type clnt,
				 qmi_client_error_type error,
				 void *error_cb_data);

static int modem_ts_reg_notify(void *clnt, const char *sensor_id,
				int send_current_temp_report,
				int high_valid,
				int high_thresh,
				int low_valid,
				int low_thresh);

/*===========================================================================
LOCAL FUNCTION qmi_ts_ind_cb

Handle QMI TS indication callbacks.

ARGUMENTS
	user_handle - QMI client user handle.
	msg_id - Indication message id.
	ind_buf - Indication encoded data.
	ind_buf_len - Indication encoded data length.
	ind_cb_data - TS provided callback data.

RETURN VALUE
	None.
===========================================================================*/
static void qmi_ts_ind_cb(qmi_client_type user_handle,
			   unsigned int     msg_id,
			   void            *ind_buf,
			   unsigned int     ind_buf_len,
			   void            *ind_cb_data)
{
	qmi_client_error_type rc = QMI_NO_ERR;
	unsigned char *buf = ind_buf;

	if (msg_id == QMI_TS_TEMP_REPORT_IND_V01) {
		memset((void *)&ind_struct, 0,
		       sizeof(ts_temp_report_ind_msg_v01));
		rc = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
					       msg_id, ind_buf, ind_buf_len,
					       &ind_struct,
					       sizeof(ts_temp_report_ind_msg_v01));

		if (rc != QMI_NO_ERR) {
			msg("Error invalid indication message received.\n");
		} else {
			if (ind_struct.temp_valid) {
				qmi_ts_update_temperature(ind_struct.sensor_id.sensor_id,
							  (int)ind_struct.temp);
			} else {
				msg("Error invalid temperature field.");
			}
		}
	} else {
		printf("\nWarning invalid indication message received.\n");
	}
}

/*===========================================================================
LOCAL FUNCTION modem_verify_ts_device

Helper function to verify QMI_SENSOR_MODEM thermal sensor exits on remote QMI
TS service.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -(ERRNO) Code on failure.
===========================================================================*/
static int modem_verify_ts_device(void *clnt)
{
	int rc;
	int ret = -(EPERM);
	unsigned int i;
	static int func_exec_count;

	ts_get_sensor_list_resp_msg_v01 *list_resp = NULL;
	ts_register_notification_temp_req_msg_v01 *reg_notify_req = NULL;
	ts_register_notification_temp_resp_msg_v01 *reg_notify_resp = NULL;
	/* Large structs let's not put it on the stack. */

	do {
		if (clnt == NULL) {
			ret = -(EINVAL);
			break;
		}

		list_resp = malloc(sizeof(ts_get_sensor_list_resp_msg_v01));
		if (list_resp == NULL) {
			msg("%s: Malloc list_resp failure", __func__);
			ret = -(ENOMEM);
			break;
		}

		reg_notify_req = malloc(sizeof(ts_register_notification_temp_req_msg_v01));
		if (reg_notify_req == NULL) {
			msg("%s: Malloc reg_notify_req failure", __func__);
			ret = -(ENOMEM);
			break;
		}

		reg_notify_resp = malloc(sizeof(ts_register_notification_temp_resp_msg_v01));
		if (reg_notify_resp == NULL) {
			msg("%s: Malloc reg_notify_resp failure", __func__);
			ret = -(ENOMEM);
			break;
		}

		memset(list_resp, 0, sizeof(ts_get_sensor_list_resp_msg_v01));
		rc = qmi_client_send_msg_sync(clnt,
					      QMI_TS_GET_SENSOR_LIST_REQ_V01,
					      NULL, 0,
					      list_resp, sizeof(ts_get_sensor_list_resp_msg_v01), 0);
		if (rc == QMI_NO_ERR) {
			for (i = 0; i < list_resp->sensor_list_len; i++) {
				if (0 == strncasecmp(QMI_SENSOR_MODEM,
						     list_resp->sensor_list[i].sensor_id,
						     QMI_TS_SENSOR_ID_LENGTH_MAX_V01)) {
					/* found matching device name */
					ret = 0;
				}

				if (func_exec_count) {
					/* Chances are the service reset, so send out
					   current temp notify request to get clients to configure
					   threshold triggers again.*/
					modem_ts_reg_notify(clnt, list_resp->sensor_list[i].sensor_id,
							    1, 0, 0, 0, 0);
				}
			}
		} else {
			msg("%s: QMI send_msg_sync failed with error %d", __func__, rc);
			ret = -(EFAULT);
		}
	} while (0);

	if (list_resp != NULL)
		free(list_resp);

	if (reg_notify_req != NULL)
		free(reg_notify_req);

	if (reg_notify_resp != NULL)
		free(reg_notify_resp);

	func_exec_count++;
	return ret;
}

/*===========================================================================
LOCAL FUNCTION fusion_qmi_register

Helper function to initialize QMI connection to modem service.

ARGUMENTS
	None.

RETURN VALUE
	NULL on exit
===========================================================================*/
static void *fusion_qmi_register(void *data)
{
	int rc;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	qmi_client_type notifier = NULL;
	void *modem_clnt_local = NULL;
	qmi_service_instance instance_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0;

	/* release any old handles for fusion_modem_clnt */
	if (fusion_modem_clnt) {
		qmi_client_release(fusion_modem_clnt);
		fusion_modem_clnt = NULL;
	}

	do {
		rc = qmi_client_notifier_init(ts_service_object, &os_params, &notifier);
		if (rc != QMI_NO_ERR) {
			msg("qmi: qmi_client_notifier_init failed.\n");
			break;
		}

		info("qmi: Instance id %d for fusion TS", instance_id);
		while (1) {
			QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
			rc = qmi_client_get_service_instance(ts_service_object,
							     instance_id,
							     &info);
			if (rc == QMI_NO_ERR)
				break;
			/* wait for server to come up */
			QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
		};

		rc = qmi_client_init(&info, ts_service_object, qmi_ts_ind_cb, NULL, NULL,
				     (qmi_client_type *) (&modem_clnt_local));
		if (rc != QMI_NO_ERR) {
			msg("Modem thermal sensor service not available.\n");
			break;
		}
		/* Verify modem sensor service present on modem */
		rc = modem_verify_ts_device(modem_clnt_local);
		if (rc != 0) {
			qmi_client_release(modem_clnt_local);
			modem_clnt_local = NULL;
			break;
		}

		/* best effort register for error */
		qmi_client_register_error_cb(modem_clnt_local, modem_clnt_error_cb, NULL);
		fusion_modem_clnt = modem_clnt_local;

		/* notify waiting threads */
		pthread_mutex_lock(&fusion_modem_mtx);
		fusion_modem_qmi_ts_ready = 1;
		pthread_cond_broadcast(&fusion_modem_cond);
		pthread_mutex_unlock(&fusion_modem_mtx);
		info("Modem thermal sensor service available.\n");
	} while (0);

	if (notifier != NULL)
		qmi_client_release(notifier);

	return NULL;
}

/*===========================================================================
LOCAL FUNCTION modem_clnt_error_cb

Callback function called by the QCCI infrastructure when it receives a
REMOVE SERVER message from the modem.

ARGUMENTS
	None.

RETURN VALUE
	None.
===========================================================================*/
static void modem_clnt_error_cb(qmi_client_type clnt,
				 qmi_client_error_type error,
				 void *error_cb_data)
{
	info("%s: with %d called for clnt %p\n", __func__, error, (void *)clnt);
	if (clnt == NULL)
		return;

	if (clnt == fusion_modem_clnt) {
		pthread_mutex_lock(&fusion_modem_mtx);
		fusion_modem_qmi_ts_ready = 0;
		pthread_mutex_unlock(&fusion_modem_mtx);
		pthread_join(fusion_qmi_register_thread, NULL);
		pthread_create(&fusion_qmi_register_thread, NULL,
			       fusion_qmi_register, NULL);
	}
}

/*===========================================================================
LOCAL FUNCTION modem_ts_reg_notify

Common TS qmi modem register notify function.

ARGUMENTS
	clnt - client on which to request throttling
	sensor_id - name of sensor_id
	send_current_temp_report - 1 for trigger and immediate sensor reading,
				   0 for set threshold.
	high_thresh
	low_thresh

RETURN VALUE
	0 on success, -(ERRNO) on failure.
===========================================================================*/
static int modem_ts_reg_notify(void *clnt, const char *sensor_id,
				int send_current_temp_report,
				int high_valid,
				int high_thresh,
				int low_valid,
				int low_thresh)
{
	int ret = -(EPERM);
	qmi_client_error_type qmi_error = QMI_NO_ERR;

	ts_register_notification_temp_req_msg_v01  data_req;
	ts_register_notification_temp_resp_msg_v01 data_resp;

	if (!clnt || !sensor_id)
		return -(EINVAL);

	memset(&data_req, 0x0, sizeof(data_req));
	strlcpy(data_req.sensor_id.sensor_id, sensor_id,
		QMI_TS_SENSOR_ID_LENGTH_MAX_V01);

	if (send_current_temp_report) {
		data_req.send_current_temp_report = 1;
	} else {
		data_req.temp_threshold_high_valid = high_valid;
		data_req.temp_threshold_high = (float)high_thresh;
		data_req.temp_threshold_low_valid = low_valid;
		data_req.temp_threshold_low = (float)low_thresh;
	}

	qmi_error = qmi_client_send_msg_sync((qmi_client_type) clnt,
				       QMI_TS_REGISTER_NOTIFICATION_TEMP_REQ_V01,
				       &data_req, sizeof(data_req),
				       &data_resp, sizeof(data_resp), 0);
	if (qmi_error == QMI_NO_ERR) {
		ret = 0;
	} else {
		msg("qmi: qmi_client_send_msg_sync failed. Error %d\n", qmi_error);
	}

	return ret;
}

/*===========================================================================
FUNCTION modem_ts_qmi_init

Helper function to initialize TS qmi communication to modem.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -(ERRNO) on failure.
===========================================================================*/
int modem_ts_qmi_init(void)
{
	/* Get the service object for the ts API */
	ts_service_object = ts_get_service_object_v01();
	if (!ts_service_object) {
		msg("qmi: ts_get_service_object failed.\n");
		return -(EPERM);
	}

	/* start thread to register with QMI services */
	pthread_create(&fusion_qmi_register_thread, NULL, fusion_qmi_register, NULL);

	return 0;
}

/*===========================================================================
FUNCTION modem_ts_temp_request

Function to request sensor read or notify threshold functionality.

ARGUMENTS
	sensor_id - name of sensor_id
	send_current_temp_report - 1 for trigger and immediate sensor reading, 0 for
				   set threshold.
	high_valid
	high_thresh
	low_valid
	low_thresh

RETURN VALUE
	0 on success, -(ERRNO) on failure.
===========================================================================*/
int modem_ts_temp_request(const char *sensor_id,
			   int send_current_temp_report,
			   int high_valid,
			   int high_thresh,
			   int low_valid,
			   int low_thresh)
{
	int ret = -(EPERM);

	if ((send_current_temp_report == 0) && ((high_valid == 1) &&
						(low_valid == 1)) &&
	    (high_thresh <= low_thresh)) {
		msg("Invalid thresh level. High %d, Low %d", high_thresh,
		    low_thresh);
		return -(EINVAL);
	}

	/* Make sure the QMI connection is established before proceeding. */
	pthread_mutex_lock(&fusion_modem_mtx);
	while (!fusion_modem_qmi_ts_ready)
		pthread_cond_wait(&fusion_modem_cond, &fusion_modem_mtx);
	pthread_mutex_unlock(&fusion_modem_mtx);

	if (!fusion_modem_clnt) {
		msg("Modem TS service failed - QMI registration incomplete");
		return ret;
	}

	ret = modem_ts_reg_notify(fusion_modem_clnt, sensor_id, send_current_temp_report,
				  high_valid,
				  high_thresh,
				  low_valid,
				  low_thresh);
	dbgmsg("%s %s, sensor %s, Get Immediate: %s, High valid: %s, High %d "
	       "Low valid: %s, Low %d\n",
	       __func__, (ret) ? ("Failed") : ("Success"), sensor_id,
	       (send_current_temp_report) ? ("YES") : ("NO"),
	       (high_valid) ? ("YES") : ("NO"), high_thresh,
	       (low_valid) ? ("YES") : ("NO"), low_thresh);

	return ret;
}

/*===========================================================================
FUNCTION modem_qmi_ts_comm_release

Release function for modem communication to clean up resources.
Called after use of client handles is complete.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -(ERRNO) on failure.
===========================================================================*/
int modem_qmi_ts_comm_release(void)
{
	int rc;
	int ret = 0;

	pthread_join(fusion_qmi_register_thread, NULL);

	if (fusion_modem_clnt) {
		rc = qmi_client_release(fusion_modem_clnt);
		if (rc) {
			msg("qmi: qmi_client_release modem clnt failed.\n");
			ret = -(EPERM);
		}
		fusion_modem_clnt = 0;
	}

	return ret;
}
