'use strict';
'require fs';
'require ui';

return L.view.extend({
	load: function() {
		return L.resolveDefault(fs.read('/etc/firewall.user'), '');
	},

	handleSave: function(ev) {
		var value = (document.querySelector('textarea').value || '').trim().replace(/\r\n/g, '\n') + '\n';

		return fs.write('/etc/firewall.user', value).then(function(rc) {
			document.querySelector('textarea').value = value;
			ui.addNotification(null, E('p', _('Contents have been saved.')), 'info');
			fs.exec('/etc/init.d/firewall', ['restart']);
		}).catch(function(e) {
			ui.addNotification(null, E('p', _('Unable to save contents: %s').format(e.message)));
		});
	},

	render: function(fwuser) {
		return E([
			E('h2', _('Firewall - Custom Rules')),
			E('p', {}, _('Custom rules allow you to execute arbitrary iptables commands which are not otherwise covered by the firewall framework. The commands are executed after each firewall restart, right after the default ruleset has been loaded.')),
			E('p', {}, E('textarea', { 'style': 'width:100%', 'rows': 10 }, [ fwuser != null ? fwuser : '' ]))
		]);
	},

	handleSaveApply: null,
	handleReset: null
});
