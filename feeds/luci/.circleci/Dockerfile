FROM debian:9


# Configuration version history
# v1.0   - Initial version by Etienne Champetier
# v1.0.1 - Run as non-root, add unzip, xz-utils
# v1.0.2 - Add bzr

RUN apt update && apt install -y \
build-essential \
curl \
jq \
gawk \
gettext \
git \
libncurses5-dev \
libssl-dev \
python \
subversion \
bzr \
time \
wget \
zlib1g-dev \
unzip \
xz-utils \
&& rm -rf /var/lib/apt/lists/*

RUN useradd -c "OpenWrt Builder" -m -d /home/build -s /bin/bash build
USER build
ENV HOME /home/build

# LEDE Build System (LEDE GnuPG key for unattended build jobs)
RUN curl 'https://git.openwrt.org/?p=keyring.git;a=blob_plain;f=gpg/626471F1.asc' | gpg --import \
 && echo '54CC74307A2C6DC9CE618269CD84BCED626471F1:6:' | gpg --import-ownertrust

# LEDE Release Builder (17.01 "Reboot" Signing Key)
RUN curl 'https://git.openwrt.org/?p=keyring.git;a=blob_plain;f=gpg/D52BBB6B.asc' | gpg --import \
 && echo 'B09BE781AE8A0CD4702FDCD3833C6010D52BBB6B:6:' | gpg --import-ownertrust

# OpenWrt Release Builder (18.06 Signing Key)
RUN curl 'https://git.openwrt.org/?p=keyring.git;a=blob_plain;f=gpg/17E1CE16.asc' | gpg --import \
 && echo '6768C55E79B032D77A28DA5F0F20257417E1CE16:6:' | gpg --import-ownertrust
