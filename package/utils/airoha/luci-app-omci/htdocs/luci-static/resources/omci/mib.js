'use strict';
'require baseclass';
'require ui';
'require omci.rpc as omci';

var groups = {
	inventory: function(c) { return c.category_name === 'equipment' || c.category_name === 'management'; },
	gpon: function(c) { return c.category_name === 'ani'; },
	bridge: function(c) { return c.category_name === 'uni' || c.category_name === 'layer2' || !!(c.flags & (1 << 6)); },
	multicast: function(c) { return c.category_name === 'multicast'; },
	other: function(c) {
		return c.category_name !== 'equipment' && c.category_name !== 'management' &&
		       c.category_name !== 'ani' && c.category_name !== 'uni' &&
		       c.category_name !== 'layer2' && c.category_name !== 'multicast' &&
		       !(c.flags & (1 << 6));
	}
};

function load() {
	return Promise.all([ omci.classList(0), omci.mibList(0) ]);
}

function parseUint(value, max) {
	value = String(value || '').trim();
	if (!/^(?:0[xX][0-9a-fA-F]+|[0-9]+)$/.test(value))
		return null;

	var number = Number(value);
	if (!Number.isInteger(number) || number < 0 || number > max)
		return null;

	return number;
}

function normalizeHex(value) {
	value = String(value || '').trim();
	if (/^0[xX]/.test(value))
		value = value.substring(2);
	if (!/^(?:[0-9a-fA-F]{2})*$/.test(value) || value.length > 128)
		return null;
	return value.toLowerCase();
}

function field(label, node, description) {
	return E('div', { 'class': 'cbi-value' }, [
		E('label', { 'class': 'cbi-value-title' }, label),
		E('div', { 'class': 'cbi-value-field' }, [
			node,
			description ? E('div', { 'class': 'cbi-value-description' }, description) : ''
		])
	]);
}

function notifyError(operation, error) {
	ui.addNotification(null,
		E('p', {}, _('%s failed: %s').format(operation, error?.message || error)),
		'error');
}

function render(group, data, title, description) {
	var classes = data[0] || [];
	var objects = data[1] || [];
	var byId = {};
	classes.forEach(function(c) { byId[c.class_id] = c; });
	var predicate = groups[group] || groups.other;
	var groupClasses = classes.filter(predicate).sort(function(a, b) {
		return Number(a.class_id) - Number(b.class_id);
	});
	var originLabels = {
		'default': _('Default / seed'),
		'local': _('Local'),
		'olt': _('Provisioned by OLT'),
		'hardware': _('Hardware')
	};
	var originFilter = E('select', { 'class': 'cbi-input-select' }, [
		E('option', { 'value': 'all' }, _('All origins')),
		E('option', { 'value': 'default' }, _('Default / seed')),
		E('option', { 'value': 'olt' }, _('Provisioned by OLT')),
		E('option', { 'value': 'local' }, _('Local edits')),
		E('option', { 'value': 'hardware' }, _('Hardware'))
	]);
	var tableRoot = E('div');
	var count = E('span', { 'style': 'margin-left:1em' });

	function reloadObjects() {
		return omci.mibList(0).then(function(result) {
			objects = result || [];
			renderTable();
		});
	}

	function showEditor(object) {
		var creating = object == null;
		var classNode;

		if (creating) {
			classNode = E('select', { 'class': 'cbi-input-select' }, groupClasses.map(function(c) {
				return E('option', { 'value': String(c.class_id) },
					'%d — %s'.format(c.class_id, c.name || _('unnamed class')));
			}));
		}
		else {
			classNode = E('input', {
				'class': 'cbi-input-text',
				'type': 'text',
				'value': String(object.class_id),
				'readonly': 'readonly'
			});
		}

		var entityNode = E('input', {
			'class': 'cbi-input-text',
			'type': 'text',
			'value': creating ? '' : '0x' + Number(object.entity_id || 0).toString(16).padStart(4, '0'),
			'placeholder': '0x0000',
			'readonly': creating ? null : 'readonly'
		});
		var maskNode = E('input', {
			'class': 'cbi-input-text',
			'type': 'text',
			'value': creating ? '0xffff' : '0x' + Number(object.attr_mask || 0).toString(16).padStart(4, '0'),
			'placeholder': '0xffff'
		});
		var dataNode = E('textarea', {
			'class': 'cbi-input-text',
			'rows': 5,
			'style': 'width:100%;font-family:monospace',
			'placeholder': _('Hex bytes without separators (maximum 64 bytes)')
		}, creating ? '' : (object.data || ''));

		ui.showModal(creating ? _('Create MIB object') : _('Edit MIB object'), [
			E('p', {}, creating ?
				_('Creation uses the existing MIB set RPC. The class must be active in the kernel catalog.') :
				_('Saving writes the object through the existing MIB set RPC and records its origin as local.')),
			field(_('Class'), classNode),
			field(_('Entity ID'), entityNode, _('Decimal or hexadecimal value from 0 to 65535.')),
			field(_('Attribute mask'), maskNode, _('Decimal or hexadecimal value from 0 to 65535.')),
			field(_('Attribute data'), dataNode, _('An even number of hexadecimal digits; 0x prefix is optional.')),
			E('div', { 'class': 'right' }, [
				E('button', { 'class': 'btn', 'click': ui.hideModal }, _('Cancel')),
				' ',
				E('button', {
					'class': 'btn cbi-button-positive important',
					'click': function() {
						var classId = parseUint(classNode.value, 0xffff);
						var entityId = parseUint(entityNode.value, 0xffff);
						var mask = parseUint(maskNode.value, 0xffff);
						var hex = normalizeHex(dataNode.value);

						if (classId == null || entityId == null || mask == null || hex == null) {
							ui.addNotification(null, E('p', {},
								_('Invalid class, entity, mask, or attribute data. Data is limited to 64 bytes of hexadecimal.')), 'error');
							return;
						}

						return omci.mibSet(0, classId, entityId, mask, hex).then(function() {
							ui.hideModal();
							ui.addNotification(null, E('p', {}, creating ?
								_('MIB object created successfully.') : _('MIB object updated successfully.')));
							return reloadObjects();
						}).catch(function(error) { notifyError(_('MIB write'), error); });
					}
				}, creating ? _('Create') : _('Save'))
			])
		]);
	}

	function showDelete(object) {
		ui.showModal(_('Delete MIB object'), [
			E('p', {}, _('Delete class %d, entity 0x%s? This operation updates the live kernel MIB and cannot be undone except by recreating or resetting the object.').format(
				object.class_id, Number(object.entity_id || 0).toString(16).padStart(4, '0'))),
			E('div', { 'class': 'right' }, [
				E('button', { 'class': 'btn', 'click': ui.hideModal }, _('Cancel')),
				' ',
				E('button', {
					'class': 'btn cbi-button-negative important',
					'click': function() {
						return omci.mibDelete(0, object.class_id, object.entity_id).then(function() {
							ui.hideModal();
							ui.addNotification(null, E('p', {}, _('MIB object deleted successfully.')));
							return reloadObjects();
						}).catch(function(error) { notifyError(_('MIB delete'), error); });
					}
				}, _('Delete'))
			])
		]);
	}

	function renderTable() {
		var filtered = objects.filter(function(o) {
			var c = byId[o.class_id];
			return c && predicate(c) &&
				(originFilter.value === 'all' || o.origin_name === originFilter.value);
		});
		count.textContent = _('%d object(s)').format(filtered.length);

		if (!filtered.length) {
			tableRoot.replaceChildren(E('div', { 'class': 'alert-message notice' },
				_('No MIB objects match this category and origin filter.')));
			return;
		}

		var table = E('table', { 'class': 'table' }, [
			E('tr', { 'class': 'tr table-titles' }, [
				E('th', { 'class': 'th' }, _('Class')),
				E('th', { 'class': 'th' }, _('Entity')),
				E('th', { 'class': 'th' }, _('Managed entity')),
				E('th', { 'class': 'th' }, _('Origin')),
				E('th', { 'class': 'th' }, _('Mask')),
				E('th', { 'class': 'th' }, _('Data')),
				E('th', { 'class': 'th' }, _('Actions'))
			])
		]);

		filtered.forEach(function(o) {
			var c = byId[o.class_id] || {};
			table.appendChild(E('tr', { 'class': 'tr' }, [
				E('td', { 'class': 'td' }, String(o.class_id)),
				E('td', { 'class': 'td' }, '0x' + Number(o.entity_id || 0).toString(16).padStart(4, '0')),
				E('td', { 'class': 'td' }, o.name || c.name || '-'),
				E('td', { 'class': 'td' }, originLabels[o.origin_name] || o.origin_name || '-'),
				E('td', { 'class': 'td' }, '0x' + Number(o.attr_mask || 0).toString(16).padStart(4, '0')),
				E('td', { 'class': 'td', 'style': 'font-family:monospace;overflow-wrap:anywhere' }, o.data || ''),
				E('td', { 'class': 'td nowrap' }, [
					E('button', { 'class': 'btn cbi-button-action', 'click': function() { showEditor(o); } }, _('Edit')),
					' ',
					E('button', { 'class': 'btn cbi-button-negative', 'click': function() { showDelete(o); } }, _('Delete'))
				])
			]));
		});

		tableRoot.replaceChildren(table);
	}

	originFilter.addEventListener('change', renderTable);
	renderTable();

	return E([], [
		E('h2', {}, title),
		E('p', {}, description),
		E('p', {}, _('Object origin is reported explicitly by Linux OMCI UAPI v13; no userspace inference is used.')),
		E('div', { 'class': 'cbi-section' }, [
			E('div', { 'class': 'cbi-value' }, [
				E('label', { 'class': 'cbi-value-title' }, _('Origin filter')),
				E('div', { 'class': 'cbi-value-field' }, [ originFilter, count ])
			])
		]),
		tableRoot,
		E('div', { 'class': 'cbi-page-actions' }, [
			E('button', {
				'class': 'btn cbi-button-action',
				'disabled': groupClasses.length ? null : 'disabled',
				'click': function() { showEditor(null); }
			}, _('Create MIB object')),
			' ',
			E('button', { 'class': 'btn', 'click': reloadObjects }, _('Refresh'))
		])
	]);
}

return baseclass.extend({
	load: load,
	render: render
});
