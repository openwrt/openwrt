--[[
LuCI model for net-snmp configuration management
Copyright Karl Palsson <karlp@etactica.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

]]--

local datatypes = require("luci.cbi.datatypes")

m = Map("snmpd", "net-snmp's SNMPD",
    [[SNMPD is a master daemon/agent for SNMP, from the <a href='http://www.net-snmp.org'>
 net-snmp project</a>.
 Note, OpenWrt has mostly complete UCI support for snmpd, but this LuCI applet
 only covers a few of those options. In particular, there is very little/no validation
 or help.
See /etc/config/snmpd for manual configuration.
 ]])
 
s = m:section(TypedSection, "agent", "Agent settings")
s.anonymous = true
p = s:option(Value, "agentaddress", "The address the agent should listen on",
	[[Eg: UDP:161, or UDP:10.5.4.3:161 to only listen on a given interface]])

s = m:section(TypedSection, "agentx", "AgentX settings", "Delete this section to disable agentx")
s.anonymous = true
p = s:option(Value, "agentxsocket", "The address the agent should allow agentX connections to",
    [[This is only necessary if you have subagents using the agentX socket protocol.
    Eg: /var/run/agentx.sock]])
s.addremove=true

s = m:section(TypedSection, "com2sec", "com2sec security")
p = s:option(Value, "secname", "secname")
p = s:option(Value, "source", "source")
p = s:option(Value, "community", "community")

s = m:section(TypedSection, "group", "group", "Groups help define access methods")
s.addremove=true
s:option(Value, "group", "group")
s:option(Value, "version", "version")
s:option(Value, "secname", "secname")

s = m:section(TypedSection, "access", "access")
s:option(Value, "group", "group")
s:option(Value, "context", "context")
s:option(Value, "version", "version")
s:option(Value, "level", "level")
s:option(Value, "prefix", "prefix")
s:option(Value, "read", "read")
s:option(Value, "write", "write")
s:option(Value, "notify", "notify")

s = m:section(TypedSection, "system", "System", "Values used in the MIB2 System tree")
s.anonymous = true
s:option(Value, "sysLocation", "sysLocation")
s:option(Value, "sysContact", "sysContact")
s:option(Value, "sysName", "sysName")

return m
