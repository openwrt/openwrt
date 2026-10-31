#!/bin/sh

for secret in /etc/ppp/chap-secrets /etc/ppp/pap-secrets; do
	[ -n "$(find "$secret" -user root -group ppp -perm 0640 2>/dev/null)" ] || continue
	chown root:root "$secret"
	chmod 0600 "$secret"
done

exit 0
