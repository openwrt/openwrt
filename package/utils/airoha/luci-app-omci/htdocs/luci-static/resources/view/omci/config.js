'use strict';
'require view';
'require form';

function validateByte(sectionId, value) {
	var parsed;

	if (value == null || value === '')
		return true;

	if (/^0[xX][0-9a-fA-F]+$/.test(value))
		parsed = parseInt(value.substring(2), 16);
	else if (/^[0-9]+$/.test(value))
		parsed = parseInt(value, 10);
	else
		return _('Enter a decimal byte or a hexadecimal value such as 0x80.');

	return parsed >= 1 && parsed <= 255 ? true :
		_('The value must be between 1 and 255 (0x01–0xff).');
}

function addProfileValues(option, allowAuto, allowUnspecified) {
	if (allowUnspecified)
		option.value('0', _('None'));

	option.value('1', _('Generic'));
	if (allowAuto)
		option.value('2', _('Automatic detection'));
	option.value('3', _('Nokia / Alcatel-Lucent'));
	option.value('4', _('DASAN'));
	option.value('5', _('Huawei'));
	option.value('6', _('FiberHome'));
	option.value('7', _('ZTE'));
}

return view.extend({
	render: function() {
		var m = new form.Map('omci', _('OMCI Agent'),
			_('Persistent settings are applied to the in-kernel agent through its RPC service. Identity and interoperability changes are propagated to the xPON driver at runtime.'));
		var s = m.section(form.NamedSection, 'main', 'agent', _('Agent settings'));
		var o;

		o = s.option(form.Value, 'device', _('Device ID'));
		o.datatype = 'uinteger';
		o.default = '0';

		o = s.option(form.Flag, 'enabled', _('Enable kernel agent'));
		o.default = o.enabled;

		o = s.option(form.Flag, 'permissive', _('Permissive unknown ME handling'));
		o.default = o.enabled;
		o.description = _('Unknown managed entities are stored as opaque MIB objects when possible.');

		o = s.option(form.Flag, 'fake_omci', _('FAKE OMCI compatibility mode'));
		o.default = o.disabled;
		o.description = _('Acknowledge selected unsupported or inconsistent OMCI operations as successful. Hardware, parameter and VLAN table errors are not hidden. Enable only for OLT compatibility testing.');

		o = s.option(form.Flag, 'dying_gasp', _('Send OMCI dying-gasp alarm'));
		o.default = o.disabled;
		o.description = _('When the xPON dying-gasp interrupt is raised, send the ONU-G alarm notification to the OLT before shutdown.');

		o = s.option(form.ListValue, 'olt_profile', _('OLT interoperability profile'));
		o.value('', _('Keep kernel/default value'));
		addProfileValues(o, true, false);
		o.description = _('Select a standards-oriented profile, automatically detect the OLT vendor from OLT-G, or use a vendor-specific interoperability policy.');

		o = s.option(form.ListValue, 'olt_profile_force', _('Force effective OLT profile'));
		o.value('', _('Keep current kernel value'));
		addProfileValues(o, false, true);
		o.description = _('Overrides automatic detection. Select “None” to clear a previously forced profile and return to the configured profile policy.');

		o = s.option(form.Flag, 'reset_mib', _('Reset operational MIB on reload'));
		o.default = o.disabled;

		o = s.option(form.Value, 'serial', _('Serial number'));
		o.placeholder = 'INVP12345678';
		o.description = _('Accepted forms: four-character vendor ID plus eight hexadecimal digits, sixteen hexadecimal digits, or an eight-byte raw value. Leave empty to retain the value supplied by NVMEM, the U-Boot environment, Device Tree or the hardware driver.');

		o = s.option(form.Value, 'vendor_id', _('Vendor ID'));
		o.datatype = 'maxlength(16)';
		o.placeholder = 'INVP';
		o.description = _('Accepted as four raw/ASCII bytes or eight hexadecimal digits.');

		o = s.option(form.Value, 'hardware_version', _('Hardware version'));
		o.datatype = 'maxlength(14)';
		o.placeholder = 'GG-GAPL100v02';
		o.description = _('Advertised by ONU-G and equipment inventory managed entities.');

		o = s.option(form.Value, 'software_version_0', _('Software image 0 version'));
		o.datatype = 'maxlength(14)';
		o.placeholder = 'GG-11000-C003';
		o.description = _('Version advertised by Software Image entity 0.');

		o = s.option(form.Value, 'software_version_1', _('Software image 1 version'));
		o.datatype = 'maxlength(14)';
		o.placeholder = 'GG-11000-C003';
		o.description = _('Version advertised by Software Image entity 1.');

		o = s.option(form.Value, 'version', _('Legacy combined version'));
		o.datatype = 'maxlength(14)';
		o.description = _('Compatibility option from package versions before 0.7.0. It sets the hardware version and both software image versions first; the three explicit fields above override it.');

		o = s.option(form.Value, 'equipment_id', _('Equipment ID'));
		o.datatype = 'maxlength(20)';

		o = s.option(form.Value, 'password', _('PLOAM password'));
		o.password = true;
		o.datatype = 'maxlength(32)';
		o.description = _('Accepted as up to ten ASCII bytes or twenty hexadecimal digits.');

		o = s.option(form.Value, 'omcc_version', _('OMCC version'));
		o.validate = validateByte;
		o.placeholder = '0x80';
		o.description = _('Byte advertised by ONU2-G. Decimal and hexadecimal forms are accepted, for example 128 or 0x80. Common values are 0x80 and 0xa0. Leave empty to retain the kernel or firmware value.');

		o = s.option(form.Value, 'traffic_management_option', _('Traffic management option'));
		o.datatype = 'range(0,255)';

		o = s.option(form.Value, 'onu_type', _('ONU type'));
		o.datatype = 'range(0,255)';

		o = s.option(form.Value, 'uni_count', _('UNI count'));
		o.datatype = 'range(0,255)';

		var ms = m.section(form.GridSection, 'mib', _('Persistent MIB overrides'));
		ms.addremove = true;
		ms.anonymous = true;
		ms.description = _('These entries are replayed into the operational kernel MIB whenever the service reloads.');

		o = ms.option(form.Flag, 'enabled', _('Enabled'));
		o.default = o.enabled;
		o = ms.option(form.Value, 'class_id', _('Class ID'));
		o.datatype = 'range(0,65535)';
		o.rmempty = false;
		o = ms.option(form.Value, 'entity_id', _('Entity ID'));
		o.datatype = 'range(0,65535)';
		o.rmempty = false;
		o = ms.option(form.Value, 'attr_mask', _('Attribute mask'));
		o.datatype = 'range(0,65535)';
		o.rmempty = false;
		o = ms.option(form.Value, 'data', _('Attribute data'));
		o.datatype = 'hexstring';
		o.rmempty = false;

		return m.render();
	}
});
