FROM ubuntu:24.04

ENV GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
WORKDIR /workdir

RUN apt-get update && apt-get install --no-install-recommends --no-install-suggests --yes \
	build-essential clang flex bison g++ gawk device-tree-compiler \
	gcc-multilib g++-multilib gettext git libncurses-dev libssl-dev \
	rsync swig unzip zlib1g-dev zstd file wget upx-ucl ca-certificates \
	python3-dev python3-pyelftools python3-setuptools elfutils libelf-dev && \
  rm -rf /var/lib/apt /var/cache/apt && \
  mkdir -p /workdir/build && chmod -R 777 /workdir && \
  useradd -m -s /bin/bash -U buildbot

USER buildbot

