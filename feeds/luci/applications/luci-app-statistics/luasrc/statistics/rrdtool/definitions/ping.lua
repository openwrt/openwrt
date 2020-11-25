-- Copyright 2008 Freifunk Leipzig / Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.statistics.rrdtool.definitions.ping", package.seeall)

function item()
	return luci.i18n.translate("Ping")
end

function rrdargs( graph, plugin, plugin_instance, dtype )

	local ping = {
		title = "%H: ICMP Round Trip Time",
		vlabel = "ms",
		number_format = "%5.1lf ms",
		data = {
			sources = {
				ping = {
					"value"
				}
			},
			options = {
				ping__value = {
					noarea = true,
					overlay = true,
					title = "%di"
				}
			}
		}
	}

	local droprate = {
		title = "%H: ICMP Drop Rate",
		vlabel = "%",
		number_format = "%5.2lf %%",
		data = {
			types   = {
				"ping_droprate"
			},
			options = {
				ping_droprate = {
					noarea = true,
					overlay = true,
					title = "%di",
					transform_rpn = "100,*"
				}
			}
		}
	}

	local stddev = {
		title = "%H: ICMP Standard Deviation",
		vlabel = "ms",
		number_format = "%5.1lf ms",
		data = {
			types   = {
				"ping_stddev"
			},
			options = {
				ping_stddev = {
					noarea = true,
					overlay = true,
					title = "%di"
				}
			}
		}
	}

	return { ping, droprate, stddev }
end
