#!/bin/sh

lantiq_soc_name() {
	grep ^system /proc/cpuinfo | sed "s/system type.*: \(.*\)/\1/g"
}

lantiq_board_name() {
	grep ^machine /proc/cpuinfo | sed "s/machine.*: \(.*\)/\1/g" | sed "s/\(.*\) - .*/\1/g"
}
