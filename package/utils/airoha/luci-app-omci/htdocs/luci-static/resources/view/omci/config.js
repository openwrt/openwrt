'use strict';
'require view';
'require form';
'require uci';
'require ui';
'require omci.rpc as omci';

return view.extend({
	load: function() {
		return Promise.all([ uci.load('omci'), omci.family() ]);
	},

	render: function(data) {
		var m, s, o;
		m = new form.Map('omci', _('OMCI configuration'),
			_('Persistent userspace overrides for the in-kernel OMCI agent. Empty identity fields leave the value supplied by the kernel, device tree, NVMEM or driver unchanged.'));

		s = m.section(form.NamedSection, 'main', 'agent', _('Agent'));
		s.anonymous = true;
		s.addremove = false;

		o = s.option(form.Value, 'device', _('OMCI device'));
		o.datatype = 'uinteger';
		o.default = '0';

		o = s.option(form.Flag, 'enabled', _('Enable baseline agent'));
		o.default = o.enabled;

		o = s.option(form.Flag, 'permissive', _('Permissive mode'));
		o.default = o.enabled;
		o.description = _('Allow interoperability fallbacks for OLT behavior not covered by the strict standard path.');

		o = s.option(form.Flag, 'fake_omci', _('Fake unsupported OMCI responses'));
		o.default = o.disabled;

		o = s.option(form.Flag, 'dying_gasp', _('Dying gasp'));
		o.default = o.enabled;

		o = s.option(form.ListValue, 'olt_profile', _('OLT profile'));
		[ ['auto', _('Auto detect')], ['generic', _('Generic')], ['nokia-alcl', _('Nokia / Alcatel-Lucent')],
		  ['dasan', _('DASAN')], ['huawei', _('Huawei')], ['fiberhome', _('FiberHome')], ['zte', _('ZTE')],
		  ['unspecified', _('Unspecified')] ].forEach(function(v) { o.value(v[0], v[1]); });
		o.default = 'auto';

		o = s.option(form.ListValue, 'olt_profile_force', _('Force OLT profile'));
		[ ['unspecified', _('Do not force')], ['generic', _('Generic')], ['nokia-alcl', _('Nokia / Alcatel-Lucent')],
		  ['dasan', _('DASAN')], ['huawei', _('Huawei')], ['fiberhome', _('FiberHome')], ['zte', _('ZTE')] ].forEach(function(v) { o.value(v[0], v[1]); });
		o.default = 'unspecified';

		s = m.section(form.NamedSection, 'main', 'agent', _('ONU identity'));
		s.anonymous = true;
		s.addremove = false;

		o = s.option(form.Value, 'serial', _('GPON serial number'));
		o.datatype = 'maxlength(32)';
		o.placeholder = 'MSTC12345678';
		o.description = _('Accepted by the kernel as the configured serial representation.');

		o = s.option(form.Value, 'vendor_id', _('Vendor ID'));
		o.datatype = 'maxlength(16)';
		o.placeholder = 'MSTC';

		o = s.option(form.Value, 'hardware_version', _('Hardware version'));
		o.datatype = 'maxlength(14)';

		o = s.option(form.Value, 'software_version_0', _('Software image 0 version'));
		o.datatype = 'maxlength(14)';

		o = s.option(form.Value, 'software_version_1', _('Software image 1 version'));
		o.datatype = 'maxlength(14)';

		o = s.option(form.Value, 'equipment_id', _('Equipment ID'));
		o.datatype = 'maxlength(20)';

		o = s.option(form.Value, 'password', _('SLID / GPON password'));
		o.password = true;
		o.datatype = 'maxlength(32)';

		o = s.option(form.Value, 'omcc_version', _('OMCC version'));
		o.placeholder = '0xA1';
		o.description = _('Decimal or hexadecimal byte value. Leave empty to retain the current kernel value.');

		o = s.option(form.Value, 'traffic_management_option', _('Traffic management option'));
		o.datatype = 'range(0,255)';

		o = s.option(form.Value, 'onu_type', _('ONU type'));
		o.datatype = 'range(0,255)';

		o = s.option(form.Value, 'uni_count', _('UNI count'));
		o.datatype = 'range(0,255)';

		s = m.section(form.NamedSection, 'main', 'agent', _('Apply'));
		s.anonymous = true;
		s.addremove = false;
		o = s.option(form.Button, '_apply_kernel', _('Apply saved configuration to OMCI'));
		o.inputstyle = 'apply';
		o.description = _('Save the UCI form first, then use this button to push all non-empty overrides through rpcd and the native C binding.');
		o.onclick = function() {
			return omci.apply(0).then(function() {
				ui.addNotification(null, E('p', {}, _('OMCI configuration applied successfully.')));
			}).catch(function(err) {
				ui.addNotification(null, E('p', {}, _('Failed to apply OMCI configuration: %s').format(err)), 'error');
			});
		};

		return m.render();
	}
});
