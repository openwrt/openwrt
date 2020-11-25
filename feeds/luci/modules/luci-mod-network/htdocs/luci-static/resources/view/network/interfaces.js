'use strict';
'require fs';
'require ui';
'require uci';
'require form';
'require network';
'require firewall';
'require tools.widgets as widgets';

function count_changes(section_id) {
	var changes = ui.changes.changes, n = 0;

	if (!L.isObject(changes))
		return n;

	if (Array.isArray(changes.network))
		for (var i = 0; i < changes.network.length; i++)
			n += (changes.network[i][1] == section_id);

	if (Array.isArray(changes.dhcp))
		for (var i = 0; i < changes.dhcp.length; i++)
			n += (changes.dhcp[i][1] == section_id);

	return n;
}

function render_iface(dev, alias) {
	var type = dev ? dev.getType() : 'ethernet',
	    up   = dev ? dev.isUp() : false;

	return E('span', { class: 'cbi-tooltip-container' }, [
		E('img', { 'class' : 'middle', 'src': L.resource('icons/%s%s.png').format(
			alias ? 'alias' : type,
			up ? '' : '_disabled') }),
		E('span', { 'class': 'cbi-tooltip ifacebadge large' }, [
			E('img', { 'src': L.resource('icons/%s%s.png').format(
				type, up ? '' : '_disabled') }),
			L.itemlist(E('span', { 'class': 'left' }), [
				_('Type'),      dev ? dev.getTypeI18n() : null,
				_('Device'),    dev ? dev.getName() : _('Not present'),
				_('Connected'), up ? _('yes') : _('no'),
				_('MAC'),       dev ? dev.getMAC() : null,
				_('RX'),        dev ? '%.2mB (%d %s)'.format(dev.getRXBytes(), dev.getRXPackets(), _('Pkts.')) : null,
				_('TX'),        dev ? '%.2mB (%d %s)'.format(dev.getTXBytes(), dev.getTXPackets(), _('Pkts.')) : null
			])
		])
	]);
}

function render_status(node, ifc, with_device) {
	var desc = null, c = [];

	if (ifc.isDynamic())
		desc = _('Virtual dynamic interface');
	else if (ifc.isAlias())
		desc = _('Alias Interface');
	else if (!uci.get('network', ifc.getName()))
		return L.itemlist(node, [
			null, E('em', _('Interface is marked for deletion'))
		]);

	var i18n = ifc.getI18n();
	if (i18n)
		desc = desc ? '%s (%s)'.format(desc, i18n) : i18n;

	var changecount = with_device ? 0 : count_changes(ifc.getName()),
	    ipaddrs = changecount ? [] : ifc.getIPAddrs(),
	    ip6addrs = changecount ? [] : ifc.getIP6Addrs(),
	    errors = ifc.getErrors(),
	    maindev = ifc.getL3Device() || ifc.getDevice(),
	    macaddr = maindev ? maindev.getMAC() : null;

	return L.itemlist(node, [
		_('Protocol'), with_device ? null : (desc || '?'),
		_('Device'),   with_device ? (maindev ? maindev.getShortName() : E('em', _('Not present'))) : null,
		_('Uptime'),   (!changecount && ifc.isUp()) ? '%t'.format(ifc.getUptime()) : null,
		_('MAC'),      (!changecount && !ifc.isDynamic() && !ifc.isAlias() && macaddr) ? macaddr : null,
		_('RX'),       (!changecount && !ifc.isDynamic() && !ifc.isAlias() && maindev) ? '%.2mB (%d %s)'.format(maindev.getRXBytes(), maindev.getRXPackets(), _('Pkts.')) : null,
		_('TX'),       (!changecount && !ifc.isDynamic() && !ifc.isAlias() && maindev) ? '%.2mB (%d %s)'.format(maindev.getTXBytes(), maindev.getTXPackets(), _('Pkts.')) : null,
		_('IPv4'),     ipaddrs[0],
		_('IPv4'),     ipaddrs[1],
		_('IPv4'),     ipaddrs[2],
		_('IPv4'),     ipaddrs[3],
		_('IPv4'),     ipaddrs[4],
		_('IPv6'),     ip6addrs[0],
		_('IPv6'),     ip6addrs[1],
		_('IPv6'),     ip6addrs[2],
		_('IPv6'),     ip6addrs[3],
		_('IPv6'),     ip6addrs[4],
		_('IPv6'),     ip6addrs[5],
		_('IPv6'),     ip6addrs[6],
		_('IPv6'),     ip6addrs[7],
		_('IPv6'),     ip6addrs[8],
		_('IPv6'),     ip6addrs[9],
		_('IPv6-PD'),  changecount ? null : ifc.getIP6Prefix(),
		_('Information'), with_device ? null : (ifc.get('auto') != '0' ? null : _('Not started on boot')),
		_('Error'),    errors ? errors[0] : null,
		_('Error'),    errors ? errors[1] : null,
		_('Error'),    errors ? errors[2] : null,
		_('Error'),    errors ? errors[3] : null,
		_('Error'),    errors ? errors[4] : null,
		null, changecount ? E('a', {
			href: '#',
			click: L.bind(ui.changes.displayChanges, ui.changes)
		}, _('Interface has %d pending changes').format(changecount)) : null
	]);
}

function render_modal_status(node, ifc) {
	var dev = ifc ? (ifc.getDevice() || ifc.getL3Device() || ifc.getL3Device()) : null;

	L.dom.content(node, [
		E('img', {
			'src': L.resource('icons/%s%s.png').format(dev ? dev.getType() : 'ethernet', (dev && dev.isUp()) ? '' : '_disabled'),
			'title': dev ? dev.getTypeI18n() : _('Not present')
		}),
		ifc ? render_status(E('span'), ifc, true) : E('em', _('Interface not present or not connected yet.'))
	]);

	return node;
}

function render_ifacebox_status(node, ifc) {
	var dev = ifc.getL3Device() || ifc.getDevice(),
	    subdevs = ifc.getDevices(),
	    c = [ render_iface(dev, ifc.isAlias()) ];

	if (subdevs && subdevs.length) {
		var sifs = [ ' (' ];

		for (var j = 0; j < subdevs.length; j++)
			sifs.push(render_iface(subdevs[j]));

		sifs.push(')');

		c.push(E('span', {}, sifs));
	}

	c.push(E('br'));
	c.push(E('small', {}, ifc.isAlias() ? _('Alias of "%s"').format(ifc.isAlias())
	                                    : (dev ? dev.getName() : E('em', _('Not present')))));

	L.dom.content(node, c);

	return firewall.getZoneByNetwork(ifc.getName()).then(L.bind(function(zone) {
		this.style.backgroundColor = zone ? zone.getColor() : '#EEEEEE';
		this.title = zone ? _('Part of zone %q').format(zone.getName()) : _('No zone assigned');
	}, node.previousElementSibling));
}

function iface_updown(up, id, ev, force) {
	var row = document.querySelector('.cbi-section-table-row[data-sid="%s"]'.format(id)),
	    dsc = row.querySelector('[data-name="_ifacestat"] > div'),
	    btns = row.querySelectorAll('.cbi-section-actions .reconnect, .cbi-section-actions .down');

	btns[+!up].blur();
	btns[+!up].classList.add('spinning');

	btns[0].disabled = true;
	btns[1].disabled = true;

	if (!up) {
		L.Request.get(L.url('admin/network/remote_addr')).then(function(res) {
			var info = res.json();

			if (L.isObject(info) &&
			    Array.isArray(info.inbound_interfaces) &&
			    info.inbound_interfaces.filter(function(i) { return i == id })[0]) {

				ui.showModal(_('Confirm disconnect'), [
					E('p', _('You appear to be currently connected to the device via the "%h" interface. Do you really want to shut down the interface?').format(id)),
					E('div', { 'class': 'right' }, [
						E('button', {
							'class': 'cbi-button cbi-button-neutral',
							'click': function(ev) {
								btns[1].classList.remove('spinning');
								btns[1].disabled = false;
								btns[0].disabled = false;

								ui.hideModal();
							}
						}, _('Cancel')),
						' ',
						E('button', {
							'class': 'cbi-button cbi-button-negative important',
							'click': function(ev) {
								dsc.setAttribute('disconnect', '');
								L.dom.content(dsc, E('em', _('Interface is shutting down...')));

								ui.hideModal();
							}
						}, _('Disconnect'))
					])
				]);
			}
			else {
				dsc.setAttribute('disconnect', '');
				L.dom.content(dsc, E('em', _('Interface is shutting down...')));
			}
		});
	}
	else {
		dsc.setAttribute(up ? 'reconnect' : 'disconnect', force ? 'force' : '');
		L.dom.content(dsc, E('em', up ? _('Interface is reconnecting...') : _('Interface is shutting down...')));
	}
}

function get_netmask(s, use_cfgvalue) {
	var readfn = use_cfgvalue ? 'cfgvalue' : 'formvalue',
	    addropt = s.children.filter(function(o) { return o.option == 'ipaddr'})[0],
	    addrvals = addropt ? L.toArray(addropt[readfn](s.section)) : [],
	    maskopt = s.children.filter(function(o) { return o.option == 'netmask'})[0],
	    maskval = maskopt ? maskopt[readfn](s.section) : null,
	    firstsubnet = maskval ? addrvals[0] + '/' + maskval : addrvals.filter(function(a) { return a.indexOf('/') > 0 })[0];

	if (firstsubnet == null)
		return null;

	var mask = firstsubnet.split('/')[1];

	if (!isNaN(mask))
		mask = network.prefixToMask(+mask);

	return mask;
}

return L.view.extend({
	poll_status: function(map, networks) {
		var resolveZone = null;

		for (var i = 0; i < networks.length; i++) {
			var ifc = networks[i],
			    row = map.querySelector('.cbi-section-table-row[data-sid="%s"]'.format(ifc.getName()));

			if (row == null)
				continue;

			var dsc = row.querySelector('[data-name="_ifacestat"] > div'),
			    box = row.querySelector('[data-name="_ifacebox"] .ifacebox-body'),
			    btn1 = row.querySelector('.cbi-section-actions .reconnect'),
			    btn2 = row.querySelector('.cbi-section-actions .down'),
			    stat = document.querySelector('[id="%s-ifc-status"]'.format(ifc.getName())),
			    resolveZone = render_ifacebox_status(box, ifc),
			    disabled = ifc ? !ifc.isUp() : true,
			    dynamic = ifc ? ifc.isDynamic() : false;

			if (dsc.hasAttribute('reconnect')) {
				L.dom.content(dsc, E('em', _('Interface is starting...')));
			}
			else if (dsc.hasAttribute('disconnect')) {
				L.dom.content(dsc, E('em', _('Interface is stopping...')));
			}
			else if (ifc.getProtocol() || uci.get('network', ifc.getName()) == null) {
				render_status(dsc, ifc, false);
			}
			else if (!ifc.getProtocol()) {
				var e = map.querySelector('[id="cbi-network-%s"] .cbi-button-edit'.format(ifc.getName()));
				if (e) e.disabled = true;

				var link = L.url('admin/system/opkg') + '?query=luci-proto';
				L.dom.content(dsc, [
					E('em', _('Unsupported protocol type.')), E('br'),
					E('a', { href: link }, _('Install protocol extensions...'))
				]);
			}
			else {
				L.dom.content(dsc, E('em', _('Interface not present or not connected yet.')));
			}

			if (stat) {
				var dev = ifc.getDevice();
				L.dom.content(stat, [
					E('img', {
						'src': L.resource('icons/%s%s.png').format(dev ? dev.getType() : 'ethernet', (dev && dev.isUp()) ? '' : '_disabled'),
						'title': dev ? dev.getTypeI18n() : _('Not present')
					}),
					render_status(E('span'), ifc, true)
				]);
			}

			btn1.disabled = btn1.classList.contains('spinning') || btn2.classList.contains('spinning') || dynamic;
			btn2.disabled = btn1.classList.contains('spinning') || btn2.classList.contains('spinning') || dynamic || disabled;
		}

		return Promise.all([ resolveZone, network.flushCache() ]);
	},

	load: function() {
		return Promise.all([
			network.getDSLModemType(),
			uci.changes()
		]);
	},

	render: function(data) {
		var dslModemType = data[0],
		    m, s, o;

		m = new form.Map('network');
		m.tabbed = true;
		m.chain('dhcp');

		s = m.section(form.GridSection, 'interface', _('Interfaces'));
		s.anonymous = true;
		s.addremove = true;
		s.addbtntitle = _('Add new interface...');

		s.load = function() {
			return Promise.all([
				network.getNetworks(),
				firewall.getZones()
			]).then(L.bind(function(data) {
				this.networks = data[0];
				this.zones = data[1];
			}, this));
		};

		s.tab('general', _('General Settings'));
		s.tab('advanced', _('Advanced Settings'));
		s.tab('physical', _('Physical Settings'));
		s.tab('firewall', _('Firewall Settings'));
		s.tab('dhcp', _('DHCP Server'));

		s.cfgsections = function() {
			return this.networks.map(function(n) { return n.getName() })
				.filter(function(n) { return n != 'loopback' });
		};

		s.modaltitle = function(section_id) {
			return _('Interfaces') + ' » ' + section_id.toUpperCase();
		};

		s.renderRowActions = function(section_id) {
			var tdEl = this.super('renderRowActions', [ section_id, _('Edit') ]),
			    net = this.networks.filter(function(n) { return n.getName() == section_id })[0],
			    disabled = net ? !net.isUp() : true,
			    dynamic = net ? net.isDynamic() : false;

			L.dom.content(tdEl.lastChild, [
				E('button', {
					'class': 'cbi-button cbi-button-neutral reconnect',
					'click': iface_updown.bind(this, true, section_id),
					'title': _('Reconnect this interface'),
					'disabled': dynamic ? 'disabled' : null
				}, _('Restart')),
				E('button', {
					'class': 'cbi-button cbi-button-neutral down',
					'click': iface_updown.bind(this, false, section_id),
					'title': _('Shutdown this interface'),
					'disabled': (dynamic || disabled) ? 'disabled' : null
				}, _('Stop')),
				tdEl.lastChild.firstChild,
				tdEl.lastChild.lastChild
			]);

			if (!dynamic && net && !uci.get('network', net.getName())) {
				tdEl.lastChild.childNodes[0].disabled = true;
				tdEl.lastChild.childNodes[2].disabled = true;
				tdEl.lastChild.childNodes[3].disabled = true;
			}

			return tdEl;
		};

		s.addModalOptions = function(s) {
			var protoval = uci.get('network', s.section, 'proto'),
			    protoclass = protoval ? network.getProtocol(protoval) : null,
			    o, ifname_single, ifname_multi, proto_select, proto_switch, type, stp, igmp, ss, so;

			if (!protoval)
				return;

			return network.getNetwork(s.section).then(L.bind(function(ifc) {
				var protocols = network.getProtocols();

				protocols.sort(function(a, b) {
					return a.getProtocol() > b.getProtocol();
				});

				o = s.taboption('general', form.DummyValue, '_ifacestat_modal', _('Status'));
				o.modalonly = true;
				o.cfgvalue = L.bind(function(section_id) {
					var net = this.networks.filter(function(n) { return n.getName() == section_id })[0];

					return render_modal_status(E('div', {
						'id': '%s-ifc-status'.format(section_id),
						'class': 'ifacebadge large'
					}), net);
				}, this);
				o.write = function() {};

				proto_select = s.taboption('general', form.ListValue, 'proto', _('Protocol'));
				proto_select.modalonly = true;

				proto_switch = s.taboption('general', form.Button, '_switch_proto');
				proto_switch.modalonly  = true;
				proto_switch.title      = _('Really switch protocol?');
				proto_switch.inputtitle = _('Switch protocol');
				proto_switch.inputstyle = 'apply';
				proto_switch.onclick = L.bind(function(ev) {
					s.map.save()
						.then(L.bind(m.load, m))
						.then(L.bind(m.render, m))
						.then(L.bind(this.renderMoreOptionsModal, this, s.section));
				}, this);

				o = s.taboption('general', form.Flag, 'auto', _('Bring up on boot'));
				o.modalonly = true;
				o.default = o.enabled;

				type = s.taboption('physical', form.Flag, 'type', _('Bridge interfaces'), _('creates a bridge over specified interface(s)'));
				type.modalonly = true;
				type.disabled = '';
				type.enabled = 'bridge';
				type.write = type.remove = function(section_id, value) {
					var protocol = network.getProtocol(proto_select.formvalue(section_id)),
					    ifnameopt = this.section.children.filter(function(o) { return o.option == (value ? 'ifname_multi' : 'ifname_single') })[0];

					if (!protocol.isVirtual() && !this.isActive(section_id))
						return;

					var old_ifnames = [],
					    devs = ifc.getDevices() || L.toArray(ifc.getDevice());

					for (var i = 0; i < devs.length; i++)
						old_ifnames.push(devs[i].getName());

					var new_ifnames = L.toArray(ifnameopt.formvalue(section_id));

					if (!value)
						new_ifnames.length = Math.max(new_ifnames.length, 1);

					old_ifnames.sort();
					new_ifnames.sort();

					for (var i = 0; i < Math.max(old_ifnames.length, new_ifnames.length); i++) {
						if (old_ifnames[i] != new_ifnames[i]) {
							// backup_ifnames()
							for (var j = 0; j < old_ifnames.length; j++)
								ifc.deleteDevice(old_ifnames[j]);

							for (var j = 0; j < new_ifnames.length; j++)
								ifc.addDevice(new_ifnames[j]);

							break;
						}
					}

					if (value)
						uci.set('network', section_id, 'type', 'bridge');
					else
						uci.unset('network', section_id, 'type');
				};

				stp = s.taboption('physical', form.Flag, 'stp', _('Enable <abbr title="Spanning Tree Protocol">STP</abbr>'), _('Enables the Spanning Tree Protocol on this bridge'));

				igmp = s.taboption('physical', form.Flag, 'igmp_snooping', _('Enable <abbr title="Internet Group Management Protocol">IGMP</abbr> snooping'), _('Enables IGMP snooping on this bridge'));

				ifname_single = s.taboption('physical', widgets.DeviceSelect, 'ifname_single', _('Interface'));
				ifname_single.nobridges = ifc.isBridge();
				ifname_single.noaliases = false;
				ifname_single.optional = false;
				ifname_single.network = ifc.getName();
				ifname_single.write = ifname_single.remove = function() {};

				ifname_multi = s.taboption('physical', widgets.DeviceSelect, 'ifname_multi', _('Interface'));
				ifname_multi.nobridges = ifc.isBridge();
				ifname_multi.noaliases = true;
				ifname_multi.multiple = true;
				ifname_multi.optional = true;
				ifname_multi.network = ifc.getName();
				ifname_multi.display_size = 6;
				ifname_multi.write = ifname_multi.remove = function() {};

				ifname_single.cfgvalue = ifname_multi.cfgvalue = function(section_id) {
					var devs = ifc.getDevices() || L.toArray(ifc.getDevice()),
					    ifnames = [];

					for (var i = 0; i < devs.length; i++)
						ifnames.push(devs[i].getName());

					return ifnames;
				};

				if (L.hasSystemFeature('firewall')) {
					o = s.taboption('firewall', widgets.ZoneSelect, '_zone', _('Create / Assign firewall-zone'), _('Choose the firewall zone you want to assign to this interface. Select <em>unspecified</em> to remove the interface from the associated zone or fill out the <em>create</em> field to define a new zone and attach the interface to it.'));
					o.network = ifc.getName();
					o.optional = true;

					o.cfgvalue = function(section_id) {
						return firewall.getZoneByNetwork(ifc.getName()).then(function(zone) {
							return (zone != null ? zone.getName() : null);
						});
					};

					o.write = o.remove = function(section_id, value) {
						return Promise.all([
							firewall.getZoneByNetwork(ifc.getName()),
							(value != null) ? firewall.getZone(value) : null
						]).then(function(data) {
							var old_zone = data[0],
							    new_zone = data[1];

							if (old_zone == null && new_zone == null && (value == null || value == ''))
								return;

							if (old_zone != null && new_zone != null && old_zone.getName() == new_zone.getName())
								return;

							if (old_zone != null)
								old_zone.deleteNetwork(ifc.getName());

							if (new_zone != null)
								new_zone.addNetwork(ifc.getName());
							else if (value != null)
								return firewall.addZone(value).then(function(new_zone) {
									new_zone.addNetwork(ifc.getName());
								});
						});
					};
				}

				for (var i = 0; i < protocols.length; i++) {
					proto_select.value(protocols[i].getProtocol(), protocols[i].getI18n());

					if (protocols[i].getProtocol() != uci.get('network', s.section, 'proto'))
						proto_switch.depends('proto', protocols[i].getProtocol());

					if (!protocols[i].isVirtual()) {
						type.depends('proto', protocols[i].getProtocol());
						stp.depends({ type: 'bridge', proto: protocols[i].getProtocol() });
						igmp.depends({ type: 'bridge', proto: protocols[i].getProtocol() });
						ifname_single.depends({ type: '', proto: protocols[i].getProtocol() });
						ifname_multi.depends({ type: 'bridge', proto: protocols[i].getProtocol() });
					}
				}

				if (L.hasSystemFeature('dnsmasq') || L.hasSystemFeature('odhcpd')) {
					o = s.taboption('dhcp', form.SectionValue, '_dhcp', form.TypedSection, 'dhcp');
					o.depends('proto', 'static');

					ss = o.subsection;
					ss.uciconfig = 'dhcp';
					ss.addremove = false;
					ss.anonymous = true;

					ss.tab('general',  _('General Setup'));
					ss.tab('advanced', _('Advanced Settings'));
					ss.tab('ipv6', _('IPv6 Settings'));

					ss.filter = function(section_id) {
						return (uci.get('dhcp', section_id, 'interface') == ifc.getName());
					};

					ss.renderSectionPlaceholder = function() {
						return E('div', { 'class': 'cbi-section-create' }, [
							E('p', _('No DHCP Server configured for this interface') + ' &#160; '),
							E('button', {
								'class': 'cbi-button cbi-button-add',
								'title': _('Setup DHCP Server'),
								'click': ui.createHandlerFn(this, function(section_id, ev) {
									this.map.save(function() {
										uci.add('dhcp', 'dhcp', section_id);
										uci.set('dhcp', section_id, 'interface', section_id);
										uci.set('dhcp', section_id, 'start', 100);
										uci.set('dhcp', section_id, 'limit', 150);
										uci.set('dhcp', section_id, 'leasetime', '12h');
									});
								}, ifc.getName())
							}, _('Setup DHCP Server'))
						]);
					};

					ss.taboption('general', form.Flag, 'ignore', _('Ignore interface'), _('Disable <abbr title="Dynamic Host Configuration Protocol">DHCP</abbr> for this interface.'));

					so = ss.taboption('general', form.Value, 'start', _('Start'), _('Lowest leased address as offset from the network address.'));
					so.optional = true;
					so.datatype = 'or(uinteger,ip4addr("nomask"))';
					so.default = '100';

					so = ss.taboption('general', form.Value, 'limit', _('Limit'), _('Maximum number of leased addresses.'));
					so.optional = true;
					so.datatype = 'uinteger';
					so.default = '150';

					so = ss.taboption('general', form.Value, 'leasetime', _('Lease time'), _('Expiry time of leased addresses, minimum is 2 minutes (<code>2m</code>).'));
					so.optional = true;
					so.default = '12h';

					so = ss.taboption('advanced', form.Flag, 'dynamicdhcp', _('Dynamic <abbr title="Dynamic Host Configuration Protocol">DHCP</abbr>'), _('Dynamically allocate DHCP addresses for clients. If disabled, only clients having static leases will be served.'));
					so.default = so.enabled;

					ss.taboption('advanced', form.Flag, 'force', _('Force'), _('Force DHCP on this network even if another server is detected.'));

					// XXX: is this actually useful?
					//ss.taboption('advanced', form.Value, 'name', _('Name'), _('Define a name for this network.'));

					so = ss.taboption('advanced', form.Value, 'netmask', _('<abbr title="Internet Protocol Version 4">IPv4</abbr>-Netmask'), _('Override the netmask sent to clients. Normally it is calculated from the subnet that is served.'));
					so.optional = true;
					so.datatype = 'ip4addr';

					so.render = function(option_index, section_id, in_table) {
						this.placeholder = get_netmask(s, true);
						return form.Value.prototype.render.apply(this, [ option_index, section_id, in_table ]);
					};

					so.validate = function(section_id, value) {
						var node = this.map.findElement('id', this.cbid(section_id));
						if (node)
							node.querySelector('input').setAttribute('placeholder', get_netmask(s, false));
						return form.Value.prototype.validate.apply(this, [ section_id, value ]);
					};

					ss.taboption('advanced', form.DynamicList, 'dhcp_option', _('DHCP-Options'), _('Define additional DHCP options, for example "<code>6,192.168.2.1,192.168.2.2</code>" which advertises different DNS servers to clients.'));

					for (var i = 0; i < ss.children.length; i++)
						if (ss.children[i].option != 'ignore')
							ss.children[i].depends('ignore', '0');

					so = ss.taboption('ipv6', form.ListValue, 'ra', _('Router Advertisement-Service'));
					so.value('', _('disabled'));
					so.value('server', _('server mode'));
					so.value('relay', _('relay mode'));
					so.value('hybrid', _('hybrid mode'));

					so = ss.taboption('ipv6', form.ListValue, 'dhcpv6', _('DHCPv6-Service'));
					so.value('', _('disabled'));
					so.value('server', _('server mode'));
					so.value('relay', _('relay mode'));
					so.value('hybrid', _('hybrid mode'));

					so = ss.taboption('ipv6', form.ListValue, 'ndp', _('NDP-Proxy'));
					so.value('', _('disabled'));
					so.value('relay', _('relay mode'));
					so.value('hybrid', _('hybrid mode'));

					so = ss.taboption('ipv6', form.Flag , 'master', _('Master'), _('Set this interface as master for the dhcpv6 relay.'));
					so.depends('dhcpv6', 'relay');
					so.depends('dhcpv6', 'hybrid');

					so = ss.taboption('ipv6', form.ListValue, 'ra_management', _('DHCPv6-Mode'), _('Default is stateless + stateful'));
					so.value('0', _('stateless'));
					so.value('1', _('stateless + stateful'));
					so.value('2', _('stateful-only'));
					so.depends('dhcpv6', 'server');
					so.depends('dhcpv6', 'hybrid');
					so.default = '1';

					so = ss.taboption('ipv6', form.Flag, 'ra_default', _('Always announce default router'), _('Announce as default router even if no public prefix is available.'));
					so.depends('ra', 'server');
					so.depends('ra', 'hybrid');

					ss.taboption('ipv6', form.DynamicList, 'dns', _('Announced DNS servers'));
					ss.taboption('ipv6', form.DynamicList, 'domain', _('Announced DNS domains'));
				}

				ifc.renderFormOptions(s);

				for (var i = 0; i < s.children.length; i++) {
					o = s.children[i];

					switch (o.option) {
					case 'proto':
					case 'delegate':
					case 'auto':
					case 'type':
					case 'stp':
					case 'igmp_snooping':
					case 'ifname_single':
					case 'ifname_multi':
					case '_dhcp':
					case '_zone':
					case '_switch_proto':
					case '_ifacestat_modal':
						continue;

					default:
						if (o.deps.length)
							for (var j = 0; j < o.deps.length; j++)
								o.deps[j].proto = protoval;
						else
							o.depends('proto', protoval);
					}
				}
			}, this));
		};

		s.handleAdd = function(ev) {
			var m2 = new form.Map('network'),
			    s2 = m2.section(form.NamedSection, '_new_'),
			    protocols = network.getProtocols(),
			    proto, name, bridge, ifname_single, ifname_multi;

			protocols.sort(function(a, b) {
				return a.getProtocol() > b.getProtocol();
			});

			s2.render = function() {
				return Promise.all([
					{},
					this.renderUCISection('_new_')
				]).then(this.renderContents.bind(this));
			};

			name = s2.option(form.Value, 'name', _('Name'));
			name.rmempty = false;
			name.datatype = 'uciname';
			name.placeholder = _('New interface name…');
			name.validate = function(section_id, value) {
				if (uci.get('network', value) != null)
					return _('The interface name is already used');

				var pr = network.getProtocol(proto.formvalue(section_id), value),
				    ifname = pr.isVirtual() ? '%s-%s'.format(pr.getProtocol(), value) : 'br-%s'.format(value);

				if (value.length > 15)
					return _('The interface name is too long');

				return true;
			};

			proto = s2.option(form.ListValue, 'proto', _('Protocol'));
			proto.validate = name.validate;

			bridge = s2.option(form.Flag, 'type', _('Bridge interfaces'), _('creates a bridge over specified interface(s)'));
			bridge.modalonly = true;
			bridge.disabled = '';
			bridge.enabled = 'bridge';

			ifname_single = s2.option(widgets.DeviceSelect, 'ifname_single', _('Interface'));
			ifname_single.noaliases = false;
			ifname_single.optional = false;

			ifname_multi = s2.option(widgets.DeviceSelect, 'ifname_multi', _('Interface'));
			ifname_multi.nobridges = true;
			ifname_multi.noaliases = true;
			ifname_multi.multiple = true;
			ifname_multi.optional = true;
			ifname_multi.display_size = 6;

			for (var i = 0; i < protocols.length; i++) {
				proto.value(protocols[i].getProtocol(), protocols[i].getI18n());

				if (!protocols[i].isVirtual()) {
					bridge.depends({ proto: protocols[i].getProtocol() });
					ifname_single.depends({ type: '', proto: protocols[i].getProtocol() });
					ifname_multi.depends({ type: 'bridge', proto: protocols[i].getProtocol() });
				}
			}

			m2.render().then(L.bind(function(nodes) {
				ui.showModal(_('Add new interface...'), [
					nodes,
					E('div', { 'class': 'right' }, [
						E('button', {
							'class': 'btn',
							'click': ui.hideModal
						}, _('Cancel')), ' ',
						E('button', {
							'class': 'cbi-button cbi-button-positive important',
							'click': ui.createHandlerFn(this, function(ev) {
								var nameval = name.isValid('_new_') ? name.formvalue('_new_') : null,
								    protoval = proto.isValid('_new_') ? proto.formvalue('_new_') : null;

								if (nameval == null || protoval == null || nameval == '' || protoval == '')
									return;

								return m.save(function() {
									var section_id = uci.add('network', 'interface', nameval);

									uci.set('network', section_id, 'proto', protoval);

									if (ifname_single.isActive('_new_')) {
										uci.set('network', section_id, 'ifname', ifname_single.formvalue('_new_'));
									}
									else if (ifname_multi.isActive('_new_')) {
										uci.set('network', section_id, 'type', 'bridge');
										uci.set('network', section_id, 'ifname', L.toArray(ifname_multi.formvalue('_new_')).join(' '));
									}
								}).then(L.bind(m.children[0].renderMoreOptionsModal, m.children[0], nameval));
							})
						}, _('Create interface'))
					])
				], 'cbi-modal');

				nodes.querySelector('[id="%s"] input[type="text"]'.format(name.cbid('_new_'))).focus();
			}, this));
		};

		s.handleRemove = function(section_id, ev) {
			return network.deleteNetwork(section_id).then(L.bind(function(section_id, ev) {
				return form.GridSection.prototype.handleRemove.apply(this, [section_id, ev]);
			}, this, section_id, ev));
		};

		o = s.option(form.DummyValue, '_ifacebox');
		o.modalonly = false;
		o.textvalue = function(section_id) {
			var net = this.section.networks.filter(function(n) { return n.getName() == section_id })[0],
			    zone = net ? this.section.zones.filter(function(z) { return !!z.getNetworks().filter(function(n) { return n == section_id })[0] })[0] : null;

			if (!net)
				return;

			var node = E('div', { 'class': 'ifacebox' }, [
				E('div', {
					'class': 'ifacebox-head',
					'style': 'background-color:%s'.format(zone ? zone.getColor() : '#EEEEEE'),
					'title': zone ? _('Part of zone %q').format(zone.getName()) : _('No zone assigned')
				}, E('strong', net.getName().toUpperCase())),
				E('div', {
					'class': 'ifacebox-body',
					'id': '%s-ifc-devices'.format(section_id),
					'data-network': section_id
				}, [
					E('img', {
						'src': L.resource('icons/ethernet_disabled.png'),
						'style': 'width:16px; height:16px'
					}),
					E('br'), E('small', '?')
				])
			]);

			render_ifacebox_status(node.childNodes[1], net);

			return node;
		};

		o = s.option(form.DummyValue, '_ifacestat');
		o.modalonly = false;
		o.textvalue = function(section_id) {
			var net = this.section.networks.filter(function(n) { return n.getName() == section_id })[0];

			if (!net)
				return;

			var node = E('div', { 'id': '%s-ifc-description'.format(section_id) });

			render_status(node, net, false);

			return node;
		};

		o = s.taboption('advanced', form.Flag, 'delegate', _('Use builtin IPv6-management'));
		o.modalonly = true;
		o.default = o.enabled;

		o = s.taboption('advanced', form.Flag, 'force_link', _('Force link'), _('Set interface properties regardless of the link carrier (If set, carrier sense events do not invoke hotplug handlers).'));
		o.modalonly = true;
		o.render = function(option_index, section_id, in_table) {
			var protoopt = this.section.children.filter(function(o) { return o.option == 'proto' })[0],
			    protoval = protoopt ? protoopt.cfgvalue(section_id) : null;

			this.default = (protoval == 'static') ? this.enabled : this.disabled;
			return this.super('render', [ option_index, section_id, in_table ]);
		};


		s = m.section(form.TypedSection, 'globals', _('Global network options'));
		s.addremove = false;
		s.anonymous = true;

		o = s.option(form.Value, 'ula_prefix', _('IPv6 ULA-Prefix'));
		o.datatype = 'cidr6';


		if (dslModemType != null) {
			s = m.section(form.TypedSection, 'dsl', _('DSL'));
			s.anonymous = true;

			o = s.option(form.ListValue, 'annex', _('Annex'));
			o.value('a', _('Annex A + L + M (all)'));
			o.value('b', _('Annex B (all)'));
			o.value('j', _('Annex J (all)'));
			o.value('m', _('Annex M (all)'));
			o.value('bdmt', _('Annex B G.992.1'));
			o.value('b2', _('Annex B G.992.3'));
			o.value('b2p', _('Annex B G.992.5'));
			o.value('at1', _('ANSI T1.413'));
			o.value('admt', _('Annex A G.992.1'));
			o.value('alite', _('Annex A G.992.2'));
			o.value('a2', _('Annex A G.992.3'));
			o.value('a2p', _('Annex A G.992.5'));
			o.value('l', _('Annex L G.992.3 POTS 1'));
			o.value('m2', _('Annex M G.992.3'));
			o.value('m2p', _('Annex M G.992.5'));

			o = s.option(form.ListValue, 'tone', _('Tone'));
			o.value('', _('auto'));
			o.value('a', _('A43C + J43 + A43'));
			o.value('av', _('A43C + J43 + A43 + V43'));
			o.value('b', _('B43 + B43C'));
			o.value('bv', _('B43 + B43C + V43'));

			if (dslModemType == 'vdsl') {
				o = s.option(form.ListValue, 'xfer_mode', _('Encapsulation mode'));
				o.value('', _('auto'));
				o.value('atm', _('ATM (Asynchronous Transfer Mode)'));
				o.value('ptm', _('PTM/EFM (Packet Transfer Mode)'));

				o = s.option(form.ListValue, 'line_mode', _('DSL line mode'));
				o.value('', _('auto'));
				o.value('adsl', _('ADSL'));
				o.value('vdsl', _('VDSL'));

				o = s.option(form.ListValue, 'ds_snr_offset', _('Downstream SNR offset'));
				o.default = '0';

				for (var i = -100; i <= 100; i += 5)
					o.value(i, _('%.1f dB').format(i / 10));
			}

			s.option(form.Value, 'firmware', _('Firmware File'));
		}


		// Show ATM bridge section if we have the capabilities
		if (L.hasSystemFeature('br2684ctl')) {
			s = m.section(form.TypedSection, 'atm-bridge', _('ATM Bridges'), _('ATM bridges expose encapsulated ethernet in AAL5 connections as virtual Linux network interfaces which can be used in conjunction with DHCP or PPP to dial into the provider network.'));

			s.addremove = true;
			s.anonymous = true;
			s.addbtntitle = _('Add ATM Bridge');

			s.handleAdd = function(ev) {
				var sections = uci.sections('network', 'atm-bridge'),
				    max_unit = -1;

				for (var i = 0; i < sections.length; i++) {
					var unit = +sections[i].unit;

					if (!isNaN(unit) && unit > max_unit)
						max_unit = unit;
				}

				return this.map.save(function() {
					var sid = uci.add('network', 'atm-bridge');

					uci.set('network', sid, 'unit', max_unit + 1);
					uci.set('network', sid, 'atmdev', 0);
					uci.set('network', sid, 'encaps', 'llc');
					uci.set('network', sid, 'payload', 'bridged');
					uci.set('network', sid, 'vci', 35);
					uci.set('network', sid, 'vpi', 8);
				});
			};

			s.tab('general', _('General Setup'));
			s.tab('advanced', _('Advanced Settings'));

			o = s.taboption('general', form.Value, 'vci', _('ATM Virtual Channel Identifier (VCI)'));
			s.taboption('general', form.Value, 'vpi', _('ATM Virtual Path Identifier (VPI)'));

			o = s.taboption('general', form.ListValue, 'encaps', _('Encapsulation mode'));
			o.value('llc', _('LLC'));
			o.value('vc', _('VC-Mux'));

			s.taboption('advanced', form.Value, 'atmdev', _('ATM device number'));
			s.taboption('advanced', form.Value, 'unit', _('Bridge unit number'));

			o = s.taboption('advanced', form.ListValue, 'payload', _('Forwarding mode'));
			o.value('bridged', _('bridged'));
			o.value('routed', _('routed'));
		}


		return m.render().then(L.bind(function(m, nodes) {
			L.Poll.add(L.bind(function() {
				var section_ids = m.children[0].cfgsections(),
				    tasks = [];

				for (var i = 0; i < section_ids.length; i++) {
					var row = nodes.querySelector('.cbi-section-table-row[data-sid="%s"]'.format(section_ids[i])),
					    dsc = row.querySelector('[data-name="_ifacestat"] > div'),
					    btn1 = row.querySelector('.cbi-section-actions .reconnect'),
					    btn2 = row.querySelector('.cbi-section-actions .down');

					if (dsc.getAttribute('reconnect') == '') {
						dsc.setAttribute('reconnect', '1');
						tasks.push(fs.exec('/sbin/ifup', [section_ids[i]]).catch(function(e) {
							ui.addNotification(null, E('p', e.message));
						}));
					}
					else if (dsc.getAttribute('disconnect') == '') {
						dsc.setAttribute('disconnect', '1');
						tasks.push(fs.exec('/sbin/ifdown', [section_ids[i]]).catch(function(e) {
							ui.addNotification(null, E('p', e.message));
						}));
					}
					else if (dsc.getAttribute('reconnect') == '1') {
						dsc.removeAttribute('reconnect');
						btn1.classList.remove('spinning');
						btn1.disabled = false;
					}
					else if (dsc.getAttribute('disconnect') == '1') {
						dsc.removeAttribute('disconnect');
						btn2.classList.remove('spinning');
						btn2.disabled = false;
					}
				}

				return Promise.all(tasks)
					.then(L.bind(network.getNetworks, network))
					.then(L.bind(this.poll_status, this, nodes));
			}, this), 5);

			return nodes;
		}, this, m));
	}
});
