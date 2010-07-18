#!/bin/sh
# Copyright (C) 2006 OpenWrt.org


regpassresult="0"
grep '^root:[^!]' /etc/passwd >&- 2>&-
regpassresult="$?"
shadowresult="0"
if [ -e "/etc/shadow" ]; then
	grep '^root:[^!]' /etc/shadow >&- 2>&-
        shadowresult="$?"
fi
[ "$regpassresult" = "0" ] && [ "$shadowresult" = "0" ] && [ -z "$FAILSAFE" ] && {
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
