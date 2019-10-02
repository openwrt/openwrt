#!/bin/bash
status=$(ps|grep -c /usr/share/openclash/yml_proxys_get.sh)
[ "$status" -gt "3" ] && exit 0

START_LOG="/tmp/openclash_start.log"

if [ ! -f "/etc/openclash/config.yml" ] && [ ! -f "/etc/openclash/config.yaml" ]; then
  exit 0
elif [ ! -f "/etc/openclash/config.yaml" ] && [ "$(ls -l /etc/openclash/config.yml 2>/dev/null |awk '{print int($5/1024)}')" -gt 0 ]; then
   mv "/etc/openclash/config.yml" "/etc/openclash/config.yaml"
fi

echo "开始更新服务器节点配置..." >$START_LOG

awk '/^ {0,}Proxy:/,/^ {0,}Proxy Group:/{print}' /etc/openclash/config.yaml 2>/dev/null |sed 's/\"//g' 2>/dev/null |sed "s/\'//g" 2>/dev/null |sed 's/\t/ /g' 2>/dev/null >/tmp/yaml_proxy.yaml 2>&1

server_file="/tmp/yaml_proxy.yaml"
single_server="/tmp/servers.yaml"
group_num=$(grep -c "name:" /tmp/yaml_group.yaml)
count=1
line=$(sed -n '/^ \{0,\}-/=' $server_file)
num=$(grep -c "^ \{0,\}-" $server_file)

cfg_get()
{
	echo "$(grep "$1" $single_server 2>/dev/null |awk -v tag=$1 'BEGIN{FS=tag} {print $2}' 2>/dev/null |sed 's/,.*//' 2>/dev/null |sed 's/^ \{0,\}//g' 2>/dev/null |sed 's/ \{0,\}$//g' 2>/dev/null |sed 's/ \{0,\}\}\{0,\}$//g' 2>/dev/null)"
}

for n in $line
do

   [ "$count" -eq 1 ] && {
      startLine="$n"
  }

   count=$(expr "$count" + 1)
   if [ "$count" -gt "$num" ]; then
      endLine=$(sed -n '$=' $server_file)
   else
      endLine=$(expr $(echo "$line" | sed -n "${count}p") - 1)
   fi
  
   sed -n "${startLine},${endLine}p" $server_file >$single_server
   startLine=$(expr "$endLine" + 1)
   
   #type
   server_type="$(cfg_get "type:")"
   #name
   server_name="$(cfg_get "name:")"
   #server
   server="$(cfg_get "server:")"
   #port
   port="$(cfg_get "port:")"
   #cipher
   cipher="$(cfg_get "cipher:")"
   #password
   password="$(cfg_get "password:")"
   #udp
   udp="$(cfg_get "udp:")"
   #plugin:
   plugin="$(cfg_get "plugin:")"
   #plugin-opts:
   plugin_opts="$(cfg_get "plugin-opts:")"
   #obfs:
   obfs="$(cfg_get "obfs:")"
   #obfs-host:
   obfs_host="$(cfg_get "obfs-host:")"
   #mode:
   mode="$(cfg_get "mode:")"
   #tls:
   tls="$(cfg_get "tls:")"
   #skip-cert-verify:
   verify="$(cfg_get "skip-cert-verify:")"
   #mux:
   mux="$(cfg_get "mux:")"
   #host:
   host="$(cfg_get "host:")"
   #Host:
   Host="$(cfg_get "Host:")"
   #path:
   path="$(cfg_get "path:")"
   #ws-path:
   ws_path="$(cfg_get "ws-path:")"
   #headers_custom:
   headers="$(cfg_get "custom:")"
   #uuid:
   uuid="$(cfg_get "uuid:")"
   #alterId:
   alterId="$(cfg_get "alterId:")"
   #network
   network="$(cfg_get "network:")"
   #username
   username="$(cfg_get "username:")"
   
   echo "正在读取【$server_type】-【$server_name】服务器节点配置..." >$START_LOG
   
   name=openclash
   uci_name_tmp=$(uci add $name servers)

   uci_set="uci -q set $name.$uci_name_tmp."
   uci_add="uci -q add_list $name.$uci_name_tmp."
    
   ${uci_set}name="$server_name"
   ${uci_set}type="$server_type"
   ${uci_set}server="$server"
   ${uci_set}port="$port"
   if [ "$server_type" = "vmess" ]; then
      ${uci_set}securitys="$cipher"
   else
      ${uci_set}cipher="$cipher"
   fi
   ${uci_set}udp="$udp"
   ${uci_set}obfs="$obfs"
   ${uci_set}host="$obfs_host"
   [ -z "$obfs" ] && ${uci_set}obfs="$mode"
   [ -z "$mode" ] && [ ! -z "$network" ] && ${uci_set}obfs_vmess="websocket"
   [ -z "$mode" ] && [ -z "$network" ] && ${uci_set}obfs_vmess="none"
   [ -z "$obfs_host" ] && ${uci_set}host="$host"
   ${uci_set}tls="$tls"
   ${uci_set}skip_cert_verify="$verify"
   ${uci_set}path="$path"
   [ -z "$path" ] && ${uci_set}path="$ws_path"
   ${uci_set}mux="$mux"
   ${uci_set}custom="$headers"
   [ -z "$headers" ] && ${uci_set}custom="$Host"
    
	if [ "$server_type" = "vmess" ]; then
    #v2ray
    ${uci_set}alterId="$alterId"
    ${uci_set}uuid="$uuid"
	fi
	
	if [ "$server_type" = "socks5" ] || [ "$server_type" = "http" ]; then
     ${uci_set}auth_name="$username"
     ${uci_set}auth_pass="$password"
  else
     ${uci_set}password="$password"
	fi
	
	for ((i=1;i<=$group_num;i++)) #循环加入策略组
	do
	   single_group="/tmp/group_$i.yaml"
     if [ ! -z "$(grep -F "$server_name" "$single_group")" ]; then
        group_name=$(grep "name:" $single_group 2>/dev/null |awk -F 'name:' '{print $2}' 2>/dev/null |sed 's/,.*//' 2>/dev/null |sed 's/^ \{0,\}//g' 2>/dev/null |sed 's/ \{0,\}$//g' 2>/dev/null)
        ${uci_add}groups="$group_name"
     fi
	done
done
echo "配置文件读取完成！" >$START_LOG
sleep 3
echo "" >$START_LOG
uci commit openclash
rm -rf /tmp/servers.yaml 2>/dev/null
rm -rf /tmp/yaml_proxy.yaml 2>/dev/null
rm -rf /tmp/group_*.yaml 2>/dev/null
rm -rf /tmp/yaml_group.yaml 2>/dev/null