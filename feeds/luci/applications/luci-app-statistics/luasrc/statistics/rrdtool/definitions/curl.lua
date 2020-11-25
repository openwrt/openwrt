-- Copyright 2018 Chizhong Jin <pjincz@gmail.com>
-- Licensed to the public under the BSD 3-clause license

module("luci.statistics.rrdtool.definitions.curl", package.seeall)

function item()
	return luci.i18n.translate("cUrl")
end

function rrdargs( graph, plugin, plugin_instance, dtype )

	return {
		title = "%H: cUrl Response Time for #%pi",
		y_min = "0",
		alt_autoscale_max = true,
		vlabel = "Response Time",
		number_format = "%5.1lf%Ss",
		data = {
			types = { "response_time" },
			options = {
				response_time = {
					title = ""
				}
			}
		}
	}
end
