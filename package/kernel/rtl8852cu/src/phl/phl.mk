########### COMMON PATH  #################################
ifeq ($(CONFIG_HWSIM), y)
	HAL = hal_sim
else
	ifeq ($(CONFIG_WIFI_6), y)
	HAL = hal_g6
	else
	HAL = hal
	endif
endif

ifeq ($(CONFIG_PHL_ARCH), y)
phl_path := phl/
phl_path_d1 := $(src)/phl/$(HAL)
else
phl_path :=
phl_path_d1 := $(src)/$(HAL)
endif

_PHL_FILES := $(phl_path)phl_init.o \
			$(phl_path)phl_debug.o \
			$(phl_path)phl_tx.o \
			$(phl_path)phl_rx.o \
			$(phl_path)phl_rx_agg.o \
			$(phl_path)phl_api_drv.o \
			$(phl_path)phl_role.o \
			$(phl_path)phl_sta.o \
			$(phl_path)phl_mr.o \
			$(phl_path)phl_mr_coex.o \
			$(phl_path)phl_sec.o \
			$(phl_path)phl_chan.o \
			$(phl_path)phl_sw_cap.o \
			$(phl_path)phl_util.o \
			$(phl_path)phl_pkt_ofld.o \
			$(phl_path)phl_connect.o \
			$(phl_path)phl_chan_info.o \
			$(phl_path)phl_wow.o\
			$(phl_path)phl_dm.o \
			$(phl_path)phl_chnlplan.o \
			$(phl_path)phl_country.o \
			$(phl_path)phl_chnlplan_6g.o \
			$(phl_path)phl_regulation.o \
			$(phl_path)phl_regulation_6g.o \
			$(phl_path)phl_led.o \
			$(phl_path)phl_trx_mit.o \
			$(phl_path)phl_acs.o \
			$(phl_path)phl_mcc.o \
			$(phl_path)phl_ecsa.o \
			$(phl_path)test/phl_dbg_cmd.o \
			$(phl_path)test/phl_ser_dbg_cmd.o \
			$(phl_path)phl_msg_hub.o \
			$(phl_path)phl_sound.o \
			$(phl_path)phl_twt.o \
			$(phl_path)phl_notify.o \
			$(phl_path)phl_sound_cmd.o \
			$(phl_path)phl_p2pps.o \
			$(phl_path)phl_thermal.o \
			$(phl_path)phl_txpwr.o \
			$(phl_path)phl_ext_tx_pwr_lmt.o \
			$(phl_path)phl_dfs.o \
			$(phl_path)phl_scanofld.o \
			$(phl_path)phl_ie.o

ifeq ($(CONFIG_POWER_SAVE), y)
_PHL_FILES += $(phl_path)phl_ps.o \
						$(phl_path)test/phl_ps_dbg_cmd.o \
						$(phl_path)phl_cmd_ps.o
endif

ifeq ($(CONFIG_FSM), y)
_PHL_FILES += $(phl_path)phl_fsm.o \
						$(phl_path)phl_cmd_fsm.o \
						$(phl_path)phl_cmd_job.o \
						$(phl_path)phl_ser_fsm.o \
						$(phl_path)phl_btc_fsm.o
endif

_PHL_FILES += $(phl_path)phl_cmd_dispatch_engine.o\
						$(phl_path)phl_cmd_dispatcher.o\
						$(phl_path)phl_cmd_dispr_controller.o \
						$(phl_path)phl_cmd_ser.o \
						$(phl_path)phl_cmd_general.o \
						$(phl_path)phl_cmd_scan.o \
						$(phl_path)phl_cmd_btc.o \
						$(phl_path)phl_sound_cmd.o \
						$(phl_path)phl_watchdog.o \

ifeq ($(CONFIG_PCI_HCI), y)
_PHL_FILES += $(phl_path)hci/phl_trx_pcie.o
endif
ifeq ($(CONFIG_USB_HCI), y)
_PHL_FILES += $(phl_path)hci/phl_trx_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PHL_FILES += $(phl_path)hci/phl_trx_sdio.o
endif

ifeq ($(CONFIG_PHL_CUSTOM_FEATURE), y)
_PHL_FILES += $(phl_path)custom/phl_custom.o
endif

ifeq ($(CONFIG_PHL_TEST_SUITE), y)
_PHL_FILES += $(phl_path)test/trx_test.o
_PHL_FILES += $(phl_path)test/test_module.o
_PHL_FILES += $(phl_path)test/cmd_disp_test.o
_PHL_FILES += $(phl_path)test/mp/phl_test_mp.o
_PHL_FILES += $(phl_path)test/mp/phl_test_mp_config.o
_PHL_FILES += $(phl_path)test/mp/phl_test_mp_tx.o
_PHL_FILES += $(phl_path)test/mp/phl_test_mp_rx.o
_PHL_FILES += $(phl_path)test/mp/phl_test_mp_reg.o
_PHL_FILES += $(phl_path)test/mp/phl_test_mp_efuse.o
_PHL_FILES += $(phl_path)test/mp/phl_test_mp_txpwr.o
_PHL_FILES += $(phl_path)test/mp/phl_test_mp_cal.o
_PHL_FILES += $(phl_path)test/verify/phl_test_verify.o
_PHL_FILES += $(phl_path)test/verify/dbcc/phl_test_dbcc.o
endif

OBJS += $(_PHL_FILES)

EXTRA_CFLAGS += -I$(phl_path_d1)
include $(phl_path_d1)/hal.mk
