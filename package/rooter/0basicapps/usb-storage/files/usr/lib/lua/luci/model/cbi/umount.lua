require("nixio.fs")

m = Map("umount", "Safely Eject a Drive",
	translate("Safely eject a drive from the router"))

m.on_after_save = function(self)
	luci.sys.call("/etc/umount &")
end

drv = m:section(TypedSection, "umount", translate("Currently Mounted Drives"))
drv.anonymous = true

disk = drv:option(Value, "drive", translate(" "), translate("Click Save and Apply to eject drive"))
disk.rmempty = true
for dev in nixio.fs.glob("/mnt/[sh]d[a-z][1-9]") do
	dv = nixio.fs.basename(dev)
	disk:value(nixio.fs.basename(dev))
end

return m