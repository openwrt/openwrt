#!/bin/sh
#
# Copyright 2004, Broadcom Corporation      
# All Rights Reserved.      
#       
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY      
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM      
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS      
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.      
#
# $Id$
#

cat <<EOF
#include <linux/config.h>
#include <linux/module.h>
EOF

for file in $* ; do
    ${NM} $file | sed -ne 's/[0-9A-Fa-f]* [DT] \([^ ]*\)/extern void \1; EXPORT_SYMBOL(\1);/p'
done
