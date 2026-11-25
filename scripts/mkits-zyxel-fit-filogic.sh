#!/usr/bin/env bash
#
# Licensed under the terms of the GNU GPL License version 2 or later.
# Author: David Bauer <mail@david-bauer.net>, based on mkits-zyxel-factory.sh.

usage() {
	echo "Usage: `basename $0` output file model-ids"
	echo "  model-ids: 1 to 5 model IDs of 4 hex digits each, e.g. \"80e1 81e1\""
	exit 1
}

# We need at least 3 arguments
[ "$#" -lt 3 ] && usage

# Target output file
OUTPUT="$1"; shift
FILE="$1"; shift
MODEL_IDS="$1"; shift

# Like the stock firmware, list five model IDs, with unused ones set to ffff
MODELS=""
COUNT=0
for ID in ${MODEL_IDS}; do
	case "$ID" in
	[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]) ;;
	*) echo "Invalid model ID: $ID" >&2; exit 1 ;;
	esac
	MODELS="${MODELS}${ID:0:2} ${ID:2:2} "
	COUNT=$((COUNT + 1))
done
[ "$COUNT" -lt 1 ] || [ "$COUNT" -gt 5 ] && usage
while [ "$COUNT" -lt 5 ]; do
	MODELS="${MODELS}ff ff "
	COUNT=$((COUNT + 1))
done
MODELS="${MODELS% }"

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
