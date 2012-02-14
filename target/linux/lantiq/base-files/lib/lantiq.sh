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

lantiq_dsl_fwannex() {
	ls /lib/firmware/dsl-fw-*.bin 2> /dev/null | sed "s/.*\([ab]\)\.bin/\1/g"
}
