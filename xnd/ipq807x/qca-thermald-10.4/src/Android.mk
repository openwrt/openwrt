BOARD_PLATFORM_LIST := msm7630_surf
BOARD_PLATFORM_LIST += msm7630_fusion
BOARD_PLATFORM_LIST += msm8660
BOARD_PLATFORM_LIST += msm8960
BOARD_PLATFORM_LIST += msm7627a
BOARD_PLATFORM_LIST += msm8974

ifeq ($(call is-board-platform-in-list,$(BOARD_PLATFORM_LIST)),true)

IS_MSM7630:=$(strip $(call is-chipset-in-board-platform,msm7630))
IS_MSM8660:=$(strip $(call is-board-platform,msm8660))
IS_MSM8960:=$(strip $(call is-board-platform,msm8960))
IS_MSM8625:=$(strip $(call is-board-platform,msm7627a))
IS_MSM8974:=$(strip $(call is-board-platform,msm8974))

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(QC_PROP_ROOT)/common/build/remote_api_makefiles/target_api_enables.mk
include $(QC_PROP_ROOT)/common/build/remote_api_makefiles/remote_api_defines.mk

LOCAL_C_INCLUDES := \
        $(TARGET_OUT_HEADERS)/common/inc \
        $(TARGET_OUT_HEADERS)/oncrpc/inc \
        $(TARGET_OUT_HEADERS)/diag/include \
        $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES := \
        thermal.c \
        thermal_config.c \
        thermal_monitor.c \
        thermal_actions.c \
        thermal_util.c \
        thermal_server.c \
        thermal_lib_common.c

LOCAL_SHARED_LIBRARIES := \
        libcutils

LOCAL_CFLAGS := -DCONFIG_FILE_DEFAULT='"/system/etc/thermald.conf"'
LOCAL_CFLAGS += -include bionic/libc/kernel/arch-arm/asm/posix_types.h

ifneq ($(call is-platform-sdk-version-at-least,17),true)
  # Override socket-related kernel headers with Bionic version before JB MR1
  LOCAL_CFLAGS += -include bionic/libc/kernel/arch-arm/asm/byteorder.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/posix_types.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/types.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/socket.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/in.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/netlink.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/un.h
endif

ifneq ($(IS_MSM8625),)
LOCAL_SRC_FILES += sensors-8x25.c
LOCAL_CFLAGS += -DSENSORS_8625
ifeq ($(ADC_ENABLE),1)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/adc/inc
LOCAL_SHARED_LIBRARIES += libadc liboncrpc
endif
endif

ifneq ($(IS_MSM7630),)
LOCAL_SRC_FILES += sensors-7x30.c
LOCAL_CFLAGS += -DSENSORS_7630
THERMALD_CONF := thermald-7x30.conf
endif

ifneq ($(IS_MSM8660),)
LOCAL_SRC_FILES += tsens-sensor.c sensors-8660.c
LOCAL_CFLAGS += -DSENSORS_8660
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_MODEM_MITIGATION_ONCRPC:=1
ENABLE_FUSION_MODEM:=1
THERMALD_CONF := thermald-8660.conf
endif

ifneq ($(IS_MSM8960),)
LOCAL_SRC_FILES += tsens-sensor.c adc-sensor.c sensors-8960.c gen-sensor.c bcl-sensor.c pm8821-sensor.c
LOCAL_CFLAGS += -DSENSORS_8960
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_MODEM_TS_QMI:=1
endif

ifneq ($(IS_MSM8974),)
LOCAL_SRC_FILES += tsens-sensor.c adc-sensor.c sensors-8974.c gen-sensor.c bcl-sensor.c
LOCAL_CFLAGS += -DSENSORS_8974
#ENABLE_MODEM_MITIGATION_QMI:=1
endif

ifneq ($(ENABLE_MODEM_MITIGATION_ONCRPC),)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/thermal_mitigation/inc
LOCAL_CFLAGS += -DENABLE_MODEM_MITIGATION
LOCAL_SRC_FILES += modem_mitigation_oncrpc.c
LOCAL_SHARED_LIBRARIES += liboncrpc
LOCAL_SHARED_LIBRARIES += libthermal_mitigation

ifneq ($(ENABLE_FUSION_MODEM),)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/thermal_mitigation_fusion/inc
LOCAL_CFLAGS += -DENABLE_FUSION_MODEM
LOCAL_SHARED_LIBRARIES += libthermal_mitigation_fusion
endif
endif

ifneq ($(ENABLE_MODEM_MITIGATION_QMI),)
LOCAL_CFLAGS += -DENABLE_MODEM_MITIGATION
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_SRC_FILES += modem_mitigation_qmi.c
LOCAL_SRC_FILES += thermal_mitigation_device_service_v01.c
LOCAL_SHARED_LIBRARIES += libqmi_cci libqmi_common_so
endif

ifneq ($(ENABLE_MODEM_TS_QMI),)
LOCAL_CFLAGS += -DENABLE_MODEM_TS
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_SRC_FILES += modem_sensor_qmi.c qmi-ts-sensor.c
LOCAL_SRC_FILES += thermal_sensor_service_v01.c
LOCAL_SHARED_LIBRARIES += libqmi_cci libqmi_common_so
endif

LOCAL_CFLAGS += \
        -DUSE_ANDROID_LOG \

LOCAL_MODULE := thermald

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

#Install thermal client library
#include $(CLEAR_VARS)

#LOCAL_C_INCLUDES := \
        $(TARGET_OUT_HEADERS)/common/inc \
        $(TARGET_OUT_HEADERS)/diag/include

#LOCAL_SHARED_LIBRARIES := libcutils \
                          libdiag

#LOCAL_SRC_FILES:= thermal_client.c thermal_lib_common.c
#LOCAL_MODULE:= libthermalclient
#LOCAL_MODULE_TAGS := optional
#LOCAL_CFLAGS = \
        -DUSE_ANDROID_LOG
#LOCAL_MODULE_OWNER := qcom
#LOCAL_PROPRIETARY_MODULE := true

#include $(BUILD_SHARED_LIBRARY)

# Install thermal configuration
ifneq ($(IS_MSM8974),)
include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8974.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8974.conf
include $(BUILD_PREBUILT)

else ifneq ($(IS_MSM8960),)
include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8960.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8960.conf
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8064.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8064.conf
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8930.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8930.conf
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8960ab.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8960ab.conf
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8064ab.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8064ab.conf
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8930ab.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8930ab.conf
include $(BUILD_PREBUILT)

else ifneq ($(IS_MSM8625),)
include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8x25-msm1-pmic_therm.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8x25-msm1-pmic_therm.conf
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8x25-msm2-pmic_therm.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8x25-msm2-pmic_therm.conf
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermald-8x25-msm2-msm_therm.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermald-8x25-msm2-msm_therm.conf
include $(BUILD_PREBUILT)
else
include $(CLEAR_VARS)
LOCAL_MODULE       := thermald.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(THERMALD_CONF)
include $(BUILD_PREBUILT)
endif

endif
