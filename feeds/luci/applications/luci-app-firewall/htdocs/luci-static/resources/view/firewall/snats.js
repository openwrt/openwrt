'use strict';
'require ui';
'require rpc';
'require uci';
'require form';
'require firewall as fwmodel';
'require tools.firewall as fwtool';
'require tools.widgets as widgets';

function rule_proto_txt(s) {
	var proto = L.toArray(uci.get('firewall', s, 'proto')).filter(function(p) {
		return (p != '*' && p != 'any' && p != 'all');
	}).map(function(p) {
		var pr = fwtool.lookupProto(p);
		return {
			num:  pr[0],
			name: pr[1]
		};
	});

	m = String(uci.get('firewall', s, 'mark')).match(/^(!\s*)?(0x[0-9a-f]{1,8}|[0-9]{1,10})(?:\/(0x[0-9a-f]{1,8}|[0-9]{1,10}))?$/i);
	var f = m ? {
		val:  m[0].toUpperCase().replace(/X/g, 'x'),
		inv:  m[1],
		num:  '0x%02X'.format(+m[2]),
		mask: m[3] ? '0x%02X'.format(+m[3]) : null
	} : null;

	return fwtool.fmt(_('Forwarded IPv4%{proto?, protocol %{proto#%{next?, }<var>%{item.name}</var>}}%{mark?, mark <var%{mark.inv? data-tooltip="Match fwmarks except %{mark.num}%{mark.mask? with mask %{mark.mask}}.":%{mark.mask? data-tooltip="Mask fwmark value with %{mark.mask} before compare."}}>%{mark.val}</var>}'), {
		proto: proto,
		mark:  f
	});
}

function rule_src_txt(s, hosts) {
	var z = uci.get('firewall', s, 'src');

	return fwtool.fmt(_('From %{src}%{src_device?, interface <var>%{src_device}</var>}%{src_ip?, IP %{src_ip#%{next?, }<var%{item.inv? data-tooltip="Match IP addresses except %{item.val}."}>%{item.ival}</var>}}%{src_port?, port %{src_port#%{next?, }<var%{item.inv? data-tooltip="Match ports except %{item.val}."}>%{item.ival}</var>}}'), {
		src: E('span', { 'class': 'zonebadge', 'style': 'background-color:' + fwmodel.getColorForName(null) }, [E('em', _('any zone'))]),
		src_ip: fwtool.map_invert(uci.get('firewall', s, 'src_ip'), 'toLowerCase'),
		src_port: fwtool.map_invert(uci.get('firewall', s, 'src_port'))
	});
}

function rule_dest_txt(s) {
	var z = uci.get('firewall', s, 'src');

	return fwtool.fmt(_('To %{dest}%{dest_device?, via interface <var>%{dest_device}</var>}%{dest_ip?, IP %{dest_ip#%{next?, }<var%{item.inv? data-tooltip="Match IP addresses except %{item.val}."}>%{item.ival}</var>}}%{dest_port?, port %{dest_port#%{next?, }<var%{item.inv? data-tooltip="Match ports except %{item.val}."}>%{item.ival}</var>}}'), {
		dest: E('span', { 'class': 'zonebadge', 'style': 'background-color:' + fwmodel.getColorForName((z && z != '*') ? z : null) }, [(z == '*') ? E('em', _('any zone')) : (z || E('em', _('this device')))]),
		dest_ip: fwtool.map_invert(uci.get('firewall', s, 'dest_ip'), 'toLowerCase'),
		dest_port: fwtool.map_invert(uci.get('firewall', s, 'dest_port')),
		dest_device: uci.get('firewall', s, 'device')
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
	var t = uci.get('firewall', s, 'target'),
	    s = {
	    	target:    t,
	    	snat_ip:   uci.get('firewall', s, 'snat_ip'),
	    	snat_port: uci.get('firewall', s, 'snat_port')
	    };

	switch (t) {
	case 'SNAT':
		return fwtool.fmt(_('<var data-tooltip="SNAT">Statically rewrite</var> to source %{snat_ip?IP <var>%{snat_ip}</var>} %{snat_port?port <var>%{snat_port}</var>}'), s);

	case 'MASQUERADE':
		return fwtool.fmt(_('<var data-tooltip="MASQUERADE">Automatically rewrite</var> source IP'));

	case 'ACCEPT':
		return fwtool.fmt(_('<var data-tooltip="ACCEPT">Prevent source rewrite</var>'));

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

	callNetworkDevices: rpc.declare({
		object: 'luci-rpc',
		method: 'getNetworkDevices',
		expect: { '': {} }
	}),

	load: function() {
		return Promise.all([
			this.callHostHints(),
			this.callNetworkDevices(),
			uci.load('firewall')
		]);
	},

	render: function(data) {
		if (fwtool.checkLegacySNAT())
			return fwtool.renderMigration();
		else
			return this.renderNats(data);
	},

	renderNats: function(data) {
		var hosts = data[0],
		    devs = data[1],
		    m, s, o;

		m = new form.Map('firewall', _('Firewall - NAT Rules'),
			_('NAT rules allow fine grained control over the source IP to use for outbound or forwarded traffic.'));

		s = m.section(form.GridSection, 'nat', _('NAT Rules'));
		s.addremove = true;
		s.anonymous = true;
		s.sortable  = true;

		s.tab('general', _('General Settings'));
		s.tab('advanced', _('Advanced Settings'));
		s.tab('timed', _('Time Restrictions'));

		s.sectiontitle = function(section_id) {
			return uci.get('firewall', section_id, 'name') || _('Unnamed NAT');
		};

		o = s.taboption('general', form.Value, 'name', _('Name'));
		o.placeholder = _('Unnamed NAT');
		o.modalonly = true;

		o = s.option(form.DummyValue, '_match', _('Match'));
		o.modalonly = false;
		o.textvalue = function(s) {
			return E('small', [
				rule_proto_txt(s), E('br'),
				rule_src_txt(s, hosts), E('br'),
				rule_dest_txt(s), E('br'),
				rule_limit_txt(s)
			]);
		};

		o = s.option(form.ListValue, '_target', _('Action'));
		o.modalonly = false;
		o.textvalue = function(s) {
			return rule_target_txt(s);
		};

		o = s.option(form.Flag, 'enabled', _('Enable'));
		o.modalonly = false;
		o.default = o.enabled;
		o.editable = true;

		o = s.taboption('general', fwtool.CBIProtocolSelect, 'proto', _('Protocol'));
		o.modalonly = true;
		o.default = 'all';

		o = s.taboption('general', widgets.ZoneSelect, 'src', _('Outbound zone'));
		o.modalonly = true;
		o.rmempty = false;
		o.nocreate = true;
		o.allowany = true;
		o.default = 'lan';

		o = fwtool.addIPOption(s, 'general', 'src_ip', _('Source address'),
			_('Match forwarded traffic from this IP or range.'), 'ipv4', hosts);
		o.rmempty = true;
		o.datatype = 'neg(ipmask4)';

		o = s.taboption('general', form.Value, 'src_port', _('Source port'),
			_('Match forwarded traffic originating from the given source port or port range.'));
		o.modalonly = true;
		o.rmempty = true;
		o.datatype = 'neg(portrange)';
		o.placeholder = _('any');
		o.depends({ proto: 'tcp', '!contains': true });
		o.depends({ proto: 'udp', '!contains': true });

		o = fwtool.addIPOption(s, 'general', 'dest_ip', _('Destination address'),
			_('Match forwarded traffic directed at the given IP address.'), 'ipv4', hosts);
		o.rmempty = true;
		o.datatype = 'neg(ipmask4)';

		o = s.taboption('general', form.Value, 'dest_port', _('Destination port'),
			_('Match forwarded traffic directed at the given destination port or port range.'));
		o.modalonly = true;
		o.rmempty = true;
		o.placeholder = _('any');
		o.datatype = 'neg(portrange)';
		o.depends({ proto: 'tcp', '!contains': true });
		o.depends({ proto: 'udp', '!contains': true });

		o = s.taboption('general', form.ListValue, 'target', _('Action'));
		o.modalonly = true;
		o.default = 'SNAT';
		o.value('SNAT', _('SNAT - Rewrite to specific source IP or port'));
		o.value('MASQUERADE', _('MASQUERADE - Automatically rewrite to outbound interface IP'));
		o.value('ACCEPT', _('ACCEPT - Disable address rewriting'));

		o = fwtool.addLocalIPOption(s, 'general', 'snat_ip', _('Rewrite IP address'),
			_('Rewrite matched traffic to the specified source IP address.'), devs);
		o.placeholder = null;
		o.depends('target', 'SNAT');
		o.validate = function(section_id, value) {
			var port = this.map.lookupOption('snat_port', section_id),
			    a = this.formvalue(section_id),
			    p = port ? port[0].formvalue(section_id) : null;

			if ((a == null || a == '') && (p == null || p == '') && value == '')
				return _('A rewrite IP must be specified!');

			return true;
		};

		o = s.taboption('general', form.Value, 'snat_port', _('Rewrite port'),
			_('Rewrite matched traffic to the specified source port or port range.'));
		o.modalonly = true;
		o.rmempty = true;
		o.placeholder = _('do not rewrite');
		o.datatype = 'portrange';
		o.depends({ proto: 'tcp', '!contains': true });
		o.depends({ proto: 'udp', '!contains': true });

		o = s.taboption('advanced', widgets.DeviceSelect, 'device', _('Outbound device'),
			_('Matches forwarded traffic using the specified outbound network device.'));
		o.noaliases = true;
		o.modalonly = true;
		o.rmempty = true;

		fwtool.addMarkOption(s, false);
		fwtool.addLimitOption(s);
		fwtool.addLimitBurstOption(s);

		o = s.taboption('advanced', form.Value, 'extra', _('Extra arguments'),
			_('Passes additional arguments to iptables. Use with care!'));
		o.modalonly = true;
		o.rmempty = true;

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
