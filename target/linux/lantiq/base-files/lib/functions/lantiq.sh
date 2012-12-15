#!/bin/sh

lantiq_board_id() {
	grep "^system type" /proc/cpuinfo | sed "s/system type.*: \(.*\)/\1/g" | sed "s/.* - \(.*\) - .*/\1/g"
}

lantiq_board_name() {
	grep "^system type" /proc/cpuinfo | sed "s/system type.*: \(.*\)/\1/g" | sed "s/.* - \(.*\)/\1/g"
}
