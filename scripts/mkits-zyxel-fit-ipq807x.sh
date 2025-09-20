#!/usr/bin/env bash
#
# Licensed under the terms of the GNU GPL License version 2 or later.
# Author: David Bauer <mail@david-bauer.net>, based on mkits-zyxel-factory.sh.
# Modified by: Pascal Beleiu <pascal@beleiu.de>, based on mkits-zyxel-fit-filogic.sh

usage() {
	echo "Usage: `basename $0` output file compat-models"
	exit 1
}

# We need at least 3 arguments
[ "$#" -lt 3 ] && usage

# Target output file
OUTPUT="$1"; shift
FILE="$1"; shift
MODELS="$1"; shift

# Create a default, fully populated DTS file
echo "\
/dts-v1/;

/ {
	timestamp = <0x677706a7>;
	description = \"Zyxel FIT (Flattened Image Tree)\";
	compat-models = [${MODELS}];
	fw_version = \"9.99(###.1)\";

	images {
		ubi {
			data = /incbin/(\"${FILE}\");
			type = \"firmware\";
			arch = \"arm\";
			compression = \"none\";
			hash@1 {
				algo = \"crc32\";
				value = <0xabcd>;
			};
		};
	};
};" > ${OUTPUT}
