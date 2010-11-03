ifndef CFLAGS
CFLAGS = -O2 -g -I ../src
endif

CFLAGS += -Werror -Wall -std=gnu99

PJ_CFLAGS ?= `pkg-config libpjproject --cflags`
PJ_LDFLAGS ?= `pkg-config libpjproject --libs`

CFLAGS += $(PJ_CFLAGS)
LDFLAGS += $(PJ_LDFLAGS)

FPIC=-fPIC

all: tapi-sip

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

tapi-sip: contact.o session.o tapi_agent.o tapi_sip.o sip_client.o stun.o dialdetector.o
	$(CC) -o $@ $^ -ltapi -luci $(LDFLAGS)
