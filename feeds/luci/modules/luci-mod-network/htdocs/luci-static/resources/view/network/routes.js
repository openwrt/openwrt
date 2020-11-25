'use strict';
'require form';
'require network';
'require tools.widgets as widgets';

return L.view.extend({
	load: function() {
		return network.getDevices();
	},

	render: function(netdevs) {
		var m, s, o;

		m = new form.Map('network', _('Routes'), _('Routes specify over which interface and gateway a certain host or network can be reached.'));
		m.tabbed = true;

		for (var i = 4; i <= 6; i += 2) {
			s = m.section(form.GridSection, (i == 4) ? 'route' : 'route6', (i == 4) ? _('Static IPv4 Routes') : _('Static IPv6 Routes'));
			s.anonymous = true;
			s.addremove = true;
			s.sortable = true;

			s.tab('general', _('General Settings'));
			s.tab('advanced', _('Advanced Settings'));

			o = s.taboption('general', widgets.NetworkSelect, 'interface', _('Interface'));
			o.rmempty = false;
			o.nocreate = true;

			o = s.taboption('general', form.Value, 'target', _('Target'), (i == 4) ? _('Host-<abbr title="Internet Protocol Address">IP</abbr> or Network') : _('<abbr title="Internet Protocol Version 6">IPv6</abbr>-Address or Network (CIDR)'));
			o.datatype = (i == 4) ? 'ip4addr' : 'ip6addr';
			o.rmempty = false;

			if (i == 4) {
				o = s.taboption('general', form.Value, 'netmask', _('<abbr title="Internet Protocol Version 4">IPv4</abbr>-Netmask'), _('if target is a network'));
				o.placeholder = '255.255.255.255';
				o.datatype = 'ip4addr';
				o.rmempty = true;
			}

			o = s.taboption('general', form.Value, 'gateway', (i == 4) ? _('<abbr title="Internet Protocol Version 4">IPv4</abbr>-Gateway') : _('<abbr title="Internet Protocol Version 6">IPv6</abbr>-Gateway'));
			o.datatype = (i == 4) ? 'ip4addr' : 'ip6addr';
			o.rmempty = true;

			o = s.taboption('advanced', form.Value, 'metric', _('Metric'));
			o.placeholder = 0;
			o.datatype = (i == 4) ? 'range(0,255)' : 'range(0,65535)';
			o.rmempty = true;
			o.textvalue = function(section_id) {
				return this.cfgvalue(section_id) || 0;
			};

			o = s.taboption('advanced', form.Value, 'mtu', _('MTU'));
			o.placeholder = 1500;
			o.datatype = 'range(64,9000)';
			o.rmempty = true;
			o.modalonly = true;

			o = s.taboption('advanced', form.ListValue, 'type', _('Route type'));
			o.value('', 'unicast');
			o.value('local');
			o.value('broadcast');
			o.value('multicast');
			o.value('unreachable');
			o.value('prohibit');
			o.value('blackhole');
			o.value('anycast');
			o.default = '';
			o.rmempty = true;
			o.modalonly = true;

			o = s.taboption('advanced', form.Value, 'table', _('Route table'));
			o.value('local', 'local (255)');
			o.value('main', 'main (254)');
			o.value('default', 'default (253)');
			o.rmempty = true;
			o.modalonly = true;
			o.cfgvalue = function(section_id) {
				var cfgvalue = this.super('cfgvalue', [section_id]);
				return cfgvalue || 'main';
			};

			o = s.taboption('advanced', form.Value, 'source', _('Source Address'));
			o.placeholder = E('em', _('automatic'));
			for (var j = 0; j < netdevs.length; j++) {
				var addrs = netdevs[j].getIPAddrs();
				for (var k = 0; k < addrs.length; k++)
					o.value(addrs[k].split('/')[0]);
			}
			o.datatype = (i == 4) ? 'ip4addr' : 'ip6addr';
			o.default = '';
			o.rmempty = true;
			o.modalonly = true;

			o = s.taboption('advanced', form.Flag, 'onlink', _('On-Link route'));
			o.default = o.disabled;
			o.rmempty = true;
		}

		return m.render();
	}
});
