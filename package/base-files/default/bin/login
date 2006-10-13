#!/bin/sh
# Copyright (C) 2006 OpenWrt.org

grep '^root:[^!]' /etc/passwd >&- 2>&-
[ "$?" = "0" -a -z "$FAILSAFE" ]  &&  
{
    echo "Login failed."
    exit 0
} || {
cat << EOF
 === IMPORTANT ============================
  Use 'passwd' to set your login password
  this will disable telnet and enable SSH
 ------------------------------------------
EOF
}

exec /bin/ash --login
