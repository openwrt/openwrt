'use strict';
'require form';
'require network';
'require validation';

function isCIDR(value) {
	return Array.isArray(value) || /^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\/(\d{1,2}|\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})$/.test(value);
}

function calculateBroadcast(s, use_cfgvalue) {
	var readfn = use_cfgvalue ? 'cfgvalue' : 'formvalue',
	    addropt = s.children.filter(function(o) { return o.option == 'ipaddr'})[0],
	    addrvals = addropt ? L.toArray(addropt[readfn](s.section)) : [],
	    maskopt = s.children.filter(function(o) { return o.option == 'netmask'})[0],
	    maskval = maskopt ? maskopt[readfn](s.section) : null,
	    firstsubnet = maskval ? addrvals[0] + '/' + maskval : addrvals.filter(function(a) { return a.indexOf('/') > 0 })[0];

	if (firstsubnet == null)
		return null;

	var addr_mask = firstsubnet.split('/'),
	    addr = validation.parseIPv4(addr_mask[0]),
	    mask = addr_mask[1];

	if (!isNaN(mask))
		mask = validation.parseIPv4(network.prefixToMask(+mask));
	else
		mask = validation.parseIPv4(mask);

	var bc = [
		addr[0] | (~mask[0] >>> 0 & 255),
		addr[1] | (~mask[1] >>> 0 & 255),
		addr[2] | (~mask[2] >>> 0 & 255),
		addr[3] | (~mask[3] >>> 0 & 255)
	];

	return bc.join('.');
}

function validateBroadcast(section_id, value) {
	var opt = this.map.lookupOption('broadcast', section_id),
	    node = opt ? this.map.findElement('id', opt[0].cbid(section_id)) : null,
	    addr = node ? calculateBroadcast(this.section, false) : null;

	if (node != null) {
		if (addr != null)
			node.querySelector('input').setAttribute('placeholder', addr);
		else
			node.querySelector('input').removeAttribute('placeholder');
	}

	return true;
}

return network.registerProtocol('static', {
	CBIIPValue: form.Value.extend({
		handleSwitch: function(section_id, option_index, ev) {
			var maskopt = this.map.lookupOption('netmask', section_id);

			if (maskopt == null || !this.isValid(section_id))
				return;

			var maskval = maskopt[0].formvalue(section_id),
			    addrval = this.formvalue(section_id),
			    prefix = maskval ? network.maskToPrefix(maskval) : 32;

			if (prefix == null)
				return;

			this.datatype = 'or(cidr4,ipmask4)';

			var parent = L.dom.parent(ev.target, '.cbi-value-field');
			L.dom.content(parent, form.DynamicList.prototype.renderWidget.apply(this, [
				section_id,
				option_index,
				addrval ? '%s/%d'.format(addrval, prefix) : ''
			]));

			var masknode = this.map.findElement('id', maskopt[0].cbid(section_id));
			if (masknode) {
				parent = L.dom.parent(masknode, '.cbi-value');
				parent.parentNode.removeChild(parent);
			}
		},

		renderWidget: function(section_id, option_index, cfgvalue) {
			var maskopt = this.map.lookupOption('netmask', section_id),
			    widget = isCIDR(cfgvalue) ? 'DynamicList' : 'Value';

			if (widget == 'DynamicList') {
				this.datatype = 'or(cidr4,ipmask4)';
				this.placeholder = _('Add IPv4 address…');
			}
			else {
				this.datatype = 'ip4addr("nomask")';
			}

			var node = form[widget].prototype.renderWidget.apply(this, [ section_id, option_index, cfgvalue ]);

			if (widget == 'Value')
				L.dom.append(node, E('button', {
					'class': 'cbi-button cbi-button-neutral',
					'title': _('Switch to CIDR list notation'),
					'aria-label': _('Switch to CIDR list notation'),
					'click': L.bind(this.handleSwitch, this, section_id, option_index)
				}, '…'));

			return node;
		},

		validate: validateBroadcast
	}),

	CBINetmaskValue: form.Value.extend({
		render: function(option_index, section_id, in_table) {
			var addropt = this.section.children.filter(function(o) { return o.option == 'ipaddr' })[0],
			    addrval = addropt ? addropt.cfgvalue(section_id) : null;

			if (addrval != null && isCIDR(addrval))
				return E([]);

			this.value('255.255.255.0');
			this.value('255.255.0.0');
			this.value('255.0.0.0');

			return form.Value.prototype.render.apply(this, [ option_index, section_id, in_table ]);
		},

		validate: validateBroadcast
	}),

	CBIGatewayValue: form.Value.extend({
		datatype: 'ip4addr("nomask")',

		render: function(option_index, section_id, in_table) {
			return network.getWANNetworks().then(L.bind(function(wans) {
				if (wans.length == 1) {
					var gwaddr = wans[0].getGatewayAddr();
					this.placeholder = gwaddr ? '%s (%s)'.format(gwaddr, wans[0].getName()) : '';
				}

				return form.Value.prototype.render.apply(this, [ option_index, section_id, in_table ]);
			}, this));
		},

		validate: function(section_id, value) {
			var addropt = this.section.children.filter(function(o) { return o.option == 'ipaddr' })[0],
			    addrval = addropt ? L.toArray(addropt.cfgvalue(section_id)) : null;

			if (addrval != null) {
				for (var i = 0; i < addrval.length; i++) {
					var addr = addrval[i].split('/')[0];
					if (value == addr)
						return _('The gateway address must not be a local IP address');
				}
			}

			return true;
		}
	}),

	CBIBroadcastValue: form.Value.extend({
		datatype: 'ip4addr("nomask")',

		render: function(option_index, section_id, in_table) {
			this.placeholder = calculateBroadcast(this.section, true);
			return form.Value.prototype.render.apply(this, [ option_index, section_id, in_table ]);
		}
	}),

	getI18n: function() {
		return _('Static address');
	},

	renderFormOptions: function(s) {
		var dev = this.getL2Device() || this.getDevice(), o;

		s.taboption('general', this.CBIIPValue, 'ipaddr', _('IPv4 address'));
		s.taboption('general', this.CBINetmaskValue, 'netmask', _('IPv4 netmask'));
		s.taboption('general', this.CBIGatewayValue, 'gateway', _('IPv4 gateway'));
		s.taboption('general', this.CBIBroadcastValue, 'broadcast', _('IPv4 broadcast'));
		s.taboption('general', form.DynamicList, 'dns', _('Use custom DNS servers'));

		o = s.taboption('general', form.Value, 'ip6assign', _('IPv6 assignment length'), _('Assign a part of given length of every public IPv6-prefix to this interface'));
		o.value('', _('disabled'));
		o.value('64');
		o.datatype = 'max(64)';

		o = s.taboption('general', form.Value, 'ip6hint', _('IPv6 assignment hint'), _('Assign prefix parts using this hexadecimal subprefix ID for this interface.'));
		o.placeholder = '0';
		o.validate = function(section_id, value) {
			if (value == null || value == '')
				return true;

			var n = parseInt(value, 16);

			if (!/^(0x)?[0-9a-fA-F]+$/.test(value) || isNaN(n) || n >= 0xffffffff)
				return _('Expecting an hexadecimal assignment hint');

			return true;
		};
		for (var i = 33; i <= 64; i++)
			o.depends('ip6assign', String(i));

		o = s.taboption('general', form.DynamicList, 'ip6addr', _('IPv6 address'));
		o.datatype = 'ip6addr';
		o.placeholder = _('Add IPv6 address…');
		o.depends('ip6assign', '');

		o = s.taboption('general', form.Value, 'ip6gw', _('IPv6 gateway'));
		o.datatype = 'ip6addr("nomask")';
		o.depends('ip6assign', '');

		o = s.taboption('general', form.Value, 'ip6prefix', _('IPv6 routed prefix'), _('Public prefix routed to this device for distribution to clients.'));
		o.datatype = 'ip6addr';
		o.depends('ip6assign', '');

		o = s.taboption('general', form.Value, 'ip6ifaceid', _('IPv6 suffix'), _("Optional. Allowed values: 'eui64', 'random', fixed value like '::1' or '::1:2'. When IPv6 prefix (like 'a:b:c:d::') is received from a delegating server, use the suffix (like '::1') to form the IPv6 address ('a:b:c:d::1') for the interface."));
		o.datatype = 'ip6hostid';
		o.placeholder = '::1';

		o = s.taboption('advanced', form.Value, 'macaddr', _('Override MAC address'));
		o.datatype = 'macaddr';
		o.placeholder = dev ? (dev.getMAC() || '') : '';

		o = s.taboption('advanced', form.Value, 'mtu', _('Override MTU'));
		o.datatype = 'max(9200)';
		o.placeholder = dev ? (dev.getMTU() || '1500') : '1500';

		o = s.taboption('advanced', form.Value, 'metric', _('Use gateway metric'));
		o.placeholder = this.getMetric() || '0';
		o.datatype = 'uinteger';
	}
});
