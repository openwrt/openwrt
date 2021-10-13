#!/bin/sh

. /lib/functions.sh

config_load radiusd

user_add() {
	config_get username $1 username
	config_get password $1 password
	[ -z "$username" -o -z "$password" ] && return
	echo -e "$username\tCleartext-Password := \"$password\"" >> /etc/freeradius3-ucentral/mods-config/files/authorize
	echo -e "$username\tCleartext-Password := \"$password\"" >> /etc/freeradius3-ucentral/mods-config/files/accounting
	chmod 0600 /etc/freeradius3-ucentral/mods-config/files/authorize
	chmod 0600 /etc/freeradius3-ucentral/mods-config/files/accounting
}

rm /etc/freeradius3-ucentral/mods-config/files/authorize
rm /etc/freeradius3-ucentral/mods-config/files/accounting
config_foreach user_add user

client_add() {
	config_get name $1 name
	config_get secret $1 secret
	config_get ipaddr $1 ipaddr "*"
	config_get netmask $1 netmask 0
	
	echo "client $name {
	ipaddr = $ipaddr
	secret = $secret
	require_message_authenticator = no
	shortname = $name
	limit {
		max_connections = 16
		lifetime = 0
		idle_timeout = 30
	}
}
" >> /etc/freeradius3-ucentral/clients.conf
chmod 0600 /etc/freeradius3-ucentral/clients.conf
}

rm /etc/freeradius3-ucentral/clients.conf
config_foreach client_add client

/etc/init.d/radiusd restart
