#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Save" "$@"
}

state=$1

PKI_DIR="/www"
cd ${PKI_DIR}
mkdir -p package
cd ..
chmod -R 0777 ${PKI_DIR}/package

echo "$state" > ${PKI_DIR}/package/meshcfg.meshcfg