#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Save" "$@"
}

PKI_DIR="/www"
cd ${PKI_DIR}
mkdir -p package
cd ..
chmod -R 0777 ${PKI_DIR}/package

echo "***Profile***" > ${PKI_DIR}/package/profilecfg.profile
state=$(cat /etc/config/profile)
echo "$state" >> ${PKI_DIR}/package/profilecfg.profile