# SPDX-License-Identifier: MIT
"""Independent MIT implementation of the RTX830 CANZ container format.

Originally authored against an independently prepared format specification
(2026-10-10). The layout, bounds and checks are defined by the constants and
validator below; no proprietary firmware or executable code is embedded.

Validation establishes structural consistency, not payload safety or successful
hardware execution. A generated payload must be tested separately. This is not
a generic firmware updater. The secondary is fixed to 256 zero bytes.
HMAC-MD5 is a format checksum, not a security signature.
"""

from __future__ import annotations

import argparse
import gzip
import hashlib
import hmac
import io
import json
from pathlib import Path
import struct
import sys
import zlib

HEADER_SIZE = 256
MAX_IMAGE_SIZE = 10 * 1024 * 1024
RAM_SIZE = 256 * 1024 * 1024
MAIN_LOAD = 0x00030000
SECONDARY_LOAD = 0x0F410000
MAIN_ENTRY = MAIN_LOAD + HEADER_SIZE
MAX_MAIN_SIZE = SECONDARY_LOAD - MAIN_ENTRY
SECONDARY = bytes(256)
HMAC_OFFSET = 0x3C
HMAC_SIZE = 16
HMAC_KEY = b"CANZONE"


class ValidationError(ValueError):
    """The candidate does not satisfy the supplied external specification."""


def _word(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def _set_word(data: bytearray, offset: int, value: int) -> None:
    struct.pack_into("<I", data, offset, value)


def _digest(image: bytes | bytearray) -> bytes:
    digest = hmac.new(HMAC_KEY, digestmod=hashlib.md5)
    digest.update(image[:HMAC_OFFSET])
    digest.update(bytes(HMAC_SIZE))
    digest.update(image[HMAC_OFFSET + HMAC_SIZE:])
    return digest.digest()


def _gzip(payload: bytes) -> bytes:
    buffer = io.BytesIO()
    # GzipFile fixes the OS marker across Python versions, and emits no filename.
    with gzip.GzipFile(filename="", mode="wb", fileobj=buffer,
                       compresslevel=9, mtime=0) as stream:
        stream.write(payload)
    return buffer.getvalue()


def _padded(data: bytes) -> bytes:
    return data + bytes((-len(data)) % 4)


def _main_header(payload_size: int, secondary_offset: int,
                 image_size: int) -> bytearray:
    header = bytearray(HEADER_SIZE)
    fields = {0: MAIN_LOAD, 4: HEADER_SIZE + payload_size, 8: 0,
              12: secondary_offset, 16: MAIN_ENTRY, 20: 0x64,
              24: 0x0002000F, 28: 0, 32: 0x21, 56: image_size}
    for offset, value in fields.items():
        _set_word(header, offset, value)
    header[36:40] = b"CANZ"
    return header


def _secondary_header(payload_size: int, container_size: int) -> bytearray:
    header = bytearray(HEADER_SIZE)
    for offset, value in {0: SECONDARY_LOAD, 4: HEADER_SIZE + payload_size,
                          8: 0x10000, 12: container_size,
                          16: 0x0F410100, 20: 0x0F410154}.items():
        _set_word(header, offset, value)
    return header


def _check_regions(main_size: int, secondary_size: int) -> None:
    if not 1 <= main_size <= MAX_MAIN_SIZE:
        raise ValidationError("main payload is empty, too large, or overlaps secondary")
    if secondary_size != len(SECONDARY):
        raise ValidationError("this candidate requires exactly 256 secondary zero bytes")
    main_end = MAIN_ENTRY + main_size
    secondary_end = SECONDARY_LOAD + HEADER_SIZE + secondary_size
    if not (0 <= MAIN_LOAD < main_end <= RAM_SIZE
            and 0 <= SECONDARY_LOAD < secondary_end <= RAM_SIZE):
        raise ValidationError("load regions exceed 256 MiB RAM")
    if MAIN_LOAD < secondary_end and SECONDARY_LOAD < main_end:
        raise ValidationError("main and secondary load regions overlap")
    if not MAIN_LOAD + HEADER_SIZE <= MAIN_ENTRY < main_end:
        raise ValidationError("entry is outside main payload")


def create(main: bytes, secondary: bytes = SECONDARY) -> bytes:
    """Build the experimental candidate from an independently authored main.

    Secondary is fixed to 256 zero bytes. Raises ValidationError on invalid sizes
    or data. Does not access files, devices, or the network.
    """
    if not isinstance(main, bytes) or not isinstance(secondary, bytes):
        raise TypeError("main and secondary must be bytes")
    _check_regions(len(main), len(secondary))
    if secondary != SECONDARY:
        raise ValidationError("secondary must contain only zero bytes")
    main_gzip = _padded(_gzip(main))
    secondary_gzip = _padded(_gzip(secondary))
    secondary_offset = HEADER_SIZE + len(main_gzip)
    secondary_size = HEADER_SIZE + len(secondary_gzip)
    image_size = secondary_offset + secondary_size
    if image_size > MAX_IMAGE_SIZE:
        raise ValidationError("container exceeds 10 MiB")
    image = (_main_header(len(main), secondary_offset, image_size) + main_gzip
             + _secondary_header(len(secondary), secondary_size) + secondary_gzip)
    image[HMAC_OFFSET:HMAC_OFFSET + HMAC_SIZE] = _digest(image)
    return bytes(image)


def _inspect_gzip(data: bytes, expected: int, limit: int,
                  *, require_zeros: bool = False) -> dict:
    if not 0 < expected <= limit:
        raise ValidationError("gzip declared size exceeds permitted bounds")
    decoder = zlib.decompressobj(16 + zlib.MAX_WBITS)
    pending = data
    produced = 0
    digest = hashlib.sha256()
    while True:
        # The extra byte detects an oversize stream without unbounded expansion.
        bound = min(65536, expected - produced + 1)
        try:
            chunk = decoder.decompress(pending, bound)
        except zlib.error as exc:
            raise ValidationError("invalid gzip stream, CRC, or ISIZE") from exc
        produced += len(chunk)
        if produced > expected:
            raise ValidationError("gzip expansion exceeds declared payload size")
        if require_zeros and any(chunk):
            raise ValidationError("secondary must contain only zero bytes")
        digest.update(chunk)
        if decoder.eof:
            padding = decoder.unused_data
            if len(padding) > 3 or any(padding):
                raise ValidationError("gzip has extra members/data or invalid zero padding")
            break
        pending = decoder.unconsumed_tail
        if not pending:
            raise ValidationError("gzip stream is truncated or lacks stream end")
    if produced != expected:
        raise ValidationError("gzip size does not match header")
    return {"size": produced, "sha256": digest.hexdigest(),
            "gzip_size": len(data) - len(padding), "padding_size": len(padding)}


def validate(image: bytes) -> dict:
    """Check structure, checksum, bounded gzip, and RAM layout; return a manifest.

    Successful validation does not establish hardware acceptance or update safety.
    """
    if not isinstance(image, bytes):
        raise TypeError("image must be bytes")
    if not 2 * HEADER_SIZE < len(image) <= MAX_IMAGE_SIZE:
        raise ValidationError("container is truncated or exceeds 10 MiB")
    if len(image) % 4:
        raise ValidationError("container length is not aligned to four bytes")
    if _word(image, 56) != len(image):
        raise ValidationError("declared image length does not match file")
    secondary_offset = _word(image, 12)
    if secondary_offset % 4 or not HEADER_SIZE < secondary_offset < len(image) - HEADER_SIZE:
        raise ValidationError("secondary offset is unaligned or outside image")
    main_size = _word(image, 4) - HEADER_SIZE
    secondary_header = image[secondary_offset:secondary_offset + HEADER_SIZE]
    secondary_size = _word(secondary_header, 4) - HEADER_SIZE
    _check_regions(main_size, secondary_size)
    expected_main = _main_header(main_size, secondary_offset, len(image))
    expected_main[HMAC_OFFSET:HMAC_OFFSET + HMAC_SIZE] = image[HMAC_OFFSET:HMAC_OFFSET + HMAC_SIZE]
    if image[:HEADER_SIZE] != expected_main:
        raise ValidationError("main header constants or reserved zero fields are invalid")
    secondary_container_size = len(image) - secondary_offset
    if secondary_header != _secondary_header(secondary_size, secondary_container_size):
        raise ValidationError("secondary header fields or container length are invalid")
    if not hmac.compare_digest(image[HMAC_OFFSET:HMAC_OFFSET + HMAC_SIZE], _digest(image)):
        raise ValidationError("HMAC-MD5 format checksum mismatch")
    main_info = _inspect_gzip(image[HEADER_SIZE:secondary_offset], main_size, MAX_MAIN_SIZE)
    secondary_info = _inspect_gzip(image[secondary_offset + HEADER_SIZE:], secondary_size,
                                   len(SECONDARY), require_zeros=True)
    return {"format": "CANZ", "specification": "independent CANZ format description (2026-10-10); implemented in this file",
            "experimental": True, "hardware_unverified": True,
            "warning": "Specification consistency only; hardware acceptance and OEM updater safety unverified. Not a generic updater.",
            "hmac_md5_is_security_signature": False,
            "unknown_fields_zeroed": {"main_0x28_to_0x37": True,
                                       "remaining_reserved_header_bytes": True},
            "image": {"size": len(image), "sha256": hashlib.sha256(image).hexdigest()},
            "main": main_info, "secondary": secondary_info,
            "secondary_offset": secondary_offset}


def _read_bounded(path: Path, limit: int) -> bytes:
    with path.open("rb") as stream:
        data = stream.read(limit + 1)
    if len(data) > limit:
        raise ValidationError(f"input exceeds {limit} bytes: {path}")
    return data


def _write_new_files(files: list[tuple[Path, bytes]]) -> None:
    resolved = [path.resolve() for path, _ in files]
    if len(set(resolved)) != len(resolved):
        raise ValidationError("output and manifest paths must differ")
    for path in resolved:
        if path.exists():
            raise FileExistsError(f"refusing to overwrite existing output: {path}")
    created = []
    try:
        # Exclusive creation also handles races after the existence checks.
        for path, data in files:
            with path.open("xb") as stream:
                created.append(path)
                stream.write(data)
    except BaseException:
        for path in created:
            path.unlink(missing_ok=True)
        raise


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    build = commands.add_parser("create", help="package an independently authored main")
    build.add_argument("--main", type=Path, required=True)
    build.add_argument("--output", type=Path, required=True)
    build.add_argument("--manifest", type=Path, help="default: OUTPUT.manifest.json")
    check = commands.add_parser("validate", help="check specification consistency only")
    check.add_argument("image", type=Path)
    check.add_argument("--manifest", type=Path, help="optional new manifest output")
    args = parser.parse_args(argv)
    try:
        if args.command == "create":
            image = create(_read_bounded(args.main, MAX_MAIN_SIZE))
            manifest = validate(image)
            manifest_path = args.manifest or Path(str(args.output) + ".manifest.json")
            document = (json.dumps(manifest, indent=2, sort_keys=True) + "\n").encode("utf-8")
            _write_new_files([(args.output, image), (manifest_path, document)])
        else:
            manifest = validate(_read_bounded(args.image, MAX_IMAGE_SIZE))
            if args.manifest:
                document = (json.dumps(manifest, indent=2, sort_keys=True) + "\n").encode("utf-8")
                _write_new_files([(args.manifest, document)])
        print(json.dumps(manifest, indent=2, sort_keys=True))
        return 0
    except (OSError, ValidationError) as exc:
        print(f"pack_canz: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
