-- Copyright  2011 Manuel Munz <freifunk at somakoma dot de>
-- Licensed to the public under the Apache License 2.0.

module("luci.statistics.rrdtool.definitions.memory",package.seeall)

function item()
	return luci.i18n.translate("Memory")
end

function rrdargs( graph, plugin, plugin_instance, dtype )
	local p = {}

	local memory = {
		title = "%H: Memory usage",
		vlabel = "MB",
		number_format = "%5.1lf%s",
		y_min = "0",
		alt_autoscale_max = true,
		data = {
			instances = {
				memory = {
					"free",
					"buffered",
					"cached",
					"used"
				}
			},

			options = {
				memory_buffered = {
					color = "0000ff",
					title = "Buffered"
				},
				memory_cached = {
					color = "ff00ff",
					title = "Cached"
				},
				memory_used = {
					color = "ff0000",
					title = "Used"
				},
				memory_free = {
					color = "00ff00",
					title = "Free"
				}
			}
		}
	}

	local percent = {
		title = "%H: Memory usage",
		vlabel = "Percent",
		number_format = "%5.1lf%%",
		y_min = "0",
		alt_autoscale_max = true,
		data = {
			instances = {
				percent = {
					"free",
					"buffered",
					"cached",
					"used"
				}
			},
			options = {
				percent_buffered = {
					color = "0000ff",
					title = "Buffered"
				},
				percent_cached = {
					color = "ff00ff",
					title = "Cached"
				},
				percent_used = {
					color = "ff0000",
					title = "Used"
				},
				percent_free = {
					color = "00ff00",
					title = "Free"
				}
			}
		}
	}

	local types = graph.tree:data_types( plugin, plugin_instance )

	for _, t in ipairs(types) do
		if t == "percent" then
			p[#p+1] = percent
		end

		if t == "memory" then
			p[#p+1] = memory
		end
	end

	return p
end
