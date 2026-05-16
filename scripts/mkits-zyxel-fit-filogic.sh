#!/usr/bin/env bash
#
# Licensed under the terms of the GNU GPL License version 2 or later.
# Author: David Bauer <mail@david-bauer.net>, based on mkits-zyxel-factory.sh.

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
	timestamp = <0x684090B4>;
	description = \"Zyxel FIT (Flattened Image Tree)\";
	compat-models = [${MODELS}];
	fw_version = \"9.99(###.1)\";
	#address-cells = <1>;

	images {
		ubi {
			data = /incbin/(\"${FILE}\");
			type = \"firmware\";
			compression = \"none\";
			hash {
				algo = \"sha256\";
			};
		};
	};
};" > ${OUTPUT}
