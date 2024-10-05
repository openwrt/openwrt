#!/bin/sh
# shellcheck disable=3037,3060,2034,1091

# check if stdout is a terminal, then set colors.
if test -t 1; then
  fg=$(printf '\033[38;5;')
  bg=$(printf '\033[48;5;')
  red=$(printf '\033[31m')
  green=$(printf '\033[32m')
  yellow=$(printf '\033[33m')
  blue=$(printf '\033[34m')
  magenta=$(printf '\033[35m')
  cyan=$(printf '\033[36m')
  white=$(printf '\033[37m')
  reset=$(printf '\033[m')
  bold=$(printf '\033[1m')
  underline=$(printf '\033[4m')
  blink=$(printf '\033[5m')
  invert=$(printf '\033[7m')
  resetbold=$(printf '\033[21m')
  resetunderline=$(printf '\033[24m')
else
  fg=""
  bg=""
  red=""
  green=""
  yellow=""
  blue=""
  magenta=""
  cyan=""
  white=""
  reset=""
  bold=""
  underline=""
  blink=""
  invert=""
  resetbold=""
  resetunderline=""
fi

[ -r /etc/openwrt_version ] && openwrt_rev=$(cat /etc/openwrt_version)
model=$(jsonfilter -e ''@.model.name'' < /etc/board.json | sed -e "s/,/_/g")

# NSS firmware version
nss_fw="/lib/firmware/qca*.bin"
[ "$(ls $nss_fw 2> /dev/null)" ] && nss_version=$(grep -h -m 1 -a -o 'Version:.[^[:cntrl:]]*' $nss_fw | head -1 | cut -d ' ' -f 2)

# ATH11K firmware version
ath11k_fw=$(grep -h -m 1 -a -o 'WLAN.[^[:cntrl:]]*SILICONZ-1' /lib/firmware/*/q6* | head -1)

# MAC80211 (backports) version
mac80211_version=$(awk '/version/{print $NF;exit}' /lib/modules/*/compat.ko)

# OpenWRT IPQ
[ -r /etc/ipq_release ] && . /etc/ipq_release
ipq_branch=${IPQ_BRANCH:-"N/A"}
ipq_commit=${IPQ_COMMIT:-"N/A"}
ipq_date=${IPQ_DATE:-"N/A"}

[ -z "$openwrt_rev" ] && openwrt_rev="N/A"
[ -z "$model" ] && model="N/A"


[ -z "$nss_version" ] && nss_version="N/A"
[ -z "$ath11k_fw" ] && ath11k_fw="N/A"
[ -z "$mac80211_version" ] && mac80211_version="N/A"

echo -e "${bold}${red}     MODEL${reset}: ${white}${model}${reset}"
echo -e "${bold}${red}   OPENWRT${reset}: ${yellow}${openwrt_rev}${reset}"
echo -e "${bold}${red}IPQ BRANCH${reset}: ${green}${ipq_branch}${reset}"
echo -e "${bold}${red}IPQ COMMIT${reset}: ${green}${ipq_commit}${reset}"
echo -e "${bold}${red}  IPQ DATE${reset}: ${green}${ipq_date}${reset}"
echo -e "${bold}${red}    NSS FW${reset}: ${cyan}${nss_version}${reset}"
echo -e "${bold}${red}  MAC80211${reset}: ${white}${mac80211_version}${reset}${reset}"
echo -e "${bold}${red} ATH11K FW${reset}: ${cyan}${ath11k_fw}${reset}"
echo -ne "${bold}${red}  GRO FRAG${reset}: ${white}"

count=0

for i in wan br-lan /sys/class/net/br-lan/brif/*; do
  i=${i##*\/}
  ethtool -k ${i} | awk -v count=$count -v i=$i -v white=$white -v green=$green -v r=$reset '
  /rx-gro-list/ {
  if(count>0) tab="            "
  color=red
  if($2=="off") color=green
  printf "%s%-11s : %s%s%s\n",tab,i,color,$2,r
  }'
  count=$((count + 1))
done

echo -e "${reset}"
echo -ne "${bold}${red}  NSS PKGS${reset}: ${white}"
opkg list-installed | awk -v count=0 '
  /kmod-qca-nss|^nss/ {
  if(count>0) tab="            "
  print tab $0
  count++
}'
echo -ne "${reset}"
