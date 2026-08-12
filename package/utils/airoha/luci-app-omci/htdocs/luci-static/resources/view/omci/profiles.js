'use strict';
'require view';
'require ui';
'require omci.rpc as omci';

function notifyError(err) {
	ui.addNotification(null, E('p', {}, _('Failed to apply OMCI profile: %s').format(err)), 'error');
}

function profileById(profiles, id) {
	for (var i = 0; i < profiles.length; i++)
		if (profiles[i].id === id)
			return profiles[i];
	return null;
}

function renderFields(profile, root, nodes) {
	root.replaceChildren();
	Object.keys(nodes).forEach(function(k) { delete nodes[k]; });

	(profile.inputs || []).forEach(function(input) {
		var node = E('input', {
			'class': 'cbi-input-text',
			'type': input.name === 'password' ? 'password' : 'text',
			'placeholder': input.label || input.name
		});
		nodes[input.name] = node;
		root.appendChild(E('div', { 'class': 'cbi-value' }, [
			E('label', { 'class': 'cbi-value-title' }, input.label || input.name),
			E('div', { 'class': 'cbi-value-field' }, node)
		]));
	});
}

return view.extend({
	load: function() {
		return omci.profileList();
	},

	render: function(profiles) {
		profiles = profiles || [];
		if (!profiles.length)
			return E([], [ E('h2', {}, _('OMCI profiles')), E('p', {}, _('No built-in profiles are available.')) ]);

		var inputNodes = {};
		var select = E('select', { 'class': 'cbi-input-select' }, profiles.map(function(profile) {
			return E('option', { 'value': profile.id }, profile.name);
		}));
		var fields = E('div', { 'class': 'cbi-section' });
		var description = E('p', {}, profiles[0].description || '');

		function selectProfile() {
			var profile = profileById(profiles, select.value);
			if (!profile) return;
			description.textContent = profile.description || '';
			renderFields(profile, fields, inputNodes);
		}

		select.addEventListener('change', selectProfile);
		selectProfile();

		var apply = E('button', {
			'class': 'btn cbi-button-positive important',
			'click': function(ev) {
				ev.preventDefault();
				var profile = profileById(profiles, select.value);
				if (!profile) return;

				var serial = inputNodes.serial ? inputNodes.serial.value.trim() : '';
				var password = inputNodes.password ? inputNodes.password.value : '';
				for (var i = 0; i < (profile.inputs || []).length; i++) {
					var input = profile.inputs[i];
					if (input.required && (!inputNodes[input.name] || !inputNodes[input.name].value)) {
						ui.addNotification(null, E('p', {}, _('Required field is empty: %s').format(input.label || input.name)), 'error');
						return;
					}
				}

				return ui.showModal(_('Apply OMCI profile'), [
					E('p', {}, _('Apply “%s” and immediately push the resulting identity/configuration to the kernel OMCI agent?').format(profile.name)),
					E('div', { 'class': 'right' }, [
						E('button', { 'class': 'btn', 'click': ui.hideModal }, _('Cancel')),
						' ',
						E('button', {
							'class': 'btn cbi-button-positive important',
							'click': function() {
								return omci.profileApply(0, profile.id, serial, password).then(function() {
									ui.hideModal();
									ui.addNotification(null, E('p', {}, _('Profile “%s” applied successfully.').format(profile.name)));
								}).catch(notifyError);
							}
						}, _('Apply'))
					])
				]);
			}
		}, _('Apply profile'));

		return E([], [
			E('h2', {}, _('OMCI profiles')),
			E('p', {}, _('Built-in profiles are structured data handled by the rpcd ucode backend; no profile shell scripts or command wrappers are executed.')),
			E('div', { 'class': 'cbi-section' }, [
				E('div', { 'class': 'cbi-value' }, [
					E('label', { 'class': 'cbi-value-title' }, _('Profile')),
					E('div', { 'class': 'cbi-value-field' }, select)
				]),
				description
			]),
			fields,
			E('div', { 'class': 'cbi-page-actions' }, apply)
		]);
	},

	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
