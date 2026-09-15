#!/usr/bin/env bash
#
# Licensed under the terms of the GNU GPL License version 2 or later.
# Author: Leonard Anderweit <leonard.anderweit@gmail.com>, based on mkits-zyxel-factory-filogic.sh.

usage() {
	echo "Usage: `basename $0` output fitimage conf loadaddr"
	exit 1
}

# We need at least 4 arguments
[ "$#" -lt 4 ] && usage

# Target output file
OUTPUT="$1"; shift
FITIMAGE="$1"; shift
CONF="$1"; shift
LOADADDR="$1"; shift

# Create a default, fully populated ITS file
echo "\
/dts-v1/;

/ {
	description = \"AVM FIT (Flattened Image Tree)\";
	images {
		kernel-1 {
			data = /incbin/(\"${FITIMAGE}\");
			type = \"avm,fit\";
			arch = \"arm64\";
			os = \"linux\";
			compression = \"none\";
			load = <${LOADADDR}>;
		};
	};
	configurations {
		default = \"${CONF}\";
		${CONF} {
			kernel = \"kernel-1\";
		};
	};
};" > ${OUTPUT}
