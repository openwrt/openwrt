#!/usr/bin/env python3
#
# This script compares local file sizes with remote files and prints an overview
#
# env variables
#   TARGET_DIR where to look for the local files
#   REMOTE_URL where to look for the remote files
#

import os
import subprocess
import multiprocessing
from urllib import request

target_dir = "./bin" + os.environ.get("TARGET_DIR", "")

remote_url = os.environ.get("REMOTE_URL", "https://downloads.openwrt.org/snapshots")

local_files = []
results_queue = multiprocessing.Queue()


def compare_sizes(filename):
    """
    compare local and remote filesize
    """
    try:
        size_local = os.path.getsize("./bin" + filename)
        response = request.urlopen(remote_url + filename)
        size_remote = int(response.info()["Content-Length"])
        results_queue.put((filename, size_local - size_remote))
    except:
        print("error on {}".format(filename))


for root, dirs, files in os.walk(target_dir):
    for filename in files:
        if not filename in [
            "Packages",
            "Packages.gz",
            "Packages.manifest",
            "Packages.sig",
            "sha256sums",
            "config.buildinfo",
            "feeds.buildinfo",
            "version.buildinfo",
        ]:
            local_files.append(os.path.join(root[5:], filename))

if not os.path.exists(target_dir):
    print(f"{target_dir} not found")
    quit(1)

if not remote_url.startswith("http"):
    print(f"{remote_url} must be a valid url")
    quit(1)


# check 50 files in parallel
pool = multiprocessing.Pool(50)
pool.map(compare_sizes, local_files)

results = []

# convert queue to array
for i in range(0, results_queue.qsize()):
    results.append(results_queue.get())


# sort array
for filename, size_diff in sorted(results):
    if size_diff == 0:
        continue
    prefix = "+"
    if size_diff < 0:
        prefix = "-"
    print("{} {:>10}B {}".format(prefix, size_diff, filename))
