-- Copyright 2008 Freifunk Leipzig / Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.statistics.rrdtool.definitions.cpu",package.seeall)

local uci = require("luci.model.uci").cursor()
local reportbystate = uci:get("luci_statistics", "collectd_cpu", "ReportByState") or "0"

function item()
	return luci.i18n.translate("Processor")
end

function rrdargs( graph, plugin, plugin_instance, dtype )
	local p = {}

	local title = "%H: Processor usage"
	if #plugin_instance > 0 then
		title = "%H: Processor usage on core #%pi"
	end

	if reportbystate == "1" then
		local cpu = {
			title = title,
			y_min = "0",
			alt_autoscale_max = true,
			vlabel = "Jiffies",
			number_format = "%5.1lf",
			data = {
				instances = {
					cpu = {
						"idle",
						"interrupt",
						"nice",
						"softirq",
						"steal",
						"system",
						"user",
						"wait"
					}
				},
				options = {
					cpu_idle = {
						color = "ffffff",
						title = "Idle"
					},
					cpu_interrupt = {
						color = "a000a0",
						title = "Interrupt"
					},
					cpu_nice = {
						color = "00e000",
						title = "Nice"
					},
					cpu_softirq = {
						color = "ff00ff",
						title = "Softirq"
					},
					cpu_steal = {
						color = "000000",
						title = "Steal"
					},
					cpu_system = {
						color = "ff0000",
						title = "System"
					},
					cpu_user = {
						color = "0000ff",
						title = "User"
					},
					cpu_wait = {
						color = "ffb000",
						title = "Wait"
					}
				}
			}
		}

		local percent = {
			title = title,
			y_min = "0",
			alt_autoscale_max = true,
			vlabel = "Percent",
			number_format = "%5.1lf%%",
			data = {
				instances = {
					percent = {
						"idle",
						"interrupt",
						"nice",
						"softirq",
						"steal",
						"system",
						"user",
						"wait"
					}
				},
				options = {
					percent_idle = {
						color = "ffffff",
						title = "Idle"
					},
					percent_interrupt = {
						color = "a000a0",
						title = "Interrupt"
					},
					percent_nice = {
						color = "00e000",
						title = "Nice"
					},
					percent_softirq = {
						color = "ff00ff",
						title = "Softirq"
					},
					percent_steal = {
						color = "000000",
						title = "Steal"
					},
					percent_system = {
						color = "ff0000",
						title = "System"
					},
					percent_user = {
						color = "0000ff",
						title = "User"
					},
					percent_wait = {
						color = "ffb000",
						title = "Wait"
					}
				}
			}
		}

		local types = graph.tree:data_types( plugin, plugin_instance )

		for _, t in ipairs(types) do
			if t == "cpu" then
				p[#p+1] = cpu
			end

			if t == "percent" then
				p[#p+1] = percent
			end
		end
	else
		p = {
			title = title,
			y_min = "0",
			alt_autoscale_max = true,
			vlabel = "Percent",
			number_format = "%5.1lf%%",
			data = {
				instances = {
					percent = {
						"active",
					}
				},
				options = {
					percent_active = {
						color = "00e000",
						title = "Active"
					}
				}
			}
		}
	end

	return p
end
