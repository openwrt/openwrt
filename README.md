
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

## Testing a LibreMesh branch

Edit feeds.conf.default, for example to test the `add-fft-eval` branch:

```
diff --git a/feeds.conf.default b/feeds.conf.default
index 5a681c88c4..ee2c5f1223 100644
--- a/feeds.conf.default
+++ b/feeds.conf.default
@@ -3,6 +3,6 @@ src-git luci https://git.openwrt.org/project/luci.git^f64b1523447547032d5280fb0b
 src-git routing https://git.openwrt.org/feed/routing.git^1b9d1c419f0ecefda51922a7845ab2183d6acd76
 src-git telephony https://git.openwrt.org/feed/telephony.git^b9d7b321d15a44c5abb9e5d43a4ec78abfd9031b

-src-git libremesh https://github.com/libremesh/lime-packages.git;master
+src-git libremesh https://github.com/libremesh/lime-packages.git;add-fft-eval
 src-git libremap https://github.com/libremap/libremap-agent-openwrt.git;master
 src-git fbw https://github.com/libremesh/FirstBootWizard.git;master
```

```
$ ./scripts/feeds update -a
```

update the feed as new packages may have beed added to a feed
```
$ ./scripts/feeds install  -a
```

## Changing package and options selection

The config documentation is here `https://openwrt.org/docs/guide-developer/build-system/use-buildsystem`.
Some tips:
* The config is stored in .config
* To change the config use `make manuconfig`
* To know the difference between two configs, for example from the current config (.config) and
the default configuration run `$ ./scripts/diffconfig.sh  | diff configs/default_config -`
* To save the new config to a new file (for backup purposes, to add it to a git repo, etc) run
`$ ./scripts/diffconfig.sh  > configs/your_config`
* To change the default configuration, for example to perform a pull request to `git@github.com:LibreRouterOrg/openwrt.git` run
`$ ./scripts/diffconfig.sh  > configs/your_config`


## Upgrading the buildroot

```
$ git pull

$ ./scripts/feeds update -a

$ ./scripts/feeds install  -a
```

Copy the new config, be aware that this overwrites you local config!!
```
$ cp configs/default_config .config
```

```
$ make defconfig
$ make -j4
...
```

## Notes

* You can speed up the build process using more CPUs, for example with `make -j3`
the build uses 3 CPUs.
* For aditional documentation see
  * https://openwrt.org/docs/guide-developer/build-system/use-buildsystem
  * https://openwrt.org/docs/guide-developer/feeds
  * https://openwrt.org/docs/guide-developer/packages
  * https://openwrt.org/docs/guide-developer/start
