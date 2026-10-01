'use strict';
'require baseclass';
'require ui';

const ICONS = {
	status: '<path d="M3 12h4l3-8 4 16 3-8h4"/>',
	system: '<path d="M4 7h9M17 7h3M4 17h3M11 17h9"/><circle cx="15" cy="7" r="2"/><circle cx="9" cy="17" r="2"/>',
	services: '<rect x="4" y="4" width="6.5" height="6.5" rx="1.6"/><rect x="13.5" y="4" width="6.5" height="6.5" rx="1.6"/><rect x="4" y="13.5" width="6.5" height="6.5" rx="1.6"/><rect x="13.5" y="13.5" width="6.5" height="6.5" rx="1.6"/>',
	network: '<circle cx="12" cy="12" r="8.5"/><path d="M3.5 12h17M12 3.5c2.5 2.4 3.7 5.3 3.7 8.5s-1.2 6.1-3.7 8.5c-2.5-2.4-3.7-5.3-3.7-8.5s1.2-6.1 3.7-8.5z"/>',
	vpn: '<path d="M12 3.5 19 6v5.5c0 4.3-2.9 7.6-7 9-4.1-1.4-7-4.7-7-9V6z"/><path d="m9 12 2 2 4-4"/>',
	nas: '<rect x="3.5" y="13" width="17" height="7" rx="2"/><path d="M5.5 13 8 5h8l2.5 8M7.5 16.5h.01M11 16.5h.01"/>',
	statistics: '<path d="M5 20v-9M11 20V5M17 20v-6M3 20h18"/>',
	logout: '<path d="M15 4h3a2 2 0 0 1 2 2v12a2 2 0 0 1-2 2h-3"/><path d="m10 16-4-4 4-4M6 12h10"/>',
	folder: '<path d="M4 7a2 2 0 0 1 2-2h4l2 2h6a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2z"/>',
	chevron: '<path d="m9 6 6 6-6 6"/>',
	search: '<circle cx="11" cy="11" r="6.5"/><path d="m20 20-4.2-4.2"/>',
	auto: '<circle cx="12" cy="12" r="8.5"/><path d="M12 3.5a8.5 8.5 0 0 1 0 17z" fill="currentColor"/>',
	light: '<circle cx="12" cy="12" r="4"/><path d="M12 2.5v2M12 19.5v2M4.6 4.6l1.4 1.4M18 18l1.4 1.4M2.5 12h2M19.5 12h2M4.6 19.4 6 18M18 6l1.4-1.4"/>',
	dark: '<path d="M20 14.5A8 8 0 0 1 9.5 4a8 8 0 1 0 10.5 10.5z"/>',
	enter: '<path d="M20 5v7a3 3 0 0 1-3 3H5M9 11l-4 4 4 4"/>'
};

const STRINGS = {
	ru: {
		search: 'Поиск', placeholder: 'Найти страницу или настройку…', empty: 'Ничего не найдено',
		auto: 'Авто', light: 'Светлая', dark: 'Тёмная', theme: 'Оформление', logout: 'Выйти',
		go: 'открыть', move: 'выбор', close: 'закрыть'
	},
	en: {
		search: 'Search', placeholder: 'Jump to a page or setting…', empty: 'Nothing found',
		auto: 'Auto', light: 'Light', dark: 'Dark', theme: 'Appearance', logout: 'Log out',
		go: 'open', move: 'navigate', close: 'close'
	}
};

function icon(name) {
	const svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
	svg.setAttribute('viewBox', '0 0 24 24');
	svg.setAttribute('aria-hidden', 'true');
	svg.innerHTML = ICONS[name] || ICONS.folder;
	return svg;
}

function storeGet(key, fallback) {
	try { const v = localStorage.getItem(key); return v != null ? v : fallback; } catch (e) { return fallback; }
}

function storeSet(key, val) {
	try { localStorage.setItem(key, val); } catch (e) {}
}

return baseclass.extend({
	__init__() {
		const lang = (document.documentElement.getAttribute('lang') || 'en').toLowerCase();
		this.t = STRINGS[lang.indexOf('ru') === 0 ? 'ru' : 'en'];
		this.pages = [];

		this.bindShell();
		ui.menu.load().then((tree) => this.render(tree));
	},

	bindShell() {
		const body = document.body;
		const toggle = document.querySelector('#nb-nav-toggle');
		const backdrop = document.querySelector('#nb-backdrop');

		if (toggle)
			toggle.addEventListener('click', () => body.classList.toggle('nb-nav-open'));

		if (backdrop)
			backdrop.addEventListener('click', () => body.classList.remove('nb-nav-open'));

		document.addEventListener('keydown', (ev) => {
			const tag = (ev.target && ev.target.tagName) || '';
			const typing = /^(INPUT|TEXTAREA|SELECT)$/.test(tag) || (ev.target && ev.target.isContentEditable);

			if ((ev.key === 'k' || ev.key === 'K') && (ev.metaKey || ev.ctrlKey)) {
				ev.preventDefault();
				this.openPalette();
			}
			else if (ev.key === '/' && !typing && !document.body.classList.contains('modal-overlay-active')) {
				ev.preventDefault();
				this.openPalette();
			}
			else if (ev.key === 'Escape') {
				body.classList.remove('nb-nav-open');
			}
		});

		const isMac = /Mac|iPhone|iPad/.test(navigator.platform || navigator.userAgent);
		const btn = document.querySelector('#nb-search-open');
		const mini = document.querySelector('#nb-search-mini');

		if (btn) {
			btn.querySelector('.nb-search-label').textContent = this.t.search;
			btn.querySelector('kbd').textContent = isMac ? '⌘K' : 'Ctrl K';
			btn.addEventListener('click', () => this.openPalette());
			btn.hidden = false;
		}

		if (mini) {
			mini.addEventListener('click', () => this.openPalette());
			mini.hidden = false;
		}

		this.renderFoot();
	},

	renderFoot() {
		const foot = document.querySelector('#nb-sidebar-foot');
		if (!foot)
			return;

		const current = storeGet('nimbus-theme', 'auto');
		const group = E('div', { 'class': 'nb-theme', 'role': 'radiogroup', 'aria-label': this.t.theme });

		['auto', 'light', 'dark'].forEach((mode) => {
			const b = E('button', {
				'type': 'button',
				'class': 'nb-theme-opt' + (mode === current ? ' active' : ''),
				'role': 'radio',
				'aria-checked': mode === current ? 'true' : 'false',
				'title': this.t[mode],
				'data-mode': mode
			}, [ icon(mode), E('span', {}, [ this.t[mode] ]) ]);

			b.addEventListener('click', () => {
				window.nimbusSetTheme(mode);
				group.querySelectorAll('.nb-theme-opt').forEach((o) => {
					const on = (o.getAttribute('data-mode') === mode);
					o.classList.toggle('active', on);
					o.setAttribute('aria-checked', on ? 'true' : 'false');
				});
			});

			group.appendChild(b);
		});

		foot.appendChild(group);
	},

	render(tree) {
		this.renderModeMenu(tree);

		const path = L.env.dispatchpath || [];
		let node = tree;
		let url = '';

		if (path.length >= 3) {
			for (let i = 0; i < 3 && node; i++) {
				node = node.children[path[i]];
				url = url + (url ? '/' : '') + path[i];
			}

			if (node)
				this.renderTabMenu(node, url);
		}

		this.renderCrumbs(tree);
	},

	renderModeMenu(tree) {
		const ul = document.querySelector('#modemenu');
		const children = ui.menu.getChildren(tree);
		const req = L.env.requestpath || [];

		children.forEach((child, index) => {
			const isActive = req.length ? child.name === req[0] : index === 0;

			ul.appendChild(E('li', { 'class': isActive ? 'active' : '' }, [
				E('a', { 'href': L.url(child.name) }, [ _(child.title) ])
			]));

			if (isActive) {
				this.modeName = child.name;
				this.modeNode = child;
				this.renderMainMenu(child, child.name);
			}
		});

		if (ul.children.length > 1)
			ul.style.display = '';
	},

	renderMainMenu(tree, url) {
		const nav = document.querySelector('#topmenu');
		const foot = document.querySelector('#nb-sidebar-foot');
		const path = L.env.dispatchpath || [];
		const categories = ui.menu.getChildren(tree);
		let openState = {};

		try { openState = JSON.parse(storeGet('nimbus-nav-open', '{}')) || {}; } catch (e) {}

		categories.forEach((cat) => {
			const pages = ui.menu.getChildren(cat);
			const catActive = (path[1] === cat.name);

			if (cat.name === 'logout') {
				if (foot)
					foot.appendChild(E('a', { 'class': 'nb-logout', 'href': L.url(url, cat.name) }, [
						icon('logout'), E('span', {}, [ this.t.logout ])
					]));
				return;
			}

			if (!pages.length) {
				nav.appendChild(E('a', {
					'class': 'nb-nav-top' + (catActive ? ' active' : ''),
					'href': L.url(url, cat.name)
				}, [ icon(cat.name), E('span', { 'class': 'nb-nav-label' }, [ _(cat.title) ]) ]));

				this.pages.push({ title: _(cat.title), group: '', url: L.url(url, cat.name) });
				return;
			}

			const isOpen = catActive || openState[cat.name] === true;
			const sub = E('ul', { 'class': 'nb-nav-sub' });
			const group = E('div', {
				'class': 'nb-nav-group' + (catActive ? ' active' : '') + (isOpen ? ' open' : ''),
				'data-name': cat.name
			});

			const toggle = E('button', {
				'type': 'button',
				'class': 'nb-nav-toggle-group',
				'aria-expanded': isOpen ? 'true' : 'false'
			}, [
				icon(cat.name),
				E('span', { 'class': 'nb-nav-label' }, [ _(cat.title) ]),
				E('span', { 'class': 'nb-nav-chevron' }, [ icon('chevron') ])
			]);

			toggle.addEventListener('click', () => {
				const open = !group.classList.contains('open');
				group.classList.toggle('open', open);
				toggle.setAttribute('aria-expanded', open ? 'true' : 'false');
				openState[cat.name] = open;
				storeSet('nimbus-nav-open', JSON.stringify(openState));
			});

			pages.forEach((page) => {
				const active = catActive && path[2] === page.name;
				const href = L.url(url, cat.name, page.name);

				const attrs = { 'class': active ? 'active' : '', 'href': href };
				if (active)
					attrs['aria-current'] = 'page';

				sub.appendChild(E('li', {}, [ E('a', attrs, [ _(page.title) ]) ]));

				this.pages.push({ title: _(page.title), group: _(cat.title), url: href });

				ui.menu.getChildren(page).forEach((tab) => {
					this.pages.push({
						title: _(tab.title),
						group: _(cat.title) + ' › ' + _(page.title),
						url: L.url(url, cat.name, page.name, tab.name)
					});
				});
			});

			group.appendChild(toggle);
			group.appendChild(sub);
			nav.appendChild(group);
		});

		const active = nav.querySelector('.nb-nav-sub a.active');
		if (active && active.scrollIntoView)
			active.scrollIntoView({ block: 'nearest' });
	},

	renderTabMenu(tree, url, level) {
		const container = document.querySelector('#tabmenu');
		const ul = E('ul', { 'class': 'tabs' });
		const children = ui.menu.getChildren(tree);
		let activeNode = null;

		children.forEach((child) => {
			const isActive = (L.env.dispatchpath[3 + (level || 0)] == child.name);

			ul.appendChild(E('li', { 'class': 'tabmenu-item-%s %s'.format(child.name, isActive ? 'active' : '') }, [
				E('a', { 'href': L.url(url, child.name) }, [ _(child.title) ])
			]));

			if (isActive)
				activeNode = child;
		});

		if (ul.children.length == 0)
			return E([]);

		container.appendChild(ul);
		container.style.display = '';

		const act = ul.querySelector('li.active');
		if (act)
			requestAnimationFrame(() => { ul.scrollLeft = act.offsetLeft - 16; });

		if (activeNode)
			this.renderTabMenu(activeNode, url + '/' + activeNode.name, (level || 0) + 1);

		return ul;
	},

	renderCrumbs(tree) {
		const crumbs = document.querySelector('#nb-crumbs');
		const path = L.env.dispatchpath || [];
		const items = [];
		let node = tree;
		let url = '';

		for (let i = 0; i < path.length && node && node.children; i++) {
			node = node.children[path[i]];
			url = url + (url ? '/' : '') + path[i];

			if (i === 0 || !node || !node.title)
				continue;

			items.push({ title: _(node.title), url: L.url(url) });
		}

		items.forEach((item, i) => {
			if (i > 0)
				crumbs.appendChild(E('span', { 'class': 'nb-crumb-sep' }, [ icon('chevron') ]));

			const last = (i === items.length - 1);
			crumbs.appendChild(last
				? E('span', { 'class': 'nb-crumb current' }, [ item.title ])
				: E('a', { 'class': 'nb-crumb', 'href': item.url }, [ item.title ]));
		});
	},

	openPalette() {
		if (this.palette) {
			this.palette.classList.add('open');
			this.paletteInput.value = '';
			this.filterPalette('');
			this.paletteInput.focus();
			return;
		}

		const input = E('input', {
			'type': 'text',
			'class': 'nb-palette-input',
			'placeholder': this.t.placeholder,
			'autocomplete': 'off',
			'spellcheck': 'false',
			'aria-label': this.t.search
		});

		const list = E('ul', { 'class': 'nb-palette-list', 'role': 'listbox' });
		const box = E('div', { 'class': 'nb-palette-box', 'role': 'dialog', 'aria-modal': 'true' }, [
			E('div', { 'class': 'nb-palette-search' }, [ icon('search'), input, E('kbd', {}, [ 'Esc' ]) ]),
			list,
			E('div', { 'class': 'nb-palette-hint' }, [
				E('span', {}, [ E('kbd', {}, [ '↑' ]), E('kbd', {}, [ '↓' ]), ' ', this.t.move ]),
				E('span', {}, [ E('kbd', {}, [ '↵' ]), ' ', this.t.go ]),
				E('span', {}, [ E('kbd', {}, [ 'Esc' ]), ' ', this.t.close ])
			])
		]);

		const overlay = E('div', { 'class': 'nb-palette open' }, [ box ]);

		overlay.addEventListener('mousedown', (ev) => {
			if (ev.target === overlay)
				this.closePalette();
		});

		input.addEventListener('input', () => this.filterPalette(input.value));
		input.addEventListener('keydown', (ev) => {
			if (ev.key === 'ArrowDown' || ev.key === 'ArrowUp') {
				ev.preventDefault();
				this.movePalette(ev.key === 'ArrowDown' ? 1 : -1);
			}
			else if (ev.key === 'Enter') {
				ev.preventDefault();
				const sel = list.querySelector('li.active a');
				if (sel)
					window.location.href = sel.getAttribute('href');
			}
			else if (ev.key === 'Escape') {
				ev.preventDefault();
				ev.stopPropagation();
				this.closePalette();
			}
		});

		document.body.appendChild(overlay);

		this.palette = overlay;
		this.paletteInput = input;
		this.paletteList = list;

		this.filterPalette('');
		input.focus();
	},

	closePalette() {
		if (this.palette)
			this.palette.classList.remove('open');
	},

	filterPalette(query) {
		const list = this.paletteList;
		const q = String(query || '').trim().toLowerCase();
		const words = q.split(/\s+/).filter(Boolean);

		const scored = this.pages.map((p) => {
			const title = p.title.toLowerCase();
			const hay = (p.group + ' ' + p.title).toLowerCase();
			let score = 0;

			if (!words.length)
				return { p, score: 1 };

			for (const w of words) {
				if (hay.indexOf(w) < 0)
					return { p, score: 0 };
			}

			if (title.indexOf(q) === 0) score += 100;
			else if (title.indexOf(q) >= 0) score += 60;
			else score += 20;

			score -= p.group.length / 100;
			return { p, score };
		}).filter((s) => s.score > 0);

		if (words.length)
			scored.sort((a, b) => b.score - a.score);

		list.innerHTML = '';

		scored.slice(0, 60).forEach((s, i) => {
			const li = E('li', { 'class': i === 0 ? 'active' : '', 'role': 'option' }, [
				E('a', { 'href': s.p.url }, [
					E('span', { 'class': 'nb-palette-title' }, [ s.p.title ]),
					s.p.group ? E('span', { 'class': 'nb-palette-group' }, [ s.p.group ]) : ''
				])
			]);

			li.addEventListener('mousemove', () => {
				list.querySelectorAll('li.active').forEach((a) => a.classList.remove('active'));
				li.classList.add('active');
			});

			list.appendChild(li);
		});

		if (!scored.length)
			list.appendChild(E('li', { 'class': 'nb-palette-empty' }, [ this.t.empty ]));
	},

	movePalette(dir) {
		const items = Array.from(this.paletteList.querySelectorAll('li[role="option"]'));
		if (!items.length)
			return;

		let idx = items.findIndex((li) => li.classList.contains('active'));
		if (idx >= 0)
			items[idx].classList.remove('active');

		idx = (idx + dir + items.length) % items.length;
		items[idx].classList.add('active');
		items[idx].scrollIntoView({ block: 'nearest' });
	}
});
