#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

# Check the ksmbd response-signing invariant in an already prepared kernel
# source tree.  Run this after OpenWrt has applied its generic kernel patches:
#
#   sh tools/testing/selftests/ksmbd/ksmbd-signing.sh "$LINUX_DIR"
#
# The unpatched 6.12/6.18 source fails because it only signs the final iovec
# except for SMB2_READ.  QUERY_INFO and CHANGE_NOTIFY responses can put the
# SMB header and payload in separate iovecs, so every iovec from the one that
# starts at the current header must be signed.

set -eu

if [ "$#" -ne 1 ]; then
	echo "usage: $0 KERNEL_SOURCE_DIR" >&2
	exit 2
fi

source_dir=$1
source_file="$source_dir/fs/smb/server/smb2pdu.c"

[ -r "$source_file" ] || {
	echo "missing kernel source: $source_file" >&2
	exit 2
}

grep -Fq 'static struct kvec *smb2_get_sign_rsp_iov' "$source_file" || {
	echo "ksmbd multi-iovec signing helper is missing" >&2
	exit 1
}

[ "$(grep -Fc 'iov = smb2_get_sign_rsp_iov(work, hdr, &n_vec);' "$source_file")" -eq 2 ] || {
	echo "SMB2 and SMB3 response signing do not use the helper" >&2
	exit 1
}

grep -Fq 'for (i = 1; i <= work->iov_idx; i++)' "$source_file" || {
	echo "response signing does not search from the first SMB iovec" >&2
	exit 1
}

echo "ksmbd response signing covers all iovecs from the SMB header"
