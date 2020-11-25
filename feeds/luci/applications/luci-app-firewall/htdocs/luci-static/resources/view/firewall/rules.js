'use strict';
'require ui';
'require rpc';
'require uci';
'require form';
'require firewall as fwmodel';
'require tools.firewall as fwtool';
'require tools.widgets as widgets';

function rule_proto_txt(s, ctHelpers) {
	var f = (uci.get('firewall', s, 'family') || '').toLowerCase().replace(/^(?:any|\*)$/, '');

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
	var w = m ? {
		val:  m[0].toUpperCase().replace(/X/g, 'x'),
		inv:  m[1],
		num:  '0x%02X'.format(+m[2]),
		mask: m[3] ? '0x%02X'.format(+m[3]) : null
	} : null;

	m = String(uci.get('firewall', s, 'dscp')).match(/^(!\s*)?(?:(CS[0-7]|BE|AF[1234][123]|EF)|(0x[0-9a-f]{1,2}|[0-9]{1,2}))$/);
	var d = m ? {
		val:  m[0],
		inv:  m[1],
		name: m[2],
		num:  m[3] ? '0x%02X'.format(+m[3]) : null
	} : null;

	return fwtool.fmt(_('%{src?%{dest?Forwarded:Incoming}:Outgoing} %{ipv6?%{ipv4?<var>IPv4</var> and <var>IPv6</var>:<var>IPv6</var>}:<var>IPv4</var>}%{proto?, protocol %{proto#%{next?, }%{item.types?<var class="cbi-tooltip-container">%{item.name}<span class="cbi-tooltip">ICMP with types %{item.types#%{next?, }<var>%{item}</var>}</span></var>:<var>%{item.name}</var>}}}%{mark?, mark <var%{mark.inv? data-tooltip="Match fwmarks except %{mark.num}%{mark.mask? with mask %{mark.mask}}.":%{mark.mask? data-tooltip="Mask fwmark value with %{mark.mask} before compare."}}>%{mark.val}</var>}%{dscp?, DSCP %{dscp.inv?<var data-tooltip="Match DSCP classifications except %{dscp.num?:%{dscp.name}}">%{dscp.val}</var>:<var>%{dscp.val}</var>}}%{helper?, helper %{helper.inv?<var data-tooltip="Match any helper except &quot;%{helper.name}&quot;">%{helper.val}</var>:<var data-tooltip="%{helper.name}">%{helper.val}</var>}}'), {
		ipv4: (!f || f == 'ipv4'),
		ipv6: (!f || f == 'ipv6'),
		src:  uci.get('firewall', s, 'src'),
		dest: uci.get('firewall', s, 'dest'),
		proto: proto,
		helper: h,
		mark:   w,
		dscp:   d
	});
}

function rule_src_txt(s, hosts) {
	var z = uci.get('firewall', s, 'src'),
	    d = (uci.get('firewall', s, 'direction') == 'in') ? uci.get('firewall', s, 'device') : null;

	return fwtool.fmt(_('From %{src}%{src_device?, interface <var>%{src_device}</var>}%{src_ip?, IP %{src_ip#%{next?, }<var%{item.inv? data-tooltip="Match IP addresses except %{item.val}."}>%{item.ival}</var>}}%{src_port?, port %{src_port#%{next?, }<var%{item.inv? data-tooltip="Match ports except %{item.val}."}>%{item.ival}</var>}}%{src_mac?, MAC %{src_mac#%{next?, }<var%{item.inv? data-tooltip="Match MACs except %{item.val}%{item.hint.name? a.k.a. %{item.hint.name}}.":%{item.hint.name? data-tooltip="%{item.hint.name}"}}>%{item.ival}</var>}}'), {
		src: E('span', { 'class': 'zonebadge', 'style': 'background-color:' + fwmodel.getColorForName((z && z != '*') ? z : null) }, [(z == '*') ? E('em', _('any zone')) : (z || E('em', _('this device')))]),
		src_ip: fwtool.map_invert(uci.get('firewall', s, 'src_ip'), 'toLowerCase'),
		src_mac: fwtool.map_invert(uci.get('firewall', s, 'src_mac'), 'toUpperCase').map(function(v) { return Object.assign(v, { hint: hosts[v.val] }) }),
		src_port: fwtool.map_invert(uci.get('firewall', s, 'src_port')),
		src_device: d
	});
}

function rule_dest_txt(s) {
	var z = uci.get('firewall', s, 'dest'),
	    d = (uci.get('firewall', s, 'direction') == 'out') ? uci.get('firewall', s, 'device') : null;

	return fwtool.fmt(_('To %{dest}%{dest_device?, interface <var>%{dest_device}</var>}%{dest_ip?, IP %{dest_ip#%{next?, }<var%{item.inv? data-tooltip="Match IP addresses except %{item.val}."}>%{item.ival}</var>}}%{dest_port?, port %{dest_port#%{next?, }<var%{item.inv? data-tooltip="Match ports except %{item.val}."}>%{item.ival}</var>}}'), {
		dest: E('span', { 'class': 'zonebadge', 'style': 'background-color:' + fwmodel.getColorForName((z && z != '*') ? z : null) }, [(z == '*') ? E('em', _('any zone')) : (z || E('em', _('this device')))]),
		dest_ip: fwtool.map_invert(uci.get('firewall', s, 'dest_ip'), 'toLowerCase'),
		dest_port: fwtool.map_invert(uci.get('firewall', s, 'dest_port')),
		dest_device: d
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

function rule_target_txt(s, ctHelpers) {
	var t = uci.get('firewall', s, 'target'),
	    h = (uci.get('firewall', s, 'set_helper') || '').toUpperCase(),
	    s = {
	    	target: t,
	    	src:    uci.get('firewall', s, 'src'),
	    	dest:   uci.get('firewall', s, 'dest'),
	    	set_helper: h,
	    	set_mark:   uci.get('firewall', s, 'set_mark'),
	    	set_xmark:  uci.get('firewall', s, 'set_xmark'),
	    	set_dscp:   uci.get('firewall', s, 'set_dscp'),
	    	helper_name: (ctHelpers.filter(function(ctH) { return ctH.name.toUpperCase() == h })[0] || {}).description
	    };

	switch (t) {
	case 'DROP':
		return fwtool.fmt(_('<var data-tooltip="DROP">Drop</var> %{src?%{dest?forward:input}:output}'), s);

	case 'ACCEPT':
		return fwtool.fmt(_('<var data-tooltip="ACCEPT">Accept</var> %{src?%{dest?forward:input}:output}'), s);

	case 'REJECT':
		return fwtool.fmt(_('<var data-tooltip="REJECT">Reject</var> %{src?%{dest?forward:input}:output}'), s);

	case 'NOTRACK':
		return fwtool.fmt(_('<var data-tooltip="NOTRACK">Do not track</var> %{src?%{dest?forward:input}:output}'), s);

	case 'HELPER':
		return fwtool.fmt(_('<var data-tooltip="HELPER">Assign conntrack</var> helper <var%{helper_name? data-tooltip="%{helper_name}"}>%{set_helper}</var>'), s);

	case 'MARK':
		return fwtool.fmt(_('<var data-tooltip="MARK">%{set_mark?Assign:XOR}</var> firewall mark <var>%{set_mark?:%{set_xmark}}</var>'), s);

	case 'DSCP':
		return fwtool.fmt(_('<var data-tooltip="DSCP">Assign DSCP</var> classification <var>%{set_dscp}</var>'), s);

	default:
		return t;
	}
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

	load: function() {
		return Promise.all([
			this.callHostHints(),
			this.callConntrackHelpers(),
			uci.load('firewall')
		]);
	},

	render: function(data) {
		if (fwtool.checkLegacySNAT())
			return fwtool.renderMigration();
		else
			return this.renderRules(data);
	},

	renderRules: function(data) {
		var hosts = data[0],
		    ctHelpers = data[1],
		    m, s, o;

		m = new form.Map('firewall', _('Firewall - Traffic Rules'),
			_('Traffic rules define policies for packets traveling between different zones, for example to reject traffic between certain hosts or to open WAN ports on the router.'));

		s = m.section(form.GridSection, 'rule', _('Traffic Rules'));
		s.addremove = true;
		s.anonymous = true;
		s.sortable  = true;

		s.tab('general', _('General Settings'));
		s.tab('advanced', _('Advanced Settings'));
		s.tab('timed', _('Time Restrictions'));

		s.filter = function(section_id) {
			return (uci.get('firewall', section_id, 'target') != 'SNAT');
		};

		s.sectiontitle = function(section_id) {
			return uci.get('firewall', section_id, 'name') || _('Unnamed rule');
		};

		s.handleAdd = function(ev) {
			var config_name = this.uciconfig || this.map.config,
			    section_id = uci.add(config_name, this.sectiontype),
			    opt1, opt2;

			for (var i = 0; i < this.children.length; i++)
				if (this.children[i].option == 'src')
					opt1 = this.children[i];
				else if (this.children[i].option == 'dest')
					opt2 = this.children[i];

			opt1.default = 'wan';
			opt2.default = 'lan';

			this.addedSection = section_id;
			this.renderMoreOptionsModal(section_id);

			delete opt1.default;
			delete opt2.default;
		};

		o = s.taboption('general', form.Value, 'name', _('Name'));
		o.placeholder = _('Unnamed rule');
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

		o = s.option(form.ListValue, '_target', _('Action'));
		o.modalonly = false;
		o.textvalue = function(s) {
			return rule_target_txt(s, ctHelpers);
		};

		o = s.option(form.Flag, 'enabled', _('Enable'));
		o.modalonly = false;
		o.default = o.enabled;
		o.editable = true;


		o = s.taboption('advanced', form.ListValue, 'direction', _('Match device'));
		o.modalonly = true;
		o.value('', _('unspecified'));
		o.value('in', _('Inbound device'));
		o.value('out', _('Outbound device'));
		o.cfgvalue = function(section_id) {
			var val = uci.get('firewall', section_id, 'direction');
			switch (val) {
				case 'in':
				case 'ingress':
					return 'in';

				case 'out':
				case 'egress':
					return 'out';
			}

			return null;
		};

		o = s.taboption('advanced', widgets.DeviceSelect, 'device', _('Device name'),
			_('Specifies whether to tie this traffic rule to a specific inbound or outbound network device.'));
		o.modalonly = true;
		o.noaliases = true;
		o.rmempty = false;
		o.depends('direction', 'in');
		o.depends('direction', 'out');

		o = s.taboption('advanced', form.ListValue, 'family', _('Restrict to address family'));
		o.modalonly = true;
		o.rmempty = true;
		o.value('', _('IPv4 and IPv6'));
		o.value('ipv4', _('IPv4 only'));
		o.value('ipv6', _('IPv6 only'));
		o.validate = function(section_id, value) {
			fwtool.updateHostHints(this.map, section_id, 'src_ip', value, hosts);
			fwtool.updateHostHints(this.map, section_id, 'dest_ip', value, hosts);
			return true;
		};

		o = s.taboption('general', fwtool.CBIProtocolSelect, 'proto', _('Protocol'));
		o.modalonly = true;
		o.default = 'tcp udp';

		o = s.taboption('advanced', form.MultiValue, 'icmp_type', _('Match ICMP type'));
		o.modalonly = true;
		o.multiple = true;
		o.custom = true;
		o.cast = 'table';
		o.placeholder = _('any');
		o.value('', 'any');
		o.value('address-mask-reply');
		o.value('address-mask-request');
		o.value('communication-prohibited');
		o.value('destination-unreachable');
		o.value('echo-reply');
		o.value('echo-request');
		o.value('fragmentation-needed');
		o.value('host-precedence-violation');
		o.value('host-prohibited');
		o.value('host-redirect');
		o.value('host-unknown');
		o.value('host-unreachable');
		o.value('ip-header-bad');
		o.value('neighbour-advertisement');
		o.value('neighbour-solicitation');
		o.value('network-prohibited');
		o.value('network-redirect');
		o.value('network-unknown');
		o.value('network-unreachable');
		o.value('parameter-problem');
		o.value('port-unreachable');
		o.value('precedence-cutoff');
		o.value('protocol-unreachable');
		o.value('redirect');
		o.value('required-option-missing');
		o.value('router-advertisement');
		o.value('router-solicitation');
		o.value('source-quench');
		o.value('source-route-failed');
		o.value('time-exceeded');
		o.value('timestamp-reply');
		o.value('timestamp-request');
		o.value('TOS-host-redirect');
		o.value('TOS-host-unreachable');
		o.value('TOS-network-redirect');
		o.value('TOS-network-unreachable');
		o.value('ttl-zero-during-reassembly');
		o.value('ttl-zero-during-transit');
		o.depends({ proto: 'icmp', '!contains': true });
		o.depends({ proto: 'icmpv6', '!contains': true });

		o = s.taboption('general', widgets.ZoneSelect, 'src', _('Source zone'));
		o.modalonly = true;
		o.nocreate = true;
		o.allowany = true;
		o.allowlocal = 'src';

		fwtool.addMACOption(s, 'advanced', 'src_mac', _('Source MAC address'), null, hosts);
		fwtool.addIPOption(s, 'general', 'src_ip', _('Source address'), null, '', hosts, true);

		o = s.taboption('general', form.Value, 'src_port', _('Source port'));
		o.modalonly = true;
		o.datatype = 'list(neg(portrange))';
		o.placeholder = _('any');
		o.depends({ proto: 'tcp', '!contains': true });
		o.depends({ proto: 'udp', '!contains': true });

		o = s.taboption('general', widgets.ZoneSelect, 'dest', _('Destination zone'));
		o.modalonly = true;
		o.nocreate = true;
		o.allowany = true;
		o.allowlocal = true;

		fwtool.addIPOption(s, 'general', 'dest_ip', _('Destination address'), null, '', hosts, true);

		o = s.taboption('general', form.Value, 'dest_port', _('Destination port'));
		o.modalonly = true;
		o.datatype = 'list(neg(portrange))';
		o.placeholder = _('any');
		o.depends({ proto: 'tcp', '!contains': true });
		o.depends({ proto: 'udp', '!contains': true });

		o = s.taboption('general', form.ListValue, 'target', _('Action'));
		o.modalonly = true;
		o.default = 'ACCEPT';
		o.value('DROP', _('drop'));
		o.value('ACCEPT', _('accept'));
		o.value('REJECT', _('reject'));
		o.value('NOTRACK', _("don't track"));
		o.value('HELPER', _('assign conntrack helper'));
		o.value('MARK_SET', _('apply firewall mark'));
		o.value('MARK_XOR', _('XOR firewall mark'));
		o.value('DSCP', _('DSCP classification'));
		o.cfgvalue = function(section_id) {
			var t = uci.get('firewall', section_id, 'target'),
			    m = uci.get('firewall', section_id, 'set_mark');

			if (t == 'MARK')
				return m ? 'MARK_SET' : 'MARK_XOR';

			return t;
		};
		o.write = function(section_id, value) {
			return this.super('write', [section_id, (value == 'MARK_SET' || value == 'MARK_XOR') ? 'MARK' : value]);
		};

		fwtool.addMarkOption(s, 1);
		fwtool.addMarkOption(s, 2);
		fwtool.addDSCPOption(s, true);

		o = s.taboption('general', form.ListValue, 'set_helper', _('Tracking helper'), _('Assign the specified connection tracking helper to matched traffic.'));
		o.modalonly = true;
		o.placeholder = _('any');
		o.depends('target', 'HELPER');
		for (var i = 0; i < ctHelpers.length; i++)
			o.value(ctHelpers[i].name, '%s (%s)'.format(ctHelpers[i].description, ctHelpers[i].name.toUpperCase()));

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
		fwtool.addDSCPOption(s, false);
		fwtool.addLimitOption(s);
		fwtool.addLimitBurstOption(s);

		o = s.taboption('advanced', form.Value, 'extra', _('Extra arguments'),
			_('Passes additional arguments to iptables. Use with care!'));
		o.modalonly = true;

		o = s.taboption('timed', form.MultiValue, 'weekdays', _('Week Days'));
		o.modalonly = true;
		o.multiple = true;
		o.display = 5;
		o.placeholder = _('Any day');
		o.value('Sun', _('Sunday'));
		o.value('Mon', _('Monday'));
		o.value('Tue', _('Tuesday'));
		o.value('Wed', _('Wednesday'));
		o.value('Thu', _('Thursday'));
		o.value('Fri', _('Friday'));
		o.value('Sat', _('Saturday'));
		o.write = function(section_id, value) {
			return this.super('write', [ section_id, L.toArray(value).join(' ') ]);
		};

		o = s.taboption('timed', form.MultiValue, 'monthdays', _('Month Days'));
		o.modalonly = true;
		o.multiple = true;
		o.display_size = 15;
		o.placeholder = _('Any day');
		o.write = function(section_id, value) {
			return this.super('write', [ section_id, L.toArray(value).join(' ') ]);
		};
		for (var i = 1; i <= 31; i++)
			o.value(i);

		o = s.taboption('timed', form.Value, 'start_time', _('Start Time (hh.mm.ss)'));
		o.modalonly = true;
		o.datatype = 'timehhmmss';

		o = s.taboption('timed', form.Value, 'stop_time', _('Stop Time (hh.mm.ss)'));
		o.modalonly = true;
		o.datatype = 'timehhmmss';

		o = s.taboption('timed', form.Value, 'start_date', _('Start Date (yyyy-mm-dd)'));
		o.modalonly = true;
		o.datatype = 'dateyyyymmdd';

		o = s.taboption('timed', form.Value, 'stop_date', _('Stop Date (yyyy-mm-dd)'));
		o.modalonly = true;
		o.datatype = 'dateyyyymmdd';

		o = s.taboption('timed', form.Flag, 'utc_time', _('Time in UTC'));
		o.modalonly = true;
		o.default = o.disabled;

		return m.render();
	}
});
