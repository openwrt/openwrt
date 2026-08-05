'use strict';
'require view';
'require form';
'require uci';

return view.extend({
	load: function() {
		return uci.load('econet-xpon');
	},

	render: function() {
		var m, s, o;

		m = new form.Map('econet-xpon', _('GPON ONU Identity'),
			_('Identity used by the <em>econet-xpon</em> GPON driver. These values are ' +
			  'passed to the kernel module as parameters; the driver hardcodes nothing. ' +
			  'Changing them takes effect after the driver reloads (a reboot is the safe ' +
			  'way, since unloading the module while the fiber is up is not).'));

		s = m.section(form.NamedSection, 'identity', 'econet-xpon', _('ONU identity'));
		s.anonymous = true;
		s.addremove = false;

		o = s.option(form.Value, 'gpon_sn', _('GPON serial number'),
			_('4 vendor ASCII characters followed by 8 hex digits, e.g. <code>MTKG00000001</code>. ' +
			  'Leave empty to use the driver default. Read this from your unit\'s factory label/flash.'));
		o.placeholder = 'MTKG00000001';
		o.validate = function(section_id, value) {
			if (value == null || value == '')
				return true;
			if (!/^[\x20-\x7e]{4}[0-9A-Fa-f]{8}$/.test(value))
				return _('Expected 4 vendor characters + 8 hex digits (12 characters total)');
			return true;
		};

		o = s.option(form.Value, 'gpon_pw', _('PLOAM password'),
			_('GPON registration password (ASCII, up to 10 characters). Leave empty for none.'));
		o.password = true;
		o.validate = function(section_id, value) {
			if (value != null && value.length > 10)
				return _('Maximum 10 characters');
			return true;
		};

		o = s.option(form.Value, 'wan_mac', _('WAN MAC address'),
			_('MAC address for the <code>ponwan0</code> WAN interface. ' +
			  'Leave empty for a random locally-administered address.'));
		o.datatype = 'macaddr';
		o.placeholder = '02:00:00:00:00:01';

		return m.render();
	}
});
