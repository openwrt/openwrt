REV ?= 2

PWD := $(shell pwd)

EXTRA_CFLAGS += -I$(src)/include

EXTRA_CFLAGS += -DLINUX -D__KERNEL__ -DHTC_RAW_INTERFACE\
                -DTCMD -DUSER_KEYS \
                -DNO_SYNC_FLUSH #\
                -DMULTIPLE_FRAMES_PER_INTERRUPT -DAR6000REV$(REV) \
                -DBLOCK_TX_PATH_FLAG \
		-DSDIO \

EXTRA_CFLAGS += -DKERNEL_2_6

obj-$(CONFIG_AR6000_WLAN) += ar6000.o

ar6000-objs += htc/ar6k.o      		   \
	       htc/ar6k_events.o 	   \
               htc/htc_send.o       	   \
               htc/htc_recv.o       	   \
               htc/htc_services.o          \
               htc/htc.o     		   \
               hif/hif2.o     		   \
               bmi/bmi.o                   \
               ar6000/ar6000_drv.o         \
               ar6000/ar6000_raw_if.o	   \
               ar6000/netbuf.o		   \
               ar6000/wireless_ext.o	   \
               ar6000/ioctl.o		   \
               miscdrv/common_drv.o	   \
               miscdrv/credit_dist.o	   \
               wmi/wmi.o                   \
               wlan/wlan_node.o            \
               wlan/wlan_recv_beacon.o     \
               wlan/wlan_utils.o


