#!/bin/bash
# This is a Softfloat version of gcc for ARM suitable to compile U-Boot.
# It was compiled using crosstool-0.42 using
export	GCCROOT=/usr/local/arm
export	GCC_VERSION=3.4.5
export	GLIBC_VERSION=2.3.6
export	CROSS_CHAIN=gcc-$GCC_VERSION-glibc-$GLIBC_VERSION
export	PATH=$PATH:$GCCROOT/$CROSS_CHAIN/arm-softfloat-linux-gnu/bin
export	CROSS_COMPILE=arm-softfloat-linux-gnu-
