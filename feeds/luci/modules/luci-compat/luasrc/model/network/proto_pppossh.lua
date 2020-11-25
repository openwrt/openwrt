-- Copyright 2018 Florian Eckert <fe@dev.tdt.de>
-- Licensed to the public under the Apache License 2.0.

local netmod = luci.model.network

local proto = netmod:register_protocol("pppossh")

function proto.get_i18n(self)
	return luci.i18n.translate("PPPoSSH")
end

function proto.ifname(self)
	return "pppossh-" .. self.sid
end

function proto.opkg_package(self)
	return "pppossh"
end

function proto.is_installed(self)
	return nixio.fs.access("/lib/netifd/proto/pppossh.sh")
end

function proto.is_floating(self)
	return true
end

function proto.is_virtual(self)
	return true
end

function proto.get_interfaces(self)
	return nil
end

function proto.contains_interface(self, ifc)
	return (netmod:ifnameof(ifc) == self:ifname())
end

netmod:register_pattern_virtual("^pppossh%-%w")
