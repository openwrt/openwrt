'use strict';
'require view';
'require rpc';
'require ui';
'require poll';

var callStatus = rpc.declare({ object: 'be7000-storage', method: 'status' });
var callList = rpc.declare({ object: 'be7000-storage', method: 'list' });
var callUse = rpc.declare({ object: 'be7000-storage', method: 'use', params: ['device'] });
var callRevert = rpc.declare({ object: 'be7000-storage', method: 'revert' });

function mb(kb) {
	if (!kb)
		return '—';
	return (kb / 1024).toFixed(1) + ' MB';
}

function gb(sizeMb) {
	if (!sizeMb)
		return '—';
	if (sizeMb >= 1024)
		return (sizeMb / 1024).toFixed(1) + ' GB';
	return sizeMb + ' MB';
}

return view.extend({
	load: function() {
		return Promise.all([
			callStatus().catch(function() { return {}; }),
			callList().catch(function() { return {}; })
		]);
	},

	reload: function() {
		var self = this;
		return this.load().then(function(data) {
			var fresh = self.render(data);
			var node = document.getElementById('hf-storage');
			if (node && fresh)
				node.parentNode.replaceChild(fresh, node);
		});
	},

	handleUse: function(dev, label) {
		var self = this;
		ui.showModal(_('将 /overlay 迁移到 %s').format(dev), [
			E('p', {}, _('磁盘将被格式化，其所有内容都会丢失。')),
			E('p', {}, _('当前设置和已安装的软件包将迁移到该磁盘。更改将在重启后生效。')),
			E('p', { 'class': 'alert-message warning' }, label || dev),
			E('div', { 'class': 'right' }, [
				E('button', { 'class': 'btn', 'click': ui.hideModal }, _('取消')),
				' ',
				E('button', {
					'class': 'btn cbi-button-negative',
					'click': function() {
						ui.showModal(_('正在迁移'), [E('p', { 'class': 'spinning' }, _('正在格式化和复制，这可能需要一分钟'))]);
						callUse(dev).then(function(res) {
							ui.hideModal();
							if (res && res.ok) {
								ui.addNotification(null, E('p', {}, _('完成。请重启路由器，以便 /overlay 迁移。')), 'info');
								self.reload();
							}
							else {
								ui.addNotification(null, E('pre', {}, (res && res.output) || _('失败')), 'error');
							}
						}).catch(function(err) {
							ui.hideModal();
							ui.addNotification(null, E('p', {}, String(err)), 'error');
						});
					}
				}, _('格式化并迁移'))
			])
		]);
	},

	handleRevert: function() {
		var self = this;
		ui.showModal(_('将 /overlay 恢复到内部闪存'), [
			E('p', {}, _('磁盘上的数据将保留，但不再使用。更改将在重启后生效。')),
			E('div', { 'class': 'right' }, [
				E('button', { 'class': 'btn', 'click': ui.hideModal }, _('取消')),
				' ',
				E('button', {
					'class': 'btn cbi-button-action',
					'click': function() {
						callRevert().then(function(res) {
							ui.hideModal();
							if (res && res.ok) {
								ui.addNotification(null, E('p', {}, _('完成。请重启路由器。')), 'info');
								self.reload();
							}
							else {
								ui.addNotification(null, E('pre', {}, (res && res.output) || _('失败')), 'error');
							}
						});
					}
				}, _('恢复'))
			])
		]);
	},

	render: function(data) {
		var self = this;
		var status = (data[0] && data[0].data) || {};
		var disks = (data[1] && data[1].data) || [];

		var rows = [];
		if (!disks.length) {
			rows.push(E('p', {}, _('未找到存储设备。请将磁盘插入 USB 端口并刷新页面。')));
		}
		else {
			var table = E('table', { 'class': 'table' }, [
				E('tr', { 'class': 'tr table-titles' }, [
					E('th', { 'class': 'th' }, _('设备')),
					E('th', { 'class': 'th' }, _('大小')),
					E('th', { 'class': 'th' }, _('型号')),
					E('th', { 'class': 'th' }, '')
				])
			]);
			disks.forEach(function(d) {
				var action;
				if (d.mounted) {
					action = E('span', {}, _('正在使用'));
				}
				else {
					action = E('button', {
						'class': 'btn cbi-button-action',
						'click': function() { self.handleUse(d.device, d.model); }
					}, _('迁移到此处'));
				}
				table.appendChild(E('tr', { 'class': 'tr' }, [
					E('td', { 'class': 'td' }, d.device),
					E('td', { 'class': 'td' }, gb(d.size_mb)),
					E('td', { 'class': 'td' }, d.model || '—'),
					E('td', { 'class': 'td' }, action)
				]));
			});
			rows.push(table);
		}

		var onExternal = status.extroot_enabled === '1';

		return E('div', { 'id': 'hf-storage' }, [
			E('h2', {}, _('存储设备')),
			E('p', {}, _('路由器在重启之间记住的所有内容都位于 /overlay 中：设置、已安装的软件包、列表。内置空间不多，因此可以将其迁移到 USB 磁盘。')),

			E('h3', {}, _('当前')),
			E('table', { 'class': 'table' }, [
				E('tr', { 'class': 'tr' }, [
					E('td', { 'class': 'td', 'width': '33%' }, _('分区')),
					E('td', { 'class': 'td' }, status.device || '—')
				]),
				E('tr', { 'class': 'tr' }, [
					E('td', { 'class': 'td' }, _('总计')),
					E('td', { 'class': 'td' }, mb(status.total_kb))
				]),
				E('tr', { 'class': 'tr' }, [
					E('td', { 'class': 'td' }, _('可用')),
					E('td', { 'class': 'td' }, mb(status.available_kb))
				])
			]),

			onExternal ? E('p', {}, [
				E('button', {
					'class': 'btn cbi-button-reset',
					'click': function() { self.handleRevert(); }
				}, _('恢复到内部闪存'))
			]) : E('span'),

			E('h3', {}, _('可用磁盘')),
			E('div', {}, rows)
		]);
	},

	handleSave: null,
	handleSaveApply: null,
	handleReset: null
});
