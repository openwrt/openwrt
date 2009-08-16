HOST_OS=$(shell uname)

CC=gcc
AR=ar
RANLIB=ranlib

WFLAGS = -Wall -Werror -Wno-format
CFLAGS?=-O2
CPPFLAGS=
LDFLAGS=
LIBS=

