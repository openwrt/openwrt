#
# Makefile for Nftables/netfilter fullcone expression support.
#

# module name, should not have the same name as src file names
TARGET = nft_fullcone

obj-m += $(TARGET).o

$(TARGET)-objs := \
	nf_nat_fullcone.o \
	nft_ext_fullcone.o

# product
ccflags-y += -Werror -Wall

# develop
#ccflags-y += -Wall -Wno-unused-function