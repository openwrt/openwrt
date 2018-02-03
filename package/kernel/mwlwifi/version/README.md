`package/kernel/mwlwifi/version`
--------------------------------
Description
===========
`package/kernel/mwlwifi/version/Makefile` updates mwlwifi package Makefile's
`PKG_VERSION` to a `git describe` style of version, as if the upstream authors
were tagging it with `mwlwifi-src/hif/pcie/dev.h:PCIE_DRV_VERSION` whenever it's
changed, by looking up the source's revision history from a local git repo. This
Makefile also downloads mirror archive, and then updates package Makefile's
`PKG_MIRROR_HASH` during the same process.

Usage
=====
```
make [REV=revision_hash] [REPO_PATH=src_repo_path] [FORCE_FETCH=y | NO_FETCH=y]
     [NO_REUSE_REPO=y]
```
`REV`
-----
`revision_hash` is the upstream revision hash you want mwlwifi to be built
against. It can be as short as it shown in the first field of
`git log --oneline`. Defaults to package Makefile's `PKG_SOURCE_VERSION`.

`REPO_PATH`
-----------
`src_repo_path` is the path to upstream git repo that is already on the machine.
This Makefile will create a symlink pointing to this path when the local git
repo is missing. Without this path, the local git repo will be cloned from
`PKG_SOURCE_URL` of package Makefile.

`FORCE_FETCH`
-------------
By defaults, the local git repo won't be updated more than once a day. If you
want to force it to update, set `FORCE_FETCH` to any non-empty value.

`NO_FETCH`
----------
If the last time the local git repo was updated was more than one day ago, and
you want to prevent it from updating, set `NO_FETCH` to any non-empty value.

`NO_REUSE_REPO`
---------------
By defaults, this Makefile will temporarily set `PKG_SOURCE_URL` to the location
of the local git repo for openwrt's build system to clone from, in order to
create the mirror archive. If you want the build system to clone the upstream
git repo from the actual `PKG_SOURCE_URL` instead, set `NO_REUSE_REPO` to any
non-empty value.

Example
=======
* At an already configured openwrt source root directory, run:

  `make -C package/kernel/mwlwifi/version REV=abc1234`

  will update `package/kernel/mwlwifi/Makefile`:

  - `PKG_VERSION` => (`w.x.y.z-yyyymmdd-0-gabc1234`)

  - `PKG_SOURCE_VERSION` => (full 40 characters hash of `abc1234`)

  - `PKG_MIRROR_HASH` => (hash of newly downloaded archive)

* To remove the local git repo( or the symlink pointing to it), run:

  `make -C package/kernel/mwlwifi/version clean-all`
