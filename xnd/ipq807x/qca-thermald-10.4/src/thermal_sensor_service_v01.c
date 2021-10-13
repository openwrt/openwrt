/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        T H E R M A L _ S E N S O R _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the ts service Data structures.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 4.2
   It was generated on: Mon Dec  5 2011
   From IDL File: thermal_sensor_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "thermal_sensor_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t ts_sensor_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ts_sensor_type_v01, sensor_id),
  QMI_TS_SENSOR_ID_LENGTH_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * ts_get_sensor_list_req_msg is empty
 * static const uint8_t ts_get_sensor_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ts_get_sensor_list_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ts_get_sensor_list_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ts_get_sensor_list_resp_msg_v01, sensor_list) - QMI_IDL_OFFSET8(ts_get_sensor_list_resp_msg_v01, sensor_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ts_get_sensor_list_resp_msg_v01, sensor_list),
  QMI_TS_SENSOR_LIST_MAX_V01,
  QMI_IDL_OFFSET8(ts_get_sensor_list_resp_msg_v01, sensor_list) - QMI_IDL_OFFSET8(ts_get_sensor_list_resp_msg_v01, sensor_list_len),
  0, 0
};

static const uint8_t ts_register_notification_temp_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, sensor_id),
  0, 0,

  0x02,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, send_current_temp_report),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, temp_threshold_high) - QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, temp_threshold_high_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, temp_threshold_high),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, temp_threshold_low) - QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, temp_threshold_low_valid)),
  0x11,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, temp_threshold_low),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, seq_num) - QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, seq_num_valid)),
  0x12,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ts_register_notification_temp_req_msg_v01, seq_num)
};

static const uint8_t ts_register_notification_temp_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ts_register_notification_temp_resp_msg_v01, resp),
  0, 1
};

static const uint8_t ts_temp_report_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ts_temp_report_ind_msg_v01, sensor_id),
  0, 0,

  0x02,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ts_temp_report_ind_msg_v01, report_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ts_temp_report_ind_msg_v01, temp) - QMI_IDL_OFFSET8(ts_temp_report_ind_msg_v01, temp_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ts_temp_report_ind_msg_v01, temp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ts_temp_report_ind_msg_v01, seq_num) - QMI_IDL_OFFSET8(ts_temp_report_ind_msg_v01, seq_num_valid)),
  0x11,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ts_temp_report_ind_msg_v01, seq_num)
};

/* Type Table */
static const qmi_idl_type_table_entry  ts_type_table_v01[] = {
  {sizeof(ts_sensor_type_v01), ts_sensor_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry ts_message_table_v01[] = {
  {0, 0},
  {sizeof(ts_get_sensor_list_resp_msg_v01), ts_get_sensor_list_resp_msg_data_v01},
  {sizeof(ts_register_notification_temp_req_msg_v01), ts_register_notification_temp_req_msg_data_v01},
  {sizeof(ts_register_notification_temp_resp_msg_v01), ts_register_notification_temp_resp_msg_data_v01},
  {sizeof(ts_temp_report_ind_msg_v01), ts_temp_report_ind_msg_data_v01}
};

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object ts_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *ts_qmi_idl_type_table_object_referenced_tables_v01[] =
{&ts_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object ts_qmi_idl_type_table_object_v01 = {
  sizeof(ts_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(ts_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  ts_type_table_v01,
  ts_message_table_v01,
  ts_qmi_idl_type_table_object_referenced_tables_v01
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry ts_service_command_messages_v01[] = {
  {QMI_TS_GET_SENSOR_LIST_REQ_V01, TYPE16(0, 0), 0},
  {QMI_TS_REGISTER_NOTIFICATION_TEMP_REQ_V01, TYPE16(0, 2), 61}
};

static const qmi_idl_service_message_table_entry ts_service_response_messages_v01[] = {
  {QMI_TS_GET_SENSOR_LIST_RESP_V01, TYPE16(0, 1), 1067},
  {QMI_TS_REGISTER_NOTIFICATION_TEMP_RESP_V01, TYPE16(0, 3), 7}
};

static const qmi_idl_service_message_table_entry ts_service_indication_messages_v01[] = {
  {QMI_TS_TEMP_REPORT_IND_V01, TYPE16(0, 4), 57}
};

/*Service Object*/
const struct qmi_idl_service_object ts_qmi_idl_service_object_v01 = {
  0x04,
  0x01,
  23,
  1067,
  { sizeof(ts_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ts_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ts_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { ts_service_command_messages_v01, ts_service_response_messages_v01, ts_service_indication_messages_v01},
  &ts_qmi_idl_type_table_object_v01
};

/* Service Object Accessor */
qmi_idl_service_object_type ts_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( TS_V01_IDL_MAJOR_VERS != idl_maj_version || TS_V01_IDL_MINOR_VERS != idl_min_version
       || TS_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&ts_qmi_idl_service_object_v01;
}
