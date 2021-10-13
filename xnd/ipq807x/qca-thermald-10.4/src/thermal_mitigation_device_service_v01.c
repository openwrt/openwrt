/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        T H E R M A L _ M I T I G A T I O N _ D E V I C E _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the tmd service Data structures.

  Copyright (c) 2011 Qualcomm Technologies, Inc.  All Rights Reserved. 
 Qualcomm Technologies Proprietary and Confidential.

  $Header: //source/qcom/qct/mpss/api/tmd/main/latest/src/thermal_mitigation_device_service_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 2.7
   It was generated on: Thu Sep  1 2011
   From IDL File: thermal_mitigation_device_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "thermal_mitigation_device_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t tmd_mitigation_dev_id_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(tmd_mitigation_dev_id_type_v01, mitigation_dev_id),
  QMI_TMD_MITIGATION_DEV_ID_LENGTH_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t tmd_mitigation_dev_list_type_data_v01[] = {
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_mitigation_dev_list_type_v01, mitigation_dev_id),
 0, 0,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(tmd_mitigation_dev_list_type_v01, max_mitigation_level),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/* 
 * tmd_get_mitigation_device_list_req_msg is empty
 * static const uint8_t tmd_get_mitigation_device_list_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t tmd_get_mitigation_device_list_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_get_mitigation_device_list_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(tmd_get_mitigation_device_list_resp_msg_v01, mitigation_device_list) - QMI_IDL_OFFSET8(tmd_get_mitigation_device_list_resp_msg_v01, mitigation_device_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_get_mitigation_device_list_resp_msg_v01, mitigation_device_list),
  QMI_TMD_MITIGATION_DEV_LIST_MAX_V01,
  QMI_IDL_OFFSET8(tmd_get_mitigation_device_list_resp_msg_v01, mitigation_device_list) - QMI_IDL_OFFSET8(tmd_get_mitigation_device_list_resp_msg_v01, mitigation_device_list_len),
  1, 0
};

static const uint8_t tmd_set_mitigation_level_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_set_mitigation_level_req_msg_v01, mitigation_dev_id),
  0, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(tmd_set_mitigation_level_req_msg_v01, mitigation_level)
};

static const uint8_t tmd_set_mitigation_level_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_set_mitigation_level_resp_msg_v01, resp),
  0, 1
};

static const uint8_t tmd_get_mitigation_level_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_get_mitigation_level_req_msg_v01, mitigation_device),
  0, 0
};

static const uint8_t tmd_get_mitigation_level_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_get_mitigation_level_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(tmd_get_mitigation_level_resp_msg_v01, current_mitigation_level) - QMI_IDL_OFFSET8(tmd_get_mitigation_level_resp_msg_v01, current_mitigation_level_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(tmd_get_mitigation_level_resp_msg_v01, current_mitigation_level),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(tmd_get_mitigation_level_resp_msg_v01, requested_mitigation_level) - QMI_IDL_OFFSET8(tmd_get_mitigation_level_resp_msg_v01, requested_mitigation_level_valid)),
  0x11,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(tmd_get_mitigation_level_resp_msg_v01, requested_mitigation_level)
};

static const uint8_t tmd_register_notification_mitigation_level_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_register_notification_mitigation_level_req_msg_v01, mitigation_device),
  0, 0
};

static const uint8_t tmd_register_notification_mitigation_level_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_register_notification_mitigation_level_resp_msg_v01, resp),
  0, 1
};

static const uint8_t tmd_deregister_notification_mitigation_level_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_deregister_notification_mitigation_level_req_msg_v01, mitigation_device),
  0, 0
};

static const uint8_t tmd_deregister_notification_mitigation_level_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_deregister_notification_mitigation_level_resp_msg_v01, resp),
  0, 1
};

static const uint8_t tmd_mitigation_level_report_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(tmd_mitigation_level_report_ind_msg_v01, mitigation_device),
  0, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(tmd_mitigation_level_report_ind_msg_v01, current_mitigation_level)
};

/* Type Table */
static const qmi_idl_type_table_entry  tmd_type_table_v01[] = {
  {sizeof(tmd_mitigation_dev_id_type_v01), tmd_mitigation_dev_id_type_data_v01},
  {sizeof(tmd_mitigation_dev_list_type_v01), tmd_mitigation_dev_list_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry tmd_message_table_v01[] = {
  {0, 0},
  {sizeof(tmd_get_mitigation_device_list_resp_msg_v01), tmd_get_mitigation_device_list_resp_msg_data_v01},
  {sizeof(tmd_set_mitigation_level_req_msg_v01), tmd_set_mitigation_level_req_msg_data_v01},
  {sizeof(tmd_set_mitigation_level_resp_msg_v01), tmd_set_mitigation_level_resp_msg_data_v01},
  {sizeof(tmd_get_mitigation_level_req_msg_v01), tmd_get_mitigation_level_req_msg_data_v01},
  {sizeof(tmd_get_mitigation_level_resp_msg_v01), tmd_get_mitigation_level_resp_msg_data_v01},
  {sizeof(tmd_register_notification_mitigation_level_req_msg_v01), tmd_register_notification_mitigation_level_req_msg_data_v01},
  {sizeof(tmd_register_notification_mitigation_level_resp_msg_v01), tmd_register_notification_mitigation_level_resp_msg_data_v01},
  {sizeof(tmd_deregister_notification_mitigation_level_req_msg_v01), tmd_deregister_notification_mitigation_level_req_msg_data_v01},
  {sizeof(tmd_deregister_notification_mitigation_level_resp_msg_v01), tmd_deregister_notification_mitigation_level_resp_msg_data_v01},
  {sizeof(tmd_mitigation_level_report_ind_msg_v01), tmd_mitigation_level_report_ind_msg_data_v01}
};

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object tmd_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *tmd_qmi_idl_type_table_object_referenced_tables_v01[] =
{&tmd_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object tmd_qmi_idl_type_table_object_v01 = {
  sizeof(tmd_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(tmd_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  tmd_type_table_v01,
  tmd_message_table_v01,
  tmd_qmi_idl_type_table_object_referenced_tables_v01
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry tmd_service_command_messages_v01[] = {
  {QMI_TMD_GET_MITIGATION_DEVICE_LIST_REQ_V01, TYPE16(0, 0), 0},
  {QMI_TMD_SET_MITIGATION_LEVEL_REQ_V01, TYPE16(0, 2), 40},
  {QMI_TMD_GET_MITIGATION_LEVEL_REQ_V01, TYPE16(0, 4), 36},
  {QMI_TMD_REGISTER_NOTIFICATION_MITIGATION_LEVEL_REQ_V01, TYPE16(0, 6), 36},
  {QMI_TMD_DEREGISTER_NOTIFICATION_MITIGATION_LEVEL_REQ_V01, TYPE16(0, 8), 36}
};

static const qmi_idl_service_message_table_entry tmd_service_response_messages_v01[] = {
  {QMI_TMD_GET_MITIGATION_DEVICE_LIST_RESP_V01, TYPE16(0, 1), 1099},
  {QMI_TMD_SET_MITIGATION_LEVEL_RESP_V01, TYPE16(0, 3), 7},
  {QMI_TMD_GET_MITIGATION_LEVEL_RESP_V01, TYPE16(0, 5), 15},
  {QMI_TMD_REGISTER_NOTIFICATION_MITIGATION_LEVEL_RESP_V01, TYPE16(0, 7), 7},
  {QMI_TMD_DEREGISTER_NOTIFICATION_MITIGATION_LEVEL_RESP_V01, TYPE16(0, 9), 7}
};

static const qmi_idl_service_message_table_entry tmd_service_indication_messages_v01[] = {
  {QMI_TMD_MITIGATION_LEVEL_REPORT_IND_V01, TYPE16(0, 10), 40}
};

/*Service Object*/
const struct qmi_idl_service_object tmd_qmi_idl_service_object_v01 = {
  0x02,
  0x01,
  24,
  1099,
  { sizeof(tmd_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(tmd_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(tmd_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { tmd_service_command_messages_v01, tmd_service_response_messages_v01, tmd_service_indication_messages_v01},
  &tmd_qmi_idl_type_table_object_v01
};

/* Service Object Accessor */
qmi_idl_service_object_type tmd_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( TMD_V01_IDL_MAJOR_VERS != idl_maj_version || TMD_V01_IDL_MINOR_VERS != idl_min_version 
       || TMD_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&tmd_qmi_idl_service_object_v01;
}

