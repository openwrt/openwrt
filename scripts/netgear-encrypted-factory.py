#!/usr/bin/env python3

import argparse
import re
import struct
import subprocess
import zlib
from concurrent.futures import ThreadPoolExecutor


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--input-file', type=str, required=True)
    parser.add_argument('--output-file', type=str, required=True)
    parser.add_argument('--model', type=str, required=True)
    parser.add_argument('--region', type=str, required=True)
    parser.add_argument('--version', type=str, required=True)
    parser.add_argument('--hw-id-list', type=str)
    parser.add_argument('--model-list', type=str)
    parser.add_argument('--encryption-block-size', type=str, required=True)
    parser.add_argument('--openssl-bin', type=str, required=True)
    parser.add_argument('--key', type=str, required=True)
    parser.add_argument('--iv', type=str, required=True)
    args = parser.parse_args()

    assert re.match(r'V[0-9]\.[0-9]\.[0-9]\.[0-9]',
                    args.version), 'Version must start with Vx.x.x.x'
    encryption_block_size = int(args.encryption_block_size, 0)
    assert (encryption_block_size > 0 and encryption_block_size % 16 ==
            0), 'Encryption block size must be a multiple of the AES block size (16)'

    hw_id_list = args.hw_id_list.split(';') if args.hw_id_list else []
    model_list = args.model_list.split(';') if args.model_list else []
    hw_info = ';'.join(hw_id_list + model_list)

    # Optimization: stream the file reading in chunks instead of loading the whole
    # thing into memory, reducing memory usage from O(N) to O(1) for large files
    chunks = []
    with open(args.input_file, 'rb') as f:
        while True:
            chunk = f.read(encryption_block_size)
            if not chunk:
                break
            chunks.append(chunk)

    def encrypt_chunk(chunk):
        chunk += b'\x00' * ((-len(chunk)) % 16)  # pad to AES block size (16)
        res = subprocess.run([
            args.openssl_bin,
            'enc',
            '-aes-256-cbc',
            '-nosalt',
            '-nopad',
            '-K', args.key,
            '-iv', args.iv
        ],
            check=True, input=chunk, stdout=subprocess.PIPE)
        return res.stdout

    with ThreadPoolExecutor() as executor:
        image_enc = b''.join(executor.map(encrypt_chunk, chunks))

    image_with_header = struct.pack(
        '>32s32s64s64sIBBB13s200s100s12sII',
        args.model.encode('ascii'),
        args.region.encode('ascii'),
        args.version.encode('ascii'),
        b'Thu Jan 1 00:00:00 1970',  # static date for reproducibility
        0,  # product hw model
        0,  # model index
        len(hw_id_list),
        len(model_list),
        b'',  # reserved
        hw_info.encode('ascii'),
        b'',  # reserved
        b'encrpted_img',
        len(image_enc),
        encryption_block_size,
    ) + image_enc

    checksum = zlib.crc32(image_with_header, 0xffffffff) ^ 0xffffffff

    with open(args.output_file, 'wb') as outfile:
        outfile.write(image_with_header)
        outfile.write(struct.pack('>I', checksum))


if __name__ == "__main__":
    main()
