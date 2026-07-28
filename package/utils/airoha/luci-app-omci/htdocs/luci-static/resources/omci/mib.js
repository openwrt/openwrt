'use strict';
'require baseclass';
'require view';
'require rpc';
'require ui';
'require uci';

var callMibList = rpc.declare({
	object: 'omci.agent', method: 'mib_list', params: [ 'device' ],
	expect: { objects: [] }, reject: true
});

var callMibGet = rpc.declare({
	object: 'omci.agent', method: 'mib_get',
	params: [ 'device', 'class_id', 'entity_id' ],
	expect: { object: {} }, reject: true
});

var callMibSet = rpc.declare({
	object: 'omci.agent', method: 'mib_set',
	params: [ 'device', 'class_id', 'entity_id', 'mask', 'data' ],
	expect: { success: false }, reject: true
});

var callMibDelete = rpc.declare({
	object: 'omci.agent', method: 'mib_delete',
	params: [ 'device', 'class_id', 'entity_id' ],
	expect: { success: false }, reject: true
});

var callMibReset = rpc.declare({
	object: 'omci.agent', method: 'mib_reset', params: [ 'device' ],
	expect: { success: false }, reject: true
});

var inventoryClasses = [ 2, 5, 6, 7, 131, 256, 257 ];
var gponClasses = [ 262, 263, 264, 265, 266, 267, 268, 269, 270, 271,
	272, 273, 274, 275, 277, 278 ];
var bridgeClasses = [ 11, 45, 47, 130, 134, 329 ];
var multicastClasses = [ 281, 309, 310, 311 ];

function number(value, fallback) {
	var parsed = Number(value);

	return Number.isFinite(parsed) ? parsed : fallback;
}

function hex(value, width) {
	return '0x' + number(value, 0).toString(16).padStart(width || 4, '0');
}

function objectName(object) {
	return String(object && object.name || '');
}

function listContains(list, value) {
	return list.indexOf(number(value, -1)) !== -1;
}

function categoryOf(object) {
	var classId = number(object && object.class_id, -1);
	var name = objectName(object);

	if (classId === 84)
		return 'vlan-filter';
	if (classId === 171)
		return 'extended-vlan';
	if (listContains(multicastClasses, classId) || /multicast/i.test(name))
		return 'multicast';
	if (listContains(gponClasses, classId) ||
	    /(?:T-CONT|ANI-G|GEM port|GEM interworking|GAL profile|traffic scheduler|priority queue)/i.test(name))
		return 'gpon';
	if (listContains(bridgeClasses, classId) ||
	    /(?:Ethernet|bridge|802\.1p|mapper|VEIP|\bUNI\b)/i.test(name))
		return 'bridge';
	if (listContains(inventoryClasses, classId) ||
	    /(?:ONU|software image|equipment|authentication|cardholder|circuit pack)/i.test(name))
		return 'inventory';

	return 'other';
}

function sortObjects(objects) {
	return (objects || []).slice().sort(function(a, b) {
		var classDiff = number(a.class_id, 0) - number(b.class_id, 0);

		return classDiff || number(a.entity_id, 0) - number(b.entity_id, 0);
	});
}

function filterObjects(objects, category) {
	if (!category || category === 'all')
		return sortObjects(objects);

	return sortObjects((objects || []).filter(function(object) {
		return categoryOf(object) === category;
	}));
}

function originLabel(origin) {
	switch (number(origin, -1)) {
	case 0:
		return _('Default');
	case 1:
		return _('Local');
	case 2:
		return _('OLT');
	case 3:
		return _('Hardware');
	default:
		return String(origin == null ? '-' : origin);
	}
}

function refresh() {
	window.location.reload();
}

function notifyError(err) {
	ui.addNotification(null, E('p', {}, err && err.message || String(err)));
}

function editObject(device, object) {
	var classId = E('input', {
		'class': 'cbi-input-text',
		'value': object ? object.class_id : ''
	});
	var entityId = E('input', {
		'class': 'cbi-input-text',
		'value': object ? object.entity_id : ''
	});
	var mask = E('input', {
		'class': 'cbi-input-text',
		'value': object ? object.mask : '65535'
	});
	var data = E('input', {
		'class': 'cbi-input-text',
		'maxlength': 191,
		'placeholder': _('Up to 64 bytes (128 hexadecimal digits)'),
		'value': object ? object.data : ''
	});

	ui.showModal(object ? _('Update MIB object') : _('Add MIB object'), [
		E('div', { 'class': 'cbi-value' }, [
			E('label', { 'class': 'cbi-value-title' }, _('Class ID')),
			E('div', { 'class': 'cbi-value-field' }, classId)
		]),
		E('div', { 'class': 'cbi-value' }, [
			E('label', { 'class': 'cbi-value-title' }, _('Entity ID')),
			E('div', { 'class': 'cbi-value-field' }, entityId)
		]),
		E('div', { 'class': 'cbi-value' }, [
			E('label', { 'class': 'cbi-value-title' }, _('Attribute mask')),
			E('div', { 'class': 'cbi-value-field' }, mask)
		]),
		E('div', { 'class': 'cbi-value' }, [
			E('label', { 'class': 'cbi-value-title' }, _('Hexadecimal data')),
			E('div', { 'class': 'cbi-value-field' }, data)
		]),
		E('div', { 'class': 'right' }, [
			E('button', { 'class': 'btn', 'click': ui.hideModal }, _('Cancel')),
			' ',
			E('button', {
				'class': 'btn cbi-button-positive important',
				'click': function() {
					callMibSet(device, number(classId.value, 0),
						number(entityId.value, 0), number(mask.value, 0),
						data.value).then(refresh).catch(notifyError);
				}
			}, _('Apply'))
		])
	]);
}

function deleteObject(device, object) {
	ui.showModal(_('Delete MIB object'), [
		E('p', {}, _('Delete class %d entity %d from the operational MIB?')
			.format(number(object.class_id, 0), number(object.entity_id, 0))),
		E('div', { 'class': 'right' }, [
			E('button', { 'class': 'btn', 'click': ui.hideModal }, _('Cancel')),
			' ',
			E('button', {
				'class': 'btn cbi-button-negative important',
				'click': function() {
					callMibDelete(device, number(object.class_id, 0),
						number(object.entity_id, 0))
						.then(refresh).catch(notifyError);
				}
			}, _('Delete'))
		])
	]);
}

function resetMib(device) {
	ui.showModal(_('Reset MIB'), [
		E('p', {}, _('Reset the operational MIB to the kernel-created defaults?')),
		E('div', { 'class': 'right' }, [
			E('button', { 'class': 'btn', 'click': ui.hideModal }, _('Cancel')),
			' ',
			E('button', {
				'class': 'btn cbi-button-negative important',
				'click': function() {
					callMibReset(device).then(refresh).catch(notifyError);
				}
			}, _('Reset'))
		])
	]);
}

function actionButtons(device, object) {
	return E('td', { 'class': 'td right', 'style': 'white-space:nowrap' }, [
		E('button', {
			'class': 'btn cbi-button-edit',
			'click': function() { editObject(device, object); }
		}, _('Edit')),
		' ',
		E('button', {
			'class': 'btn cbi-button-negative',
			'click': function() { deleteObject(device, object); }
		}, _('Delete'))
	]);
}

function renderObjectTable(device, objects) {
	var rows = (objects || []).map(function(object) {
		return E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, object.class_id),
			E('td', { 'class': 'td' }, object.entity_id),
			E('td', { 'class': 'td' }, object.name || '-'),
			E('td', { 'class': 'td' }, hex(object.mask, 4)),
			E('td', { 'class': 'td' }, originLabel(object.origin)),
			E('td', {
				'class': 'td',
				'style': 'word-break:break-all;font-family:monospace'
			}, object.data || '-'),
			actionButtons(device, object)
		]);
	});

	if (!rows.length)
		return E('div', { 'class': 'alert-message info' },
			_('No MIB objects are available in this category.'));

	return E('div', { 'style': 'overflow-x:auto' }, [
		E('table', { 'class': 'table' }, [
			E('tr', { 'class': 'tr table-titles' }, [
				E('th', { 'class': 'th' }, _('Class')),
				E('th', { 'class': 'th' }, _('Entity')),
				E('th', { 'class': 'th' }, _('Name')),
				E('th', { 'class': 'th' }, _('Mask')),
				E('th', { 'class': 'th' }, _('Origin')),
				E('th', { 'class': 'th' }, _('Data')),
				E('th', { 'class': 'th' }, _('Actions'))
			])
		].concat(rows))
	]);
}

function renderToolbar(device, allowReset) {
	var children = [
		E('button', {
			'class': 'btn cbi-button-add',
			'click': function() { editObject(device, null); }
		}, _('Add object'))
	];

	if (allowReset) {
		children.push(' ');
		children.push(E('button', {
			'class': 'btn cbi-button-negative',
			'click': function() { resetMib(device); }
		}, _('Reset MIB')));
	}

	return E('div', { 'class': 'right', 'style': 'margin-bottom:1em' }, children);
}

function loadDevice() {
	return uci.load('omci').then(function() {
		return number(uci.get('omci', 'main', 'device'), 0);
	});
}

function loadObjects() {
	return loadDevice().then(function(device) {
		return callMibList(device).then(function(objects) {
			return { device: device, objects: objects || [] };
		});
	});
}

function loadClass(classId) {
	return loadObjects().then(function(data) {
		var selectors = data.objects.filter(function(object) {
			return number(object.class_id, -1) === classId;
		});

		return Promise.all(selectors.map(function(object) {
			return L.resolveDefault(callMibGet(data.device, classId,
				number(object.entity_id, 0)), object);
		})).then(function(objects) {
			return { device: data.device, objects: sortObjects(objects) };
		});
	});
}

function metadataTable(rows) {
	return E('table', { 'class': 'table' }, rows.map(function(entry) {
		return E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td left', 'style': 'width:30%' }, entry[0]),
			E('td', { 'class': 'td left' }, entry[1])
		]);
	}));
}

function createListView(options) {
	return view.extend({
		load: loadObjects,

		render: function(data) {
			var objects = filterObjects(data.objects, options.category);
			var content = [
				E('h2', {}, options.title),
				E('div', { 'class': 'cbi-map-descr' }, options.description)
			];

			if (options.toolbar !== false)
				content.push(renderToolbar(data.device, options.reset === true));
			content.push(renderObjectTable(data.device, objects));
			return E([], content);
		},

		handleSaveApply: null,
		handleSave: null,
		handleReset: null
	});
}

function renderVlanFilterObject(device, object) {
	var filter = object.vlan_filter || {};
	var entries = filter.entries || [];
	var entryRows = entries.map(function(entry) {
		return E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, entry.index),
			E('td', { 'class': 'td' }, hex(entry.tci, 4)),
			E('td', { 'class': 'td' }, entry.pbit),
			E('td', { 'class': 'td' }, entry.dei),
			E('td', { 'class': 'td' }, entry.vid)
		]);
	});

	return E('div', { 'class': 'cbi-section' }, [
		E('h3', {}, _('Entity %d').format(number(object.entity_id, 0))),
		metadataTable([
			[ _('Managed entity'), (object.name || _('VLAN Tagging Filter Data')) +
				' (' + object.class_id + '/' + object.entity_id + ')' ],
			[ _('Attribute mask'), hex(object.mask, 4) ],
			[ _('Origin'), originLabel(object.origin) ],
			[ _('Forward operation'), String(filter.forward_operation == null ? '-' : filter.forward_operation) ],
			[ _('Number of entries'), String(filter.number_of_entries == null ? entries.length : filter.number_of_entries) ],
			[ _('Raw object data'), object.data || '-' ]
		]),
		E('div', { 'class': 'right', 'style': 'margin:0.5em 0' }, [
			E('button', {
				'class': 'btn cbi-button-edit',
				'click': function() { editObject(device, object); }
			}, _('Edit object')),
			' ',
			E('button', {
				'class': 'btn cbi-button-negative',
				'click': function() { deleteObject(device, object); }
			}, _('Delete object'))
		]),
		entryRows.length ? E('table', { 'class': 'table' }, [
			E('tr', { 'class': 'tr table-titles' }, [
				E('th', { 'class': 'th' }, _('Index')),
				E('th', { 'class': 'th' }, _('TCI')),
				E('th', { 'class': 'th' }, _('P-bit')),
				E('th', { 'class': 'th' }, _('DEI')),
				E('th', { 'class': 'th' }, _('VID'))
			])
		].concat(entryRows)) : E('div', { 'class': 'alert-message info' },
			_('This entity does not contain VLAN filter entries.'))
	]);
}

function createVlanFilterView() {
	return view.extend({
		load: function() { return loadClass(84); },

		render: function(data) {
			var content = [
				E('h2', {}, _('VLAN Tagging Filter Data (class 84)')),
				E('div', { 'class': 'cbi-map-descr' },
					_('Each entity is decoded into an array of VLAN TCI entries.')),
				renderToolbar(data.device, false)
			];

			if (!data.objects.length)
				content.push(E('div', { 'class': 'alert-message info' },
					_('No class 84 entities are present in the operational MIB.')));
			else
				data.objects.forEach(function(object) {
					content.push(renderVlanFilterObject(data.device, object));
				});

			return E([], content);
		},

		handleSaveApply: null,
		handleSave: null,
		handleReset: null
	});
}

function ruleTuple(rule, prefix) {
	return '%s / %s / %s'.format(
		String(rule[prefix + '_pbit'] == null ? '-' : rule[prefix + '_pbit']),
		String(rule[prefix + '_vid'] == null ? '-' : rule[prefix + '_vid']),
		String(rule[prefix + '_tpid_dei'] == null ? '-' : rule[prefix + '_tpid_dei']));
}

function renderExtendedVlanObject(device, object) {
	var vlan = object.extended_vlan || {};
	var rules = vlan.rules || [];
	var ruleRows = rules.map(function(rule) {
		return E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, rule.index),
			E('td', {
				'class': 'td',
				'style': 'word-break:break-all;font-family:monospace;min-width:17em'
			}, rule.raw || '-'),
			E('td', { 'class': 'td' }, rule.delete ? _('Yes') : _('No')),
			E('td', { 'class': 'td', 'style': 'white-space:nowrap' },
				ruleTuple(rule, 'filter_outer')),
			E('td', { 'class': 'td', 'style': 'white-space:nowrap' },
				ruleTuple(rule, 'filter_inner')),
			E('td', { 'class': 'td' }, rule.filter_ethertype),
			E('td', { 'class': 'td' }, rule.tags_to_remove),
			E('td', { 'class': 'td', 'style': 'white-space:nowrap' },
				ruleTuple(rule, 'treat_outer')),
			E('td', { 'class': 'td', 'style': 'white-space:nowrap' },
				ruleTuple(rule, 'treat_inner'))
		]);
	});

	return E('div', { 'class': 'cbi-section' }, [
		E('h3', {}, _('Entity %d').format(number(object.entity_id, 0))),
		metadataTable([
			[ _('Managed entity'), (object.name || _('Extended VLAN Tagging Operation Configuration Data')) +
				' (' + object.class_id + '/' + object.entity_id + ')' ],
			[ _('Attribute mask'), hex(object.mask, 4) ],
			[ _('Origin'), originLabel(object.origin) ],
			[ _('Association type'), String(vlan.association_type == null ? '-' : vlan.association_type) ],
			[ _('Associated ME pointer'), vlan.associated_me == null ? '-' : hex(vlan.associated_me, 4) ],
			[ _('Input TPID'), vlan.input_tpid == null ? '-' : hex(vlan.input_tpid, 4) ],
			[ _('Output TPID'), vlan.output_tpid == null ? '-' : hex(vlan.output_tpid, 4) ],
			[ _('Downstream mode'), String(vlan.downstream_mode == null ? '-' : vlan.downstream_mode) ],
			[ _('Maximum table size'), String(vlan.max_table_size == null ? '-' : vlan.max_table_size) ],
			[ _('DSCP to P-bit table'), vlan.dscp_to_pbit || '-' ],
			[ _('Rule count'), String(rules.length) ],
			[ _('Raw object data'), object.data || '-' ]
		]),
		E('div', { 'class': 'right', 'style': 'margin:0.5em 0' }, [
			E('button', {
				'class': 'btn cbi-button-edit',
				'click': function() { editObject(device, object); }
			}, _('Edit object')),
			' ',
			E('button', {
				'class': 'btn cbi-button-negative',
				'click': function() { deleteObject(device, object); }
			}, _('Delete object'))
		]),
		ruleRows.length ? E('div', { 'style': 'overflow-x:auto' }, [
			E('table', { 'class': 'table' }, [
				E('tr', { 'class': 'tr table-titles' }, [
					E('th', { 'class': 'th' }, _('Index')),
					E('th', { 'class': 'th' }, _('Raw rule')),
					E('th', { 'class': 'th' }, _('Delete')),
					E('th', { 'class': 'th' }, _('Filter outer P-bit / VID / TPID-DEI')),
					E('th', { 'class': 'th' }, _('Filter inner P-bit / VID / TPID-DEI')),
					E('th', { 'class': 'th' }, _('EtherType')),
					E('th', { 'class': 'th' }, _('Remove tags')),
					E('th', { 'class': 'th' }, _('Treat outer P-bit / VID / TPID-DEI')),
					E('th', { 'class': 'th' }, _('Treat inner P-bit / VID / TPID-DEI'))
				])
			].concat(ruleRows))
		]) : E('div', { 'class': 'alert-message info' },
			_('This entity does not contain extended VLAN rules.'))
	]);
}

function createExtendedVlanView() {
	return view.extend({
		load: function() { return loadClass(171); },

		render: function(data) {
			var content = [
				E('h2', {}, _('Extended VLAN rules (class 171)')),
				E('div', { 'class': 'cbi-map-descr' },
					_('Each 16-byte table row is decoded into filter and treatment fields.')),
				renderToolbar(data.device, false)
			];

			if (!data.objects.length)
				content.push(E('div', { 'class': 'alert-message info' },
					_('No class 171 entities are present in the operational MIB.')));
			else
				data.objects.forEach(function(object) {
					content.push(renderExtendedVlanObject(data.device, object));
				});

			return E([], content);
		},

		handleSaveApply: null,
		handleSave: null,
		handleReset: null
	});
}

return baseclass.extend({
	createListView: createListView,
	createVlanFilterView: createVlanFilterView,
	createExtendedVlanView: createExtendedVlanView
});
