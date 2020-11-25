'use strict';
'require ui';
'require form';
'require network';
'require firewall';
'require fs';

function getUsers() {
    return fs.lines('/etc/passwd').then(function(lines) {
        return lines.map(function(line) { return line.split(/:/)[0] });
    });
}

function getGroups() {
    return fs.lines('/etc/group').then(function(lines) {
        return lines.map(function(line) { return line.split(/:/)[0] });
    });
}

var CBIZoneSelect = form.ListValue.extend({
	__name__: 'CBI.ZoneSelect',

	load: function(section_id) {
		return Promise.all([ firewall.getZones(), network.getNetworks() ]).then(L.bind(function(zn) {
			this.zones = zn[0];
			this.networks = zn[1];

			return this.super('load', section_id);
		}, this));
	},

	filter: function(section_id, value) {
		return true;
	},

	lookupZone: function(name) {
		return this.zones.filter(function(zone) { return zone.getName() == name })[0];
	},

	lookupNetwork: function(name) {
		return this.networks.filter(function(network) { return network.getName() == name })[0];
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var values = L.toArray((cfgvalue != null) ? cfgvalue : this.default),
		    isOutputOnly = false,
		    choices = {};

		if (this.option == 'dest') {
			for (var i = 0; i < this.section.children.length; i++) {
				var opt = this.section.children[i];
				if (opt.option == 'src') {
					var val = opt.cfgvalue(section_id) || opt.default;
					isOutputOnly = (val == null || val == '');
					break;
				}
			}

			this.title = isOutputOnly ? _('Output zone') :  _('Destination zone');
		}

		if (this.allowlocal) {
			choices[''] = E('span', {
				'class': 'zonebadge',
				'style': 'background-color:' + firewall.getColorForName(null)
			}, [
				E('strong', _('Device')),
				(this.allowany || this.allowlocal)
					? ' (%s)'.format(this.option != 'dest' ? _('output') : _('input')) : ''
			]);
		}
		else if (!this.multiple && (this.rmempty || this.optional)) {
			choices[''] = E('span', {
				'class': 'zonebadge',
				'style': 'background-color:' + firewall.getColorForName(null)
			}, E('em', _('unspecified')));
		}

		if (this.allowany) {
			choices['*'] = E('span', {
				'class': 'zonebadge',
				'style': 'background-color:' + firewall.getColorForName(null)
			}, [
				E('strong', _('Any zone')),
				(this.allowany && this.allowlocal && !isOutputOnly) ? ' (%s)'.format(_('forward')) : ''
			]);
		}

		for (var i = 0; i < this.zones.length; i++) {
			var zone = this.zones[i],
			    name = zone.getName(),
			    networks = zone.getNetworks(),
			    ifaces = [];

			if (!this.filter(section_id, name))
				continue;

			for (var j = 0; j < networks.length; j++) {
				var network = this.lookupNetwork(networks[j]);

				if (!network)
					continue;

				var span = E('span', {
					'class': 'ifacebadge' + (network.getName() == this.network ? ' ifacebadge-active' : '')
				}, network.getName() + ': ');

				var devices = network.isBridge() ? network.getDevices() : L.toArray(network.getDevice());

				for (var k = 0; k < devices.length; k++) {
					span.appendChild(E('img', {
						'title': devices[k].getI18n(),
						'src': L.resource('icons/%s%s.png'.format(devices[k].getType(), devices[k].isUp() ? '' : '_disabled'))
					}));
				}

				if (!devices.length)
					span.appendChild(E('em', _('(empty)')));

				ifaces.push(span);
			}

			if (!ifaces.length)
				ifaces.push(E('em', _('(empty)')));

			choices[name] = E('span', {
				'class': 'zonebadge',
				'style': 'background-color:' + zone.getColor()
			}, [ E('strong', name) ].concat(ifaces));
		}

		var widget = new ui.Dropdown(values, choices, {
			id: this.cbid(section_id),
			sort: true,
			multiple: this.multiple,
			optional: this.optional || this.rmempty,
			select_placeholder: E('em', _('unspecified')),
			display_items: this.display_size || this.size || 3,
			dropdown_items: this.dropdown_size || this.size || 5,
			validate: L.bind(this.validate, this, section_id),
			create: !this.nocreate,
			create_markup: '' +
				'<li data-value="{{value}}">' +
					'<span class="zonebadge" style="background:repeating-linear-gradient(45deg,rgba(204,204,204,0.5),rgba(204,204,204,0.5) 5px,rgba(255,255,255,0.5) 5px,rgba(255,255,255,0.5) 10px)">' +
						'<strong>{{value}}:</strong> <em>('+_('create')+')</em>' +
					'</span>' +
				'</li>'
		});

		var elem = widget.render();

		if (this.option == 'src') {
			elem.addEventListener('cbi-dropdown-change', L.bind(function(ev) {
				var opt = this.map.lookupOption('dest', section_id),
				    val = ev.detail.instance.getValue();

				if (opt == null)
					return;

				var cbid = opt[0].cbid(section_id),
				    label = document.querySelector('label[for="widget.%s"]'.format(cbid)),
				    node = document.getElementById(cbid);

				L.dom.content(label, val == '' ? _('Output zone') : _('Destination zone'));

				if (val == '') {
					if (L.dom.callClassMethod(node, 'getValue') == '')
						L.dom.callClassMethod(node, 'setValue', '*');

					var emptyval = node.querySelector('[data-value=""]'),
					    anyval = node.querySelector('[data-value="*"]');

					L.dom.content(anyval.querySelector('span'), E('strong', _('Any zone')));

					if (emptyval != null)
						emptyval.parentNode.removeChild(emptyval);
				}
				else {
					var anyval = node.querySelector('[data-value="*"]'),
					    emptyval = node.querySelector('[data-value=""]');

					if (emptyval == null) {
						emptyval = anyval.cloneNode(true);
						emptyval.removeAttribute('display');
						emptyval.removeAttribute('selected');
						emptyval.setAttribute('data-value', '');
					}

					L.dom.content(emptyval.querySelector('span'), [
						E('strong', _('Device')), ' (%s)'.format(_('input'))
					]);

					L.dom.content(anyval.querySelector('span'), [
						E('strong', _('Any zone')), ' (%s)'.format(_('forward'))
					]);

					anyval.parentNode.insertBefore(emptyval, anyval);
				}

			}, this));
		}
		else if (isOutputOnly) {
			var emptyval = elem.querySelector('[data-value=""]');
			emptyval.parentNode.removeChild(emptyval);
		}

		return elem;
	},
});

var CBIZoneForwards = form.DummyValue.extend({
	__name__: 'CBI.ZoneForwards',

	load: function(section_id) {
		return Promise.all([
			firewall.getDefaults(),
			firewall.getZones(),
			network.getNetworks(),
			network.getDevices()
		]).then(L.bind(function(dznd) {
			this.defaults = dznd[0];
			this.zones = dznd[1];
			this.networks = dznd[2];
			this.devices = dznd[3];

			return this.super('load', section_id);
		}, this));
	},

	renderZone: function(zone) {
		var name = zone.getName(),
		    networks = zone.getNetworks(),
		    devices = zone.getDevices(),
		    subnets = zone.getSubnets(),
		    ifaces = [];

		for (var j = 0; j < networks.length; j++) {
			var network = this.networks.filter(function(net) { return net.getName() == networks[j] })[0];

			if (!network)
				continue;

			var span = E('span', {
				'class': 'ifacebadge' + (network.getName() == this.network ? ' ifacebadge-active' : '')
			}, network.getName() + ': ');

			var subdevs = network.isBridge() ? network.getDevices() : L.toArray(network.getDevice());

			for (var k = 0; k < subdevs.length && subdevs[k]; k++) {
				span.appendChild(E('img', {
					'title': subdevs[k].getI18n(),
					'src': L.resource('icons/%s%s.png'.format(subdevs[k].getType(), subdevs[k].isUp() ? '' : '_disabled'))
				}));
			}

			if (!subdevs.length)
				span.appendChild(E('em', _('(empty)')));

			ifaces.push(span);
		}

		for (var i = 0; i < devices.length; i++) {
			var device = this.devices.filter(function(dev) { return dev.getName() == devices[i] })[0],
			    title = device ? device.getI18n() : _('Absent Interface'),
			    type = device ? device.getType() : 'ethernet',
			    up = device ? device.isUp() : false;

			ifaces.push(E('span', { 'class': 'ifacebadge' }, [
				E('img', {
					'title': title,
					'src': L.resource('icons/%s%s.png'.format(type, up ? '' : '_disabled'))
				}),
				device ? device.getName() : devices[i]
			]));
		}

		if (subnets.length > 0)
			ifaces.push(E('span', { 'class': 'ifacebadge' }, [ '{ %s }'.format(subnets.join('; ')) ]));

		if (!ifaces.length)
			ifaces.push(E('span', { 'class': 'ifacebadge' }, E('em', _('(empty)'))));

		return E('label', {
			'class': 'zonebadge cbi-tooltip-container',
			'style': 'background-color:' + zone.getColor()
		}, [
			E('strong', name),
			E('div', { 'class': 'cbi-tooltip' }, ifaces)
		]);
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var value = (cfgvalue != null) ? cfgvalue : this.default,
		    zone = this.zones.filter(function(z) { return z.getName() == value })[0];

		if (!zone)
			return E([]);

		var forwards = zone.getForwardingsBy('src'),
		    dzones = [];

		for (var i = 0; i < forwards.length; i++) {
			var dzone = forwards[i].getDestinationZone();

			if (!dzone)
				continue;

			dzones.push(this.renderZone(dzone));
		}

		if (!dzones.length)
			dzones.push(E('label', { 'class': 'zonebadge zonebadge-empty' },
				E('strong', this.defaults.getForward())));

		return E('div', { 'class': 'zone-forwards' }, [
			E('div', { 'class': 'zone-src' }, this.renderZone(zone)),
			E('span', '⇒'),
			E('div', { 'class': 'zone-dest' }, dzones)
		]);
	},
});

var CBINetworkSelect = form.ListValue.extend({
	__name__: 'CBI.NetworkSelect',

	load: function(section_id) {
		return network.getNetworks().then(L.bind(function(networks) {
			this.networks = networks;

			return this.super('load', section_id);
		}, this));
	},

	filter: function(section_id, value) {
		return true;
	},

	renderIfaceBadge: function(network) {
		var span = E('span', { 'class': 'ifacebadge' }, network.getName() + ': '),
		    devices = network.isBridge() ? network.getDevices() : L.toArray(network.getDevice());

		for (var j = 0; j < devices.length && devices[j]; j++) {
			span.appendChild(E('img', {
				'title': devices[j].getI18n(),
				'src': L.resource('icons/%s%s.png'.format(devices[j].getType(), devices[j].isUp() ? '' : '_disabled'))
			}));
		}

		if (!devices.length) {
			span.appendChild(E('em', { 'class': 'hide-close' }, _('(no interfaces attached)')));
			span.appendChild(E('em', { 'class': 'hide-open' }, '-'));
		}

		return span;
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var values = L.toArray((cfgvalue != null) ? cfgvalue : this.default),
		    choices = {},
		    checked = {};

		for (var i = 0; i < values.length; i++)
			checked[values[i]] = true;

		values = [];

		if (!this.multiple && (this.rmempty || this.optional))
			choices[''] = E('em', _('unspecified'));

		for (var i = 0; i < this.networks.length; i++) {
			var network = this.networks[i],
			    name = network.getName();

			if (name == 'loopback' || name == this.exclude || !this.filter(section_id, name))
				continue;

			if (this.novirtual && network.isVirtual())
				continue;

			if (checked[name])
				values.push(name);

			choices[name] = this.renderIfaceBadge(network);
		}

		var widget = new ui.Dropdown(this.multiple ? values : values[0], choices, {
			id: this.cbid(section_id),
			sort: true,
			multiple: this.multiple,
			optional: this.optional || this.rmempty,
			select_placeholder: E('em', _('unspecified')),
			display_items: this.display_size || this.size || 3,
			dropdown_items: this.dropdown_size || this.size || 5,
			validate: L.bind(this.validate, this, section_id),
			create: !this.nocreate,
			create_markup: '' +
				'<li data-value="{{value}}">' +
					'<span class="ifacebadge" style="background:repeating-linear-gradient(45deg,rgba(204,204,204,0.5),rgba(204,204,204,0.5) 5px,rgba(255,255,255,0.5) 5px,rgba(255,255,255,0.5) 10px)">' +
						'{{value}}: <em>('+_('create')+')</em>' +
					'</span>' +
				'</li>'
		});

		return widget.render();
	},

	textvalue: function(section_id) {
		var cfgvalue = this.cfgvalue(section_id),
		    values = L.toArray((cfgvalue != null) ? cfgvalue : this.default),
		    rv = E([]);

		for (var i = 0; i < (this.networks || []).length; i++) {
			var network = this.networks[i],
			    name = network.getName();

			if (values.indexOf(name) == -1)
				continue;

			if (rv.length)
				L.dom.append(rv, ' ');

			L.dom.append(rv, this.renderIfaceBadge(network));
		}

		if (!rv.firstChild)
			rv.appendChild(E('em', _('unspecified')));

		return rv;
	},
});

var CBIDeviceSelect = form.ListValue.extend({
	__name__: 'CBI.DeviceSelect',

	load: function(section_id) {
		return Promise.all([
			network.getDevices(),
			this.noaliases ? null : network.getNetworks()
		]).then(L.bind(function(data) {
			this.devices = data[0];
			this.networks = data[1];

			return this.super('load', section_id);
		}, this));
	},

	filter: function(section_id, value) {
		return true;
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var values = L.toArray((cfgvalue != null) ? cfgvalue : this.default),
		    choices = {},
		    checked = {},
		    order = [];

		for (var i = 0; i < values.length; i++)
			checked[values[i]] = true;

		values = [];

		if (!this.multiple && (this.rmempty || this.optional))
			choices[''] = E('em', _('unspecified'));

		for (var i = 0; i < this.devices.length; i++) {
			var device = this.devices[i],
			    name = device.getName(),
			    type = device.getType();

			if (name == 'lo' || name == this.exclude || !this.filter(section_id, name))
				continue;

			if (this.noaliases && type == 'alias')
				continue;

			if (this.nobridges && type == 'bridge')
				continue;

			if (this.noinactive && device.isUp() == false)
				continue;

			var item = E([
				E('img', {
					'title': device.getI18n(),
					'src': L.resource('icons/%s%s.png'.format(type, device.isUp() ? '' : '_disabled'))
				}),
				E('span', { 'class': 'hide-open' }, [ name ]),
				E('span', { 'class': 'hide-close'}, [ device.getI18n() ])
			]);

			var networks = device.getNetworks();

			if (networks.length > 0)
				L.dom.append(item.lastChild, [ ' (', networks.map(function(n) { return n.getName() }).join(', '), ')' ]);

			if (checked[name])
				values.push(name);

			choices[name] = item;
			order.push(name);
		}

		if (this.networks != null) {
			for (var i = 0; i < this.networks.length; i++) {
				var net = this.networks[i],
				    device = network.instantiateDevice('@%s'.format(net.getName()), net),
				    name = device.getName();

				if (name == '@loopback' || name == this.exclude || !this.filter(section_id, name))
					continue;

				if (this.noinactive && net.isUp() == false)
					continue;

				var item = E([
					E('img', {
						'title': device.getI18n(),
						'src': L.resource('icons/alias%s.png'.format(net.isUp() ? '' : '_disabled'))
					}),
					E('span', { 'class': 'hide-open' }, [ name ]),
					E('span', { 'class': 'hide-close'}, [ device.getI18n() ])
				]);

				if (checked[name])
					values.push(name);

				choices[name] = item;
				order.push(name);
			}
		}

		if (!this.nocreate) {
			var keys = Object.keys(checked).sort();

			for (var i = 0; i < keys.length; i++) {
				if (choices.hasOwnProperty(keys[i]))
					continue;

				choices[keys[i]] = E([
					E('img', {
						'title': _('Absent Interface'),
						'src': L.resource('icons/ethernet_disabled.png')
					}),
					E('span', { 'class': 'hide-open' }, [ keys[i] ]),
					E('span', { 'class': 'hide-close'}, [ '%s: "%h"'.format(_('Absent Interface'), keys[i]) ])
				]);

				values.push(keys[i]);
				order.push(keys[i]);
			}
		}

		var widget = new ui.Dropdown(this.multiple ? values : values[0], choices, {
			id: this.cbid(section_id),
			sort: order,
			multiple: this.multiple,
			optional: this.optional || this.rmempty,
			select_placeholder: E('em', _('unspecified')),
			display_items: this.display_size || this.size || 3,
			dropdown_items: this.dropdown_size || this.size || 5,
			validate: L.bind(this.validate, this, section_id),
			create: !this.nocreate,
			create_markup: '' +
				'<li data-value="{{value}}">' +
					'<img title="'+_('Custom Interface')+': &quot;{{value}}&quot;" src="'+L.resource('icons/ethernet_disabled.png')+'" />' +
					'<span class="hide-open">{{value}}</span>' +
					'<span class="hide-close">'+_('Custom Interface')+': "{{value}}"</span>' +
				'</li>'
		});

		return widget.render();
	},
});

var CBIUserSelect = form.ListValue.extend({
	__name__: 'CBI.UserSelect',

	load: function(section_id) {
		return getUsers().then(L.bind(function(users) {
			for (var i = 0; i < users.length; i++) {
				this.value(users[i]);
			}

			return this.super('load', section_id);
		}, this));
	},

	filter: function(section_id, value) {
		return true;
	},
});

var CBIGroupSelect = form.ListValue.extend({
	__name__: 'CBI.GroupSelect',

	load: function(section_id) {
		return getGroups().then(L.bind(function(groups) {
			for (var i = 0; i < groups.length; i++) {
				this.value(groups[i]);
			}

			return this.super('load', section_id);
		}, this));
	},

	filter: function(section_id, value) {
		return true;
	},
});


return L.Class.extend({
	ZoneSelect: CBIZoneSelect,
	ZoneForwards: CBIZoneForwards,
	NetworkSelect: CBINetworkSelect,
	DeviceSelect: CBIDeviceSelect,
	UserSelect: CBIUserSelect,
	GroupSelect: CBIGroupSelect,
});
