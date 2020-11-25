var table_names = [ 'Filter', 'NAT', 'Mangle', 'Raw' ],
    current_mode = document.querySelector('.cbi-tab[data-mode="6"]') ? 6 : 4;

function create_table_section(table)
{
	var idiv = document.getElementById('iptables'),
	    tdiv = idiv.querySelector('[data-table="%s"]'.format(table)),
	    title = '%s: %s'.format(_('Table'), table);

	if (!tdiv) {
		tdiv = E('div', { 'data-table': table }, [
			E('h3', {}, title),
			E('div')
		]);

		if (idiv.firstElementChild.nodeName.toLowerCase() === 'p')
			idiv.removeChild(idiv.firstElementChild);

		var added = false, thisIdx = table_names.indexOf(table);

		idiv.querySelectorAll('[data-table]').forEach(function(child) {
			var childIdx = table_names.indexOf(child.getAttribute('data-table'));

			if (added === false && childIdx > thisIdx) {
				idiv.insertBefore(tdiv, child);
				added = true;
			}
		});

		if (added === false)
			idiv.appendChild(tdiv);
	}

	return tdiv.lastElementChild;
}

function create_chain_section(table, chain, policy, packets, bytes, references)
{
	var tdiv = create_table_section(table),
	    cdiv = tdiv.querySelector('[data-chain="%s"]'.format(chain)),
	    title;

	if (policy)
		title = '%s <em>%s</em> <span>(%s: <em>%s</em>, %d %s, %.2mB %s)</span>'
			.format(_('Chain'), chain, _('Policy'), policy, packets, _('Packets'), bytes, _('Traffic'));
	else
		title = '%s <em>%s</em> <span class="references">(%d %s)</span>'
			.format(_('Chain'), chain, references, _('References'));

	if (!cdiv) {
		cdiv = E('div', { 'data-chain': chain }, [
			E('h4', { 'id': 'rule_%s_%s'.format(table.toLowerCase(), chain) }, title),
			E('div', { 'class': 'table' }, [
				E('div', { 'class': 'tr table-titles' }, [
					E('div', { 'class': 'th center' }, _('Pkts.')),
					E('div', { 'class': 'th center' }, _('Traffic')),
					E('div', { 'class': 'th' }, _('Target')),
					E('div', { 'class': 'th' }, _('Prot.')),
					E('div', { 'class': 'th' }, _('In')),
					E('div', { 'class': 'th' }, _('Out')),
					E('div', { 'class': 'th' }, _('Source')),
					E('div', { 'class': 'th' }, _('Destination')),
					E('div', { 'class': 'th' }, _('Options')),
					E('div', { 'class': 'th' }, _('Comment'))
				])
			])
		]);

		tdiv.appendChild(cdiv);
	}
	else {
		cdiv.firstElementChild.innerHTML = title;
	}

	return cdiv.lastElementChild;
}

function update_chain_section(chaintable, rows)
{
	if (!chaintable)
		return;

	cbi_update_table(chaintable, rows, _('No rules in this chain.'));

	if (rows.length === 0 &&
	    document.querySelector('form > [data-hide-empty="true"]'))
		chaintable.parentNode.style.display = 'none';
	else
		chaintable.parentNode.style.display = '';

	chaintable.parentNode.setAttribute('data-empty', rows.length === 0);
}

function hide_empty(btn)
{
	var hide = (btn.getAttribute('data-hide-empty') === 'false');

	btn.setAttribute('data-hide-empty', hide);
	btn.value = hide ? _('Show empty chains') : _('Hide empty chains');
	btn.blur();

	document.querySelectorAll('[data-chain][data-empty="true"]')
		.forEach(function(chaintable) {
			chaintable.style.display = hide ? 'none' : '';
		});
}

function jump_target(ev)
{
	var link = ev.target,
	    table = findParent(link, '[data-table]').getAttribute('data-table'),
	    chain = link.textContent,
	    num = +link.getAttribute('data-num'),
	    elem = document.getElementById('rule_%s_%s'.format(table.toLowerCase(), chain));

	if (elem) {
		(document.documentElement || document.body.parentNode || document.body).scrollTop = elem.offsetTop - 40;
		elem.classList.remove('flash');
		void elem.offsetWidth;
		elem.classList.add('flash');

		if (num) {
			var rule = elem.nextElementSibling.childNodes[num];
			if (rule) {
				rule.classList.remove('flash');
				void rule.offsetWidth;
				rule.classList.add('flash');
			}
		}
	}
}

function parse_output(table, s)
{
	var current_chain = null;
	var current_rules = [];
	var seen_chains = {};
	var chain_refs = {};
	var re = /([^\n]*)\n/g;
	var m, m2;

	while ((m = re.exec(s)) != null) {
		if (m[1].match(/^Chain (.+) \(policy (\w+) (\d+) packets, (\d+) bytes\)$/)) {
			var chain = RegExp.$1,
			    policy = RegExp.$2,
			    packets = +RegExp.$3,
			    bytes = +RegExp.$4;

			update_chain_section(current_chain, current_rules);

			seen_chains[chain] = true;
			current_chain = create_chain_section(table, chain, policy, packets, bytes);
			current_rules = [];
		}
		else if (m[1].match(/^Chain (.+) \((\d+) references\)$/)) {
			var chain = RegExp.$1,
			    references = +RegExp.$2;

			update_chain_section(current_chain, current_rules);

			seen_chains[chain] = true;
			current_chain = create_chain_section(table, chain, null, null, null, references);
			current_rules = [];
		}
		else if (m[1].match(/^num /)) {
			continue;
		}
		else if ((m2 = m[1].match(/^(\d+) +(\d+) +(\d+) +(.*?) +(\S+) +(\S*) +(\S+) +(\S+) +([a-f0-9:.]+(?:\/[a-f0-9:.]+)?) +([a-f0-9:.]+(?:\/[a-f0-9:.]+)?) +(.+)$/)) !== null) {
			var num = +m2[1],
			    pkts = +m2[2],
			    bytes = +m2[3],
			    target = m2[4],
			    proto = m2[5],
			    indev = m2[7],
			    outdev = m2[8],
			    srcnet = m2[9],
			    dstnet = m2[10],
			    options = m2[11] || '-',
			    comment = '-';

			options = options.trim().replace(/(?:^| )\/\* (.+) \*\//,
				function(m1, m2) {
					comment = m2.replace(/^!fw3(: |$)/, '').trim() || '-';
					return '';
				}) || '-';

			current_rules.push([
				'%.2m'.format(pkts).nobr(),
				'%.2mB'.format(bytes).nobr(),
				target ? '<span class="target">%s</span>'.format(target) : '-',
				proto,
				(indev !== '*') ? '<span class="ifacebadge">%s</span>'.format(indev) : '*',
				(outdev !== '*') ? '<span class="ifacebadge">%s</span>'.format(outdev) : '*',
				srcnet,
				dstnet,
				options,
				[ comment ]
			]);

			if (target) {
				chain_refs[target] = chain_refs[target] || [];
				chain_refs[target].push([ current_chain, num ]);
			}
		}
	}

	update_chain_section(current_chain, current_rules);

	document.querySelectorAll('[data-table="%s"] [data-chain]'.format(table))
		.forEach(function(cdiv) {
			if (!seen_chains[cdiv.getAttribute('data-chain')]) {
				cdiv.parentNode.removeChild(cdiv);
				return;
			}

			cdiv.querySelectorAll('.target').forEach(function(tspan) {
				if (seen_chains[tspan.textContent]) {
					tspan.classList.add('jump');
					tspan.addEventListener('click', jump_target);
				}
			});

			cdiv.querySelectorAll('.references').forEach(function(rspan) {
				var refs = chain_refs[cdiv.getAttribute('data-chain')];
				if (refs && refs.length) {
					rspan.classList.add('cbi-tooltip-container');
					rspan.appendChild(E('small', { 'class': 'cbi-tooltip ifacebadge', 'style': 'top:1em; left:auto' }, [ E('ul') ]));

					refs.forEach(function(ref) {
						var chain = ref[0].parentNode.getAttribute('data-chain'),
						    num = ref[1];

						rspan.lastElementChild.lastElementChild.appendChild(E('li', {}, [
							_('Chain'), ' ',
							E('span', {
								'class': 'jump',
								'data-num': num,
								'onclick': 'jump_target(event)'
							}, chain),
							', %s #%d'.format(_('Rule'), num)
						]));
					});
				}
			});
		});
}

table_names.forEach(function(table) {
	L.poll(5, L.url('admin/status/iptables_dump', current_mode, table.toLowerCase()), null,
		function (xhr) {
			parse_output(table, xhr.responseText);
		});
});
