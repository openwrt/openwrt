'use strict';
'require view';
'require dom';
'require poll';
'require fs';
'require network';

return view.extend({
	invokeIncludesLoad: function(includes) {
		var tasks = [], has_load = false;

		for (var i = 0; i < includes.length; i++) {
			if (typeof(includes[i].load) == 'function') {
				tasks.push(includes[i].load().catch(L.bind(function() {
					this.failed = true;
				}, includes[i])));
				has_load = true;
			}
			else {
				tasks.push(null);
			}
		}

		return has_load ? Promise.all(tasks) : Promise.resolve(null);
	},

	poll_status: function(includes, containers) {
		return network.flushCache().then(L.bind(
			this.invokeIncludesLoad, this, includes
		)).then(function(results) {
			for (var i = 0; i < includes.length; i++) {
				var content = null;

				if (includes[i].failed)
					continue;

				if (typeof(includes[i].render) == 'function')
					content = includes[i].render(results ? results[i] : null);
				else if (includes[i].content != null)
					content = includes[i].content;

				if (typeof(includes[i].oneshot) == 'function') {
					includes[i].oneshot(results ? results[i] : null);
					includes[i].oneshot = null;
				}

				if (content != null) {
					containers[i].parentNode.style.display = '';
					containers[i].parentNode.classList.add('fade-in');
					dom.content(containers[i], content);
				}
			}

			var ssi = document.querySelector('div.includes');
			if (ssi) {
				ssi.style.display = '';
				ssi.classList.add('fade-in');
			}
		});
	},

	load: function() {
		return L.resolveDefault(fs.list('/www' + L.resource('view/status/include')), []).then(function(entries) {
			return Promise.all(entries.filter(function(e) {
				return (e.type == 'file' && e.name.match(/\.js$/));
			}).map(function(e) {
				return 'view.status.include.' + e.name.replace(/\.js$/, '');
			}).sort().map(function(n) {
				return L.require(n);
			}));
		});
	},

	render: function(includes) {
		var rv = E([]), containers = [];

		for (var i = 0; i < includes.length; i++) {
			var title = null;

			if (includes[i].title != null)
				title = includes[i].title;
			else
				title = String(includes[i]).replace(/^\[ViewStatusInclude\d+_(.+)Class\]$/,
					function(m, n) { return n.replace(/(^|_)(.)/g,
						function(m, s, c) { return (s ? ' ' : '') + c.toUpperCase() })
					});

			includes[i].id = title;
			includes[i].hide = false;
			localStorage.removeItem(includes[i].id);

			var container = E('div');

			rv.appendChild(E('div', { 'class': 'cbi-section', 'style': 'display: none' }, [
				E('div', { 'class': 'cbi-title' }, [
					E('h3', {}, [ title || '-' ])
				]),
				container
			]));

			containers.push(container);
		}

		return this.poll_status(includes, containers).then(L.bind(function() {
			return poll.add(L.bind(this.poll_status, this, includes, containers));
		}, this)).then(function() {
			return rv;
		});
	},

	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
