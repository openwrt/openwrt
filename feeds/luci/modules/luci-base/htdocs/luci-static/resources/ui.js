'use strict';
'require rpc';
'require uci';
'require validation';
'require fs';

var modalDiv = null,
    tooltipDiv = null,
    tooltipTimeout = null;

var UIElement = L.Class.extend({
	getValue: function() {
		if (L.dom.matches(this.node, 'select') || L.dom.matches(this.node, 'input'))
			return this.node.value;

		return null;
	},

	setValue: function(value) {
		if (L.dom.matches(this.node, 'select') || L.dom.matches(this.node, 'input'))
			this.node.value = value;
	},

	isValid: function() {
		return (this.validState !== false);
	},

	triggerValidation: function() {
		if (typeof(this.vfunc) != 'function')
			return false;

		var wasValid = this.isValid();

		this.vfunc();

		return (wasValid != this.isValid());
	},

	registerEvents: function(targetNode, synevent, events) {
		var dispatchFn = L.bind(function(ev) {
			this.node.dispatchEvent(new CustomEvent(synevent, { bubbles: true }));
		}, this);

		for (var i = 0; i < events.length; i++)
			targetNode.addEventListener(events[i], dispatchFn);
	},

	setUpdateEvents: function(targetNode /*, ... */) {
		var datatype = this.options.datatype,
		    optional = this.options.hasOwnProperty('optional') ? this.options.optional : true,
		    validate = this.options.validate,
		    events = this.varargs(arguments, 1);

		this.registerEvents(targetNode, 'widget-update', events);

		if (!datatype && !validate)
			return;

		this.vfunc = L.ui.addValidator.apply(L.ui, [
			targetNode, datatype || 'string',
			optional, validate
		].concat(events));

		this.node.addEventListener('validation-success', L.bind(function(ev) {
			this.validState = true;
		}, this));

		this.node.addEventListener('validation-failure', L.bind(function(ev) {
			this.validState = false;
		}, this));
	},

	setChangeEvents: function(targetNode /*, ... */) {
		var tag_changed = L.bind(function(ev) { this.setAttribute('data-changed', true) }, this.node);

		for (var i = 1; i < arguments.length; i++)
			targetNode.addEventListener(arguments[i], tag_changed);

		this.registerEvents(targetNode, 'widget-change', this.varargs(arguments, 1));
	}
});

var UITextfield = UIElement.extend({
	__init__: function(value, options) {
		this.value = value;
		this.options = Object.assign({
			optional: true,
			password: false
		}, options);
	},

	render: function() {
		var frameEl = E('div', { 'id': this.options.id });

		if (this.options.password) {
			frameEl.classList.add('nowrap');
			frameEl.appendChild(E('input', {
				'type': 'password',
				'style': 'position:absolute; left:-100000px',
				'aria-hidden': true,
				'tabindex': -1,
				'name': this.options.name ? 'password.%s'.format(this.options.name) : null
			}));
		}

		frameEl.appendChild(E('input', {
			'id': this.options.id ? 'widget.' + this.options.id : null,
			'name': this.options.name,
			'type': this.options.password ? 'password' : 'text',
			'class': this.options.password ? 'cbi-input-password' : 'cbi-input-text',
			'readonly': this.options.readonly ? '' : null,
			'maxlength': this.options.maxlength,
			'placeholder': this.options.placeholder,
			'value': this.value,
		}));

		if (this.options.password)
			frameEl.appendChild(E('button', {
				'class': 'cbi-button cbi-button-neutral',
				'title': _('Reveal/hide password'),
				'aria-label': _('Reveal/hide password'),
				'click': function(ev) {
					var e = this.previousElementSibling;
					e.type = (e.type === 'password') ? 'text' : 'password';
					ev.preventDefault();
				}
			}, '∗'));

		return this.bind(frameEl);
	},

	bind: function(frameEl) {
		var inputEl = frameEl.childNodes[+!!this.options.password];

		this.node = frameEl;

		this.setUpdateEvents(inputEl, 'keyup', 'blur');
		this.setChangeEvents(inputEl, 'change');

		L.dom.bindClassInstance(frameEl, this);

		return frameEl;
	},

	getValue: function() {
		var inputEl = this.node.childNodes[+!!this.options.password];
		return inputEl.value;
	},

	setValue: function(value) {
		var inputEl = this.node.childNodes[+!!this.options.password];
		inputEl.value = value;
	}
});

var UITextarea = UIElement.extend({
	__init__: function(value, options) {
		this.value = value;
		this.options = Object.assign({
			optional: true,
			wrap: false,
			cols: null,
			rows: null
		}, options);
	},

	render: function() {
		var frameEl = E('div', { 'id': this.options.id }),
		    value = (this.value != null) ? String(this.value) : '';

		frameEl.appendChild(E('textarea', {
			'id': this.options.id ? 'widget.' + this.options.id : null,
			'name': this.options.name,
			'class': 'cbi-input-textarea',
			'readonly': this.options.readonly ? '' : null,
			'placeholder': this.options.placeholder,
			'style': !this.options.cols ? 'width:100%' : null,
			'cols': this.options.cols,
			'rows': this.options.rows,
			'wrap': this.options.wrap ? '' : null
		}, [ value ]));

		if (this.options.monospace)
			frameEl.firstElementChild.style.fontFamily = 'monospace';

		return this.bind(frameEl);
	},

	bind: function(frameEl) {
		var inputEl = frameEl.firstElementChild;

		this.node = frameEl;

		this.setUpdateEvents(inputEl, 'keyup', 'blur');
		this.setChangeEvents(inputEl, 'change');

		L.dom.bindClassInstance(frameEl, this);

		return frameEl;
	},

	getValue: function() {
		return this.node.firstElementChild.value;
	},

	setValue: function(value) {
		this.node.firstElementChild.value = value;
	}
});

var UICheckbox = UIElement.extend({
	__init__: function(value, options) {
		this.value = value;
		this.options = Object.assign({
			value_enabled: '1',
			value_disabled: '0'
		}, options);
	},

	render: function() {
		var frameEl = E('div', {
			'id': this.options.id,
			'class': 'cbi-checkbox'
		});

		if (this.options.hiddenname)
			frameEl.appendChild(E('input', {
				'type': 'hidden',
				'name': this.options.hiddenname,
				'value': 1
			}));

		frameEl.appendChild(E('input', {
			'id': this.options.id ? 'widget.' + this.options.id : null,
			'name': this.options.name,
			'type': 'checkbox',
			'value': this.options.value_enabled,
			'checked': (this.value == this.options.value_enabled) ? '' : null
		}));

		return this.bind(frameEl);
	},

	bind: function(frameEl) {
		this.node = frameEl;

		this.setUpdateEvents(frameEl.lastElementChild, 'click', 'blur');
		this.setChangeEvents(frameEl.lastElementChild, 'change');

		L.dom.bindClassInstance(frameEl, this);

		return frameEl;
	},

	isChecked: function() {
		return this.node.lastElementChild.checked;
	},

	getValue: function() {
		return this.isChecked()
			? this.options.value_enabled
			: this.options.value_disabled;
	},

	setValue: function(value) {
		this.node.lastElementChild.checked = (value == this.options.value_enabled);
	}
});

var UISelect = UIElement.extend({
	__init__: function(value, choices, options) {
		if (!L.isObject(choices))
			choices = {};

		if (!Array.isArray(value))
			value = (value != null && value != '') ? [ value ] : [];

		if (!options.multiple && value.length > 1)
			value.length = 1;

		this.values = value;
		this.choices = choices;
		this.options = Object.assign({
			multiple: false,
			widget: 'select',
			orientation: 'horizontal'
		}, options);

		if (this.choices.hasOwnProperty(''))
			this.options.optional = true;
	},

	render: function() {
		var frameEl = E('div', { 'id': this.options.id }),
		    keys = Object.keys(this.choices);

		if (this.options.sort === true)
			keys.sort();
		else if (Array.isArray(this.options.sort))
			keys = this.options.sort;

		if (this.options.widget == 'select') {
			frameEl.appendChild(E('select', {
				'id': this.options.id ? 'widget.' + this.options.id : null,
				'name': this.options.name,
				'size': this.options.size,
				'class': 'cbi-input-select',
				'multiple': this.options.multiple ? '' : null
			}));

			if (this.options.optional)
				frameEl.lastChild.appendChild(E('option', {
					'value': '',
					'selected': (this.values.length == 0 || this.values[0] == '') ? '' : null
				}, [ this.choices[''] || this.options.placeholder || _('-- Please choose --') ]));

			for (var i = 0; i < keys.length; i++) {
				if (keys[i] == null || keys[i] == '')
					continue;

				frameEl.lastChild.appendChild(E('option', {
					'value': keys[i],
					'selected': (this.values.indexOf(keys[i]) > -1) ? '' : null
				}, [ this.choices[keys[i]] || keys[i] ]));
			}
		}
		else {
			var brEl = (this.options.orientation === 'horizontal') ? document.createTextNode(' ') : E('br');

			for (var i = 0; i < keys.length; i++) {
				frameEl.appendChild(E('label', {}, [
					E('input', {
						'id': this.options.id ? 'widget.' + this.options.id : null,
						'name': this.options.id || this.options.name,
						'type': this.options.multiple ? 'checkbox' : 'radio',
						'class': this.options.multiple ? 'cbi-input-checkbox' : 'cbi-input-radio',
						'value': keys[i],
						'checked': (this.values.indexOf(keys[i]) > -1) ? '' : null
					}),
					this.choices[keys[i]] || keys[i]
				]));

				if (i + 1 == this.options.size)
					frameEl.appendChild(brEl);
			}
		}

		return this.bind(frameEl);
	},

	bind: function(frameEl) {
		this.node = frameEl;

		if (this.options.widget == 'select') {
			this.setUpdateEvents(frameEl.firstChild, 'change', 'click', 'blur');
			this.setChangeEvents(frameEl.firstChild, 'change');
		}
		else {
			var radioEls = frameEl.querySelectorAll('input[type="radio"]');
			for (var i = 0; i < radioEls.length; i++) {
				this.setUpdateEvents(radioEls[i], 'change', 'click', 'blur');
				this.setChangeEvents(radioEls[i], 'change', 'click', 'blur');
			}
		}

		L.dom.bindClassInstance(frameEl, this);

		return frameEl;
	},

	getValue: function() {
		if (this.options.widget == 'select')
			return this.node.firstChild.value;

		var radioEls = frameEl.querySelectorAll('input[type="radio"]');
		for (var i = 0; i < radioEls.length; i++)
			if (radioEls[i].checked)
				return radioEls[i].value;

		return null;
	},

	setValue: function(value) {
		if (this.options.widget == 'select') {
			if (value == null)
				value = '';

			for (var i = 0; i < this.node.firstChild.options.length; i++)
				this.node.firstChild.options[i].selected = (this.node.firstChild.options[i].value == value);

			return;
		}

		var radioEls = frameEl.querySelectorAll('input[type="radio"]');
		for (var i = 0; i < radioEls.length; i++)
			radioEls[i].checked = (radioEls[i].value == value);
	}
});

var UIDropdown = UIElement.extend({
	__init__: function(value, choices, options) {
		if (typeof(choices) != 'object')
			choices = {};

		if (!Array.isArray(value))
			this.values = (value != null && value != '') ? [ value ] : [];
		else
			this.values = value;

		this.choices = choices;
		this.options = Object.assign({
			sort:               true,
			multiple:           Array.isArray(value),
			optional:           true,
			select_placeholder: _('-- Please choose --'),
			custom_placeholder: _('-- custom --'),
			display_items:      3,
			dropdown_items:     -1,
			create:             false,
			create_query:       '.create-item-input',
			create_template:    'script[type="item-template"]'
		}, options);
	},

	render: function() {
		var sb = E('div', {
			'id': this.options.id,
			'class': 'cbi-dropdown',
			'multiple': this.options.multiple ? '' : null,
			'optional': this.options.optional ? '' : null,
		}, E('ul'));

		var keys = Object.keys(this.choices);

		if (this.options.sort === true)
			keys.sort();
		else if (Array.isArray(this.options.sort))
			keys = this.options.sort;

		if (this.options.create)
			for (var i = 0; i < this.values.length; i++)
				if (!this.choices.hasOwnProperty(this.values[i]))
					keys.push(this.values[i]);

		for (var i = 0; i < keys.length; i++) {
			var label = this.choices[keys[i]];

			if (L.dom.elem(label))
				label = label.cloneNode(true);

			sb.lastElementChild.appendChild(E('li', {
				'data-value': keys[i],
				'selected': (this.values.indexOf(keys[i]) > -1) ? '' : null
			}, [ label || keys[i] ]));
		}

		if (this.options.create) {
			var createEl = E('input', {
				'type': 'text',
				'class': 'create-item-input',
				'readonly': this.options.readonly ? '' : null,
				'maxlength': this.options.maxlength,
				'placeholder': this.options.custom_placeholder || this.options.placeholder
			});

			if (this.options.datatype)
				L.ui.addValidator(createEl, this.options.datatype,
				                  true, null, 'blur', 'keyup');

			sb.lastElementChild.appendChild(E('li', { 'data-value': '-' }, createEl));
		}

		if (this.options.create_markup)
			sb.appendChild(E('script', { type: 'item-template' },
				this.options.create_markup));

		return this.bind(sb);
	},

	bind: function(sb) {
		var o = this.options;

		o.multiple = sb.hasAttribute('multiple');
		o.optional = sb.hasAttribute('optional');
		o.placeholder = sb.getAttribute('placeholder') || o.placeholder;
		o.display_items = parseInt(sb.getAttribute('display-items') || o.display_items);
		o.dropdown_items = parseInt(sb.getAttribute('dropdown-items') || o.dropdown_items);
		o.create_query = sb.getAttribute('item-create') || o.create_query;
		o.create_template = sb.getAttribute('item-template') || o.create_template;

		var ul = sb.querySelector('ul'),
		    more = sb.appendChild(E('span', { class: 'more', tabindex: -1 }, '···')),
		    open = sb.appendChild(E('span', { class: 'open', tabindex: -1 }, '▾')),
		    canary = sb.appendChild(E('div')),
		    create = sb.querySelector(this.options.create_query),
		    ndisplay = this.options.display_items,
		    n = 0;

		if (this.options.multiple) {
			var items = ul.querySelectorAll('li');

			for (var i = 0; i < items.length; i++) {
				this.transformItem(sb, items[i]);

				if (items[i].hasAttribute('selected') && ndisplay-- > 0)
					items[i].setAttribute('display', n++);
			}
		}
		else {
			if (this.options.optional && !ul.querySelector('li[data-value=""]')) {
				var placeholder = E('li', { placeholder: '' },
					this.options.select_placeholder || this.options.placeholder);

				ul.firstChild
					? ul.insertBefore(placeholder, ul.firstChild)
					: ul.appendChild(placeholder);
			}

			var items = ul.querySelectorAll('li'),
			    sel = sb.querySelectorAll('[selected]');

			sel.forEach(function(s) {
				s.removeAttribute('selected');
			});

			var s = sel[0] || items[0];
			if (s) {
				s.setAttribute('selected', '');
				s.setAttribute('display', n++);
			}

			ndisplay--;
		}

		this.saveValues(sb, ul);

		ul.setAttribute('tabindex', -1);
		sb.setAttribute('tabindex', 0);

		if (ndisplay < 0)
			sb.setAttribute('more', '')
		else
			sb.removeAttribute('more');

		if (ndisplay == this.options.display_items)
			sb.setAttribute('empty', '')
		else
			sb.removeAttribute('empty');

		L.dom.content(more, (ndisplay == this.options.display_items)
			? (this.options.select_placeholder || this.options.placeholder) : '···');


		sb.addEventListener('click', this.handleClick.bind(this));
		sb.addEventListener('keydown', this.handleKeydown.bind(this));
		sb.addEventListener('cbi-dropdown-close', this.handleDropdownClose.bind(this));
		sb.addEventListener('cbi-dropdown-select', this.handleDropdownSelect.bind(this));

		if ('ontouchstart' in window) {
			sb.addEventListener('touchstart', function(ev) { ev.stopPropagation(); });
			window.addEventListener('touchstart', this.closeAllDropdowns);
		}
		else {
			sb.addEventListener('mouseover', this.handleMouseover.bind(this));
			sb.addEventListener('focus', this.handleFocus.bind(this));

			canary.addEventListener('focus', this.handleCanaryFocus.bind(this));

			window.addEventListener('mouseover', this.setFocus);
			window.addEventListener('click', this.closeAllDropdowns);
		}

		if (create) {
			create.addEventListener('keydown', this.handleCreateKeydown.bind(this));
			create.addEventListener('focus', this.handleCreateFocus.bind(this));
			create.addEventListener('blur', this.handleCreateBlur.bind(this));

			var li = findParent(create, 'li');

			li.setAttribute('unselectable', '');
			li.addEventListener('click', this.handleCreateClick.bind(this));
		}

		this.node = sb;

		this.setUpdateEvents(sb, 'cbi-dropdown-open', 'cbi-dropdown-close');
		this.setChangeEvents(sb, 'cbi-dropdown-change', 'cbi-dropdown-close');

		L.dom.bindClassInstance(sb, this);

		return sb;
	},

	openDropdown: function(sb) {
		var st = window.getComputedStyle(sb, null),
		    ul = sb.querySelector('ul'),
		    li = ul.querySelectorAll('li'),
		    fl = findParent(sb, '.cbi-value-field'),
		    sel = ul.querySelector('[selected]'),
		    rect = sb.getBoundingClientRect(),
		    items = Math.min(this.options.dropdown_items, li.length);

		document.querySelectorAll('.cbi-dropdown[open]').forEach(function(s) {
			s.dispatchEvent(new CustomEvent('cbi-dropdown-close', {}));
		});

		sb.setAttribute('open', '');

		var pv = ul.cloneNode(true);
		    pv.classList.add('preview');

		if (fl)
			fl.classList.add('cbi-dropdown-open');

		if ('ontouchstart' in window) {
			var vpWidth = Math.max(document.documentElement.clientWidth, window.innerWidth || 0),
			    vpHeight = Math.max(document.documentElement.clientHeight, window.innerHeight || 0),
			    scrollFrom = window.pageYOffset,
			    scrollTo = scrollFrom + rect.top - vpHeight * 0.5,
			    start = null;

			ul.style.top = sb.offsetHeight + 'px';
			ul.style.left = -rect.left + 'px';
			ul.style.right = (rect.right - vpWidth) + 'px';
			ul.style.maxHeight = (vpHeight * 0.5) + 'px';
			ul.style.WebkitOverflowScrolling = 'touch';

			var scrollStep = function(timestamp) {
				if (!start) {
					start = timestamp;
					ul.scrollTop = sel ? Math.max(sel.offsetTop - sel.offsetHeight, 0) : 0;
				}

				var duration = Math.max(timestamp - start, 1);
				if (duration < 100) {
					document.body.scrollTop = scrollFrom + (scrollTo - scrollFrom) * (duration / 100);
					window.requestAnimationFrame(scrollStep);
				}
				else {
					document.body.scrollTop = scrollTo;
				}
			};

			window.requestAnimationFrame(scrollStep);
		}
		else {
			ul.style.maxHeight = '1px';
			ul.style.top = ul.style.bottom = '';

			window.requestAnimationFrame(function() {
				var itemHeight = li[Math.max(0, li.length - 2)].getBoundingClientRect().height,
				    fullHeight = 0,
				    spaceAbove = rect.top,
				    spaceBelow = window.innerHeight - rect.height - rect.top;

				for (var i = 0; i < (items == -1 ? li.length : items); i++)
					fullHeight += li[i].getBoundingClientRect().height;

				if (fullHeight <= spaceBelow) {
					ul.style.top = rect.height + 'px';
					ul.style.maxHeight = spaceBelow + 'px';
				}
				else if (fullHeight <= spaceAbove) {
					ul.style.bottom = rect.height + 'px';
					ul.style.maxHeight = spaceAbove + 'px';
				}
				else if (spaceBelow >= spaceAbove) {
					ul.style.top = rect.height + 'px';
					ul.style.maxHeight = (spaceBelow - (spaceBelow % itemHeight)) + 'px';
				}
				else {
					ul.style.bottom = rect.height + 'px';
					ul.style.maxHeight = (spaceAbove - (spaceAbove % itemHeight)) + 'px';
				}

				ul.scrollTop = sel ? Math.max(sel.offsetTop - sel.offsetHeight, 0) : 0;
			});
		}

		var cboxes = ul.querySelectorAll('[selected] input[type="checkbox"]');
		for (var i = 0; i < cboxes.length; i++) {
			cboxes[i].checked = true;
			cboxes[i].disabled = (cboxes.length == 1 && !this.options.optional);
		};

		ul.classList.add('dropdown');

		sb.insertBefore(pv, ul.nextElementSibling);

		li.forEach(function(l) {
			l.setAttribute('tabindex', 0);
		});

		sb.lastElementChild.setAttribute('tabindex', 0);

		this.setFocus(sb, sel || li[0], true);
	},

	closeDropdown: function(sb, no_focus) {
		if (!sb.hasAttribute('open'))
			return;

		var pv = sb.querySelector('ul.preview'),
		    ul = sb.querySelector('ul.dropdown'),
		    li = ul.querySelectorAll('li'),
		    fl = findParent(sb, '.cbi-value-field');

		li.forEach(function(l) { l.removeAttribute('tabindex'); });
		sb.lastElementChild.removeAttribute('tabindex');

		sb.removeChild(pv);
		sb.removeAttribute('open');
		sb.style.width = sb.style.height = '';

		ul.classList.remove('dropdown');
		ul.style.top = ul.style.bottom = ul.style.maxHeight = '';

		if (fl)
			fl.classList.remove('cbi-dropdown-open');

		if (!no_focus)
			this.setFocus(sb, sb);

		this.saveValues(sb, ul);
	},

	toggleItem: function(sb, li, force_state) {
		if (li.hasAttribute('unselectable'))
			return;

		if (this.options.multiple) {
			var cbox = li.querySelector('input[type="checkbox"]'),
			    items = li.parentNode.querySelectorAll('li'),
			    label = sb.querySelector('ul.preview'),
			    sel = li.parentNode.querySelectorAll('[selected]').length,
			    more = sb.querySelector('.more'),
			    ndisplay = this.options.display_items,
			    n = 0;

			if (li.hasAttribute('selected')) {
				if (force_state !== true) {
					if (sel > 1 || this.options.optional) {
						li.removeAttribute('selected');
						cbox.checked = cbox.disabled = false;
						sel--;
					}
					else {
						cbox.disabled = true;
					}
				}
			}
			else {
				if (force_state !== false) {
					li.setAttribute('selected', '');
					cbox.checked = true;
					cbox.disabled = false;
					sel++;
				}
			}

			while (label && label.firstElementChild)
				label.removeChild(label.firstElementChild);

			for (var i = 0; i < items.length; i++) {
				items[i].removeAttribute('display');
				if (items[i].hasAttribute('selected')) {
					if (ndisplay-- > 0) {
						items[i].setAttribute('display', n++);
						if (label)
							label.appendChild(items[i].cloneNode(true));
					}
					var c = items[i].querySelector('input[type="checkbox"]');
					if (c)
						c.disabled = (sel == 1 && !this.options.optional);
				}
			}

			if (ndisplay < 0)
				sb.setAttribute('more', '');
			else
				sb.removeAttribute('more');

			if (ndisplay === this.options.display_items)
				sb.setAttribute('empty', '');
			else
				sb.removeAttribute('empty');

			L.dom.content(more, (ndisplay === this.options.display_items)
				? (this.options.select_placeholder || this.options.placeholder) : '···');
		}
		else {
			var sel = li.parentNode.querySelector('[selected]');
			if (sel) {
				sel.removeAttribute('display');
				sel.removeAttribute('selected');
			}

			li.setAttribute('display', 0);
			li.setAttribute('selected', '');

			this.closeDropdown(sb, true);
		}

		this.saveValues(sb, li.parentNode);
	},

	transformItem: function(sb, li) {
		var cbox = E('form', {}, E('input', { type: 'checkbox', tabindex: -1, onclick: 'event.preventDefault()' })),
		    label = E('label');

		while (li.firstChild)
			label.appendChild(li.firstChild);

		li.appendChild(cbox);
		li.appendChild(label);
	},

	saveValues: function(sb, ul) {
		var sel = ul.querySelectorAll('li[selected]'),
		    div = sb.lastElementChild,
		    name = this.options.name,
		    strval = '',
		    values = [];

		while (div.lastElementChild)
			div.removeChild(div.lastElementChild);

		sel.forEach(function (s) {
			if (s.hasAttribute('placeholder'))
				return;

			var v = {
				text: s.innerText,
				value: s.hasAttribute('data-value') ? s.getAttribute('data-value') : s.innerText,
				element: s
			};

			div.appendChild(E('input', {
				type: 'hidden',
				name: name,
				value: v.value
			}));

			values.push(v);

			strval += strval.length ? ' ' + v.value : v.value;
		});

		var detail = {
			instance: this,
			element: sb
		};

		if (this.options.multiple)
			detail.values = values;
		else
			detail.value = values.length ? values[0] : null;

		sb.value = strval;

		sb.dispatchEvent(new CustomEvent('cbi-dropdown-change', {
			bubbles: true,
			detail: detail
		}));
	},

	setValues: function(sb, values) {
		var ul = sb.querySelector('ul');

		if (this.options.create) {
			for (var value in values) {
				this.createItems(sb, value);

				if (!this.options.multiple)
					break;
			}
		}

		if (this.options.multiple) {
			var lis = ul.querySelectorAll('li[data-value]');
			for (var i = 0; i < lis.length; i++) {
				var value = lis[i].getAttribute('data-value');
				if (values === null || !(value in values))
					this.toggleItem(sb, lis[i], false);
				else
					this.toggleItem(sb, lis[i], true);
			}
		}
		else {
			var ph = ul.querySelector('li[placeholder]');
			if (ph)
				this.toggleItem(sb, ph);

			var lis = ul.querySelectorAll('li[data-value]');
			for (var i = 0; i < lis.length; i++) {
				var value = lis[i].getAttribute('data-value');
				if (values !== null && (value in values))
					this.toggleItem(sb, lis[i]);
			}
		}
	},

	setFocus: function(sb, elem, scroll) {
		if (sb && sb.hasAttribute && sb.hasAttribute('locked-in'))
			return;

		if (sb.target && findParent(sb.target, 'ul.dropdown'))
			return;

		document.querySelectorAll('.focus').forEach(function(e) {
			if (!matchesElem(e, 'input')) {
				e.classList.remove('focus');
				e.blur();
			}
		});

		if (elem) {
			elem.focus();
			elem.classList.add('focus');

			if (scroll)
				elem.parentNode.scrollTop = elem.offsetTop - elem.parentNode.offsetTop;
		}
	},

	createChoiceElement: function(sb, value, label) {
		var tpl = sb.querySelector(this.options.create_template),
		    markup = null;

		if (tpl)
			markup = (tpl.textContent || tpl.innerHTML || tpl.firstChild.data).replace(/^<!--|-->$/, '').trim();
		else
			markup = '<li data-value="{{value}}"><span data-label-placeholder="true" /></li>';

		var new_item = E(markup.replace(/{{value}}/g, '%h'.format(value))),
		    placeholder = new_item.querySelector('[data-label-placeholder]');

		if (placeholder) {
			var content = E('span', {}, label || this.choices[value] || [ value ]);

			while (content.firstChild)
				placeholder.parentNode.insertBefore(content.firstChild, placeholder);

			placeholder.parentNode.removeChild(placeholder);
		}

		if (this.options.multiple)
			this.transformItem(sb, new_item);

		return new_item;
	},

	createItems: function(sb, value) {
		var sbox = this,
		    val = (value || '').trim(),
		    ul = sb.querySelector('ul');

		if (!sbox.options.multiple)
			val = val.length ? [ val ] : [];
		else
			val = val.length ? val.split(/\s+/) : [];

		val.forEach(function(item) {
			var new_item = null;

			ul.childNodes.forEach(function(li) {
				if (li.getAttribute && li.getAttribute('data-value') === item)
					new_item = li;
			});

			if (!new_item) {
				new_item = sbox.createChoiceElement(sb, item);

				if (!sbox.options.multiple) {
					var old = ul.querySelector('li[created]');
					if (old)
						ul.removeChild(old);

					new_item.setAttribute('created', '');
				}

				new_item = ul.insertBefore(new_item, ul.lastElementChild);
			}

			sbox.toggleItem(sb, new_item, true);
			sbox.setFocus(sb, new_item, true);
		});
	},

	clearChoices: function(reset_value) {
		var ul = this.node.querySelector('ul'),
		    lis = ul ? ul.querySelectorAll('li[data-value]') : [],
		    len = lis.length - (this.options.create ? 1 : 0),
		    val = reset_value ? null : this.getValue();

		for (var i = 0; i < len; i++) {
			var lival = lis[i].getAttribute('data-value');
			if (val == null ||
				(!this.options.multiple && val != lival) ||
				(this.options.multiple && val.indexOf(lival) == -1))
				ul.removeChild(lis[i]);
		}

		if (reset_value)
			this.setValues(this.node, {});
	},

	addChoices: function(values, labels) {
		var sb = this.node,
		    ul = sb.querySelector('ul'),
		    lis = ul ? ul.querySelectorAll('li[data-value]') : [];

		if (!Array.isArray(values))
			values = L.toArray(values);

		if (!L.isObject(labels))
			labels = {};

		for (var i = 0; i < values.length; i++) {
			var found = false;

			for (var j = 0; j < lis.length; j++) {
				if (lis[j].getAttribute('data-value') === values[i]) {
					found = true;
					break;
				}
			}

			if (found)
				continue;

			ul.insertBefore(
				this.createChoiceElement(sb, values[i], labels[values[i]]),
				ul.lastElementChild);
		}
	},

	closeAllDropdowns: function() {
		document.querySelectorAll('.cbi-dropdown[open]').forEach(function(s) {
			s.dispatchEvent(new CustomEvent('cbi-dropdown-close', {}));
		});
	},

	handleClick: function(ev) {
		var sb = ev.currentTarget;

		if (!sb.hasAttribute('open')) {
			if (!matchesElem(ev.target, 'input'))
				this.openDropdown(sb);
		}
		else {
			var li = findParent(ev.target, 'li');
			if (li && li.parentNode.classList.contains('dropdown'))
				this.toggleItem(sb, li);
			else if (li && li.parentNode.classList.contains('preview'))
				this.closeDropdown(sb);
			else if (matchesElem(ev.target, 'span.open, span.more'))
				this.closeDropdown(sb);
		}

		ev.preventDefault();
		ev.stopPropagation();
	},

	handleKeydown: function(ev) {
		var sb = ev.currentTarget;

		if (matchesElem(ev.target, 'input'))
			return;

		if (!sb.hasAttribute('open')) {
			switch (ev.keyCode) {
			case 37:
			case 38:
			case 39:
			case 40:
				this.openDropdown(sb);
				ev.preventDefault();
			}
		}
		else {
			var active = findParent(document.activeElement, 'li');

			switch (ev.keyCode) {
			case 27:
				this.closeDropdown(sb);
				break;

			case 13:
				if (active) {
					if (!active.hasAttribute('selected'))
						this.toggleItem(sb, active);
					this.closeDropdown(sb);
					ev.preventDefault();
				}
				break;

			case 32:
				if (active) {
					this.toggleItem(sb, active);
					ev.preventDefault();
				}
				break;

			case 38:
				if (active && active.previousElementSibling) {
					this.setFocus(sb, active.previousElementSibling);
					ev.preventDefault();
				}
				break;

			case 40:
				if (active && active.nextElementSibling) {
					this.setFocus(sb, active.nextElementSibling);
					ev.preventDefault();
				}
				break;
			}
		}
	},

	handleDropdownClose: function(ev) {
		var sb = ev.currentTarget;

		this.closeDropdown(sb, true);
	},

	handleDropdownSelect: function(ev) {
		var sb = ev.currentTarget,
		    li = findParent(ev.target, 'li');

		if (!li)
			return;

		this.toggleItem(sb, li);
		this.closeDropdown(sb, true);
	},

	handleMouseover: function(ev) {
		var sb = ev.currentTarget;

		if (!sb.hasAttribute('open'))
			return;

		var li = findParent(ev.target, 'li');

		if (li && li.parentNode.classList.contains('dropdown'))
			this.setFocus(sb, li);
	},

	handleFocus: function(ev) {
		var sb = ev.currentTarget;

		document.querySelectorAll('.cbi-dropdown[open]').forEach(function(s) {
			if (s !== sb || sb.hasAttribute('open'))
				s.dispatchEvent(new CustomEvent('cbi-dropdown-close', {}));
		});
	},

	handleCanaryFocus: function(ev) {
		this.closeDropdown(ev.currentTarget.parentNode);
	},

	handleCreateKeydown: function(ev) {
		var input = ev.currentTarget,
		    sb = findParent(input, '.cbi-dropdown');

		switch (ev.keyCode) {
		case 13:
			ev.preventDefault();

			if (input.classList.contains('cbi-input-invalid'))
				return;

			this.createItems(sb, input.value);
			input.value = '';
			input.blur();
			break;
		}
	},

	handleCreateFocus: function(ev) {
		var input = ev.currentTarget,
		    cbox = findParent(input, 'li').querySelector('input[type="checkbox"]'),
		    sb = findParent(input, '.cbi-dropdown');

		if (cbox)
			cbox.checked = true;

		sb.setAttribute('locked-in', '');
	},

	handleCreateBlur: function(ev) {
		var input = ev.currentTarget,
		    cbox = findParent(input, 'li').querySelector('input[type="checkbox"]'),
		    sb = findParent(input, '.cbi-dropdown');

		if (cbox)
			cbox.checked = false;

		sb.removeAttribute('locked-in');
	},

	handleCreateClick: function(ev) {
		ev.currentTarget.querySelector(this.options.create_query).focus();
	},

	setValue: function(values) {
		if (this.options.multiple) {
			if (!Array.isArray(values))
				values = (values != null && values != '') ? [ values ] : [];

			var v = {};

			for (var i = 0; i < values.length; i++)
				v[values[i]] = true;

			this.setValues(this.node, v);
		}
		else {
			var v = {};

			if (values != null) {
				if (Array.isArray(values))
					v[values[0]] = true;
				else
					v[values] = true;
			}

			this.setValues(this.node, v);
		}
	},

	getValue: function() {
		var div = this.node.lastElementChild,
		    h = div.querySelectorAll('input[type="hidden"]'),
			v = [];

		for (var i = 0; i < h.length; i++)
			v.push(h[i].value);

		return this.options.multiple ? v : v[0];
	}
});

var UICombobox = UIDropdown.extend({
	__init__: function(value, choices, options) {
		this.super('__init__', [ value, choices, Object.assign({
			select_placeholder: _('-- Please choose --'),
			custom_placeholder: _('-- custom --'),
			dropdown_items: -1,
			sort: true
		}, options, {
			multiple: false,
			create: true,
			optional: true
		}) ]);
	}
});

var UIComboButton = UIDropdown.extend({
	__init__: function(value, choices, options) {
		this.super('__init__', [ value, choices, Object.assign({
			sort: true
		}, options, {
			multiple: false,
			create: false,
			optional: false
		}) ]);
	},

	render: function(/* ... */) {
		var node = UIDropdown.prototype.render.apply(this, arguments),
		    val = this.getValue();

		if (L.isObject(this.options.classes) && this.options.classes.hasOwnProperty(val))
			node.setAttribute('class', 'cbi-dropdown ' + this.options.classes[val]);

		return node;
	},

	handleClick: function(ev) {
		var sb = ev.currentTarget,
		    t = ev.target;

		if (sb.hasAttribute('open') || L.dom.matches(t, '.cbi-dropdown > span.open'))
			return UIDropdown.prototype.handleClick.apply(this, arguments);

		if (this.options.click)
			return this.options.click.call(sb, ev, this.getValue());
	},

	toggleItem: function(sb /*, ... */) {
		var rv = UIDropdown.prototype.toggleItem.apply(this, arguments),
		    val = this.getValue();

		if (L.isObject(this.options.classes) && this.options.classes.hasOwnProperty(val))
			sb.setAttribute('class', 'cbi-dropdown ' + this.options.classes[val]);
		else
			sb.setAttribute('class', 'cbi-dropdown');

		return rv;
	}
});

var UIDynamicList = UIElement.extend({
	__init__: function(values, choices, options) {
		if (!Array.isArray(values))
			values = (values != null && values != '') ? [ values ] : [];

		if (typeof(choices) != 'object')
			choices = null;

		this.values = values;
		this.choices = choices;
		this.options = Object.assign({}, options, {
			multiple: false,
			optional: true
		});
	},

	render: function() {
		var dl = E('div', {
			'id': this.options.id,
			'class': 'cbi-dynlist'
		}, E('div', { 'class': 'add-item' }));

		if (this.choices) {
			if (this.options.placeholder != null)
				this.options.select_placeholder = this.options.placeholder;

			var cbox = new UICombobox(null, this.choices, this.options);

			dl.lastElementChild.appendChild(cbox.render());
		}
		else {
			var inputEl = E('input', {
				'id': this.options.id ? 'widget.' + this.options.id : null,
				'type': 'text',
				'class': 'cbi-input-text',
				'placeholder': this.options.placeholder
			});

			dl.lastElementChild.appendChild(inputEl);
			dl.lastElementChild.appendChild(E('div', { 'class': 'cbi-button cbi-button-add' }, '+'));

			if (this.options.datatype)
				L.ui.addValidator(inputEl, this.options.datatype,
				                  true, null, 'blur', 'keyup');
		}

		for (var i = 0; i < this.values.length; i++) {
			var label = this.choices ? this.choices[this.values[i]] : null;

			if (L.dom.elem(label))
				label = label.cloneNode(true);

			this.addItem(dl, this.values[i], label);
		}

		return this.bind(dl);
	},

	bind: function(dl) {
		dl.addEventListener('click', L.bind(this.handleClick, this));
		dl.addEventListener('keydown', L.bind(this.handleKeydown, this));
		dl.addEventListener('cbi-dropdown-change', L.bind(this.handleDropdownChange, this));

		this.node = dl;

		this.setUpdateEvents(dl, 'cbi-dynlist-change');
		this.setChangeEvents(dl, 'cbi-dynlist-change');

		L.dom.bindClassInstance(dl, this);

		return dl;
	},

	addItem: function(dl, value, text, flash) {
		var exists = false,
		    new_item = E('div', { 'class': flash ? 'item flash' : 'item', 'tabindex': 0 }, [
				E('span', {}, [ text || value ]),
				E('input', {
					'type': 'hidden',
					'name': this.options.name,
					'value': value })]);

		dl.querySelectorAll('.item').forEach(function(item) {
			if (exists)
				return;

			var hidden = item.querySelector('input[type="hidden"]');

			if (hidden && hidden.parentNode !== item)
				hidden = null;

			if (hidden && hidden.value === value)
				exists = true;
		});

		if (!exists) {
			var ai = dl.querySelector('.add-item');
			ai.parentNode.insertBefore(new_item, ai);
		}

		dl.dispatchEvent(new CustomEvent('cbi-dynlist-change', {
			bubbles: true,
			detail: {
				instance: this,
				element: dl,
				value: value,
				add: true
			}
		}));
	},

	removeItem: function(dl, item) {
		var value = item.querySelector('input[type="hidden"]').value;
		var sb = dl.querySelector('.cbi-dropdown');
		if (sb)
			sb.querySelectorAll('ul > li').forEach(function(li) {
				if (li.getAttribute('data-value') === value) {
					if (li.hasAttribute('dynlistcustom'))
						li.parentNode.removeChild(li);
					else
						li.removeAttribute('unselectable');
				}
			});

		item.parentNode.removeChild(item);

		dl.dispatchEvent(new CustomEvent('cbi-dynlist-change', {
			bubbles: true,
			detail: {
				instance: this,
				element: dl,
				value: value,
				remove: true
			}
		}));
	},

	handleClick: function(ev) {
		var dl = ev.currentTarget,
		    item = findParent(ev.target, '.item');

		if (item) {
			this.removeItem(dl, item);
		}
		else if (matchesElem(ev.target, '.cbi-button-add')) {
			var input = ev.target.previousElementSibling;
			if (input.value.length && !input.classList.contains('cbi-input-invalid')) {
				this.addItem(dl, input.value, null, true);
				input.value = '';
			}
		}
	},

	handleDropdownChange: function(ev) {
		var dl = ev.currentTarget,
		    sbIn = ev.detail.instance,
		    sbEl = ev.detail.element,
		    sbVal = ev.detail.value;

		if (sbVal === null)
			return;

		sbIn.setValues(sbEl, null);
		sbVal.element.setAttribute('unselectable', '');

		if (sbVal.element.hasAttribute('created')) {
			sbVal.element.removeAttribute('created');
			sbVal.element.setAttribute('dynlistcustom', '');
		}

		var label = sbVal.text;

		if (sbVal.element) {
			label = E([]);

			for (var i = 0; i < sbVal.element.childNodes.length; i++)
				label.appendChild(sbVal.element.childNodes[i].cloneNode(true));
		}

		this.addItem(dl, sbVal.value, label, true);
	},

	handleKeydown: function(ev) {
		var dl = ev.currentTarget,
		    item = findParent(ev.target, '.item');

		if (item) {
			switch (ev.keyCode) {
			case 8: /* backspace */
				if (item.previousElementSibling)
					item.previousElementSibling.focus();

				this.removeItem(dl, item);
				break;

			case 46: /* delete */
				if (item.nextElementSibling) {
					if (item.nextElementSibling.classList.contains('item'))
						item.nextElementSibling.focus();
					else
						item.nextElementSibling.firstElementChild.focus();
				}

				this.removeItem(dl, item);
				break;
			}
		}
		else if (matchesElem(ev.target, '.cbi-input-text')) {
			switch (ev.keyCode) {
			case 13: /* enter */
				if (ev.target.value.length && !ev.target.classList.contains('cbi-input-invalid')) {
					this.addItem(dl, ev.target.value, null, true);
					ev.target.value = '';
					ev.target.blur();
					ev.target.focus();
				}

				ev.preventDefault();
				break;
			}
		}
	},

	getValue: function() {
		var items = this.node.querySelectorAll('.item > input[type="hidden"]'),
		    input = this.node.querySelector('.add-item > input[type="text"]'),
		    v = [];

		for (var i = 0; i < items.length; i++)
			v.push(items[i].value);

		if (input && input.value != null && input.value.match(/\S/) &&
		    input.classList.contains('cbi-input-invalid') == false &&
		    v.filter(function(s) { return s == input.value }).length == 0)
			v.push(input.value);

		return v;
	},

	setValue: function(values) {
		if (!Array.isArray(values))
			values = (values != null && values != '') ? [ values ] : [];

		var items = this.node.querySelectorAll('.item');

		for (var i = 0; i < items.length; i++)
			if (items[i].parentNode === this.node)
				this.removeItem(this.node, items[i]);

		for (var i = 0; i < values.length; i++)
			this.addItem(this.node, values[i],
				this.choices ? this.choices[values[i]] : null);
	},

	addChoices: function(values, labels) {
		var dl = this.node.lastElementChild.firstElementChild;
		L.dom.callClassMethod(dl, 'addChoices', values, labels);
	},

	clearChoices: function() {
		var dl = this.node.lastElementChild.firstElementChild;
		L.dom.callClassMethod(dl, 'clearChoices');
	}
});

var UIHiddenfield = UIElement.extend({
	__init__: function(value, options) {
		this.value = value;
		this.options = Object.assign({

		}, options);
	},

	render: function() {
		var hiddenEl = E('input', {
			'id': this.options.id,
			'type': 'hidden',
			'value': this.value
		});

		return this.bind(hiddenEl);
	},

	bind: function(hiddenEl) {
		this.node = hiddenEl;

		L.dom.bindClassInstance(hiddenEl, this);

		return hiddenEl;
	},

	getValue: function() {
		return this.node.value;
	},

	setValue: function(value) {
		this.node.value = value;
	}
});

var UIFileUpload = UIElement.extend({
	__init__: function(value, options) {
		this.value = value;
		this.options = Object.assign({
			show_hidden: false,
			enable_upload: true,
			enable_remove: true,
			root_directory: '/etc/luci-uploads'
		}, options);
	},

	bind: function(browserEl) {
		this.node = browserEl;

		this.setUpdateEvents(browserEl, 'cbi-fileupload-select', 'cbi-fileupload-cancel');
		this.setChangeEvents(browserEl, 'cbi-fileupload-select', 'cbi-fileupload-cancel');

		L.dom.bindClassInstance(browserEl, this);

		return browserEl;
	},

	render: function() {
		return L.resolveDefault(this.value != null ? fs.stat(this.value) : null).then(L.bind(function(stat) {
			var label;

			if (L.isObject(stat) && stat.type != 'directory')
				this.stat = stat;

			if (this.stat != null)
				label = [ this.iconForType(this.stat.type), ' %s (%1000mB)'.format(this.truncatePath(this.stat.path), this.stat.size) ];
			else if (this.value != null)
				label = [ this.iconForType('file'), ' %s (%s)'.format(this.truncatePath(this.value), _('File not accessible')) ];
			else
				label = [ _('Select file…') ];

			return this.bind(E('div', { 'id': this.options.id }, [
				E('button', {
					'class': 'btn',
					'click': L.ui.createHandlerFn(this, 'handleFileBrowser')
				}, label),
				E('div', {
					'class': 'cbi-filebrowser'
				}),
				E('input', {
					'type': 'hidden',
					'name': this.options.name,
					'value': this.value
				})
			]));
		}, this));
	},

	truncatePath: function(path) {
		if (path.length > 50)
			path = path.substring(0, 25) + '…' + path.substring(path.length - 25);

		return path;
	},

	iconForType: function(type) {
		switch (type) {
		case 'symlink':
			return E('img', {
				'src': L.resource('cbi/link.gif'),
				'title': _('Symbolic link'),
				'class': 'middle'
			});

		case 'directory':
			return E('img', {
				'src': L.resource('cbi/folder.gif'),
				'title': _('Directory'),
				'class': 'middle'
			});

		default:
			return E('img', {
				'src': L.resource('cbi/file.gif'),
				'title': _('File'),
				'class': 'middle'
			});
		}
	},

	canonicalizePath: function(path) {
		return path.replace(/\/{2,}/, '/')
			.replace(/\/\.(\/|$)/g, '/')
			.replace(/[^\/]+\/\.\.(\/|$)/g, '/')
			.replace(/\/$/, '');
	},

	splitPath: function(path) {
		var croot = this.canonicalizePath(this.options.root_directory || '/'),
		    cpath = this.canonicalizePath(path || '/');

		if (cpath.length <= croot.length)
			return [ croot ];

		if (cpath.charAt(croot.length) != '/')
			return [ croot ];

		var parts = cpath.substring(croot.length + 1).split(/\//);

		parts.unshift(croot);

		return parts;
	},

	handleUpload: function(path, list, ev) {
		var form = ev.target.parentNode,
		    fileinput = form.querySelector('input[type="file"]'),
		    nameinput = form.querySelector('input[type="text"]'),
		    filename = (nameinput.value != null ? nameinput.value : '').trim();

		ev.preventDefault();

		if (filename == '' || filename.match(/\//) || fileinput.files[0] == null)
			return;

		var existing = list.filter(function(e) { return e.name == filename })[0];

		if (existing != null && existing.type == 'directory')
			return alert(_('A directory with the same name already exists.'));
		else if (existing != null && !confirm(_('Overwrite existing file "%s" ?').format(filename)))
			return;

		var data = new FormData();

		data.append('sessionid', L.env.sessionid);
		data.append('filename', path + '/' + filename);
		data.append('filedata', fileinput.files[0]);

		return L.Request.post('/cgi-bin/cgi-upload', data, {
			progress: L.bind(function(btn, ev) {
				btn.firstChild.data = '%.2f%%'.format((ev.loaded / ev.total) * 100);
			}, this, ev.target)
		}).then(L.bind(function(path, ev, res) {
			var reply = res.json();

			if (L.isObject(reply) && reply.failure)
				alert(_('Upload request failed: %s').format(reply.message));

			return this.handleSelect(path, null, ev);
		}, this, path, ev));
	},

	handleDelete: function(path, fileStat, ev) {
		var parent = path.replace(/\/[^\/]+$/, '') || '/',
		    name = path.replace(/^.+\//, ''),
		    msg;

		ev.preventDefault();

		if (fileStat.type == 'directory')
			msg = _('Do you really want to recursively delete the directory "%s" ?').format(name);
		else
			msg = _('Do you really want to delete "%s" ?').format(name);

		if (confirm(msg)) {
			var button = this.node.firstElementChild,
			    hidden = this.node.lastElementChild;

			if (path == hidden.value) {
				L.dom.content(button, _('Select file…'));
				hidden.value = '';
			}

			return fs.remove(path).then(L.bind(function(parent, ev) {
				return this.handleSelect(parent, null, ev);
			}, this, parent, ev)).catch(function(err) {
				alert(_('Delete request failed: %s').format(err.message));
			});
		}
	},

	renderUpload: function(path, list) {
		if (!this.options.enable_upload)
			return E([]);

		return E([
			E('a', {
				'href': '#',
				'class': 'btn cbi-button-positive',
				'click': function(ev) {
					var uploadForm = ev.target.nextElementSibling,
					    fileInput = uploadForm.querySelector('input[type="file"]');

					ev.target.style.display = 'none';
					uploadForm.style.display = '';
					fileInput.click();
				}
			}, _('Upload file…')),
			E('div', { 'class': 'upload', 'style': 'display:none' }, [
				E('input', {
					'type': 'file',
					'style': 'display:none',
					'change': function(ev) {
						var nameinput = ev.target.parentNode.querySelector('input[type="text"]'),
						    uploadbtn = ev.target.parentNode.querySelector('button.cbi-button-save');

						nameinput.value = ev.target.value.replace(/^.+[\/\\]/, '');
						uploadbtn.disabled = false;
					}
				}),
				E('button', {
					'class': 'btn',
					'click': function(ev) {
						ev.preventDefault();
						ev.target.previousElementSibling.click();
					}
				}, [ _('Browse…') ]),
				E('div', {}, E('input', { 'type': 'text', 'placeholder': _('Filename') })),
				E('button', {
					'class': 'btn cbi-button-save',
					'click': L.ui.createHandlerFn(this, 'handleUpload', path, list),
					'disabled': true
				}, [ _('Upload file') ])
			])
		]);
	},

	renderListing: function(container, path, list) {
		var breadcrumb = E('p'),
		    rows = E('ul');

		list.sort(function(a, b) {
			var isDirA = (a.type == 'directory'),
			    isDirB = (b.type == 'directory');

			if (isDirA != isDirB)
				return isDirA < isDirB;

			return a.name > b.name;
		});

		for (var i = 0; i < list.length; i++) {
			if (!this.options.show_hidden && list[i].name.charAt(0) == '.')
				continue;

			var entrypath = this.canonicalizePath(path + '/' + list[i].name),
			    selected = (entrypath == this.node.lastElementChild.value),
			    mtime = new Date(list[i].mtime * 1000);

			rows.appendChild(E('li', [
				E('div', { 'class': 'name' }, [
					this.iconForType(list[i].type),
					' ',
					E('a', {
						'href': '#',
						'style': selected ? 'font-weight:bold' : null,
						'click': L.ui.createHandlerFn(this, 'handleSelect',
							entrypath, list[i].type != 'directory' ? list[i] : null)
					}, '%h'.format(list[i].name))
				]),
				E('div', { 'class': 'mtime hide-xs' }, [
					' %04d-%02d-%02d %02d:%02d:%02d '.format(
						mtime.getFullYear(),
						mtime.getMonth() + 1,
						mtime.getDate(),
						mtime.getHours(),
						mtime.getMinutes(),
						mtime.getSeconds())
				]),
				E('div', [
					selected ? E('button', {
						'class': 'btn',
						'click': L.ui.createHandlerFn(this, 'handleReset')
					}, [ _('Deselect') ]) : '',
					this.options.enable_remove ? E('button', {
						'class': 'btn cbi-button-negative',
						'click': L.ui.createHandlerFn(this, 'handleDelete', entrypath, list[i])
					}, [ _('Delete') ]) : ''
				])
			]));
		}

		if (!rows.firstElementChild)
			rows.appendChild(E('em', _('No entries in this directory')));

		var dirs = this.splitPath(path),
		    cur = '';

		for (var i = 0; i < dirs.length; i++) {
			cur = cur ? cur + '/' + dirs[i] : dirs[i];
			L.dom.append(breadcrumb, [
				i ? ' » ' : '',
				E('a', {
					'href': '#',
					'click': L.ui.createHandlerFn(this, 'handleSelect', cur || '/', null)
				}, dirs[i] != '' ? '%h'.format(dirs[i]) : E('em', '(root)')),
			]);
		}

		L.dom.content(container, [
			breadcrumb,
			rows,
			E('div', { 'class': 'right' }, [
				this.renderUpload(path, list),
				E('a', {
					'href': '#',
					'class': 'btn',
					'click': L.ui.createHandlerFn(this, 'handleCancel')
				}, _('Cancel'))
			]),
		]);
	},

	handleCancel: function(ev) {
		var button = this.node.firstElementChild,
		    browser = button.nextElementSibling;

		browser.classList.remove('open');
		button.style.display = '';

		this.node.dispatchEvent(new CustomEvent('cbi-fileupload-cancel', {}));

		ev.preventDefault();
	},

	handleReset: function(ev) {
		var button = this.node.firstElementChild,
		    hidden = this.node.lastElementChild;

		hidden.value = '';
		L.dom.content(button, _('Select file…'));

		this.handleCancel(ev);
	},

	handleSelect: function(path, fileStat, ev) {
		var browser = L.dom.parent(ev.target, '.cbi-filebrowser'),
		    ul = browser.querySelector('ul');

		if (fileStat == null) {
			L.dom.content(ul, E('em', { 'class': 'spinning' }, _('Loading directory contents…')));
			L.resolveDefault(fs.list(path), []).then(L.bind(this.renderListing, this, browser, path));
		}
		else {
			var button = this.node.firstElementChild,
			    hidden = this.node.lastElementChild;

			path = this.canonicalizePath(path);

			L.dom.content(button, [
				this.iconForType(fileStat.type),
				' %s (%1000mB)'.format(this.truncatePath(path), fileStat.size)
			]);

			browser.classList.remove('open');
			button.style.display = '';
			hidden.value = path;

			this.stat = Object.assign({ path: path }, fileStat);
			this.node.dispatchEvent(new CustomEvent('cbi-fileupload-select', { detail: this.stat }));
		}
	},

	handleFileBrowser: function(ev) {
		var button = ev.target,
		    browser = button.nextElementSibling,
		    path = this.stat ? this.stat.path.replace(/\/[^\/]+$/, '') : (this.options.initial_directory || this.options.root_directory);

		if (path.indexOf(this.options.root_directory) != 0)
			path = this.options.root_directory;

		ev.preventDefault();

		return L.resolveDefault(fs.list(path), []).then(L.bind(function(button, browser, path, list) {
			document.querySelectorAll('.cbi-filebrowser.open').forEach(function(browserEl) {
				L.dom.findClassInstance(browserEl).handleCancel(ev);
			});

			button.style.display = 'none';
			browser.classList.add('open');

			return this.renderListing(browser, path, list);
		}, this, button, browser, path));
	},

	getValue: function() {
		return this.node.lastElementChild.value;
	},

	setValue: function(value) {
		this.node.lastElementChild.value = value;
	}
});


return L.Class.extend({
	__init__: function() {
		modalDiv = document.body.appendChild(
			L.dom.create('div', { id: 'modal_overlay' },
				L.dom.create('div', { class: 'modal', role: 'dialog', 'aria-modal': true })));

		tooltipDiv = document.body.appendChild(
			L.dom.create('div', { class: 'cbi-tooltip' }));

		/* setup old aliases */
		L.showModal = this.showModal;
		L.hideModal = this.hideModal;
		L.showTooltip = this.showTooltip;
		L.hideTooltip = this.hideTooltip;
		L.itemlist = this.itemlist;

		document.addEventListener('mouseover', this.showTooltip.bind(this), true);
		document.addEventListener('mouseout', this.hideTooltip.bind(this), true);
		document.addEventListener('focus', this.showTooltip.bind(this), true);
		document.addEventListener('blur', this.hideTooltip.bind(this), true);

		document.addEventListener('luci-loaded', this.tabs.init.bind(this.tabs));
		document.addEventListener('luci-loaded', this.changes.init.bind(this.changes));
		document.addEventListener('uci-loaded', this.changes.init.bind(this.changes));
	},

	/* Modal dialog */
	showModal: function(title, children /* , ... */) {
		var dlg = modalDiv.firstElementChild;

		dlg.setAttribute('class', 'modal');

		for (var i = 2; i < arguments.length; i++)
			dlg.classList.add(arguments[i]);

		L.dom.content(dlg, L.dom.create('h4', {}, title));
		L.dom.append(dlg, children);

		document.body.classList.add('modal-overlay-active');

		return dlg;
	},

	hideModal: function() {
		document.body.classList.remove('modal-overlay-active');
	},

	/* Tooltip */
	showTooltip: function(ev) {
		var target = findParent(ev.target, '[data-tooltip]');

		if (!target)
			return;

		if (tooltipTimeout !== null) {
			window.clearTimeout(tooltipTimeout);
			tooltipTimeout = null;
		}

		var rect = target.getBoundingClientRect(),
		    x = rect.left              + window.pageXOffset,
		    y = rect.top + rect.height + window.pageYOffset;

		tooltipDiv.className = 'cbi-tooltip';
		tooltipDiv.innerHTML = '▲ ';
		tooltipDiv.firstChild.data += target.getAttribute('data-tooltip');

		if (target.hasAttribute('data-tooltip-style'))
			tooltipDiv.classList.add(target.getAttribute('data-tooltip-style'));

		if ((y + tooltipDiv.offsetHeight) > (window.innerHeight + window.pageYOffset)) {
			y -= (tooltipDiv.offsetHeight + target.offsetHeight);
			tooltipDiv.firstChild.data = '▼ ' + tooltipDiv.firstChild.data.substr(2);
		}

		tooltipDiv.style.top = y + 'px';
		tooltipDiv.style.left = x + 'px';
		tooltipDiv.style.opacity = 1;

		tooltipDiv.dispatchEvent(new CustomEvent('tooltip-open', {
			bubbles: true,
			detail: { target: target }
		}));
	},

	hideTooltip: function(ev) {
		if (ev.target === tooltipDiv || ev.relatedTarget === tooltipDiv ||
		    tooltipDiv.contains(ev.target) || tooltipDiv.contains(ev.relatedTarget))
			return;

		if (tooltipTimeout !== null) {
			window.clearTimeout(tooltipTimeout);
			tooltipTimeout = null;
		}

		tooltipDiv.style.opacity = 0;
		tooltipTimeout = window.setTimeout(function() { tooltipDiv.removeAttribute('style'); }, 250);

		tooltipDiv.dispatchEvent(new CustomEvent('tooltip-close', { bubbles: true }));
	},

	addNotification: function(title, children /*, ... */) {
		var mc = document.querySelector('#maincontent') || document.body;
		var msg = E('div', {
			'class': 'alert-message fade-in',
			'style': 'display:flex',
			'transitionend': function(ev) {
				var node = ev.currentTarget;
				if (node.parentNode && node.classList.contains('fade-out'))
					node.parentNode.removeChild(node);
			}
		}, [
			E('div', { 'style': 'flex:10' }),
			E('div', { 'style': 'flex:1 1 auto; display:flex' }, [
				E('button', {
					'class': 'btn',
					'style': 'margin-left:auto; margin-top:auto',
					'click': function(ev) {
						L.dom.parent(ev.target, '.alert-message').classList.add('fade-out');
					},

				}, [ _('Dismiss') ])
			])
		]);

		if (title != null)
			L.dom.append(msg.firstElementChild, E('h4', {}, title));

		L.dom.append(msg.firstElementChild, children);

		for (var i = 2; i < arguments.length; i++)
			msg.classList.add(arguments[i]);

		mc.insertBefore(msg, mc.firstElementChild);

		return msg;
	},

	/* Widget helper */
	itemlist: function(node, items, separators) {
		var children = [];

		if (!Array.isArray(separators))
			separators = [ separators || E('br') ];

		for (var i = 0; i < items.length; i += 2) {
			if (items[i+1] !== null && items[i+1] !== undefined) {
				var sep = separators[(i/2) % separators.length],
				    cld = [];

				children.push(E('span', { class: 'nowrap' }, [
					items[i] ? E('strong', items[i] + ': ') : '',
					items[i+1]
				]));

				if ((i+2) < items.length)
					children.push(L.dom.elem(sep) ? sep.cloneNode(true) : sep);
			}
		}

		L.dom.content(node, children);

		return node;
	},

	/* Tabs */
	tabs: L.Class.singleton({
		init: function() {
			var groups = [], prevGroup = null, currGroup = null;

			document.querySelectorAll('[data-tab]').forEach(function(tab) {
				var parent = tab.parentNode;

				if (L.dom.matches(tab, 'li') && L.dom.matches(parent, 'ul.cbi-tabmenu'))
					return;

				if (!parent.hasAttribute('data-tab-group'))
					parent.setAttribute('data-tab-group', groups.length);

				currGroup = +parent.getAttribute('data-tab-group');

				if (currGroup !== prevGroup) {
					prevGroup = currGroup;

					if (!groups[currGroup])
						groups[currGroup] = [];
				}

				groups[currGroup].push(tab);
			});

			for (var i = 0; i < groups.length; i++)
				this.initTabGroup(groups[i]);

			document.addEventListener('dependency-update', this.updateTabs.bind(this));

			this.updateTabs();
		},

		initTabGroup: function(panes) {
			if (typeof(panes) != 'object' || !('length' in panes) || panes.length === 0)
				return;

			var menu = E('ul', { 'class': 'cbi-tabmenu' }),
			    group = panes[0].parentNode,
			    groupId = +group.getAttribute('data-tab-group'),
			    selected = null;

			if (group.getAttribute('data-initialized') === 'true')
				return;

			for (var i = 0, pane; pane = panes[i]; i++) {
				var name = pane.getAttribute('data-tab'),
				    title = pane.getAttribute('data-tab-title'),
				    active = pane.getAttribute('data-tab-active') === 'true';

				menu.appendChild(E('li', {
					'style': this.isEmptyPane(pane) ? 'display:none' : null,
					'class': active ? 'cbi-tab' : 'cbi-tab-disabled',
					'data-tab': name
				}, E('a', {
					'href': '#',
					'click': this.switchTab.bind(this)
				}, title)));

				if (active)
					selected = i;
			}

			group.parentNode.insertBefore(menu, group);
			group.setAttribute('data-initialized', true);

			if (selected === null) {
				selected = this.getActiveTabId(panes[0]);

				if (selected < 0 || selected >= panes.length || this.isEmptyPane(panes[selected])) {
					for (var i = 0; i < panes.length; i++) {
						if (!this.isEmptyPane(panes[i])) {
							selected = i;
							break;
						}
					}
				}

				menu.childNodes[selected].classList.add('cbi-tab');
				menu.childNodes[selected].classList.remove('cbi-tab-disabled');
				panes[selected].setAttribute('data-tab-active', 'true');

				this.setActiveTabId(panes[selected], selected);
			}

			this.updateTabs(group);
		},

		isEmptyPane: function(pane) {
			return L.dom.isEmpty(pane, function(n) { return n.classList.contains('cbi-tab-descr') });
		},

		getPathForPane: function(pane) {
			var path = [], node = null;

			for (node = pane ? pane.parentNode : null;
			     node != null && node.hasAttribute != null;
			     node = node.parentNode)
			{
				if (node.hasAttribute('data-tab'))
					path.unshift(node.getAttribute('data-tab'));
				else if (node.hasAttribute('data-section-id'))
					path.unshift(node.getAttribute('data-section-id'));
			}

			return path.join('/');
		},

		getActiveTabState: function() {
			var page = document.body.getAttribute('data-page');

			try {
				var val = JSON.parse(window.sessionStorage.getItem('tab'));
				if (val.page === page && L.isObject(val.paths))
					return val;
			}
			catch(e) {}

			window.sessionStorage.removeItem('tab');
			return { page: page, paths: {} };
		},

		getActiveTabId: function(pane) {
			var path = this.getPathForPane(pane);
			return +this.getActiveTabState().paths[path] || 0;
		},

		setActiveTabId: function(pane, tabIndex) {
			var path = this.getPathForPane(pane);

			try {
				var state = this.getActiveTabState();
				    state.paths[path] = tabIndex;

			    window.sessionStorage.setItem('tab', JSON.stringify(state));
			}
			catch (e) { return false; }

			return true;
		},

		updateTabs: function(ev, root) {
			(root || document).querySelectorAll('[data-tab-title]').forEach(L.bind(function(pane) {
				var menu = pane.parentNode.previousElementSibling,
				    tab = menu ? menu.querySelector('[data-tab="%s"]'.format(pane.getAttribute('data-tab'))) : null,
				    n_errors = pane.querySelectorAll('.cbi-input-invalid').length;

				if (!menu || !tab)
					return;

				if (this.isEmptyPane(pane)) {
					tab.style.display = 'none';
					tab.classList.remove('flash');
				}
				else if (tab.style.display === 'none') {
					tab.style.display = '';
					requestAnimationFrame(function() { tab.classList.add('flash') });
				}

				if (n_errors) {
					tab.setAttribute('data-errors', n_errors);
					tab.setAttribute('data-tooltip', _('%d invalid field(s)').format(n_errors));
					tab.setAttribute('data-tooltip-style', 'error');
				}
				else {
					tab.removeAttribute('data-errors');
					tab.removeAttribute('data-tooltip');
				}
			}, this));
		},

		switchTab: function(ev) {
			var tab = ev.target.parentNode,
			    name = tab.getAttribute('data-tab'),
			    menu = tab.parentNode,
			    group = menu.nextElementSibling,
			    groupId = +group.getAttribute('data-tab-group'),
			    index = 0;

			ev.preventDefault();

			if (!tab.classList.contains('cbi-tab-disabled'))
				return;

			menu.querySelectorAll('[data-tab]').forEach(function(tab) {
				tab.classList.remove('cbi-tab');
				tab.classList.remove('cbi-tab-disabled');
				tab.classList.add(
					tab.getAttribute('data-tab') === name ? 'cbi-tab' : 'cbi-tab-disabled');
			});

			group.childNodes.forEach(function(pane) {
				if (L.dom.matches(pane, '[data-tab]')) {
					if (pane.getAttribute('data-tab') === name) {
						pane.setAttribute('data-tab-active', 'true');
						L.ui.tabs.setActiveTabId(pane, index);
					}
					else {
						pane.setAttribute('data-tab-active', 'false');
					}

					index++;
				}
			});
		}
	}),

	/* File uploading */
	uploadFile: function(path, progressStatusNode) {
		return new Promise(function(resolveFn, rejectFn) {
			L.ui.showModal(_('Uploading file…'), [
				E('p', _('Please select the file to upload.')),
				E('div', { 'style': 'display:flex' }, [
					E('div', { 'class': 'left', 'style': 'flex:1' }, [
						E('input', {
							type: 'file',
							style: 'display:none',
							change: function(ev) {
								var modal = L.dom.parent(ev.target, '.modal'),
								    body = modal.querySelector('p'),
								    upload = modal.querySelector('.cbi-button-action.important'),
								    file = ev.currentTarget.files[0];

								if (file == null)
									return;

								L.dom.content(body, [
									E('ul', {}, [
										E('li', {}, [ '%s: %s'.format(_('Name'), file.name.replace(/^.*[\\\/]/, '')) ]),
										E('li', {}, [ '%s: %1024mB'.format(_('Size'), file.size) ])
									])
								]);

								upload.disabled = false;
								upload.focus();
							}
						}),
						E('button', {
							'class': 'btn',
							'click': function(ev) {
								ev.target.previousElementSibling.click();
							}
						}, [ _('Browse…') ])
					]),
					E('div', { 'class': 'right', 'style': 'flex:1' }, [
						E('button', {
							'class': 'btn',
							'click': function() {
								L.ui.hideModal();
								rejectFn(new Error('Upload has been cancelled'));
							}
						}, [ _('Cancel') ]),
						' ',
						E('button', {
							'class': 'btn cbi-button-action important',
							'disabled': true,
							'click': function(ev) {
								var input = L.dom.parent(ev.target, '.modal').querySelector('input[type="file"]');

								if (!input.files[0])
									return;

								var progress = E('div', { 'class': 'cbi-progressbar', 'title': '0%' }, E('div', { 'style': 'width:0' }));

								L.ui.showModal(_('Uploading file…'), [ progress ]);

								var data = new FormData();

								data.append('sessionid', rpc.getSessionID());
								data.append('filename', path);
								data.append('filedata', input.files[0]);

								var filename = input.files[0].name;

								L.Request.post('/cgi-bin/cgi-upload', data, {
									timeout: 0,
									progress: function(pev) {
										var percent = (pev.loaded / pev.total) * 100;

										if (progressStatusNode)
											progressStatusNode.data = '%.2f%%'.format(percent);

										progress.setAttribute('title', '%.2f%%'.format(percent));
										progress.firstElementChild.style.width = '%.2f%%'.format(percent);
									}
								}).then(function(res) {
									var reply = res.json();

									L.ui.hideModal();

									if (L.isObject(reply) && reply.failure) {
										L.ui.addNotification(null, E('p', _('Upload request failed: %s').format(reply.message)));
										rejectFn(new Error(reply.failure));
									}
									else {
										reply.name = filename;
										resolveFn(reply);
									}
								}, function(err) {
									L.ui.hideModal();
									rejectFn(err);
								});
							}
						}, [ _('Upload') ])
					])
				])
			]);
		});
	},

	/* Reconnect handling */
	pingDevice: function(proto, ipaddr) {
		var target = '%s://%s%s?%s'.format(proto || 'http', ipaddr || window.location.host, L.resource('icons/loading.gif'), Math.random());

		return new Promise(function(resolveFn, rejectFn) {
			var img = new Image();

			img.onload = resolveFn;
			img.onerror = rejectFn;

			window.setTimeout(rejectFn, 1000);

			img.src = target;
		});
	},

	awaitReconnect: function(/* ... */) {
		var ipaddrs = arguments.length ? arguments : [ window.location.host ];

		window.setTimeout(L.bind(function() {
			L.Poll.add(L.bind(function() {
				var tasks = [], reachable = false;

				for (var i = 0; i < 2; i++)
					for (var j = 0; j < ipaddrs.length; j++)
						tasks.push(this.pingDevice(i ? 'https' : 'http', ipaddrs[j])
							.then(function(ev) { reachable = ev.target.src.replace(/^(https?:\/\/[^\/]+).*$/, '$1/') }, function() {}));

				return Promise.all(tasks).then(function() {
					if (reachable) {
						L.Poll.stop();
						window.location = reachable;
					}
				});
			}, this));
		}, this), 5000);
	},

	/* UCI Changes */
	changes: L.Class.singleton({
		init: function() {
			if (!L.env.sessionid)
				return;

			return uci.changes().then(L.bind(this.renderChangeIndicator, this));
		},

		setIndicator: function(n) {
			var i = document.querySelector('.uci_change_indicator');
			if (i == null) {
				var poll = document.getElementById('xhr_poll_status');
				i = poll.parentNode.insertBefore(E('a', {
					'href': '#',
					'class': 'uci_change_indicator label notice',
					'click': L.bind(this.displayChanges, this)
				}), poll);
			}

			if (n > 0) {
				L.dom.content(i, [ _('Unsaved Changes'), ': ', n ]);
				i.classList.add('flash');
				i.style.display = '';
			}
			else {
				i.classList.remove('flash');
				i.style.display = 'none';
			}
		},

		renderChangeIndicator: function(changes) {
			var n_changes = 0;

			for (var config in changes)
				if (changes.hasOwnProperty(config))
					n_changes += changes[config].length;

			this.changes = changes;
			this.setIndicator(n_changes);
		},

		changeTemplates: {
			'add-3':      '<ins>uci add %0 <strong>%3</strong> # =%2</ins>',
			'set-3':      '<ins>uci set %0.<strong>%2</strong>=%3</ins>',
			'set-4':      '<var><ins>uci set %0.%2.%3=<strong>%4</strong></ins></var>',
			'remove-2':   '<del>uci del %0.<strong>%2</strong></del>',
			'remove-3':   '<var><del>uci del %0.%2.<strong>%3</strong></del></var>',
			'order-3':    '<var>uci reorder %0.%2=<strong>%3</strong></var>',
			'list-add-4': '<var><ins>uci add_list %0.%2.%3=<strong>%4</strong></ins></var>',
			'list-del-4': '<var><del>uci del_list %0.%2.%3=<strong>%4</strong></del></var>',
			'rename-3':   '<var>uci rename %0.%2=<strong>%3</strong></var>',
			'rename-4':   '<var>uci rename %0.%2.%3=<strong>%4</strong></var>'
		},

		displayChanges: function() {
			var list = E('div', { 'class': 'uci-change-list' }),
			    dlg = L.ui.showModal(_('Configuration') + ' / ' + _('Changes'), [
				E('div', { 'class': 'cbi-section' }, [
					E('strong', _('Legend:')),
					E('div', { 'class': 'uci-change-legend' }, [
						E('div', { 'class': 'uci-change-legend-label' }, [
							E('ins', '&#160;'), ' ', _('Section added') ]),
						E('div', { 'class': 'uci-change-legend-label' }, [
							E('del', '&#160;'), ' ', _('Section removed') ]),
						E('div', { 'class': 'uci-change-legend-label' }, [
							E('var', {}, E('ins', '&#160;')), ' ', _('Option changed') ]),
						E('div', { 'class': 'uci-change-legend-label' }, [
							E('var', {}, E('del', '&#160;')), ' ', _('Option removed') ])]),
					E('br'), list,
					E('div', { 'class': 'right' }, [
						E('button', {
							'class': 'btn',
							'click': L.ui.hideModal
						}, [ _('Dismiss') ]), ' ',
						E('button', {
							'class': 'cbi-button cbi-button-positive important',
							'click': L.bind(this.apply, this, true)
						}, [ _('Save & Apply') ]), ' ',
						E('button', {
							'class': 'cbi-button cbi-button-reset',
							'click': L.bind(this.revert, this)
						}, [ _('Revert') ])])])
			]);

			for (var config in this.changes) {
				if (!this.changes.hasOwnProperty(config))
					continue;

				list.appendChild(E('h5', '# /etc/config/%s'.format(config)));

				for (var i = 0, added = null; i < this.changes[config].length; i++) {
					var chg = this.changes[config][i],
					    tpl = this.changeTemplates['%s-%d'.format(chg[0], chg.length)];

					list.appendChild(E(tpl.replace(/%([01234])/g, function(m0, m1) {
						switch (+m1) {
						case 0:
							return config;

						case 2:
							if (added != null && chg[1] == added[0])
								return '@' + added[1] + '[-1]';
							else
								return chg[1];

						case 4:
							return "'%h'".format(chg[3].replace(/'/g, "'\"'\"'"));

						default:
							return chg[m1-1];
						}
					})));

					if (chg[0] == 'add')
						added = [ chg[1], chg[2] ];
				}
			}

			list.appendChild(E('br'));
			dlg.classList.add('uci-dialog');
		},

		displayStatus: function(type, content) {
			if (type) {
				var message = L.ui.showModal('', '');

				message.classList.add('alert-message');
				DOMTokenList.prototype.add.apply(message.classList, type.split(/\s+/));

				if (content)
					L.dom.content(message, content);

				if (!this.was_polling) {
					this.was_polling = L.Request.poll.active();
					L.Request.poll.stop();
				}
			}
			else {
				L.ui.hideModal();

				if (this.was_polling)
					L.Request.poll.start();
			}
		},

		rollback: function(checked) {
			if (checked) {
				this.displayStatus('warning spinning',
					E('p', _('Failed to confirm apply within %ds, waiting for rollback…')
						.format(L.env.apply_rollback)));

				var call = function(r, data, duration) {
					if (r.status === 204) {
						L.ui.changes.displayStatus('warning', [
							E('h4', _('Configuration has been rolled back!')),
							E('p', _('The device could not be reached within %d seconds after applying the pending changes, which caused the configuration to be rolled back for safety reasons. If you believe that the configuration changes are correct nonetheless, perform an unchecked configuration apply. Alternatively, you can dismiss this warning and edit changes before attempting to apply again, or revert all pending changes to keep the currently working configuration state.').format(L.env.apply_rollback)),
							E('div', { 'class': 'right' }, [
								E('button', {
									'class': 'btn',
									'click': L.bind(L.ui.changes.displayStatus, L.ui.changes, false)
								}, [ _('Dismiss') ]), ' ',
								E('button', {
									'class': 'btn cbi-button-action important',
									'click': L.bind(L.ui.changes.revert, L.ui.changes)
								}, [ _('Revert changes') ]), ' ',
								E('button', {
									'class': 'btn cbi-button-negative important',
									'click': L.bind(L.ui.changes.apply, L.ui.changes, false)
								}, [ _('Apply unchecked') ])
							])
						]);

						return;
					}

					var delay = isNaN(duration) ? 0 : Math.max(1000 - duration, 0);
					window.setTimeout(function() {
						L.Request.request(L.url('admin/uci/confirm'), {
							method: 'post',
							timeout: L.env.apply_timeout * 1000,
							query: { sid: L.env.sessionid, token: L.env.token }
						}).then(call);
					}, delay);
				};

				call({ status: 0 });
			}
			else {
				this.displayStatus('warning', [
					E('h4', _('Device unreachable!')),
					E('p', _('Could not regain access to the device after applying the configuration changes. You might need to reconnect if you modified network related settings such as the IP address or wireless security credentials.'))
				]);
			}
		},

		confirm: function(checked, deadline, override_token) {
			var tt;
			var ts = Date.now();

			this.displayStatus('notice');

			if (override_token)
				this.confirm_auth = { token: override_token };

			var call = function(r, data, duration) {
				if (Date.now() >= deadline) {
					window.clearTimeout(tt);
					L.ui.changes.rollback(checked);
					return;
				}
				else if (r && (r.status === 200 || r.status === 204)) {
					document.dispatchEvent(new CustomEvent('uci-applied'));

					L.ui.changes.setIndicator(0);
					L.ui.changes.displayStatus('notice',
						E('p', _('Configuration has been applied.')));

					window.clearTimeout(tt);
					window.setTimeout(function() {
						//L.ui.changes.displayStatus(false);
						window.location = window.location.href.split('#')[0];
					}, L.env.apply_display * 1000);

					return;
				}

				var delay = isNaN(duration) ? 0 : Math.max(1000 - duration, 0);
				window.setTimeout(function() {
					L.Request.request(L.url('admin/uci/confirm'), {
						method: 'post',
						timeout: L.env.apply_timeout * 1000,
						query: L.ui.changes.confirm_auth
					}).then(call, call);
				}, delay);
			};

			var tick = function() {
				var now = Date.now();

				L.ui.changes.displayStatus('notice spinning',
					E('p', _('Waiting for configuration to get applied… %ds')
						.format(Math.max(Math.floor((deadline - Date.now()) / 1000), 0))));

				if (now >= deadline)
					return;

				tt = window.setTimeout(tick, 1000 - (now - ts));
				ts = now;
			};

			tick();

			/* wait a few seconds for the settings to become effective */
			window.setTimeout(call, Math.max(L.env.apply_holdoff * 1000 - ((ts + L.env.apply_rollback * 1000) - deadline), 1));
		},

		apply: function(checked) {
			this.displayStatus('notice spinning',
				E('p', _('Starting configuration apply…')));

			L.Request.request(L.url('admin/uci', checked ? 'apply_rollback' : 'apply_unchecked'), {
				method: 'post',
				query: { sid: L.env.sessionid, token: L.env.token }
			}).then(function(r) {
				if (r.status === (checked ? 200 : 204)) {
					var tok = null; try { tok = r.json(); } catch(e) {}
					if (checked && tok !== null && typeof(tok) === 'object' && typeof(tok.token) === 'string')
						L.ui.changes.confirm_auth = tok;

					L.ui.changes.confirm(checked, Date.now() + L.env.apply_rollback * 1000);
				}
				else if (checked && r.status === 204) {
					L.ui.changes.displayStatus('notice',
						E('p', _('There are no changes to apply')));

					window.setTimeout(function() {
						L.ui.changes.displayStatus(false);
					}, L.env.apply_display * 1000);
				}
				else {
					L.ui.changes.displayStatus('warning',
						E('p', _('Apply request failed with status <code>%h</code>')
							.format(r.responseText || r.statusText || r.status)));

					window.setTimeout(function() {
						L.ui.changes.displayStatus(false);
					}, L.env.apply_display * 1000);
				}
			});
		},

		revert: function() {
			this.displayStatus('notice spinning',
				E('p', _('Reverting configuration…')));

			L.Request.request(L.url('admin/uci/revert'), {
				method: 'post',
				query: { sid: L.env.sessionid, token: L.env.token }
			}).then(function(r) {
				if (r.status === 200) {
					document.dispatchEvent(new CustomEvent('uci-reverted'));

					L.ui.changes.setIndicator(0);
					L.ui.changes.displayStatus('notice',
						E('p', _('Changes have been reverted.')));

					window.setTimeout(function() {
						//L.ui.changes.displayStatus(false);
						window.location = window.location.href.split('#')[0];
					}, L.env.apply_display * 1000);
				}
				else {
					L.ui.changes.displayStatus('warning',
						E('p', _('Revert request failed with status <code>%h</code>')
							.format(r.statusText || r.status)));

					window.setTimeout(function() {
						L.ui.changes.displayStatus(false);
					}, L.env.apply_display * 1000);
				}
			});
		}
	}),

	addValidator: function(field, type, optional, vfunc /*, ... */) {
		if (type == null)
			return;

		var events = this.varargs(arguments, 3);
		if (events.length == 0)
			events.push('blur', 'keyup');

		try {
			var cbiValidator = L.validation.create(field, type, optional, vfunc),
			    validatorFn = cbiValidator.validate.bind(cbiValidator);

			for (var i = 0; i < events.length; i++)
				field.addEventListener(events[i], validatorFn);

			validatorFn();

			return validatorFn;
		}
		catch (e) { }
	},

	createHandlerFn: function(ctx, fn /*, ... */) {
		if (typeof(fn) == 'string')
			fn = ctx[fn];

		if (typeof(fn) != 'function')
			return null;

		var arg_offset = arguments.length - 2;

		return Function.prototype.bind.apply(function() {
			var t = arguments[arg_offset].target;

			t.classList.add('spinning');
			t.disabled = true;

			if (t.blur)
				t.blur();

			Promise.resolve(fn.apply(ctx, arguments)).finally(function() {
				t.classList.remove('spinning');
				t.disabled = false;
			});
		}, this.varargs(arguments, 2, ctx));
	},

	AbstractElement: UIElement,

	/* Widgets */
	Textfield: UITextfield,
	Textarea: UITextarea,
	Checkbox: UICheckbox,
	Select: UISelect,
	Dropdown: UIDropdown,
	DynamicList: UIDynamicList,
	Combobox: UICombobox,
	ComboButton: UIComboButton,
	Hiddenfield: UIHiddenfield,
	FileUpload: UIFileUpload
});
