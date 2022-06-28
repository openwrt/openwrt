SRCS= \
      libxs2184/xs2184.c \
      libxs2184/i2cbusses.c

CFLAGS += -Wall -Werror -O1 -s
LDFLAGS += -lpthread -ldl

ifeq ($(CC),cc)
CC=${CROSS_COMPILE}gcc
endif

all:
	${CC} ${CFLAGS} ${SRCS} -o xs2184 ${LDFLAGS}

clean:
	-rm -f *.o xs2184
