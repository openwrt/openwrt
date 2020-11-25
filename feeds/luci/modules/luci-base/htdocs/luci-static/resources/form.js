'use strict';
'require ui';
'require uci';

var scope = this;

var CBIJSONConfig = Class.extend({
	__init__: function(data) {
		data = Object.assign({}, data);

		this.data = {};

		var num_sections = 0,
		    section_ids = [];

		for (var sectiontype in data) {
			if (!data.hasOwnProperty(sectiontype))
				continue;

			if (L.isObject(data[sectiontype])) {
				this.data[sectiontype] = Object.assign(data[sectiontype], {
					'.anonymous': false,
					'.name': sectiontype,
					'.type': sectiontype
				});

				section_ids.push(sectiontype);
				num_sections++;
			}
			else if (Array.isArray(data[sectiontype])) {
				for (var i = 0, index = 0; i < data[sectiontype].length; i++) {
					var item = data[sectiontype][i],
					    anonymous, name;

					if (!L.isObject(item))
						continue;

					if (typeof(item['.name']) == 'string') {
						name = item['.name'];
						anonymous = false;
					}
					else {
						name = sectiontype + num_sections;
						anonymous = true;
					}

					if (!this.data.hasOwnProperty(name))
						section_ids.push(name);

					this.data[name] = Object.assign(item, {
						'.index': num_sections++,
						'.anonymous': anonymous,
						'.name': name,
						'.type': sectiontype
					});
				}
			}
		}

		section_ids.sort(L.bind(function(a, b) {
			var indexA = (this.data[a]['.index'] != null) ? +this.data[a]['.index'] : 9999,
			    indexB = (this.data[b]['.index'] != null) ? +this.data[b]['.index'] : 9999;

			if (indexA != indexB)
				return (indexA - indexB);

			return (a > b);
		}, this));

		for (var i = 0; i < section_ids.length; i++)
			this.data[section_ids[i]]['.index'] = i;
	},

	load: function() {
		return Promise.resolve(this.data);
	},

	save: function() {
		return Promise.resolve();
	},

	get: function(config, section, option) {
		if (section == null)
			return null;

		if (option == null)
			return this.data[section];

		if (!this.data.hasOwnProperty(section))
			return null;

		var value = this.data[section][option];

		if (Array.isArray(value))
			return value;

		if (value != null)
			return String(value);

		return null;
	},

	set: function(config, section, option, value) {
		if (section == null || option == null || option.charAt(0) == '.')
			return;

		if (!this.data.hasOwnProperty(section))
			return;

		if (value == null)
			delete this.data[section][option];
		else if (Array.isArray(value))
			this.data[section][option] = value;
		else
			this.data[section][option] = String(value);
	},

	unset: function(config, section, option) {
		return this.set(config, section, option, null);
	},

	sections: function(config, sectiontype, callback) {
		var rv = [];

		for (var section_id in this.data)
			if (sectiontype == null || this.data[section_id]['.type'] == sectiontype)
				rv.push(this.data[section_id]);

		rv.sort(function(a, b) { return a['.index'] - b['.index'] });

		if (typeof(callback) == 'function')
			for (var i = 0; i < rv.length; i++)
				callback.call(this, rv[i], rv[i]['.name']);

		return rv;
	},

	add: function(config, sectiontype, sectionname) {
		var num_sections_type = 0, next_index = 0;

		for (var name in this.data) {
			num_sections_type += (this.data[name]['.type'] == sectiontype);
			next_index = Math.max(next_index, this.data[name]['.index']);
		}

		var section_id = sectionname || sectiontype + num_sections_type;

		if (!this.data.hasOwnProperty(section_id)) {
			this.data[section_id] = {
				'.name': section_id,
				'.type': sectiontype,
				'.anonymous': (sectionname == null),
				'.index': next_index + 1
			};
		}

		return section_id;
	},

	remove: function(config, section) {
		if (this.data.hasOwnProperty(section))
			delete this.data[section];
	},

	resolveSID: function(config, section_id) {
		return section_id;
	},

	move: function(config, section_id1, section_id2, after) {
		return uci.move.apply(this, [config, section_id1, section_id2, after]);
	}
});

var CBINode = Class.extend({
	__init__: function(title, description) {
		this.title = title || '';
		this.description = description || '';
		this.children = [];
	},

	append: function(obj) {
		this.children.push(obj);
	},

	parse: function() {
		var args = arguments;
		this.children.forEach(function(child) {
			child.parse.apply(child, args);
		});
	},

	render: function() {
		L.error('InternalError', 'Not implemented');
	},

	loadChildren: function(/* ... */) {
		var tasks = [];

		if (Array.isArray(this.children))
			for (var i = 0; i < this.children.length; i++)
				if (!this.children[i].disable)
					tasks.push(this.children[i].load.apply(this.children[i], arguments));

		return Promise.all(tasks);
	},

	renderChildren: function(tab_name /*, ... */) {
		var tasks = [],
		    index = 0;

		if (Array.isArray(this.children))
			for (var i = 0; i < this.children.length; i++)
				if (tab_name === null || this.children[i].tab === tab_name)
					if (!this.children[i].disable)
						tasks.push(this.children[i].render.apply(
							this.children[i], this.varargs(arguments, 1, index++)));

		return Promise.all(tasks);
	},

	stripTags: function(s) {
		if (typeof(s) == 'string' && !s.match(/[<>]/))
			return s;

		var x = E('div', {}, s);
		return x.textContent || x.innerText || '';
	},

	titleFn: function(attr /*, ... */) {
		var s = null;

		if (typeof(this[attr]) == 'function')
			s = this[attr].apply(this, this.varargs(arguments, 1));
		else if (typeof(this[attr]) == 'string')
			s = (arguments.length > 1) ? ''.format.apply(this[attr], this.varargs(arguments, 1)) : this[attr];

		if (s != null)
			s = this.stripTags(String(s)).trim();

		if (s == null || s == '')
			return null;

		return s;
	}
});

var CBIMap = CBINode.extend({
	__init__: function(config /*, ... */) {
		this.super('__init__', this.varargs(arguments, 1));

		this.config = config;
		this.parsechain = [ config ];
		this.data = uci;
	},

	findElements: function(/* ... */) {
		var q = null;

		if (arguments.length == 1)
			q = arguments[0];
		else if (arguments.length == 2)
			q = '[%s="%s"]'.format(arguments[0], arguments[1]);
		else
			L.error('InternalError', 'Expecting one or two arguments to findElements()');

		return this.root.querySelectorAll(q);
	},

	findElement: function(/* ... */) {
		var res = this.findElements.apply(this, arguments);
		return res.length ? res[0] : null;
	},

	chain: function(config) {
		if (this.parsechain.indexOf(config) == -1)
			this.parsechain.push(config);
	},

	section: function(cbiClass /*, ... */) {
		if (!CBIAbstractSection.isSubclass(cbiClass))
			L.error('TypeError', 'Class must be a descendent of CBIAbstractSection');

		var obj = cbiClass.instantiate(this.varargs(arguments, 1, this));
		this.append(obj);
		return obj;
	},

	load: function() {
		return this.data.load(this.parsechain || [ this.config ])
			.then(this.loadChildren.bind(this));
	},

	parse: function() {
		var tasks = [];

		if (Array.isArray(this.children))
			for (var i = 0; i < this.children.length; i++)
				tasks.push(this.children[i].parse());

		return Promise.all(tasks);
	},

	save: function(cb, silent) {
		this.checkDepends();

		return this.parse()
			.then(cb)
			.then(this.data.save.bind(this.data))
			.then(this.load.bind(this))
			.catch(function(e) {
				if (!silent)
					alert('Cannot save due to invalid values');

				return Promise.reject();
			}).finally(this.renderContents.bind(this));
	},

	reset: function() {
		return this.renderContents();
	},

	render: function() {
		return this.load().then(this.renderContents.bind(this));
	},

	renderContents: function() {
		var mapEl = this.root || (this.root = E('div', {
			'id': 'cbi-%s'.format(this.config),
			'class': 'cbi-map',
			'cbi-dependency-check': L.bind(this.checkDepends, this)
		}));

		L.dom.bindClassInstance(mapEl, this);

		return this.renderChildren(null).then(L.bind(function(nodes) {
			var initialRender = !mapEl.firstChild;

			L.dom.content(mapEl, null);

			if (this.title != null && this.title != '')
				mapEl.appendChild(E('h2', { 'name': 'content' }, this.title));

			if (this.description != null && this.description != '')
				mapEl.appendChild(E('div', { 'class': 'cbi-map-descr' }, this.description));

			if (this.tabbed)
				L.dom.append(mapEl, E('div', { 'class': 'cbi-map-tabbed' }, nodes));
			else
				L.dom.append(mapEl, nodes);

			if (!initialRender) {
				mapEl.classList.remove('flash');

				window.setTimeout(function() {
					mapEl.classList.add('flash');
				}, 1);
			}

			this.checkDepends();

			var tabGroups = mapEl.querySelectorAll('.cbi-map-tabbed, .cbi-section-node-tabbed');

			for (var i = 0; i < tabGroups.length; i++)
				ui.tabs.initTabGroup(tabGroups[i].childNodes);

			return mapEl;
		}, this));
	},

	lookupOption: function(name, section_id, config_name) {
		var id, elem, sid, inst;

		if (name.indexOf('.') > -1)
			id = 'cbid.%s'.format(name);
		else
			id = 'cbid.%s.%s.%s'.format(config_name || this.config, section_id, name);

		elem = this.findElement('data-field', id);
		sid  = elem ? id.split(/\./)[2] : null;
		inst = elem ? L.dom.findClassInstance(elem) : null;

		return (inst instanceof CBIAbstractValue) ? [ inst, sid ] : null;
	},

	checkDepends: function(ev, n) {
		var changed = false;

		for (var i = 0, s = this.children[0]; (s = this.children[i]) != null; i++)
			if (s.checkDepends(ev, n))
				changed = true;

		if (changed && (n || 0) < 10)
			this.checkDepends(ev, (n || 10) + 1);

		ui.tabs.updateTabs(ev, this.root);
	},

	isDependencySatisfied: function(depends, config_name, section_id) {
		var def = false;

		if (!Array.isArray(depends) || !depends.length)
			return true;

		for (var i = 0; i < depends.length; i++) {
			var istat = true,
			    reverse = depends[i]['!reverse'],
			    contains = depends[i]['!contains'];

			for (var dep in depends[i]) {
				if (dep == '!reverse' || dep == '!contains') {
					continue;
				}
				else if (dep == '!default') {
					def = true;
					istat = false;
				}
				else {
					var res = this.lookupOption(dep, section_id, config_name),
					    val = (res && res[0].isActive(res[1])) ? res[0].formvalue(res[1]) : null;

					var equal = contains
						? isContained(val, depends[i][dep])
						: isEqual(val, depends[i][dep]);

					istat = (istat && equal);
				}
			}

			if (istat ^ reverse)
				return true;
		}

		return def;
	}
});

var CBIJSONMap = CBIMap.extend({
	__init__: function(data /*, ... */) {
		this.super('__init__', this.varargs(arguments, 1, 'json'));

		this.config = 'json';
		this.parsechain = [ 'json' ];
		this.data = new CBIJSONConfig(data);
	}
});

var CBIAbstractSection = CBINode.extend({
	__init__: function(map, sectionType /*, ... */) {
		this.super('__init__', this.varargs(arguments, 2));

		this.sectiontype = sectionType;
		this.map = map;
		this.config = map.config;

		this.optional = true;
		this.addremove = false;
		this.dynamic = false;
	},

	cfgsections: function() {
		L.error('InternalError', 'Not implemented');
	},

	filter: function(section_id) {
		return true;
	},

	load: function() {
		var section_ids = this.cfgsections(),
		    tasks = [];

		if (Array.isArray(this.children))
			for (var i = 0; i < section_ids.length; i++)
				tasks.push(this.loadChildren(section_ids[i])
					.then(Function.prototype.bind.call(function(section_id, set_values) {
						for (var i = 0; i < set_values.length; i++)
							this.children[i].cfgvalue(section_id, set_values[i]);
					}, this, section_ids[i])));

		return Promise.all(tasks);
	},

	parse: function() {
		var section_ids = this.cfgsections(),
		    tasks = [];

		if (Array.isArray(this.children))
			for (var i = 0; i < section_ids.length; i++)
				for (var j = 0; j < this.children.length; j++)
					tasks.push(this.children[j].parse(section_ids[i]));

		return Promise.all(tasks);
	},

	tab: function(name, title, description) {
		if (this.tabs && this.tabs[name])
			throw 'Tab already declared';

		var entry = {
			name: name,
			title: title,
			description: description,
			children: []
		};

		this.tabs = this.tabs || [];
		this.tabs.push(entry);
		this.tabs[name] = entry;

		this.tab_names = this.tab_names || [];
		this.tab_names.push(name);
	},

	option: function(cbiClass /*, ... */) {
		if (!CBIAbstractValue.isSubclass(cbiClass))
			throw L.error('TypeError', 'Class must be a descendent of CBIAbstractValue');

		var obj = cbiClass.instantiate(this.varargs(arguments, 1, this.map, this));
		this.append(obj);
		return obj;
	},

	taboption: function(tabName /*, ... */) {
		if (!this.tabs || !this.tabs[tabName])
			throw L.error('ReferenceError', 'Associated tab not declared');

		var obj = this.option.apply(this, this.varargs(arguments, 1));
		obj.tab = tabName;
		this.tabs[tabName].children.push(obj);
		return obj;
	},

	renderUCISection: function(section_id) {
		var renderTasks = [];

		if (!this.tabs)
			return this.renderOptions(null, section_id);

		for (var i = 0; i < this.tab_names.length; i++)
			renderTasks.push(this.renderOptions(this.tab_names[i], section_id));

		return Promise.all(renderTasks)
			.then(this.renderTabContainers.bind(this, section_id));
	},

	renderTabContainers: function(section_id, nodes) {
		var config_name = this.uciconfig || this.map.config,
		    containerEls = E([]);

		for (var i = 0; i < nodes.length; i++) {
			var tab_name = this.tab_names[i],
			    tab_data = this.tabs[tab_name],
			    containerEl = E('div', {
			    	'id': 'container.%s.%s.%s'.format(config_name, section_id, tab_name),
			    	'data-tab': tab_name,
			    	'data-tab-title': tab_data.title,
			    	'data-tab-active': tab_name === this.selected_tab
			    });

			if (tab_data.description != null && tab_data.description != '')
				containerEl.appendChild(
					E('div', { 'class': 'cbi-tab-descr' }, tab_data.description));

			containerEl.appendChild(nodes[i]);
			containerEls.appendChild(containerEl);
		}

		return containerEls;
	},

	renderOptions: function(tab_name, section_id) {
		var in_table = (this instanceof CBITableSection);
		return this.renderChildren(tab_name, section_id, in_table).then(function(nodes) {
			var optionEls = E([]);
			for (var i = 0; i < nodes.length; i++)
				optionEls.appendChild(nodes[i]);
			return optionEls;
		});
	},

	checkDepends: function(ev, n) {
		var changed = false,
		    sids = this.cfgsections();

		for (var i = 0, sid = sids[0]; (sid = sids[i]) != null; i++) {
			for (var j = 0, o = this.children[0]; (o = this.children[j]) != null; j++) {
				var isActive = o.isActive(sid),
				    isSatisified = o.checkDepends(sid);

				if (isActive != isSatisified) {
					o.setActive(sid, !isActive);
					isActive = !isActive;
					changed = true;
				}

				if (!n && isActive)
					o.triggerValidation(sid);
			}
		}

		return changed;
	}
});


var isEqual = function(x, y) {
	if (x != null && y != null && typeof(x) != typeof(y))
		return false;

	if ((x == null && y != null) || (x != null && y == null))
		return false;

	if (Array.isArray(x)) {
		if (x.length != y.length)
			return false;

		for (var i = 0; i < x.length; i++)
			if (!isEqual(x[i], y[i]))
				return false;
	}
	else if (typeof(x) == 'object') {
		for (var k in x) {
			if (x.hasOwnProperty(k) && !y.hasOwnProperty(k))
				return false;

			if (!isEqual(x[k], y[k]))
				return false;
		}

		for (var k in y)
			if (y.hasOwnProperty(k) && !x.hasOwnProperty(k))
				return false;
	}
	else if (x != y) {
		return false;
	}

	return true;
};

var isContained = function(x, y) {
	if (Array.isArray(x)) {
		for (var i = 0; i < x.length; i++)
			if (x[i] == y)
				return true;
	}
	else if (L.isObject(x)) {
		if (x.hasOwnProperty(y) && x[y] != null)
			return true;
	}
	else if (typeof(x) == 'string') {
		return (x.indexOf(y) > -1);
	}

	return false;
};

var CBIAbstractValue = CBINode.extend({
	__init__: function(map, section, option /*, ... */) {
		this.super('__init__', this.varargs(arguments, 3));

		this.section = section;
		this.option = option;
		this.map = map;
		this.config = map.config;

		this.deps = [];
		this.initial = {};
		this.rmempty = true;
		this.default = null;
		this.size = null;
		this.optional = false;
	},

	depends: function(field, value) {
		var deps;

		if (typeof(field) === 'string')
			deps = {}, deps[field] = value;
		else
			deps = field;

		this.deps.push(deps);
	},

	transformDepList: function(section_id, deplist) {
		var list = deplist || this.deps,
		    deps = [];

		if (Array.isArray(list)) {
			for (var i = 0; i < list.length; i++) {
				var dep = {};

				for (var k in list[i]) {
					if (list[i].hasOwnProperty(k)) {
						if (k.charAt(0) === '!')
							dep[k] = list[i][k];
						else if (k.indexOf('.') !== -1)
							dep['cbid.%s'.format(k)] = list[i][k];
						else
							dep['cbid.%s.%s.%s'.format(
								this.uciconfig || this.section.uciconfig || this.map.config,
								this.ucisection || section_id,
								k
							)] = list[i][k];
					}
				}

				for (var k in dep) {
					if (dep.hasOwnProperty(k)) {
						deps.push(dep);
						break;
					}
				}
			}
		}

		return deps;
	},

	transformChoices: function() {
		if (!Array.isArray(this.keylist) || this.keylist.length == 0)
			return null;

		var choices = {};

		for (var i = 0; i < this.keylist.length; i++)
			choices[this.keylist[i]] = this.vallist[i];

		return choices;
	},

	checkDepends: function(section_id) {
		var config_name = this.uciconfig || this.section.uciconfig || this.map.config,
		    active = this.map.isDependencySatisfied(this.deps, config_name, section_id);

		if (active)
			this.updateDefaultValue(section_id);

		return active;
	},

	updateDefaultValue: function(section_id) {
		if (!L.isObject(this.defaults))
			return;

		var config_name = this.uciconfig || this.section.uciconfig || this.map.config,
		    cfgvalue = L.toArray(this.cfgvalue(section_id))[0],
		    default_defval = null, satisified_defval = null;

		for (var value in this.defaults) {
			if (!this.defaults[value] || this.defaults[value].length == 0) {
				default_defval = value;
				continue;
			}
			else if (this.map.isDependencySatisfied(this.defaults[value], config_name, section_id)) {
				satisified_defval = value;
				break;
			}
		}

		if (satisified_defval == null)
			satisified_defval = default_defval;

		var node = this.map.findElement('id', this.cbid(section_id));
		if (node && node.getAttribute('data-changed') != 'true' && satisified_defval != null && cfgvalue == null)
			L.dom.callClassMethod(node, 'setValue', satisified_defval);

		this.default = satisified_defval;
	},

	cbid: function(section_id) {
		if (section_id == null)
			L.error('TypeError', 'Section ID required');

		return 'cbid.%s.%s.%s'.format(
			this.uciconfig || this.section.uciconfig || this.map.config,
			section_id, this.option);
	},

	load: function(section_id) {
		if (section_id == null)
			L.error('TypeError', 'Section ID required');

		return this.map.data.get(
			this.uciconfig || this.section.uciconfig || this.map.config,
			this.ucisection || section_id,
			this.ucioption || this.option);
	},

	getUIElement: function(section_id) {
		var node = this.map.findElement('id', this.cbid(section_id)),
		    inst = node ? L.dom.findClassInstance(node) : null;
		return (inst instanceof ui.AbstractElement) ? inst : null;
	},

	cfgvalue: function(section_id, set_value) {
		if (section_id == null)
			L.error('TypeError', 'Section ID required');

		if (arguments.length == 2) {
			this.data = this.data || {};
			this.data[section_id] = set_value;
		}

		return this.data ? this.data[section_id] : null;
	},

	formvalue: function(section_id) {
		var elem = this.getUIElement(section_id);
		return elem ? elem.getValue() : null;
	},

	textvalue: function(section_id) {
		var cval = this.cfgvalue(section_id);

		if (cval == null)
			cval = this.default;

		return (cval != null) ? '%h'.format(cval) : null;
	},

	validate: function(section_id, value) {
		return true;
	},

	isValid: function(section_id) {
		var elem = this.getUIElement(section_id);
		return elem ? elem.isValid() : true;
	},

	isActive: function(section_id) {
		var field = this.map.findElement('data-field', this.cbid(section_id));
		return (field != null && !field.classList.contains('hidden'));
	},

	setActive: function(section_id, active) {
		var field = this.map.findElement('data-field', this.cbid(section_id));

		if (field && field.classList.contains('hidden') == active) {
			field.classList[active ? 'remove' : 'add']('hidden');
			return true;
		}

		return false;
	},

	triggerValidation: function(section_id) {
		var elem = this.getUIElement(section_id);
		return elem ? elem.triggerValidation() : true;
	},

	parse: function(section_id) {
		var active = this.isActive(section_id),
		    cval = this.cfgvalue(section_id),
		    fval = active ? this.formvalue(section_id) : null;

		if (active && !this.isValid(section_id))
			return Promise.reject();

		if (fval != '' && fval != null) {
			if (this.forcewrite || !isEqual(cval, fval))
				return Promise.resolve(this.write(section_id, fval));
		}
		else {
			if (!active || this.rmempty || this.optional) {
				return Promise.resolve(this.remove(section_id));
			}
			else if (!isEqual(cval, fval)) {
				console.log('This should have been catched by isValid()');
				return Promise.reject();
			}
		}

		return Promise.resolve();
	},

	write: function(section_id, formvalue) {
		return this.map.data.set(
			this.uciconfig || this.section.uciconfig || this.map.config,
			this.ucisection || section_id,
			this.ucioption || this.option,
			formvalue);
	},

	remove: function(section_id) {
		return this.map.data.unset(
			this.uciconfig || this.section.uciconfig || this.map.config,
			this.ucisection || section_id,
			this.ucioption || this.option);
	}
});

var CBITypedSection = CBIAbstractSection.extend({
	__name__: 'CBI.TypedSection',

	cfgsections: function() {
		return this.map.data.sections(this.uciconfig || this.map.config, this.sectiontype)
			.map(function(s) { return s['.name'] })
			.filter(L.bind(this.filter, this));
	},

	handleAdd: function(ev, name) {
		var config_name = this.uciconfig || this.map.config;

		this.map.data.add(config_name, this.sectiontype, name);
		return this.map.save(null, true);
	},

	handleRemove: function(section_id, ev) {
		var config_name = this.uciconfig || this.map.config;

		this.map.data.remove(config_name, section_id);
		return this.map.save(null, true);
	},

	renderSectionAdd: function(extra_class) {
		if (!this.addremove)
			return E([]);

		var createEl = E('div', { 'class': 'cbi-section-create' }),
		    config_name = this.uciconfig || this.map.config,
		    btn_title = this.titleFn('addbtntitle');

		if (extra_class != null)
			createEl.classList.add(extra_class);

		if (this.anonymous) {
			createEl.appendChild(E('button', {
				'class': 'cbi-button cbi-button-add',
				'title': btn_title || _('Add'),
				'click': L.ui.createHandlerFn(this, 'handleAdd')
			}, [ btn_title || _('Add') ]));
		}
		else {
			var nameEl = E('input', {
				'type': 'text',
				'class': 'cbi-section-create-name'
			});

			L.dom.append(createEl, [
				E('div', {}, nameEl),
				E('input', {
					'class': 'cbi-button cbi-button-add',
					'type': 'submit',
					'value': btn_title || _('Add'),
					'title': btn_title || _('Add'),
					'click': L.ui.createHandlerFn(this, function(ev) {
						if (nameEl.classList.contains('cbi-input-invalid'))
							return;

						return this.handleAdd(ev, nameEl.value);
					})
				})
			]);

			ui.addValidator(nameEl, 'uciname', true, 'blur', 'keyup');
		}

		return createEl;
	},

	renderSectionPlaceholder: function() {
		return E([
			E('em', _('This section contains no values yet')),
			E('br'), E('br')
		]);
	},

	renderContents: function(cfgsections, nodes) {
		var section_id = null,
		    config_name = this.uciconfig || this.map.config,
		    sectionEl = E('div', {
				'id': 'cbi-%s-%s'.format(config_name, this.sectiontype),
				'class': 'cbi-section',
				'data-tab': (this.map.tabbed && !this.parentoption) ? this.sectiontype : null,
				'data-tab-title': (this.map.tabbed && !this.parentoption) ? this.title || this.sectiontype : null
			});

		if (this.title != null && this.title != '')
			sectionEl.appendChild(E('legend', {}, this.title));

		if (this.description != null && this.description != '')
			sectionEl.appendChild(E('div', { 'class': 'cbi-section-descr' }, this.description));

		for (var i = 0; i < nodes.length; i++) {
			if (this.addremove) {
				sectionEl.appendChild(
					E('div', { 'class': 'cbi-section-remove right' },
						E('button', {
							'class': 'cbi-button',
							'name': 'cbi.rts.%s.%s'.format(config_name, cfgsections[i]),
							'data-section-id': cfgsections[i],
							'click': L.ui.createHandlerFn(this, 'handleRemove', cfgsections[i])
						}, [ _('Delete') ])));
			}

			if (!this.anonymous)
				sectionEl.appendChild(E('h3', cfgsections[i].toUpperCase()));

			sectionEl.appendChild(E('div', {
				'id': 'cbi-%s-%s'.format(config_name, cfgsections[i]),
				'class': this.tabs
					? 'cbi-section-node cbi-section-node-tabbed' : 'cbi-section-node',
				'data-section-id': cfgsections[i]
			}, nodes[i]));
		}

		if (nodes.length == 0)
			sectionEl.appendChild(this.renderSectionPlaceholder());

		sectionEl.appendChild(this.renderSectionAdd());

		L.dom.bindClassInstance(sectionEl, this);

		return sectionEl;
	},

	render: function() {
		var cfgsections = this.cfgsections(),
		    renderTasks = [];

		for (var i = 0; i < cfgsections.length; i++)
			renderTasks.push(this.renderUCISection(cfgsections[i]));

		return Promise.all(renderTasks).then(this.renderContents.bind(this, cfgsections));
	}
});

var CBITableSection = CBITypedSection.extend({
	__name__: 'CBI.TableSection',

	tab: function() {
		throw 'Tabs are not supported by TableSection';
	},

	renderContents: function(cfgsections, nodes) {
		var section_id = null,
		    config_name = this.uciconfig || this.map.config,
		    max_cols = isNaN(this.max_cols) ? this.children.length : this.max_cols,
		    has_more = max_cols < this.children.length,
		    sectionEl = E('div', {
				'id': 'cbi-%s-%s'.format(config_name, this.sectiontype),
				'class': 'cbi-section cbi-tblsection',
				'data-tab': (this.map.tabbed && !this.parentoption) ? this.sectiontype : null,
				'data-tab-title': (this.map.tabbed && !this.parentoption) ? this.title || this.sectiontype : null
			}),
			tableEl = E('div', {
				'class': 'table cbi-section-table'
			});

		if (this.title != null && this.title != '')
			sectionEl.appendChild(E('h3', {}, this.title));

		if (this.description != null && this.description != '')
			sectionEl.appendChild(E('div', { 'class': 'cbi-section-descr' }, this.description));

		tableEl.appendChild(this.renderHeaderRows(max_cols));

		for (var i = 0; i < nodes.length; i++) {
			var sectionname = this.titleFn('sectiontitle', cfgsections[i]);

			if (sectionname == null)
				sectionname = cfgsections[i];

			var trEl = E('div', {
				'id': 'cbi-%s-%s'.format(config_name, cfgsections[i]),
				'class': 'tr cbi-section-table-row',
				'data-sid': cfgsections[i],
				'draggable': this.sortable ? true : null,
				'mousedown': this.sortable ? L.bind(this.handleDragInit, this) : null,
				'dragstart': this.sortable ? L.bind(this.handleDragStart, this) : null,
				'dragover': this.sortable ? L.bind(this.handleDragOver, this) : null,
				'dragenter': this.sortable ? L.bind(this.handleDragEnter, this) : null,
				'dragleave': this.sortable ? L.bind(this.handleDragLeave, this) : null,
				'dragend': this.sortable ? L.bind(this.handleDragEnd, this) : null,
				'drop': this.sortable ? L.bind(this.handleDrop, this) : null,
				'data-title': (sectionname && (!this.anonymous || this.sectiontitle)) ? sectionname : null,
				'data-section-id': cfgsections[i]
			});

			if (this.extedit || this.rowcolors)
				trEl.classList.add(!(tableEl.childNodes.length % 2)
					? 'cbi-rowstyle-1' : 'cbi-rowstyle-2');

			for (var j = 0; j < max_cols && nodes[i].firstChild; j++)
				trEl.appendChild(nodes[i].firstChild);

			trEl.appendChild(this.renderRowActions(cfgsections[i], has_more ? _('More…') : null));
			tableEl.appendChild(trEl);
		}

		if (nodes.length == 0)
			tableEl.appendChild(E('div', { 'class': 'tr cbi-section-table-row placeholder' },
				E('div', { 'class': 'td' },
					E('em', {}, _('This section contains no values yet')))));

		sectionEl.appendChild(tableEl);

		sectionEl.appendChild(this.renderSectionAdd('cbi-tblsection-create'));

		L.dom.bindClassInstance(sectionEl, this);

		return sectionEl;
	},

	renderHeaderRows: function(max_cols) {
		var has_titles = false,
		    has_descriptions = false,
		    max_cols = isNaN(this.max_cols) ? this.children.length : this.max_cols,
		    has_more = max_cols < this.children.length,
		    anon_class = (!this.anonymous || this.sectiontitle) ? 'named' : 'anonymous',
		    trEls = E([]);

		for (var i = 0, opt; i < max_cols && (opt = this.children[i]) != null; i++) {
			if (opt.optional || opt.modalonly)
				continue;

			has_titles = has_titles || !!opt.title;
			has_descriptions = has_descriptions || !!opt.description;
		}

		if (has_titles) {
			var trEl = E('div', {
				'class': 'tr cbi-section-table-titles ' + anon_class,
				'data-title': (!this.anonymous || this.sectiontitle) ? _('Name') : null
			});

			for (var i = 0, opt; i < max_cols && (opt = this.children[i]) != null; i++) {
				if (opt.optional || opt.modalonly)
					continue;

				trEl.appendChild(E('div', {
					'class': 'th cbi-section-table-cell',
					'data-widget': opt.__name__
				}));

				if (opt.width != null)
					trEl.lastElementChild.style.width =
						(typeof(opt.width) == 'number') ? opt.width+'px' : opt.width;

				if (opt.titleref)
					trEl.lastElementChild.appendChild(E('a', {
						'href': opt.titleref,
						'class': 'cbi-title-ref',
						'title': this.titledesc || _('Go to relevant configuration page')
					}, opt.title));
				else
					L.dom.content(trEl.lastElementChild, opt.title);
			}

			if (this.sortable || this.extedit || this.addremove || has_more)
				trEl.appendChild(E('div', {
					'class': 'th cbi-section-table-cell cbi-section-actions'
				}));

			trEls.appendChild(trEl);
		}

		if (has_descriptions) {
			var trEl = E('div', {
				'class': 'tr cbi-section-table-descr ' + anon_class
			});

			for (var i = 0, opt; i < max_cols && (opt = this.children[i]) != null; i++) {
				if (opt.optional || opt.modalonly)
					continue;

				trEl.appendChild(E('div', {
					'class': 'th cbi-section-table-cell',
					'data-widget': opt.__name__
				}, opt.description));

				if (opt.width != null)
					trEl.lastElementChild.style.width =
						(typeof(opt.width) == 'number') ? opt.width+'px' : opt.width;
			}

			if (this.sortable || this.extedit || this.addremove || has_more)
				trEl.appendChild(E('div', {
					'class': 'th cbi-section-table-cell cbi-section-actions'
				}));

			trEls.appendChild(trEl);
		}

		return trEls;
	},

	renderRowActions: function(section_id, more_label) {
		var config_name = this.uciconfig || this.map.config;

		if (!this.sortable && !this.extedit && !this.addremove && !more_label)
			return E([]);

		var tdEl = E('div', {
			'class': 'td cbi-section-table-cell nowrap cbi-section-actions'
		}, E('div'));

		if (this.sortable) {
			L.dom.append(tdEl.lastElementChild, [
				E('div', {
					'title': _('Drag to reorder'),
					'class': 'cbi-button drag-handle center',
					'style': 'cursor:move'
				}, '☰')
			]);
		}

		if (this.extedit) {
			var evFn = null;

			if (typeof(this.extedit) == 'function')
				evFn = L.bind(this.extedit, this);
			else if (typeof(this.extedit) == 'string')
				evFn = L.bind(function(sid, ev) {
					location.href = this.extedit.format(sid);
				}, this, section_id);

			L.dom.append(tdEl.lastElementChild,
				E('button', {
					'title': _('Edit'),
					'class': 'cbi-button cbi-button-edit',
					'click': evFn
				}, [ _('Edit') ])
			);
		}

		if (more_label) {
			L.dom.append(tdEl.lastElementChild,
				E('button', {
					'title': more_label,
					'class': 'cbi-button cbi-button-edit',
					'click': L.ui.createHandlerFn(this, 'renderMoreOptionsModal', section_id)
				}, [ more_label ])
			);
		}

		if (this.addremove) {
			var btn_title = this.titleFn('removebtntitle', section_id);

			L.dom.append(tdEl.lastElementChild,
				E('button', {
					'title': btn_title || _('Delete'),
					'class': 'cbi-button cbi-button-remove',
					'click': L.ui.createHandlerFn(this, 'handleRemove', section_id)
				}, [ btn_title || _('Delete') ])
			);
		}

		return tdEl;
	},

	handleDragInit: function(ev) {
		scope.dragState = { node: ev.target };
	},

	handleDragStart: function(ev) {
		if (!scope.dragState || !scope.dragState.node.classList.contains('drag-handle')) {
			scope.dragState = null;
			ev.preventDefault();
			return false;
		}

		scope.dragState.node = L.dom.parent(scope.dragState.node, '.tr');
		ev.dataTransfer.setData('text', 'drag');
		ev.target.style.opacity = 0.4;
	},

	handleDragOver: function(ev) {
		var n = scope.dragState.targetNode,
		    r = scope.dragState.rect,
		    t = r.top + r.height / 2;

		if (ev.clientY <= t) {
			n.classList.remove('drag-over-below');
			n.classList.add('drag-over-above');
		}
		else {
			n.classList.remove('drag-over-above');
			n.classList.add('drag-over-below');
		}

		ev.dataTransfer.dropEffect = 'move';
		ev.preventDefault();
		return false;
	},

	handleDragEnter: function(ev) {
		scope.dragState.rect = ev.currentTarget.getBoundingClientRect();
		scope.dragState.targetNode = ev.currentTarget;
	},

	handleDragLeave: function(ev) {
		ev.currentTarget.classList.remove('drag-over-above');
		ev.currentTarget.classList.remove('drag-over-below');
	},

	handleDragEnd: function(ev) {
		var n = ev.target;

		n.style.opacity = '';
		n.classList.add('flash');
		n.parentNode.querySelectorAll('.drag-over-above, .drag-over-below')
			.forEach(function(tr) {
				tr.classList.remove('drag-over-above');
				tr.classList.remove('drag-over-below');
			});
	},

	handleDrop: function(ev) {
		var s = scope.dragState;

		if (s.node && s.targetNode) {
			var config_name = this.uciconfig || this.map.config,
			    ref_node = s.targetNode,
			    after = false;

		    if (ref_node.classList.contains('drag-over-below')) {
		    	ref_node = ref_node.nextElementSibling;
		    	after = true;
		    }

		    var sid1 = s.node.getAttribute('data-sid'),
		        sid2 = s.targetNode.getAttribute('data-sid');

		    s.node.parentNode.insertBefore(s.node, ref_node);
		    this.map.data.move(config_name, sid1, sid2, after);
		}

		scope.dragState = null;
		ev.target.style.opacity = '';
		ev.stopPropagation();
		ev.preventDefault();
		return false;
	},

	handleModalCancel: function(modalMap, ev) {
		return Promise.resolve(L.ui.hideModal());
	},

	handleModalSave: function(modalMap, ev) {
		return modalMap.save()
			.then(L.bind(this.map.load, this.map))
			.then(L.bind(this.map.reset, this.map))
			.then(L.ui.hideModal)
			.catch(function() {});
	},

	addModalOptions: function(modalSection, section_id, ev) {

	},

	renderMoreOptionsModal: function(section_id, ev) {
		var parent = this.map,
		    title = parent.title,
		    name = null,
		    m = new CBIMap(this.map.config, null, null),
		    s = m.section(CBINamedSection, section_id, this.sectiontype);

		m.parent = parent;

		s.tabs = this.tabs;
		s.tab_names = this.tab_names;

		if ((name = this.titleFn('modaltitle', section_id)) != null)
			title = name;
		else if ((name = this.titleFn('sectiontitle', section_id)) != null)
			title = '%s - %s'.format(parent.title, name);
		else if (!this.anonymous)
			title = '%s - %s'.format(parent.title, section_id);

		for (var i = 0; i < this.children.length; i++) {
			var o1 = this.children[i];

			if (o1.modalonly === false)
				continue;

			var o2 = s.option(o1.constructor, o1.option, o1.title, o1.description);

			for (var k in o1) {
				if (!o1.hasOwnProperty(k))
					continue;

				switch (k) {
				case 'map':
				case 'section':
				case 'option':
				case 'title':
				case 'description':
					continue;

				default:
					o2[k] = o1[k];
				}
			}
		}

		return Promise.resolve(this.addModalOptions(s, section_id, ev)).then(L.bind(m.render, m)).then(L.bind(function(nodes) {
			L.ui.showModal(title, [
				nodes,
				E('div', { 'class': 'right' }, [
					E('button', {
						'class': 'btn',
						'click': L.ui.createHandlerFn(this, 'handleModalCancel', m)
					}, [ _('Dismiss') ]), ' ',
					E('button', {
						'class': 'cbi-button cbi-button-positive important',
						'click': L.ui.createHandlerFn(this, 'handleModalSave', m)
					}, [ _('Save') ])
				])
			], 'cbi-modal');
		}, this)).catch(L.error);
	}
});

var CBIGridSection = CBITableSection.extend({
	tab: function(name, title, description) {
		CBIAbstractSection.prototype.tab.call(this, name, title, description);
	},

	handleAdd: function(ev, name) {
		var config_name = this.uciconfig || this.map.config,
		    section_id = this.map.data.add(config_name, this.sectiontype, name);

		this.addedSection = section_id;
		return this.renderMoreOptionsModal(section_id);
	},

	handleModalSave: function(/* ... */) {
		return this.super('handleModalSave', arguments)
			.then(L.bind(function() { this.addedSection = null }, this));
	},

	handleModalCancel: function(/* ... */) {
		var config_name = this.uciconfig || this.map.config;

		if (this.addedSection != null) {
			this.map.data.remove(config_name, this.addedSection);
			this.addedSection = null;
		}

		return this.super('handleModalCancel', arguments);
	},

	renderUCISection: function(section_id) {
		return this.renderOptions(null, section_id);
	},

	renderChildren: function(tab_name, section_id, in_table) {
		var tasks = [], index = 0;

		for (var i = 0, opt; (opt = this.children[i]) != null; i++) {
			if (opt.disable || opt.modalonly)
				continue;

			if (opt.editable)
				tasks.push(opt.render(index++, section_id, in_table));
			else
				tasks.push(this.renderTextValue(section_id, opt));
		}

		return Promise.all(tasks);
	},

	renderTextValue: function(section_id, opt) {
		var title = this.stripTags(opt.title).trim(),
		    descr = this.stripTags(opt.description).trim(),
		    value = opt.textvalue(section_id);

		return E('div', {
			'class': 'td cbi-value-field',
			'data-title': (title != '') ? title : null,
			'data-description': (descr != '') ? descr : null,
			'data-name': opt.option,
			'data-widget': opt.typename || opt.__name__
		}, (value != null) ? value : E('em', _('none')));
	},

	renderRowActions: function(section_id) {
		return this.super('renderRowActions', [ section_id, _('Edit') ]);
	},

	parse: function() {
		var section_ids = this.cfgsections(),
		    tasks = [];

		if (Array.isArray(this.children)) {
			for (var i = 0; i < section_ids.length; i++) {
				for (var j = 0; j < this.children.length; j++) {
					if (!this.children[j].editable || this.children[j].modalonly)
						continue;

					tasks.push(this.children[j].parse(section_ids[i]));
				}
			}
		}

		return Promise.all(tasks);
	}
});

var CBINamedSection = CBIAbstractSection.extend({
	__name__: 'CBI.NamedSection',
	__init__: function(map, section_id /*, ... */) {
		this.super('__init__', this.varargs(arguments, 2, map));

		this.section = section_id;
	},

	cfgsections: function() {
		return [ this.section ];
	},

	handleAdd: function(ev) {
		var section_id = this.section,
		    config_name = this.uciconfig || this.map.config;

		this.map.data.add(config_name, this.sectiontype, section_id);
		return this.map.save(null, true);
	},

	handleRemove: function(ev) {
		var section_id = this.section,
		    config_name = this.uciconfig || this.map.config;

		this.map.data.remove(config_name, section_id);
		return this.map.save(null, true);
	},

	renderContents: function(data) {
		var ucidata = data[0], nodes = data[1],
		    section_id = this.section,
		    config_name = this.uciconfig || this.map.config,
		    sectionEl = E('div', {
				'id': ucidata ? null : 'cbi-%s-%s'.format(config_name, section_id),
				'class': 'cbi-section',
				'data-tab': (this.map.tabbed && !this.parentoption) ? this.sectiontype : null,
				'data-tab-title': (this.map.tabbed && !this.parentoption) ? this.title || this.sectiontype : null
			});

		if (typeof(this.title) === 'string' && this.title !== '')
			sectionEl.appendChild(E('legend', {}, this.title));

		if (typeof(this.description) === 'string' && this.description !== '')
			sectionEl.appendChild(E('div', { 'class': 'cbi-section-descr' }, this.description));

		if (ucidata) {
			if (this.addremove) {
				sectionEl.appendChild(
					E('div', { 'class': 'cbi-section-remove right' },
						E('button', {
							'class': 'cbi-button',
							'click': L.ui.createHandlerFn(this, 'handleRemove')
						}, [ _('Delete') ])));
			}

			sectionEl.appendChild(E('div', {
				'id': 'cbi-%s-%s'.format(config_name, section_id),
				'class': this.tabs
					? 'cbi-section-node cbi-section-node-tabbed' : 'cbi-section-node',
				'data-section-id': section_id
			}, nodes));
		}
		else if (this.addremove) {
			sectionEl.appendChild(
				E('button', {
					'class': 'cbi-button cbi-button-add',
					'click': L.ui.createHandlerFn(this, 'handleAdd')
				}, [ _('Add') ]));
		}

		L.dom.bindClassInstance(sectionEl, this);

		return sectionEl;
	},

	render: function() {
		var config_name = this.uciconfig || this.map.config,
		    section_id = this.section;

		return Promise.all([
			this.map.data.get(config_name, section_id),
			this.renderUCISection(section_id)
		]).then(this.renderContents.bind(this));
	}
});

var CBIValue = CBIAbstractValue.extend({
	__name__: 'CBI.Value',

	value: function(key, val) {
		this.keylist = this.keylist || [];
		this.keylist.push(String(key));

		this.vallist = this.vallist || [];
		this.vallist.push(L.dom.elem(val) ? val : String(val != null ? val : key));
	},

	render: function(option_index, section_id, in_table) {
		return Promise.resolve(this.cfgvalue(section_id))
			.then(this.renderWidget.bind(this, section_id, option_index))
			.then(this.renderFrame.bind(this, section_id, in_table, option_index));
	},

	renderFrame: function(section_id, in_table, option_index, nodes) {
		var config_name = this.uciconfig || this.section.uciconfig || this.map.config,
		    depend_list = this.transformDepList(section_id),
		    optionEl;

		if (in_table) {
			var title = this.stripTags(this.title).trim();
			optionEl = E('div', {
				'class': 'td cbi-value-field',
				'data-title': (title != '') ? title : null,
				'data-description': this.stripTags(this.description).trim(),
				'data-name': this.option,
				'data-widget': this.typename || (this.template ? this.template.replace(/^.+\//, '') : null) || this.__name__
			}, E('div', {
				'id': 'cbi-%s-%s-%s'.format(config_name, section_id, this.option),
				'data-index': option_index,
				'data-depends': depend_list,
				'data-field': this.cbid(section_id)
			}));
		}
		else {
			optionEl = E('div', {
				'class': 'cbi-value',
				'id': 'cbi-%s-%s-%s'.format(config_name, section_id, this.option),
				'data-index': option_index,
				'data-depends': depend_list,
				'data-field': this.cbid(section_id),
				'data-name': this.option,
				'data-widget': this.typename || (this.template ? this.template.replace(/^.+\//, '') : null) || this.__name__
			});

			if (this.last_child)
				optionEl.classList.add('cbi-value-last');

			if (typeof(this.title) === 'string' && this.title !== '') {
				optionEl.appendChild(E('label', {
					'class': 'cbi-value-title',
					'for': 'widget.cbid.%s.%s.%s'.format(config_name, section_id, this.option)
				},
				this.titleref ? E('a', {
					'class': 'cbi-title-ref',
					'href': this.titleref,
					'title': this.titledesc || _('Go to relevant configuration page')
				}, this.title) : this.title));

				optionEl.appendChild(E('div', { 'class': 'cbi-value-field' }));
			}
		}

		if (nodes)
			(optionEl.lastChild || optionEl).appendChild(nodes);

		if (!in_table && typeof(this.description) === 'string' && this.description !== '')
			L.dom.append(optionEl.lastChild || optionEl,
				E('div', { 'class': 'cbi-value-description' }, this.description));

		if (depend_list && depend_list.length)
			optionEl.classList.add('hidden');

		optionEl.addEventListener('widget-change',
			L.bind(this.map.checkDepends, this.map));

		L.dom.bindClassInstance(optionEl, this);

		return optionEl;
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var value = (cfgvalue != null) ? cfgvalue : this.default,
		    choices = this.transformChoices(),
		    widget;

		if (choices) {
			var placeholder = (this.optional || this.rmempty)
				? E('em', _('unspecified')) : _('-- Please choose --');

			widget = new ui.Combobox(Array.isArray(value) ? value.join(' ') : value, choices, {
				id: this.cbid(section_id),
				sort: this.keylist,
				optional: this.optional || this.rmempty,
				datatype: this.datatype,
				select_placeholder: this.placeholder || placeholder,
				validate: L.bind(this.validate, this, section_id)
			});
		}
		else {
			widget = new ui.Textfield(Array.isArray(value) ? value.join(' ') : value, {
				id: this.cbid(section_id),
				password: this.password,
				optional: this.optional || this.rmempty,
				datatype: this.datatype,
				placeholder: this.placeholder,
				validate: L.bind(this.validate, this, section_id)
			});
		}

		return widget.render();
	}
});

var CBIDynamicList = CBIValue.extend({
	__name__: 'CBI.DynamicList',

	renderWidget: function(section_id, option_index, cfgvalue) {
		var value = (cfgvalue != null) ? cfgvalue : this.default,
		    choices = this.transformChoices(),
		    items = L.toArray(value);

		var widget = new ui.DynamicList(items, choices, {
			id: this.cbid(section_id),
			sort: this.keylist,
			optional: this.optional || this.rmempty,
			datatype: this.datatype,
			placeholder: this.placeholder,
			validate: L.bind(this.validate, this, section_id)
		});

		return widget.render();
	},
});

var CBIListValue = CBIValue.extend({
	__name__: 'CBI.ListValue',

	__init__: function() {
		this.super('__init__', arguments);
		this.widget = 'select';
		this.deplist = [];
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var choices = this.transformChoices();
		var widget = new ui.Select((cfgvalue != null) ? cfgvalue : this.default, choices, {
			id: this.cbid(section_id),
			size: this.size,
			sort: this.keylist,
			optional: this.optional,
			placeholder: this.placeholder,
			validate: L.bind(this.validate, this, section_id)
		});

		return widget.render();
	},
});

var CBIFlagValue = CBIValue.extend({
	__name__: 'CBI.FlagValue',

	__init__: function() {
		this.super('__init__', arguments);

		this.enabled = '1';
		this.disabled = '0';
		this.default = this.disabled;
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var widget = new ui.Checkbox((cfgvalue != null) ? cfgvalue : this.default, {
			id: this.cbid(section_id),
			value_enabled: this.enabled,
			value_disabled: this.disabled,
			validate: L.bind(this.validate, this, section_id)
		});

		return widget.render();
	},

	formvalue: function(section_id) {
		var elem = this.getUIElement(section_id),
		    checked = elem ? elem.isChecked() : false;
		return checked ? this.enabled : this.disabled;
	},

	textvalue: function(section_id) {
		var cval = this.cfgvalue(section_id);

		if (cval == null)
			cval = this.default;

		return (cval == this.enabled) ? _('Yes') : _('No');
	},

	parse: function(section_id) {
		if (this.isActive(section_id)) {
			var fval = this.formvalue(section_id);

			if (!this.isValid(section_id))
				return Promise.reject();

			if (fval == this.default && (this.optional || this.rmempty))
				return Promise.resolve(this.remove(section_id));
			else
				return Promise.resolve(this.write(section_id, fval));
		}
		else {
			return Promise.resolve(this.remove(section_id));
		}
	},
});

var CBIMultiValue = CBIDynamicList.extend({
	__name__: 'CBI.MultiValue',

	__init__: function() {
		this.super('__init__', arguments);
		this.placeholder = _('-- Please choose --');
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var value = (cfgvalue != null) ? cfgvalue : this.default,
		    choices = this.transformChoices();

		var widget = new ui.Dropdown(L.toArray(value), choices, {
			id: this.cbid(section_id),
			sort: this.keylist,
			multiple: true,
			optional: this.optional || this.rmempty,
			select_placeholder: this.placeholder,
			display_items: this.display_size || this.size || 3,
			dropdown_items: this.dropdown_size || this.size || -1,
			validate: L.bind(this.validate, this, section_id)
		});

		return widget.render();
	},
});

var CBITextValue = CBIValue.extend({
	__name__: 'CBI.TextValue',

	value: null,

	renderWidget: function(section_id, option_index, cfgvalue) {
		var value = (cfgvalue != null) ? cfgvalue : this.default;

		var widget = new ui.Textarea(value, {
			id: this.cbid(section_id),
			optional: this.optional || this.rmempty,
			placeholder: this.placeholder,
			monospace: this.monospace,
			cols: this.cols,
			rows: this.rows,
			wrap: this.wrap,
			validate: L.bind(this.validate, this, section_id)
		});

		return widget.render();
	}
});

var CBIDummyValue = CBIValue.extend({
	__name__: 'CBI.DummyValue',

	renderWidget: function(section_id, option_index, cfgvalue) {
		var value = (cfgvalue != null) ? cfgvalue : this.default,
		    hiddenEl = new ui.Hiddenfield(value, { id: this.cbid(section_id) }),
		    outputEl = E('div');

		if (this.href)
			outputEl.appendChild(E('a', { 'href': this.href }));

		L.dom.append(outputEl.lastChild || outputEl,
			this.rawhtml ? value : [ value ]);

		return E([
			outputEl,
			hiddenEl.render()
		]);
	},

	remove: function() {},
	write: function() {}
});

var CBIButtonValue = CBIValue.extend({
	__name__: 'CBI.ButtonValue',

	renderWidget: function(section_id, option_index, cfgvalue) {
		var value = (cfgvalue != null) ? cfgvalue : this.default,
		    hiddenEl = new ui.Hiddenfield(value, { id: this.cbid(section_id) }),
		    outputEl = E('div'),
		    btn_title = this.titleFn('inputtitle', section_id) || this.titleFn('title', section_id);

		if (value !== false)
			L.dom.content(outputEl, [
				E('button', {
					'class': 'cbi-button cbi-button-%s'.format(this.inputstyle || 'button'),
					'click': L.ui.createHandlerFn(this, function(section_id, ev) {
						if (this.onclick)
							return this.onclick(ev, section_id);

						ev.currentTarget.parentNode.nextElementSibling.value = value;
						return this.map.save();
					}, section_id)
				}, [ btn_title ])
			]);
		else
			L.dom.content(outputEl, ' - ');

		return E([
			outputEl,
			hiddenEl.render()
		]);
	}
});

var CBIHiddenValue = CBIValue.extend({
	__name__: 'CBI.HiddenValue',

	renderWidget: function(section_id, option_index, cfgvalue) {
		var widget = new ui.Hiddenfield((cfgvalue != null) ? cfgvalue : this.default, {
			id: this.cbid(section_id)
		});

		return widget.render();
	}
});

var CBIFileUpload = CBIValue.extend({
	__name__: 'CBI.FileSelect',

	__init__: function(/* ... */) {
		this.super('__init__', arguments);

		this.show_hidden = false;
		this.enable_upload = true;
		this.enable_remove = true;
		this.root_directory = '/etc/luci-uploads';
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var browserEl = new ui.FileUpload((cfgvalue != null) ? cfgvalue : this.default, {
			id: this.cbid(section_id),
			name: this.cbid(section_id),
			show_hidden: this.show_hidden,
			enable_upload: this.enable_upload,
			enable_remove: this.enable_remove,
			root_directory: this.root_directory
		});

		return browserEl.render();
	}
});

var CBISectionValue = CBIValue.extend({
	__name__: 'CBI.ContainerValue',
	__init__: function(map, section, option, cbiClass /*, ... */) {
		this.super('__init__', [map, section, option]);

		if (!CBIAbstractSection.isSubclass(cbiClass))
			throw 'Sub section must be a descendent of CBIAbstractSection';

		this.subsection = cbiClass.instantiate(this.varargs(arguments, 4, this.map));
		this.subsection.parentoption = this;
	},

	load: function(section_id) {
		return this.subsection.load();
	},

	parse: function(section_id) {
		return this.subsection.parse();
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		return this.subsection.render();
	},

	checkDepends: function(section_id) {
		this.subsection.checkDepends();
		return CBIValue.prototype.checkDepends.apply(this, [ section_id ]);
	},

	write: function() {},
	remove: function() {},
	cfgvalue: function() { return null },
	formvalue: function() { return null }
});

return L.Class.extend({
	Map: CBIMap,
	JSONMap: CBIJSONMap,
	AbstractSection: CBIAbstractSection,
	AbstractValue: CBIAbstractValue,

	TypedSection: CBITypedSection,
	TableSection: CBITableSection,
	GridSection: CBIGridSection,
	NamedSection: CBINamedSection,

	Value: CBIValue,
	DynamicList: CBIDynamicList,
	ListValue: CBIListValue,
	Flag: CBIFlagValue,
	MultiValue: CBIMultiValue,
	TextValue: CBITextValue,
	DummyValue: CBIDummyValue,
	Button: CBIButtonValue,
	HiddenValue: CBIHiddenValue,
	FileUpload: CBIFileUpload,
	SectionValue: CBISectionValue
});
