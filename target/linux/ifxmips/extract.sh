#!/bin/sh

DIR="$1/"
FILE="$1/$2"

echo "This tool downloads the arcor a800 firmware release and extracts the voip firmware for the danube."
echo "Please only do so if it is legal in your country"

[ ! -f ${FILE} ] && {
	echo ${FILE} is missing
	exit 1
}

[ ! -f ${DIR}bbd.crypt ] && {
	dd if=${FILE} of=${DIR}bbd.crypt bs=1 skip=104 count=1472302
}

[ ! -f ${DIR}dsl_a.bin ] && {
	dd if=${FILE} of=${DIR}dsl1.lzma bs=1 skip=2168832 count=150724
	lzma d ${DIR}dsl2.lzma ${DIR}dsl_a.bin
}

[ ! -f ${DIR}dsl_b.bin ] && {
	dd if=${FILE} of=${DIR}dsl2.lzma bs=1 skip=2320384 count=148343
	lzma d ${DIR}dsl1.lzma ${DIR}dsl_b.bin
}

[ ! -f ${DIR}voip.bin ] && {
	dd if=${FILE} of=${DIR}voip.lzma bs=1 skip=2468864 count=452105
	lzma d ${DIR}voip.lzma ${DIR}voip.bin
}
exit 0

# get lzma offsets
# hexdump -C arcor_A800_452CPW_FW_1.02.206\(20081201\).bin | grep "5d 00 00 80"
# hexdump -C arcor_A800_452CPW_FW_1.02.206\(20081201\).bin | grep "00 d5 08 00"
