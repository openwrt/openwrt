#!/usr/bin/env python3

from os import getenv
from pathlib import Path
from sys import argv
import hashlib
import json

if len(argv) != 2:
    print("ERROR: JSON info script requires output arg")
    exit(1)

json_path = Path(argv[1])
bin_dir = Path(getenv("BIN_DIR"))
image_file = bin_dir / getenv("IMAGE_NAME")


if not image_file.is_file():
    print("Skip JSON creation for non existing image: {}".format(image_file))
    exit(0)


def set(obj, key, value):
    if value != None and len(value) != 0:
        obj[key] = value


def get_titles():
    titles = []
    for prefix in ["", "ALT0_", "ALT1_", "ALT2_"]:
        title = {}
        for var in ["vendor", "model", "variant"]:
            set(title, var, getenv("DEVICE_{}{}".format(prefix, var.upper())))

        if title:
            titles.append(title)

    if not titles:
        title = getenv("DEVICE_TITLE")
        if title:
            titles.append({"title": title})

    return titles


device_id = getenv("DEVICE_ID")
image_hash = hashlib.sha256(image_file.read_bytes()).hexdigest()

image_info = {
    "metadata_version": 1,
    "target": "{}/{}".format(getenv("TARGET"), getenv("SUBTARGET")),
    "version_code": getenv("VERSION_CODE"),
    "version_number": getenv("VERSION_NUMBER"),
    "source_date_epoch": getenv("SOURCE_DATE_EPOCH"),
    "profiles": {
        device_id: {
            "images": [
                {
                    "type": getenv("IMAGE_TYPE"),
                    "filesystem": getenv("IMAGE_FILESYSTEM"),
                    "name": getenv("IMAGE_NAME"),
                    "sha256": image_hash,
                }
            ]
        }
    },
}

profile = image_info["profiles"][device_id]

set(profile, "titles", get_titles())
set(profile, "device_packages", getenv("DEVICE_PACKAGES", "").split())
set(profile, "supported_devices", getenv("SUPPORTED_DEVICES", "").split())
set(profile, "image_prefix", getenv("IMAGE_PREFIX"))

json_path.write_text(json.dumps(image_info, separators=(",", ":")))
