# SPDX-License-Identifier: GPL-2.0
OBJS := main.o config.o nl.o apply.o event.o ubus.o omci.o omci_mib.o mibstore.o omci_bind.o omci_me_table.o omci_alarm.o \
	omci_test.o omci_pm.o ethtool_stats.o persist.o eth.o eth_vlan.o eth_l3.o eth_mcast.o omci_mcast.o swdl.o monitor.o mcast_gate.o

override CFLAGS += -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers \
	  -std=gnu11 -D_GNU_SOURCE -I$(STAGING_INC)/libnl-tiny
override LDLIBS += -lubus -lubox -luci -lnl-tiny

gpond: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f gpond *.o

.PHONY: clean
