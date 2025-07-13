#!/bin/sh
# four parameters
# $1 : compress type
# $2 : kernel entry address
# $3 : input bin
# $4 : output image

COMPRESS_TYPE="COMPRESS_TYPE"
KERNEL_ENTRY="KERNEL_ENTRY"
IMG_PATH="IMG_PATH"

if [ ! -f "${SIGN}/mkimage" ]; then
	echo "ERROR:mkiage not found!\n"
	exit 1
fi

if [ ! -f "${SIGN}/sign.its" ]; then
	echo "ERROR: file sign.its not found"
	exit 1
fi

if [ ! -f "${SIGN}/keys/dev.key" ];then
	if [ ! -d "${SIGN}/keys" ]; then
		mkdir "${SIGN}/keys"
	fi
	openssl genrsa -F4 -out "${SIGN}/keys/dev.key" 1024
	openssl req -batch -new -x509 -key "${SIGN}/keys/dev.key" \
	-out "${SIGN}/keys/dev.crt"
fi

cp "${SIGN}/sign.its" "${SIGN}/.sign.its"
sed -i "s/${COMPRESS_TYPE}/$1/" "${SIGN}/.sign.its"
sed -i "s/${KERNEL_ENTRY}/$2/" "${SIGN}/.sign.its"
sed -i "s:${IMG_PATH}:$3:" "${SIGN}/.sign.its"

if [ ! -f "$4" ]; then
	touch "$4"
fi

"${SIGN}/mkimage" -f "${SIGN}/.sign.its" -k "${SIGN}/keys" \
-r "$4"
rm "${SIGN}/.sign.its"
