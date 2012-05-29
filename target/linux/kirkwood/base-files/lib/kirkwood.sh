#!/bin/sh

kirkwood_hardware_name() {
	grep ^Hardware /proc/cpuinfo | sed "s/Hardware.*: \(.*\)/\1/g"
}
