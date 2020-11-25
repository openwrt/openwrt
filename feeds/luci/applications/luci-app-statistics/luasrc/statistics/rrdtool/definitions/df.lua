-- Copyright 2008 Freifunk Leipzig / Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.statistics.rrdtool.definitions.df", package.seeall)

function item()
	return luci.i18n.translate("Disk Space Usage")
end

function rrdargs( graph, plugin, plugin_instance, dtype )

	local df_complex = {
		title = "%H: Disk space usage on %pi",
		vlabel = "Bytes",
		number_format = "%5.1lf%sB",

		data = {
			instances = {
				df_complex = { "free", "used", "reserved" }
			},

			options = {
				df_complex_free = {
					color = "00ff00",
					overlay = false,
					title = "free"
				},

				df_complex_used = {
					color = "ff0000",
					overlay = false,
					title = "used"
				},

				df_complex_reserved = {
					color = "0000ff",
					overlay = false,
					title = "reserved"
				}
			}
		}
	}

	local percent_bytes = {
		title = "%H: Disk space usage on %pi",
		vlabel = "Percent",
		number_format = "%5.2lf %%",

		data = {
			instances = {
				percent_bytes = { "free", "used", "reserved" }
			},

			options = {
				percent_bytes_free = {
					color = "00ff00",
					overlay = false,
					title = "free"
				},

				percent_bytes_used = {
					color = "ff0000",
					overlay = false,
					title = "used"
				},

				percent_bytes_reserved = {
					color = "0000ff",
					overlay = false,
					title = "reserved"
				}
			}
		}
	}

	local types = graph.tree:data_types( plugin, plugin_instance )

	local p = {}
	for _, t in ipairs(types) do
		if t == "percent_bytes" then
			p[#p+1] = percent_bytes
		end

		if t == "df_complex" then
			p[#p+1] = df_complex
		end
	end

	return p
end
