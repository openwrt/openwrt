
## Build instructions

You need gcc, binutils, bzip2, flex, python, perl, make, find, grep, diff,
unzip, gawk, getopt, subversion, libz-dev and libc headers installed.

1. Run `./scripts/feeds update -a` to obtain all the latest package definitions
defined in feeds.conf.default
2. Run `./scripts/feeds install -a` to install symlinks for all obtained
packages into package/feeds/
3. Run `cp configs/default_config .config`
4. Run `make` to build your firmware.


## Development of LibreMesh packages

To do work in libremesh packages you should make a clone of the repo and then
you can use this clone as part of the build using a local symbolic link. This way
you can change the files and run `make`. When you are satisfied by your changes you
should commit your changes to the lime-packages repo in a new branch and push this
to github and do a pull request.

Fork the lime-packages repo in github and then clone it replacing yourgithubuser
```
git clone git@github.com:yourgithubuser/lime-packages.git
```

Then edit feeds.conf.default and change the line

```
src-git libremesh https://github.com/libremesh/lime-packages.git;master
```

with this line replacing the path to repo and branch name (replace all the
line, check that src-git changed to src-link):

```
src-link libremesh /home/user/path/to/lime-packages
```

Run `./scripts/feeds update -a` and then `./scripts/feeds install -a`

Now perform your changes to the lime-packages repo, editing files, etc.
If you are creating new packages you should run `feeds install -a` after you create
the package in order to be able to select the package with make menuconfig.

When you want to create an image with your changes run `make`. If you need change
the package selection run `make menuconfig` and then `make`.


## Notes

* You can speed up the build process using more CPUs, for example with `make -j3`
the build uses 3 CPUs.
* For aditional documentation see
  * https://openwrt.org/docs/guide-developer/build-system/use-buildsystem
  * https://openwrt.org/docs/guide-developer/feeds
  * https://openwrt.org/docs/guide-developer/packages
  * https://openwrt.org/docs/guide-developer/start
