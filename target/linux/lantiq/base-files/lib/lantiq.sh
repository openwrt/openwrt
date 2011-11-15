#!/bin/sh

lantiq_soc_has_adsl() {
	ls /lib/modules/*/drv_dsl_cpe_api.ko
}

lantiq_soc_name() {
	grep ^system /proc/cpuinfo | sed "s/system type.*: \(.*\)/\1/g"
}

lantiq_board_name() {
	grep ^machine /proc/cpuinfo | sed "s/machine.*: \(.*\)/\1/g" | sed "s/\(.*\) - .*/\1/g"
}
