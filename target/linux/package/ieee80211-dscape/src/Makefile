obj-$(CONFIG_IEEE80211) += 80211.o rate_control.o

80211-objs := \
	ieee80211.o \
	ieee80211_ioctl.o \
	sta_info.o \
	wep.o \
	wpa.o \
	ieee80211_proc.o \
	ieee80211_scan.o \
	ieee80211_sta.o \
	michael.o \
	tkip.o \
	aes_ccm.o \
	wme.o

ifeq ($(CONFIG_NET_SCHED),)
  80211-objs += fifo_qdisc.o
endif

ifeq ($(CONFIG_IEEE80211_LEDS),y)
  80211-objs += ieee80211_led.o
endif
