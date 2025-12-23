# # # # # # #
# Usage:
# #docker build --rm --target alpine -t buildbot:alpine .
# docker build --rm --target debian -t buildbot:debian .
# docker run --interactive --rm --tty --ulimit 'nofile=1024:262144' \
#   --volume "$(pwd):/workdir" --workdir '/workdir' buildbot:debian /bin/bash
# # # # # # #

FROM alpine:3.22 AS alpine
LABEL maintainer="xiaobo <peterwillcn@gmail.com>"

ENV GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
WORKDIR /workdir

RUN apk update && apk add --no-cache \
    argp-standalone asciidoc autoconf automake bash bc binutils bzip2 build-base \
    cdrkit coreutils diffutils elfutils-dev e2fsprogs-dev findutils flex g++ \
    gawk gcc gettext git grep gzip intltool libtool libxslt linux-headers lzo-dev \
    make musl-fts-dev musl-libintl musl-obstack-dev ncurses-dev openssl-dev patch \
    perl pkgconfig py3-distutils-extra py3-elftools py3-setuptools python3-dev \
    sed rsync swig tar unzip util-linux wget zlib-dev bsd-compat-headers && \
  addgroup buildbot && \
  adduser -s /bin/bash -G buildbot -D buildbot

USER buildbot


FROM debian:stable AS debian
LABEL maintainer="xiaobo <peterwillcn@gmail.com>"

ENV GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
WORKDIR /workdir

RUN apt-get update && apt-get install --no-install-recommends --no-install-suggests --yes \
	build-essential clang flex bison g++ gawk device-tree-compiler \
	gcc-multilib g++-multilib gettext git libncurses-dev libssl-dev \
	rsync swig unzip zlib1g-dev zstd file wget upx-ucl ca-certificates \
	python3-dev python3-pyelftools python3-setuptools elfutils libelf-dev && \
  rm -rf /var/lib/apt /var/cache/apt && \
  useradd -m -s /bin/bash -U buildbot

USER buildbot
