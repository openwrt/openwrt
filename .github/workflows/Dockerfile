FROM debian:10

RUN apt-get update -qq &&\
    apt-get install -y \
        build-essential \
        ccache \
        clang \
        curl \
        file \
        g++-multilib \
        gawk \
        gcc-multilib \
        gettext \
        git \
        libdw-dev \
        libelf-dev \
        libncurses5-dev \
        locales \
        pv \
        pwgen \
        python \
        python3 \
        python3-pip \
        qemu-utils \
        rsync \
        signify-openbsd \
        subversion \
        sudo \
        swig \
        unzip \
        wget \
        zlib1g-dev \
        && apt-get -y autoremove \
        && apt-get clean \
        && rm -rf /var/lib/apt/lists/*

RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
RUN useradd -c "OpenWrt Builder" -m -d /home/build -G sudo -s /bin/bash build

COPY --chown=build:build . /home/build/openwrt/
RUN chown build:build /home/build/openwrt/

USER build
ENV HOME /home/build
WORKDIR /home/build/openwrt/