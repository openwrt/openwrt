'use strict';
'require ui';
'require rpc';
'require uci';
'require form';
'require firewall as fwmodel';
'require tools.firewall as fwtool';
'require tools.widgets as widgets';

function rule_proto_txt(s, ctHelpers) {
	var proto = L.toArray(uci.get('firewall', s, 'proto')).filter(function(p) {
		return (p != '*' && p != 'any' && p != 'all');
	}).map(function(p) {
		var pr = fwtool.lookupProto(p);
		return {
			num:   pr[0],
			name:  pr[1],
			types: (pr[0] == 1 || pr[0] == 58) ? L.toArray(uci.get('firewall', s, 'icmp_type')) : null
		};
	});

	m = String(uci.get('firewall', s, 'helper') || '').match(/^(!\s*)?(\S+)$/);
	var h = m ? {
		val:  m[0].toUpperCase(),
		inv:  m[1],
		name: (ctHelpers.filter(function(ctH) { return ctH.name.toLowerCase() == m[2].toLowerCase() })[0] || {}).description
	} : null;

	m = String(uci.get('firewall', s, 'mark')).match(/^(!\s*)?(0x[0-9a-f]{1,8}|[0-9]{1,10})(?:\/(0x[0-9a-f]{1,8}|[0-9]{1,10}))?$/i);
	var f = m ? {
		val:  m[0].toUpperCase().replace(/X/g, 'x'),
		inv:  m[1],
		num:  '0x%02X'.format(+m[2]),
		mask: m[3] ? '0x%02X'.format(+m[3]) : null
	} : null;

	return fwtool.fmt(_('Incoming IPv4%{proto?, protocol %{proto#%{next?, }%{item.types?<var class="cbi-tooltip-container">%{item.name}<span class="cbi-tooltip">ICMP with types %{item.types#%{next?, }<var>%{item}</var>}</span></var>:<var>%{item.name}</var>}}}%{mark?, mark <var%{mark.inv? data-tooltip="Match fwmarks except %{mark.num}%{mark.mask? with mask %{mark.mask}}.":%{mark.mask? data-tooltip="Mask fwmark value with %{mark.mask} before compare."}}>%{mark.val}</var>}%{helper?, helper %{helper.inv?<var data-tooltip="Match any helper except &quot;%{helper.name}&quot;">%{helper.val}</var>:<var data-tooltip="%{helper.name}">%{helper.val}</var>}}'), {
		proto: proto,
		helper: h,
		mark:   f
	});
}

function rule_src_txt(s, hosts) {
	var z = uci.get('firewall', s, 'src');

	return fwtool.fmt(_('From %{src}%{src_ip?, IP %{src_ip#%{next?, }<var%{item.inv? data-tooltip="Match IP addresses except %{item.val}."}>%{item.ival}</var>}}%{src_port?, port %{src_port#%{next?, }<var%{item.inv? data-tooltip="Match ports except %{item.val}."}>%{item.ival}</var>}}%{src_mac?, MAC %{src_mac#%{next?, }<var%{item.inv? data-tooltip="Match MACs except %{item.val}%{item.hint.name? a.k.a. %{item.hint.name}}.":%{item.hint.name? data-tooltip="%{item.hint.name}"}}>%{item.ival}</var>}}'), {
		src: E('span', { 'class': 'zonebadge', 'style': 'background-color:' + fwmodel.getColorForName((z && z != '*') ? z : null) }, [(z == '*') ? E('em', _('any zone')) : (z || E('em', _('this device')))]),
		src_ip: fwtool.map_invert(uci.get('firewall', s, 'src_ip'), 'toLowerCase'),
		src_mac: fwtool.map_invert(uci.get('firewall', s, 'src_mac'), 'toUpperCase').map(function(v) { return Object.assign(v, { hint: hosts[v.val] }) }),
		src_port: fwtool.map_invert(uci.get('firewall', s, 'src_port'))
	});
}

function rule_dest_txt(s) {
	return fwtool.fmt(_('To %{dest}%{dest_ip?, IP %{dest_ip#%{next?, }<var%{item.inv? data-tooltip="Match IP addresses except %{item.val}."}>%{item.ival}</var>}}%{dest_port?, port %{dest_port#%{next?, }<var%{item.inv? data-tooltip="Match ports except %{item.val}."}>%{item.ival}</var>}}'), {
		dest: E('span', { 'class': 'zonebadge', 'style': 'background-color:' + fwmodel.getColorForName(null) }, [E('em', _('this device'))]),
		dest_ip: fwtool.map_invert(uci.get('firewall', s, 'src_dip'), 'toLowerCase'),
		dest_port: fwtool.map_invert(uci.get('firewall', s, 'src_dport'))
	});
}

function rule_limit_txt(s) {
	var m = String(uci.get('firewall', s, 'limit')).match(/^(\d+)\/([smhd])\w*$/i),
	    l = m ? {
			num:   +m[1],
			unit:  ({ s: _('second'), m: _('minute'), h: _('hour'), d: _('day') })[m[2]],
			burst: uci.get('firewall', s, 'limit_burst')
		} : null;

	if (!l)
		return '';

	return fwtool.fmt(_('Limit matching to <var>%{limit.num}</var> packets per <var>%{limit.unit}</var>%{limit.burst? burst <var>%{limit.burst}</var>}'), { limit: l });
}

function rule_target_txt(s) {
	var z = uci.get('firewall', s, 'dest');

	return fwtool.fmt(_('<var data-tooltip="DNAT">Forward</var> to %{dest}%{dest_ip? IP <var>%{dest_ip}</var>}%{dest_port? port <var>%{dest_port}</var>}'), {
		dest: E('span', { 'class': 'zonebadge', 'style': 'background-color:' + fwmodel.getColorForName((z && z != '*') ? z : null) }, [(z == '*') ? E('em', _('any zone')) : (z || E('em', _('this device')))]),
		dest_ip: (uci.get('firewall', s, 'dest_ip') || '').toLowerCase(),
		dest_port: uci.get('firewall', s, 'dest_port')
	});
}

return L.view.extend({
	callHostHints: rpc.declare({
		object: 'luci-rpc',
		method: 'getHostHints',
		expect: { '': {} }
	}),

	callConntrackHelpers: rpc.declare({
		object: 'luci',
		method: 'getConntrackHelpers',
		expect: { result: [] }
	}),

	callNetworkDevices: rpc.declare({
		object: 'luci-rpc',
		method: 'getNetworkDevices',
		expect: { '': {} }
	}),

	load: function() {
		return Promise.all([
			this.callHostHints(),
			this.callConntrackHelpers(),
			this.callNetworkDevices(),
			uci.load('firewall')
		]);
	},

	render: function(data) {
		if (fwtool.checkLegacySNAT())
			return fwtool.renderMigration();
		else
			return this.renderForwards(data);
	},

	renderForwards: function(data) {
		var hosts = data[0],
		    ctHelpers = data[1],
		    devs = data[2],
		    m, s, o;

		m = new form.Map('firewall', _('Firewall - Port Forwards'),
			_('Port forwarding allows remote computers on the Internet to connect to a specific computer or service within the private LAN.'));

		s = m.section(form.GridSection, 'redirect', _('Port Forwards'));
		s.addremove = true;
		s.anonymous = true;
		s.sortable  = true;

		s.tab('general', _('General Settings'));
		s.tab('advanced', _('Advanced Settings'));

		s.filter = function(section_id) {
			return (uci.get('firewall', section_id, 'target') != 'SNAT');
		};

		s.sectiontitle = function(section_id) {
			return uci.get('firewall', section_id, 'name') || _('Unnamed forward');
		};

		s.handleAdd = function(ev) {
			var config_name = this.uciconfig || this.map.config,
			    section_id = uci.add(config_name, this.sectiontype);

			uci.set(config_name, section_id, 'target', 'DNAT');

			this.addedSection = section_id;
			this.renderMoreOptionsModal(section_id);
		};

		o = s.taboption('general', form.Value, 'name', _('Name'));
		o.placeholder = _('Unnamed forward');
		o.modalonly = true;

		o = s.option(form.DummyValue, '_match', _('Match'));
		o.modalonly = false;
		o.textvalue = function(s) {
			return E('small', [
				rule_proto_txt(s, ctHelpers), E('br'),
				rule_src_txt(s, hosts), E('br'),
				rule_dest_txt(s), E('br'),
				rule_limit_txt(s)
			]);
		};

		o = s.option(form.ListValue, '_dest', _('Action'));
		o.modalonly = false;
		o.textvalue = function(s) {
			return E('small', [
				rule_target_txt(s)
			]);
		};

		o = s.option(form.Flag, 'enabled', _('Enable'));
		o.modalonly = false;
		o.default = o.enabled;
		o.editable = true;

		o = s.taboption('general', fwtool.CBIProtocolSelect, 'proto', _('Protocol'));
		o.modalonly = true;
		o.default = 'tcp udp';

		o = s.taboption('general', widgets.ZoneSelect, 'src', _('Source zone'));
		o.modalonly = true;
		o.rmempty = false;
		o.nocreate = true;
		o.default = 'wan';

		o = fwtool.addMACOption(s, 'advanced', 'src_mac', _('Source MAC address'),
			_('Only match incoming traffic from these MACs.'), hosts);
		o.rmempty = true;
		o.datatype = 'list(neg(macaddr))';

		o = fwtool.addIPOption(s, 'advanced', 'src_ip', _('Source IP address'),
			_('Only match incoming traffic from this IP or range.'), 'ipv4', hosts);
		o.rmempty = true;
		o.datatype = 'neg(ipmask4)';

		o = s.taboption('advanced', form.Value, 'src_port', _('Source port'),
			_('Only match incoming traffic originating from the given source port or port range on the client host'));
		o.modalonly = true;
		o.rmempty = true;
		o.datatype = 'neg(portrange)';
		o.placeholder = _('any');
		o.depends({ proto: 'tcp', '!contains': true });
		o.depends({ proto: 'udp', '!contains': true });

		o = fwtool.addLocalIPOption(s, 'advanced', 'src_dip', _('External IP address'),
			_('Only match incoming traffic directed at the given IP address.'), devs);
		o.datatype = 'neg(ipmask4)';
		o.rmempty = true;

		o = s.taboption('general', form.Value, 'src_dport', _('External port'),
			_('Match incoming traffic directed at the given destination port or port range on this host'));
		o.modalonly = true;
		o.rmempty = false;
		o.datatype = 'neg(portrange)';
		o.depends({ proto: 'tcp', '!contains': true });
		o.depends({ proto: 'udp', '!contains': true });

		o = s.taboption('general', widgets.ZoneSelect, 'dest', _('Internal zone'));
		o.modalonly = true;
		o.rmempty = true;
		o.nocreate = true;
		o.default = 'lan';

		o = fwtool.addIPOption(s, 'general', 'dest_ip', _('Internal IP address'),
			_('Redirect matched incoming traffic to the specified internal host'), 'ipv4', hosts);
		o.rmempty = true;
		o.datatype = 'ipmask4';

		o = s.taboption('general', form.Value, 'dest_port', _('Internal port'),
			_('Redirect matched incoming traffic to the given port on the internal host'));
		o.modalonly = true;
		o.rmempty = true;
		o.placeholder = _('any');
		o.datatype = 'portrange';
		o.depends({ proto: 'tcp', '!contains': true });
		o.depends({ proto: 'udp', '!contains': true });

		o = s.taboption('advanced', form.Flag, 'reflection', _('Enable NAT Loopback'));
		o.modalonly = true;
		o.rmempty = true;
		o.default = o.enabled;

		o = s.taboption('advanced', form.ListValue, 'reflection_src', _('Loopback source IP'), _('Specifies whether to use the external or the internal IP address for reflected traffic.'));
		o.modalonly = true;
		o.depends('reflection', '1');
		o.value('internal', _('Use internal IP address'));
		o.value('external', _('Use external IP address'));
		o.write = function(section_id, value) {
			uci.set('firewall', section_id, 'reflection_src', (value != 'internal') ? value : null);
		};

		o = s.taboption('advanced', form.Value, 'helper', _('Match helper'), _('Match traffic using the specified connection tracking helper.'));
		o.modalonly = true;
		o.placeholder = _('any');
		for (var i = 0; i < ctHelpers.length; i++)
			o.value(ctHelpers[i].name, '%s (%s)'.format(ctHelpers[i].description, ctHelpers[i].name.toUpperCase()));
		o.validate = function(section_id, value) {
			if (value == '' || value == null)
				return true;

			value = value.replace(/^!\s*/, '');

			for (var i = 0; i < ctHelpers.length; i++)
				if (value == ctHelpers[i].name)
					return true;

			return _('Unknown or not installed conntrack helper "%s"').format(value);
		};

		fwtool.addMarkOption(s, false);
		fwtool.addLimitOption(s);
		fwtool.addLimitBurstOption(s);

		o = s.taboption('advanced', form.Value, 'extra', _('Extra arguments'),
			_('Passes additional arguments to iptables. Use with care!'));
		o.modalonly = true;
		o.rmempty = true;

		return m.render();
	}
});
