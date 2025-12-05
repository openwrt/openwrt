#!/bin/sh
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Author: Jascha Sundaresan <flizarthanon@gmail.com>
#
# This helper script generates a top-level 'rootfs' node for inclusion
# in a Flattened Image Tree (FIT) source (.its) file.  The node includes
# the size and SHA1 hash of a given root filesystem image and is used by
# certain Netgear devices which expect this metadata at the top level of
# the FIT structure.
#
# The resulting block is written to standard output and can be appended
# or inserted into an existing .its file by the OpenWrt build system.
# Example:
#     scripts/gen_netgear_rootfs_node.sh build_dir/.../root.squashfs > node.txt
#
# See also: scripts/mkits.sh, which generates the main FIT .its source.

ROOTFS_FILE="$1"
ROOTFS_SIZE=$(stat -c %s "${ROOTFS_FILE}")
ROOTFS_SHA1=$(
    sha1sum "${ROOTFS_FILE}" | awk '{ print "<0x" substr($0, 1, 8) \
                                          " 0x" substr($0, 9, 8) \
                                          " 0x" substr($0, 17, 8) \
                                          " 0x" substr($0, 25, 8) \
                                          " 0x" substr($0, 33, 8) ">" }'
)
cat <<EOF | sed 's/^/\t/'
rootfs {
	size = <${ROOTFS_SIZE}>;

	hash-1 {
		value = ${ROOTFS_SHA1};
		algo = "sha1";
	};
};

EOF
