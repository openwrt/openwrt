-- Copyright 2008 Freifunk Leipzig / Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.statistics.rrdtool.definitions.load", package.seeall)

function item()
	return luci.i18n.translate("System Load")
end

function rrdargs( graph, plugin, plugin_instance, dtype )

	return {
		title = "%H: Load", vlabel = "Load",
		y_min = "0",
		units_exponent = "0",
		number_format = "%5.2lf", data = {
			sources = {
				load = { "shortterm", "midterm", "longterm" }
			},

			options = {
				load__shortterm = {
					color = "ff0000",
					title = "1 minute",
					noarea = true,
					weight = 3
				},
				load__midterm = {
					color = "ff6600",
					title = "5 minutes",
					overlay = true,
					weight = 1
				},
				load__longterm = {
					color = "ffaa00",
					title = "15 minutes",
					overlay = true,
					weight = 2
				}
			}
		}
	}
end
