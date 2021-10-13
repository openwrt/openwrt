#ifndef TS_SERVICE_H
#define TS_SERVICE_H
/**
  @file thermal_sensor_service_v01.h

  @brief This is the public header file which defines the ts service Data structures.

  This header file defines the types and structures that were defined in
  ts. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

  $Header: //source/qcom/qct/interfaces/qmi/ts/main/latest/api/thermal_sensor_service_v01.h#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 4.2
   It was generated on: Mon Dec  5 2011
   From IDL File: thermal_sensor_service_v01.idl */

/** @defgroup ts_qmi_consts Constant values defined in the IDL */
/** @defgroup ts_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ts_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ts_qmi_messages Structures sent as QMI messages */
/** @defgroup ts_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ts_qmi_accessor Accessor for QMI service object */
/** @defgroup ts_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ts_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define TS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define TS_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define TS_V01_IDL_TOOL_VERS 0x04
/** Maximum Defined Message ID */
#define TS_V01_MAX_MESSAGE_ID 0x0022;
/**
    @}
  */


/** @addtogroup ts_qmi_consts
    @{
  */
#define QMI_TS_SENSOR_LIST_MAX_V01 32
#define QMI_TS_SENSOR_ID_LENGTH_MAX_V01 32
/**
    @}
  */

/** @addtogroup ts_qmi_aggregates
    @{
  */
typedef struct {

  char sensor_id[QMI_TS_SENSOR_ID_LENGTH_MAX_V01 + 1];
  /**<   Sensor ID  */
}ts_sensor_type_v01;  /* Type */
/**
    @}
  */

/*
 * ts_get_sensor_list_req_msg is empty
 * typedef struct {
 * }ts_get_sensor_list_req_msg_v01;
 */

/** @addtogroup ts_qmi_messages
    @{
  */
/** Response Message; Gets the list of sensors from the remote endpoint. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.  */

  /* Optional */
  /*  Sensor List */
  uint8_t sensor_list_valid;  /**< Must be set to true if sensor_list is being passed */
  uint32_t sensor_list_len;  /**< Must be set to # of elements in sensor_list */
  ts_sensor_type_v01 sensor_list[QMI_TS_SENSOR_LIST_MAX_V01];
}ts_get_sensor_list_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ts_qmi_messages
    @{
  */
/** Request Message; Registers for notification of temperature sensor readings.
           Indicates threshold temperatures for notification. */
typedef struct {

  /* Mandatory */
  /*  Sensor ID */
  ts_sensor_type_v01 sensor_id;

  /* Mandatory */
  /*  Current Temperature Report */
  uint8_t send_current_temp_report;
  /**<   Request for current temperature report indication.
   */

  /* Optional */
  /*  High Threshold Temperature */
  uint8_t temp_threshold_high_valid;  /**< Must be set to true if temp_threshold_high is being passed */
  float temp_threshold_high;
  /**<   High threshold temperature in degrees Celsius.
   */

  /* Optional */
  /*  Low Threshold Temperature */
  uint8_t temp_threshold_low_valid;  /**< Must be set to true if temp_threshold_low is being passed */
  float temp_threshold_low;
  /**<   Low threshold temperature in degrees Celsius.
   */

  /* Optional */
  /*  Sequence Number */
  uint8_t seq_num_valid;  /**< Must be set to true if seq_num is being passed */
  uint32_t seq_num;
  /**<   Optional sequence cookie to match the registration with the indication.
   */
}ts_register_notification_temp_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ts_qmi_messages
    @{
  */
/** Response Message; Registers for notification of temperature sensor readings.
           Indicates threshold temperatures for notification. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.  */
}ts_register_notification_temp_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ts_qmi_enums
    @{
  */
typedef enum {
  TS_TEMP_REPORT_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_TS_TEMP_REPORT_CURRENT_TEMP_V01 = 0,
  QMI_TS_TEMP_REPORT_THRESHOLD_HIGH_V01 = 1,
  QMI_TS_TEMP_REPORT_THRESHOLD_LOW_V01 = 2,
  TS_TEMP_REPORT_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ts_temp_report_type_enum_v01;
/**
    @}
  */

/** @addtogroup ts_qmi_messages
    @{
  */
/** Indication Message; Indicates that a temperature report has been received. */
typedef struct {

  /* Mandatory */
  /*  Sensor ID */
  ts_sensor_type_v01 sensor_id;

  /* Mandatory */
  /*  Sensor Reported */
  ts_temp_report_type_enum_v01 report_type;
  /**<   Indicates temperature report type. If report_type is not recognized,
       the client ignores the report.
   */

  /* Optional */
  /*  Temperature */
  uint8_t temp_valid;  /**< Must be set to true if temp is being passed */
  float temp;
  /**<   Temperature report in degrees Celsius. If this field is omitted,
       the service cannot obtain the temperature value from the sensor.
   */

  /* Optional */
  /*  Sequence Number */
  uint8_t seq_num_valid;  /**< Must be set to true if seq_num is being passed */
  uint32_t seq_num;
  /**<   Optional sequence cookie to match the registration with the indication.
   */
}ts_temp_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup ts_qmi_msg_ids
    @{
  */
#define QMI_TS_GET_SENSOR_LIST_REQ_V01 0x0020
#define QMI_TS_GET_SENSOR_LIST_RESP_V01 0x0020
#define QMI_TS_REGISTER_NOTIFICATION_TEMP_REQ_V01 0x0021
#define QMI_TS_REGISTER_NOTIFICATION_TEMP_RESP_V01 0x0021
#define QMI_TS_TEMP_REPORT_IND_V01 0x0022
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro ts_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type ts_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define ts_get_service_object_v01( ) \
          ts_get_service_object_internal_v01( \
            TS_V01_IDL_MAJOR_VERS, TS_V01_IDL_MINOR_VERS, \
            TS_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif
