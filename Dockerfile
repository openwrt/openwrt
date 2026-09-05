# Usage:
# docker run --interactive --rm --tty --ulimit 'nofile=1024:262144' \
#   --volume "$(pwd):/workdir" --workdir '/workdir' buildbot:debian /bin/bash

# Primary (used) stage - Debian
# docker build --rm --target debian -t buildbot:debian .
FROM debian:trixie-slim AS debian
LABEL maintainer="xiaobo <peterwillcn@gmail.com>"

RUN apt-get update && apt-get install --no-install-recommends --no-install-suggests --yes \
    build-essential clang flex bison g++ gawk device-tree-compiler \
    gcc-multilib g++-multilib gettext git libncurses-dev libssl-dev \
    rsync swig unzip zlib1g-dev zstd file wget upx-ucl ca-certificates \
    python3-dev python3-pyelftools python3-setuptools elfutils libelf-dev quilt && \
  rm -rf /var/lib/apt/lists/* && \
  useradd -m -s /bin/bash -U buildbot

WORKDIR /workdir
USER buildbot

# Experimental (unused/default) stage - Alpine
# docker build --rm --target alpine -t buildbot:alpine .
FROM alpine:3.23 AS alpine
LABEL maintainer="xiaobo <peterwillcn@gmail.com>"

RUN apk update && apk add --no-cache \
    argp-standalone asciidoc autoconf automake bash bc binutils bzip2 build-base \
    cdrkit coreutils diffutils elfutils-dev e2fsprogs-dev findutils flex g++ \
    gawk gcc gettext git grep gzip intltool libtool libxslt linux-headers lzo-dev \
    make musl-fts-dev musl-libintl musl-obstack-dev ncurses-dev openssl-dev patch \
    perl pkgconfig py3-distutils-extra py3-elftools py3-setuptools python3-dev \
    sed rsync swig tar unzip util-linux wget zlib-dev bsd-compat-headers quilt && \
  addgroup buildbot && \
  adduser -s /bin/bash -G buildbot -D buildbot

WORKDIR /workdir
USER buildbot
