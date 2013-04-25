#!/bin/sh

lantiq_board_id() {
	grep "^machine" /proc/cpuinfo | sed "s/machine.*: \(.*\)/\1/g" | sed "s/\(.*\) - .*/\1/g"             
}

lantiq_board_name() {
	grep "^machine" /proc/cpuinfo | sed "s/machine.*: \(.*\)/\1/g" | sed "s/.* - \(.*\)/\1/g"
}
