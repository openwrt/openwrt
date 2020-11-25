var packages = {
	available: { providers: {}, pkgs: {} },
	installed: { providers: {}, pkgs: {} }
};

var currentDisplayMode = 'available', currentDisplayRows = [];

function parseList(s, dest)
{
	var re = /([^\n]*)\n/g,
	    pkg = null, key = null, val = null, m;

	while ((m = re.exec(s)) !== null) {
		if (m[1].match(/^\s(.*)$/)) {
			if (pkg !== null && key !== null && val !== null)
				val += '\n' + RegExp.$1.trim();

			continue;
		}

		if (key !== null && val !== null) {
			switch (key) {
			case 'package':
				pkg = { name: val };
				break;

			case 'depends':
			case 'provides':
				var list = val.split(/\s*,\s*/);
				if (list.length !== 1 || list[0].length > 0)
					pkg[key] = list;
				break;

			case 'installed-time':
				pkg.installtime = new Date(+val * 1000);
				break;

			case 'installed-size':
				pkg.installsize = +val;
				break;

			case 'status':
				var stat = val.split(/\s+/),
				    mode = stat[1],
				    installed = stat[2];

				switch (mode) {
				case 'user':
				case 'hold':
					pkg[mode] = true;
					break;
				}

				switch (installed) {
				case 'installed':
					pkg.installed = true;
					break;
				}
				break;

			case 'essential':
				if (val === 'yes')
					pkg.essential = true;
				break;

			case 'size':
				pkg.size = +val;
				break;

			case 'architecture':
			case 'auto-installed':
			case 'filename':
			case 'sha256sum':
			case 'section':
				break;

			default:
				pkg[key] = val;
				break;
			}

			key = val = null;
		}

		if (m[1].trim().match(/^([\w-]+)\s*:(.+)$/)) {
			key = RegExp.$1.toLowerCase();
			val = RegExp.$2.trim();
		}
		else {
			dest.pkgs[pkg.name] = pkg;

			var provides = dest.providers[pkg.name] ? [] : [ pkg.name ];

			if (pkg.provides)
				provides.push.apply(provides, pkg.provides);

			provides.forEach(function(p) {
				dest.providers[p] = dest.providers[p] || [];
				dest.providers[p].push(pkg);
			});
		}
	}
}

function display(pattern)
{
	var src = packages[currentDisplayMode === 'updates' ? 'installed' : currentDisplayMode],
	    table = document.querySelector('#packages'),
	    pager = document.querySelector('#pager');

	currentDisplayRows.length = 0;

	if (typeof(pattern) === 'string' && pattern.length > 0)
		pattern = new RegExp(pattern.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), 'ig');

	for (var name in src.pkgs) {
		var pkg = src.pkgs[name],
		    desc = pkg.description || '',
		    altsize = null;

		if (!pkg.size && packages.available.pkgs[name])
			altsize = packages.available.pkgs[name].size;

		if (!desc && packages.available.pkgs[name])
			desc = packages.available.pkgs[name].description || '';

		desc = desc.split(/\n/);
		desc = desc[0].trim() + (desc.length > 1 ? '…' : '');

		if ((pattern instanceof RegExp) &&
		    !name.match(pattern) && !desc.match(pattern))
			continue;

		var btn, ver;

		if (currentDisplayMode === 'updates') {
			var avail = packages.available.pkgs[name],
			    inst  = packages.installed.pkgs[name];

			if (!inst || !inst.installed)
				continue;

			if (!avail || compareVersion(avail.version, pkg.version) <= 0)
				continue;

			ver = '%s » %s'.format(
				truncateVersion(pkg.version || '-'),
				truncateVersion(avail.version || '-'));

			btn = E('div', {
				'class': 'btn cbi-button-positive',
				'data-package': name,
				'click': handleInstall
			}, _('Upgrade…'));
		}
		else if (currentDisplayMode === 'installed') {
			if (!pkg.installed)
				continue;

			ver = truncateVersion(pkg.version || '-');
			btn = E('div', {
				'class': 'btn cbi-button-negative',
				'data-package': name,
				'click': handleRemove
			}, _('Remove…'));
		}
		else {
			var inst = packages.installed.pkgs[name];

			ver = truncateVersion(pkg.version || '-');

			if (!inst || !inst.installed)
				btn = E('div', {
					'class': 'btn cbi-button-action',
					'data-package': name,
					'click': handleInstall
				}, _('Install…'));
			else if (inst.installed && inst.version != pkg.version)
				btn = E('div', {
					'class': 'btn cbi-button-positive',
					'data-package': name,
					'click': handleInstall
				}, _('Upgrade…'));
			else
				btn = E('div', {
					'class': 'btn cbi-button-neutral',
					'disabled': 'disabled'
				}, _('Installed'));
		}

		name = '%h'.format(name);
		desc = '%h'.format(desc || '-');

		if (pattern) {
			name = name.replace(pattern, '<ins>$&</ins>');
			desc = desc.replace(pattern, '<ins>$&</ins>');
		}

		currentDisplayRows.push([
			name,
			ver,
			pkg.size ? '%.1024mB'.format(pkg.size)
			         : (altsize ? '~%.1024mB'.format(altsize) : '-'),
			desc,
			btn
		]);
	}

	currentDisplayRows.sort(function(a, b) {
		if (a[0] < b[0])
			return -1;
		else if (a[0] > b[0])
			return 1;
		else
			return 0;
	});

	pager.parentNode.style.display = '';
	pager.setAttribute('data-offset', 100);
	handlePage({ target: pager.querySelector('.prev') });
}

function handlePage(ev)
{
	var filter = document.querySelector('input[name="filter"]'),
	    pager = ev.target.parentNode,
	    offset = +pager.getAttribute('data-offset'),
	    next = ev.target.classList.contains('next');

	if ((next && (offset + 100) >= currentDisplayRows.length) ||
	    (!next && (offset < 100)))
	    return;

	offset += next ? 100 : -100;
	pager.setAttribute('data-offset', offset);
	pager.querySelector('.text').firstChild.data = currentDisplayRows.length
		? _('Displaying %d-%d of %d').format(1 + offset, Math.min(offset + 100, currentDisplayRows.length), currentDisplayRows.length)
		: _('No packages');

	if (offset < 100)
		pager.querySelector('.prev').setAttribute('disabled', 'disabled');
	else
		pager.querySelector('.prev').removeAttribute('disabled');

	if ((offset + 100) >= currentDisplayRows.length)
		pager.querySelector('.next').setAttribute('disabled', 'disabled');
	else
		pager.querySelector('.next').removeAttribute('disabled');

	var placeholder = _('No information available');

	if (filter.value)
		placeholder = [
			E('span', {}, _('No packages matching "<strong>%h</strong>".').format(filter.value)), ' (',
			E('a', { href: '#', onclick: 'handleReset(event)' }, _('Reset')), ')'
		];

	cbi_update_table('#packages', currentDisplayRows.slice(offset, offset + 100),
		placeholder);
}

function handleMode(ev)
{
	var tab = findParent(ev.target, 'li');
	if (tab.getAttribute('data-mode') === currentDisplayMode)
		return;

	tab.parentNode.querySelectorAll('li').forEach(function(li) {
		li.classList.remove('cbi-tab');
		li.classList.add('cbi-tab-disabled');
	});

	tab.classList.remove('cbi-tab-disabled');
	tab.classList.add('cbi-tab');

	currentDisplayMode = tab.getAttribute('data-mode');

	display(document.querySelector('input[name="filter"]').value);

	ev.target.blur();
	ev.preventDefault();
}

function orderOf(c)
{
	if (c === '~')
		return -1;
	else if (c === '' || c >= '0' && c <= '9')
		return 0;
	else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return c.charCodeAt(0);
	else
		return c.charCodeAt(0) + 256;
}

function compareVersion(val, ref)
{
	var vi = 0, ri = 0,
	    isdigit = { 0:1, 1:1, 2:1, 3:1, 4:1, 5:1, 6:1, 7:1, 8:1, 9:1 };

	val = val || '';
	ref = ref || '';

	if (val === ref)
		return 0;

	while (vi < val.length || ri < ref.length) {
		var first_diff = 0;

		while ((vi < val.length && !isdigit[val.charAt(vi)]) ||
		       (ri < ref.length && !isdigit[ref.charAt(ri)])) {
			var vc = orderOf(val.charAt(vi)), rc = orderOf(ref.charAt(ri));
			if (vc !== rc)
				return vc - rc;

			vi++; ri++;
		}

		while (val.charAt(vi) === '0')
			vi++;

		while (ref.charAt(ri) === '0')
			ri++;

		while (isdigit[val.charAt(vi)] && isdigit[ref.charAt(ri)]) {
			first_diff = first_diff || (val.charCodeAt(vi) - ref.charCodeAt(ri));
			vi++; ri++;
		}

		if (isdigit[val.charAt(vi)])
			return 1;
		else if (isdigit[ref.charAt(ri)])
			return -1;
		else if (first_diff)
			return first_diff;
	}

	return 0;
}

function versionSatisfied(ver, ref, vop)
{
	var r = compareVersion(ver, ref);

	switch (vop) {
	case '<':
	case '<=':
		return r <= 0;

	case '>':
	case '>=':
		return r >= 0;

	case '<<':
		return r < 0;

	case '>>':
		return r > 0;

	case '=':
		return r == 0;
	}

	return false;
}

function pkgStatus(pkg, vop, ver, info)
{
	info.errors = info.errors || [];
	info.install = info.install || [];

	if (pkg.installed) {
		if (vop && !versionSatisfied(pkg.version, ver, vop)) {
			var repl = null;

			(packages.available.providers[pkg.name] || []).forEach(function(p) {
				if (!repl && versionSatisfied(p.version, ver, vop))
					repl = p;
			});

			if (repl) {
				info.install.push(repl);
				return E('span', {
					'class': 'label',
					'data-tooltip': _('Requires update to %h %h')
						.format(repl.name, repl.version)
				}, _('Needs upgrade'));
			}

			info.errors.push(_('The installed version of package <em>%h</em> is not compatible, require %s while %s is installed.').format(pkg.name, truncateVersion(ver, vop), truncateVersion(pkg.version)));

			return E('span', {
				'class': 'label warning',
				'data-tooltip': _('Require version %h %h,\ninstalled %h')
					.format(vop, ver, pkg.version)
			}, _('Version incompatible'));
		}

		return E('span', { 'class': 'label notice' }, _('Installed'));
	}
	else if (!pkg.missing) {
		if (!vop || versionSatisfied(pkg.version, ver, vop)) {
			info.install.push(pkg);
			return E('span', { 'class': 'label' }, _('Not installed'));
		}

		info.errors.push(_('The repository version of package <em>%h</em> is not compatible, require %s but only %s is available.')
				.format(pkg.name, truncateVersion(ver, vop), truncateVersion(pkg.version)));

		return E('span', {
			'class': 'label warning',
			'data-tooltip': _('Require version %h %h,\ninstalled %h')
				.format(vop, ver, pkg.version)
		}, _('Version incompatible'));
	}
	else {
		info.errors.push(_('Required dependency package <em>%h</em> is not available in any repository.').format(pkg.name));

		return E('span', { 'class': 'label warning' }, _('Not available'));
	}
}

function renderDependencyItem(dep, info)
{
	var li = E('li'),
	    vop = dep.version ? dep.version[0] : null,
	    ver = dep.version ? dep.version[1] : null,
	    depends = [];

	for (var i = 0; dep.pkgs && i < dep.pkgs.length; i++) {
		var pkg = packages.installed.pkgs[dep.pkgs[i]] ||
		          packages.available.pkgs[dep.pkgs[i]] ||
		          { name: dep.name };

		if (i > 0)
			li.appendChild(document.createTextNode(' | '));

		var text = pkg.name;

		if (pkg.installsize)
			text += ' (%.1024mB)'.format(pkg.installsize);
		else if (pkg.size)
			text += ' (~%.1024mB)'.format(pkg.size);

		li.appendChild(E('span', { 'data-tooltip': pkg.description },
			[ text, ' ', pkgStatus(pkg, vop, ver, info) ]));

		(pkg.depends || []).forEach(function(d) {
			if (depends.indexOf(d) === -1)
				depends.push(d);
		});
	}

	if (!li.firstChild)
		li.appendChild(E('span', {},
			[ dep.name, ' ',
			  pkgStatus({ name: dep.name, missing: true }, vop, ver, info) ]));

	var subdeps = renderDependencies(depends, info);
	if (subdeps)
		li.appendChild(subdeps);

	return li;
}

function renderDependencies(depends, info)
{
	var deps = depends || [],
	    items = [];

	info.seen = info.seen || [];

	for (var i = 0; i < deps.length; i++) {
		if (deps[i] === 'libc')
			continue;

		if (deps[i].match(/^(.+)\s+\((<=|<|>|>=|=|<<|>>)(.+)\)$/)) {
			dep = RegExp.$1.trim();
			vop = RegExp.$2.trim();
			ver = RegExp.$3.trim();
		}
		else {
			dep = deps[i].trim();
			vop = ver = null;
		}

		if (info.seen[dep])
			continue;

		var pkgs = [];

		(packages.installed.providers[dep] || []).forEach(function(p) {
			if (pkgs.indexOf(p.name) === -1) pkgs.push(p.name);
		});

		(packages.available.providers[dep] || []).forEach(function(p) {
			if (pkgs.indexOf(p.name) === -1) pkgs.push(p.name);
		});

		info.seen[dep] = {
			name:    dep,
			pkgs:    pkgs,
			version: [vop, ver]
		};

		items.push(renderDependencyItem(info.seen[dep], info));
	}

	if (items.length)
		return E('ul', { 'class': 'deps' }, items);

	return null;
}

function truncateVersion(v, op)
{
	v = v.replace(/\b(([a-f0-9]{8})[a-f0-9]{24,32})\b/,
		'<span data-tooltip="$1">$2…</span>');

	if (!op || op === '=')
		return v;

	return '%h %h'.format(op, v);
}

function handleReset(ev)
{
	var filter = document.querySelector('input[name="filter"]');

	filter.value = '';
	display();
}

function handleInstall(ev)
{
	var name = ev.target.getAttribute('data-package'),
	    pkg = packages.available.pkgs[name],
	    depcache = {},
	    size;

	if (pkg.installsize)
		size = _('~%.1024mB installed').format(pkg.installsize);
	else if (pkg.size)
		size = _('~%.1024mB compressed').format(pkg.size);
	else
		size = _('unknown');

	var deps = renderDependencies(pkg.depends, depcache),
	    tree = null, errs = null, inst = null, desc = null;

	if (depcache.errors && depcache.errors.length) {
		errs = E('ul', { 'class': 'errors' });
		depcache.errors.forEach(function(err) {
			errs.appendChild(E('li', {}, err));
		});
	}

	var totalsize = pkg.installsize || pkg.size || 0,
	    totalpkgs = 1;

	if (depcache.install && depcache.install.length)
		depcache.install.forEach(function(ipkg) {
			totalsize += ipkg.installsize || ipkg.size || 0;
			totalpkgs++;
		});

	inst = E('p', {},
		_('Require approx. %.1024mB size for %d package(s) to install.')
			.format(totalsize, totalpkgs));

	if (deps) {
		tree = E('li', '<strong>%s:</strong>'.format(_('Dependencies')));
		tree.appendChild(deps);
	}

	if (pkg.description) {
		desc = E('div', {}, [
			E('h5', {}, _('Description')),
			E('p', {}, pkg.description)
		]);
	}

	L.showModal(_('Details for package <em>%h</em>').format(pkg.name), [
		E('ul', {}, [
			E('li', '<strong>%s:</strong> %h'.format(_('Version'), pkg.version)),
			E('li', '<strong>%s:</strong> %h'.format(_('Size'), size)),
			tree || '',
		]),
		desc || '',
		errs || inst || '',
		E('div', { 'class': 'right' }, [
			E('label', { 'class': 'cbi-checkbox', 'style': 'float:left; padding-top:.5em' }, [
				E('input', { 'type': 'checkbox', 'name': 'overwrite' }), ' ',
				_('Overwrite files from other package(s)')
			]),
			E('div', {
				'class': 'btn',
				'click': L.hideModal
			}, _('Cancel')),
			' ',
			E('div', {
				'data-command': 'install',
				'data-package': name,
				'class': 'btn cbi-button-action',
				'click': handleOpkg
			}, _('Install'))
		])
	]);
}

function handleManualInstall(ev)
{
	var name_or_url = document.querySelector('input[name="install"]').value,
	    install = E('div', {
			'class': 'btn cbi-button-action',
			'data-command': 'install',
			'data-package': name_or_url,
			'click': function(ev) {
				document.querySelector('input[name="install"]').value = '';
				handleOpkg(ev);
			}
		}, _('Install')), warning;

	if (!name_or_url.length) {
		return;
	}
	else if (name_or_url.indexOf('/') !== -1) {
		warning = E('p', {}, _('Installing packages from untrusted sources is a potential security risk! Really attempt to install <em>%h</em>?').format(name_or_url));
	}
	else if (!packages.available.providers[name_or_url]) {
		warning = E('p', {}, _('The package <em>%h</em> is not available in any configured repository.').format(name_or_url));
		install = '';
	}
	else {
		warning = E('p', {}, _('Really attempt to install <em>%h</em>?').format(name_or_url));
	}

	L.showModal(_('Manually install package'), [
		warning,
		E('div', { 'class': 'right' }, [
			E('div', {
				'click': L.hideModal,
				'class': 'btn cbi-button-neutral'
			}, _('Cancel')),
			' ', install
		])
	]);
}

function handleConfig(ev)
{
	L.showModal(_('OPKG Configuration'), [
		E('p', { 'class': 'spinning' }, _('Loading configuration data…'))
	]);

	L.get('admin/system/opkg/config', null, function(xhr, conf) {
		var body = [
			E('p', {}, _('Below is a listing of the various configuration files used by <em>opkg</em>. Use <em>opkg.conf</em> for global settings and <em>customfeeds.conf</em> for custom repository entries. The configuration in the other files may be changed but is usually not preserved by <em>sysupgrade</em>.'))
		];

		Object.keys(conf).sort().forEach(function(file) {
			body.push(E('h5', {}, '%h'.format(file)));
			body.push(E('textarea', {
				'name': file,
				'rows': Math.max(Math.min(L.toArray(conf[file].match(/\n/g)).length, 10), 3)
			}, '%h'.format(conf[file])));
		});

		body.push(E('div', { 'class': 'right' }, [
			E('div', {
				'class': 'btn cbi-button-neutral',
				'click': L.hideModal
			}, _('Cancel')),
			' ',
			E('div', {
				'class': 'btn cbi-button-positive',
				'click': function(ev) {
					var data = {};
					findParent(ev.target, '.modal').querySelectorAll('textarea[name]')
						.forEach(function(textarea) {
							data[textarea.getAttribute('name')] = textarea.value
						});

					L.showModal(_('OPKG Configuration'), [
						E('p', { 'class': 'spinning' }, _('Saving configuration data…'))
					]);

					L.post('admin/system/opkg/config', { data: JSON.stringify(data) }, L.hideModal);
				}
			}, _('Save')),
		]));

		L.showModal(_('OPKG Configuration'), body);
	});
}

function handleRemove(ev)
{
	var name = ev.target.getAttribute('data-package'),
	    pkg = packages.installed.pkgs[name],
	    avail = packages.available.pkgs[name] || {},
	    size, desc;

	if (avail.installsize)
		size = _('~%.1024mB installed').format(avail.installsize);
	else if (avail.size)
		size = _('~%.1024mB compressed').format(avail.size);
	else
		size = _('unknown');

	if (avail.description) {
		desc = E('div', {}, [
			E('h5', {}, _('Description')),
			E('p', {}, avail.description)
		]);
	}

	L.showModal(_('Remove package <em>%h</em>').format(pkg.name), [
		E('ul', {}, [
			E('li', '<strong>%s:</strong> %h'.format(_('Version'), pkg.version)),
			E('li', '<strong>%s:</strong> %h'.format(_('Size'), size))
		]),
		desc || '',
		E('div', { 'style': 'display:flex; justify-content:space-between; flex-wrap:wrap' }, [
			E('label', {}, [
				E('input', { type: 'checkbox', checked: 'checked', name: 'autoremove' }),
				_('Automatically remove unused dependencies')
			]),
			E('div', { 'style': 'flex-grow:1', 'class': 'right' }, [
				E('div', {
					'class': 'btn',
					'click': L.hideModal
				}, _('Cancel')),
				' ',
				E('div', {
					'data-command': 'remove',
					'data-package': name,
					'class': 'btn cbi-button-negative',
					'click': handleOpkg
				}, _('Remove'))
			])
		])
	]);
}

function handleOpkg(ev)
{
	return new Promise(function(resolveFn, rejectFn) {
		var cmd = ev.target.getAttribute('data-command'),
		    pkg = ev.target.getAttribute('data-package'),
		    rem = document.querySelector('input[name="autoremove"]'),
		    owr = document.querySelector('input[name="overwrite"]'),
		    url = 'admin/system/opkg/exec/' + encodeURIComponent(cmd);

		var dlg = L.showModal(_('Executing package manager'), [
			E('p', { 'class': 'spinning' },
				_('Waiting for the <em>opkg %h</em> command to complete…').format(cmd))
		]);

		L.post(url, { package: pkg, autoremove: rem ? rem.checked : false, overwrite: owr ? owr.checked : false }, function(xhr, res) {
			dlg.removeChild(dlg.lastChild);

			if (res.stdout)
				dlg.appendChild(E('pre', [ res.stdout ]));

			if (res.stderr) {
				dlg.appendChild(E('h5', _('Errors')));
				dlg.appendChild(E('pre', { 'class': 'errors' }, [ res.stderr ]));
			}

			if (res.code !== 0)
				dlg.appendChild(E('p', _('The <em>opkg %h</em> command failed with code <code>%d</code>.').format(cmd, (res.code & 0xff) || -1)));

			dlg.appendChild(E('div', { 'class': 'right' },
				E('div', {
					'class': 'btn',
					'click': L.bind(function(res) {
						L.hideModal();
						updateLists();

						if (res.code !== 0)
							rejectFn(new Error(res.stderr || 'opkg error %d'.format(res.code)));
						else
							resolveFn(res);
					}, this, res)
				}, _('Dismiss'))));
		});
	});
}

function handleUpload(ev)
{
	var path = '/tmp/upload.ipk';
	return L.ui.uploadFile(path).then(L.bind(function(btn, res) {
		L.showModal(_('Manually install package'), [
			E('p', {}, _('Installing packages from untrusted sources is a potential security risk! Really attempt to install <em>%h</em>?').format(res.name)),
			E('ul', {}, [
				res.size ? E('li', {}, '%s: %1024.2mB'.format(_('Size'), res.size)) : '',
				res.checksum ? E('li', {}, '%s: %s'.format(_('MD5'), res.checksum)) : '',
				res.sha256sum ? E('li', {}, '%s: %s'.format(_('SHA256'), res.sha256sum)) : ''
			]),
			E('div', { 'class': 'right' }, [
				E('div', {
					'click': function(ev) {
						L.hideModal();
						L.fs.remove(path);
					},
					'class': 'btn cbi-button-neutral'
				}, _('Cancel')), ' ',
				E('div', {
					'class': 'btn cbi-button-action',
					'data-command': 'install',
					'data-package': path,
					'click': function(ev) {
						handleOpkg(ev).finally(function() {
							L.fs.remove(path)
						});
					}
				}, _('Install'))
			])
		]);
	}, this, ev.target));
}

function updateLists()
{
	cbi_update_table('#packages', [],
		E('div', { 'class': 'spinning' }, _('Loading package information…')));

	packages.available = { providers: {}, pkgs: {} };
	packages.installed = { providers: {}, pkgs: {} };

	L.get('admin/system/opkg/statvfs', null, function(xhr, stat) {
		var pg = document.querySelector('.cbi-progressbar'),
		    total = stat.blocks || 0,
		    free = stat.bfree || 0;

		pg.firstElementChild.style.width = Math.floor(total ? ((100 / total) * free) : 100) + '%';
		pg.setAttribute('title', '%s (%.1024mB)'.format(pg.firstElementChild.style.width, free * (stat.frsize || 0)));

		L.get('admin/system/opkg/list/available', null, function(xhr) {
			parseList(xhr.responseText, packages.available);
			L.get('admin/system/opkg/list/installed', null, function(xhr) {
				parseList(xhr.responseText, packages.installed);
				display(document.querySelector('input[name="filter"]').value);
			});
		});
	});
}

window.requestAnimationFrame(function() {
	var filter = document.querySelector('input[name="filter"]'),
	    keyTimeout = null;

	filter.value = filter.getAttribute('value');
	filter.addEventListener('keyup',
		function(ev) {
			if (keyTimeout !== null)
				window.clearTimeout(keyTimeout);

			keyTimeout = window.setTimeout(function() {
				display(ev.target.value);
			}, 250);
		});

	document.querySelector('#pager > .prev').addEventListener('click', handlePage);
	document.querySelector('#pager > .next').addEventListener('click', handlePage);
	document.querySelector('.cbi-tabmenu.mode').addEventListener('click', handleMode);

	updateLists();
});
