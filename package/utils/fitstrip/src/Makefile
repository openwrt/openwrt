
OBJS := fitstrip.o
LIBS := -Wl,-Bstatic -lfdt -Wl,-Bdynamic
DEPS := $(OBJS:%.o=%.d)

CC ?= gcc
CFLAGS ?= -O2 -ffunction-sections -fdata-sections -Wall -Werror
LDFLAGS ?= -Wl,--gc-sections
OPTFLAGS ?= -ggdb

all: fitstrip

fitstrip: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(OPTFLAGS) -MD -c -o $@ $<

clean:
	rm -f fitstrip $(OBJS) $(DEPS)

.PHONY: clean

-include $(DEPS)
