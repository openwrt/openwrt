'use strict';
'require dom';
'require form';
'require fs';
'require ui';
'require view';

var nfcMap;
var nfcEnable;
var nfcMode;
var nfcUrl;

function normalizeNfcUrl(url) {
	url = (url || '').trim();

	return url || 'auto';
}

function readNfcForm() {
	var enabled = nfcEnable.formvalue('nfc');
	var mode = nfcMode.formvalue('nfc') || 'url';
	var url = normalizeNfcUrl(nfcUrl.formvalue('nfc'));

	return {
		enabled: enabled == '1',
		mode: mode,
		url: url
	};
}

function writeNfcTag(values) {
	if (!values.enabled || values.mode == 'disable')
		return fs.exec('/sbin/nfc', [ 'disable' ]);

	if (values.mode == 'wifi')
		return fs.exec('/sbin/nfc', [ 'update-wifi' ]);

	return fs.exec('/sbin/nfc', [ 'update-url', values.url ]);
}

function saveAndWriteTag() {
	var values = readNfcForm();

	return nfcMap.save().then(function() {
		return writeNfcTag(values);
	}).then(function() {
		ui.addNotification(null, E('p', _('NFC tag updated.')), 'info');
	}).catch(function(e) {
		ui.addNotification(null, E('p', _('Failed to update NFC tag: %s').format(e.message)), 'error');
	});
}

return view.extend({
	render: function() {
		var m, s, o;

		m = new form.Map('nfc', _('NFC'), _('Configure the router NFC tag payload.'));
		nfcMap = m;

		s = m.section(form.NamedSection, 'nfc', 'nfc', _('Settings'));
		s.anonymous = true;

		o = s.option(form.Flag, 'nfc_enable', _('Enable'));
		o.rmempty = false;
		o.default = '1';
		nfcEnable = o;

		o = s.option(form.ListValue, 'mode', _('Payload'));
		o.value('url', _('Router URL'));
		o.value('wifi', _('Wi-Fi credentials'));
		o.value('disable', _('Disabled tag'));
		o.default = 'url';
		o.rmempty = false;
		nfcMode = o;

		o = s.option(form.Value, 'url', _('URL'));
		o.placeholder = 'https://192.168.1.1/';
		o.default = '';
		o.rmempty = true;
		o.depends('mode', 'url');
		o.load = function(section_id) {
			var value = this.map.data.get(this.map.config, section_id, this.option);

			return value == 'auto' ? '' : value;
		};
		o.write = function(section_id, value) {
			value = normalizeNfcUrl(value);

			return this.map.data.set(this.map.config, section_id, this.option, value);
		};
		o.remove = function(section_id) {
			return this.map.data.set(this.map.config, section_id, this.option, 'auto');
		};
		o.validate = function(section_id, value) {
			value = (value || '').trim();

			if (value == '')
				return true;

			if (!value.match(/^https?:\/\/[^\/\s?#]+(?:[\/?#][^\s]*)?$/i))
				return _('Enter a valid http:// or https:// URL.');

			return true;
		};
		nfcUrl = o;

		o = s.option(form.Button, '_write', _('Write tag'));
		o.inputtitle = _('Write now');
		o.inputstyle = 'apply';
		o.onclick = saveAndWriteTag;

		return m.render();
	},

	handleSave: function() {
		return saveAndWriteTag();
	},

	handleSaveApply: function(ev, mode) {
		return this.handleSave(ev).then(function() {
			ui.changes.apply(mode == '0');
		});
	},

	handleReset: function() {
		var map = document.querySelector('.cbi-map');

		return dom.callClassMethod(map, 'reset');
	}
});
