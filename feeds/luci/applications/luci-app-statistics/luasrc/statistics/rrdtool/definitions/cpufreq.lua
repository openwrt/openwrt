-- Licensed to the public under the Apache License 2.0.

module("luci.statistics.rrdtool.definitions.cpufreq",package.seeall)

local uci = require("luci.model.uci").cursor()
local extraitems = uci:get("luci_statistics", "collectd_cpufreq", "ExtraItems") or nil

function item()
	return luci.i18n.translate("CPU Frequency")
end

function rrdargs( graph, plugin, plugin_instance, dtype )

	local cpufreq =  {
		title = "%H: Processor frequency - core %pi",
		alt_autoscale = true,
		vlabel = "Frequency (Hz)",
		number_format = "%3.2lf%s",
		data = {
			types = {"cpufreq" },
			options = {
				cpufreq = { color = "ff0000", title = "Frequency" },
			}
		}
	}

    if extraitems then

	local transitions = {
		title = "%H: Frequency transitions - core %pi",
		alt_autoscale = true,
		vlabel = "Transitions",
		number_format = "%3.2lf%s",
		data = {
			types = { "transitions" },
			options = {
				transitions = { color = "0000ff", title = "Transitions", noarea=true },
			}
		}
	}

	local percentage = {
		title = "%H: Frequency distribution - core %pi",
		alt_autoscale = true,
		vlabel = "Percent",
		number_format = "%5.2lf%%",
		ordercolor = true,
		data = {
			types = { "percent" },
			options = {
				percent = { title = "%di kHz", negweight = true },
			}
		}
	}

	return { cpufreq, percentage, transitions }
    else
	return { cpufreq }
    end
end

