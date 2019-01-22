
## Build instructions

You need gcc, binutils, bzip2, flex, python, perl, make, find, grep, diff,
unzip, gawk, getopt, subversion, libz-dev and libc headers installed.

1. Run `./scripts/feeds update -a` to obtain all the latest package definitions
defined in feeds.conf.default
2. Run `./scripts/feeds install -a` to install symlinks for all obtained
packages into package/feeds/
3. Run `cp configs/default_config .config`
4. Run `make` to build your firmware.
