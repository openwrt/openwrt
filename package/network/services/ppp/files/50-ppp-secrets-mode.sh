#!/bin/sh

# pppd reads these as uid ppp, and INSTALL_CONF ships them 0600 root:root -
# as does a file preserved across sysupgrade from an install predating the uid.
for secret in /etc/ppp/chap-secrets /etc/ppp/pap-secrets; do
	[ -f "$secret" ] || continue
	chgrp ppp "$secret"
	chmod 0640 "$secret"
done

exit 0
