#!/usr/bin/env python3

from os import getenv, environ
from pathlib import Path
from subprocess import run, PIPE
from sys import argv
import json

if len(argv) != 2:
    print("JSON info files script requires ouput file as argument")
    exit(1)

output_path = Path(argv[1])

assert getenv("WORK_DIR"), "$WORK_DIR required"

# merge profiles
def get_output(work_dir):
    output = None

    for json_file in work_dir.glob("*.json"):
        image_info = json.loads(json_file.read_text())

        # use first json file as template
        if not output:
            output = image_info

        # get first (and probably only) profile in json file
        for device_id, profile in image_info["profiles"].items():
            output["profiles"][device_id] = {
                **profile,
                **output["profiles"].get(device_id, {}),
            }
            output["profiles"][device_id]["images"].extend(profile["images"])

    # make image lists unique by name
    for device_id, profile in output["profiles"].items():
        profile["images"] = list({e["name"]: e for e in profile["images"]}.values())

    return output


output = get_output(Path(getenv("WORK_DIR")))

if output:
    default_packages, output["arch_packages"] = run(
        [
            "make",
            "--no-print-directory",
            "-C",
            "target/linux/{}".format(output['target'].split('/')[0]),
            "val.DEFAULT_PACKAGES",
            "val.ARCH_PACKAGES",
        ],
        stdout=PIPE,
        stderr=PIPE,
        check=True,
        env=environ.copy().update({"TOPDIR": Path().cwd()}),
        universal_newlines=True,
    ).stdout.splitlines()

    output["default_packages"] = default_packages.split()
    output_path.write_text(json.dumps(output, sort_keys=True, separators=(",", ":")))
else:
    print("JSON info file script could not find any JSON files for target")
