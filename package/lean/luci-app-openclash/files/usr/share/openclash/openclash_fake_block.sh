#! /bin/sh
echo "设置Fake-IP黑名单..." >$START_LOG
direct_dns=$(uci get openclash.config.direct_dns 2>/dev/null)
[ -z "$direct_dns" ] && {
	direct_dns="114.114.114.114"
	}
rm -rf /etc/openclash/dnsmasq_fake_block.conf 2>/dev/null
for i in `cat /etc/config/openclash_custom_fake_black.conf`
do
   if [ -z "$(echo $i |grep '^ \{0,\}#' 2>/dev/null)" ]; then
      echo "server=/$i/$direct_dns" >>/etc/openclash/dnsmasq_fake_block.conf
	 fi
done
