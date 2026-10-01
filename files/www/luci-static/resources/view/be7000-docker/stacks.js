'use strict';
'require view';
'require rpc';
'require ui';

var callStatus = rpc.declare({ object: 'be7000-docker', method: 'status' });
var callDf = rpc.declare({ object: 'be7000-docker', method: 'df' });
var callPrune = rpc.declare({ object: 'be7000-docker', method: 'prune', params: ['what'] });
var callStacks = rpc.declare({ object: 'be7000-docker', method: 'stacks' });
var callRead = rpc.declare({ object: 'be7000-docker', method: 'stack_read', params: ['name'] });
var callWrite = rpc.declare({ object: 'be7000-docker', method: 'stack_write', params: ['name', 'compose'] });
var callUp = rpc.declare({ object: 'be7000-docker', method: 'stack_up', params: ['name'] });
var callDown = rpc.declare({ object: 'be7000-docker', method: 'stack_down', params: ['name'] });
var callPull = rpc.declare({ object: 'be7000-docker', method: 'stack_pull', params: ['name'] });
var callLogs = rpc.declare({ object: 'be7000-docker', method: 'stack_logs', params: ['name'] });
var callRemove = rpc.declare({ object: 'be7000-docker', method: 'stack_remove', params: ['name'] });

// Ready-made stacks for the things people actually run on a router. Each one
// is a plain compose file, so it stays editable after deployment.
var TEMPLATES = [
	{
		name: 'portainer',
		title: 'Portainer CE',
		hint: _('在独立的 Web 界面中全面管理 Docker，端口 9443'),
		compose: 'services:\n  portainer:\n    image: portainer/portainer-ce:latest\n    container_name: portainer\n    restart: always\n    ports:\n      - "9443:9443"\n    volumes:\n      - /var/run/docker.sock:/var/run/docker.sock\n      - portainer_data:/data\n\nvolumes:\n  portainer_data:\n'
	},
	{
		name: 'adguard',
		title: 'AdGuard Home',
		hint: _('广告拦截和自有 DNS，Web 界面在 3000 端口'),
		compose: 'services:\n  adguard:\n    image: adguard/adguardhome:latest\n    container_name: adguard\n    restart: always\n    ports:\n      - "3000:3000"\n      - "5353:53/udp"\n    volumes:\n      - adguard_work:/opt/adguardhome/work\n      - adguard_conf:/opt/adguardhome/conf\n\nvolumes:\n  adguard_work:\n  adguard_conf:\n'
	},
	{
		name: 'uptime-kuma',
		title: 'Uptime Kuma',
		hint: _('服务可用性监控，端口 3001'),
		compose: 'services:\n  uptime-kuma:\n    image: louislam/uptime-kuma:1\n    container_name: uptime-kuma\n    restart: always\n    ports:\n      - "3001:3001"\n    volumes:\n      - kuma_data:/app/data\n\nvolumes:\n  kuma_data:\n'
	},
	{
		name: 'vaultwarden',
		title: 'Vaultwarden',
		hint: _('自建密码管理器，兼容 Bitwarden 客户端，端口 8080'),
		compose: 'services:\n  vaultwarden:\n    image: vaultwarden/server:latest\n    container_name: vaultwarden\n    restart: always\n    environment:\n      - WEBSOCKET_ENABLED=true\n    ports:\n      - "8080:80"\n    volumes:\n      - vw_data:/data\n\nvolumes:\n  vw_data:\n'
	},
	{
		name: 'qbittorrent',
		title: 'qBittorrent',
		hint: _('带 Web 界面的 BT 客户端，端口 8081，下载到 /mnt'),
		compose: 'services:\n  qbittorrent:\n    image: lscr.io/linuxserver/qbittorrent:latest\n    container_name: qbittorrent\n    restart: always\n    environment:\n      - PUID=0\n      - PGID=0\n      - WEBUI_PORT=8081\n    ports:\n      - "8081:8081"\n      - "6881:6881"\n      - "6881:6881/udp"\n    volumes:\n      - qbt_config:/config\n      - /mnt:/downloads\n\nvolumes:\n  qbt_config:\n'
	},
	{
		name: 'nginx-proxy-manager',
		title: 'Nginx Proxy Manager',
		hint: _('一键申请证书的反向代理，面板在 81 端口'),
		compose: 'services:\n  npm:\n    image: jc21/nginx-proxy-manager:latest\n    container_name: npm\n    restart: always\n    ports:\n      - "8880:80"\n      - "8443:443"\n      - "81:81"\n    volumes:\n      - npm_data:/data\n      - npm_ssl:/etc/letsencrypt\n\nvolumes:\n  npm_data:\n  npm_ssl:\n'
	},
	{
		name: 'homeassistant',
		title: 'Home Assistant',
		hint: _('智能家居，使用主机网络运行'),
		compose: 'services:\n  homeassistant:\n    image: ghcr.io/home-assistant/home-assistant:stable\n    container_name: homeassistant\n    restart: always\n    network_mode: host\n    volumes:\n      - ha_config:/config\n      - /etc/localtime:/etc/localtime:ro\n\nvolumes:\n  ha_config:\n'
	},
	{
		name: 'watchtower',
		title: 'Watchtower',
		hint: _('自动将运行中的容器更新到最新镜像'),
		compose: 'services:\n  watchtower:\n    image: containrrr/watchtower:latest\n    container_name: watchtower\n    restart: always\n    command: --cleanup --interval 86400\n    volumes:\n      - /var/run/docker.sock:/var/run/docker.sock\n'
	}
];

function busy(title, promise, onDone) {
	ui.showModal(title, [E('p', { 'class': 'spinning' }, _('执行中'))]);
	return promise.then(function(res) {
		ui.hideModal();
		if (res && res.ok) {
			if (res.output)
				ui.addNotification(null, E('pre', { 'style': 'white-space:pre-wrap' }, res.output), 'info');
			else
				ui.addNotification(null, E('p', {}, _('完成')), 'info');
		}
		else {
			ui.addNotification(null, E('pre', { 'style': 'white-space:pre-wrap' }, (res && res.output) || _('失败')), 'error');
		}
		if (onDone) onDone();
	}).catch(function(err) {
		ui.hideModal();
		ui.addNotification(null, E('p', {}, String(err)), 'error');
	});
}

return view.extend({
	load: function() {
		return Promise.all([
			callStatus().catch(function() { return {}; }),
			callStacks().catch(function() { return {}; })
		]);
	},

	refresh: function() {
		var self = this;
		return self.load().then(function(data) {
			var fresh = self.render(data);
			var node = document.getElementById('be7000-docker');
			if (node && fresh) node.parentNode.replaceChild(fresh, node);
		});
	},

	editor: function(name, body, isNew) {
		var self = this;
		var ta = E('textarea', {
			'style': 'width:100%;height:24em;font-family:monospace;font-size:12px',
			'spellcheck': 'false'
		}, body || '');
		var nameInput = E('input', { 'type': 'text', 'value': name || '', 'style': 'width:20em' });

		ui.showModal(isNew ? _('新建堆栈') : _('堆栈 %s').format(name), [
			isNew ? E('p', {}, [ _('名称（字母、数字、连字符）： '), nameInput ]) : E('p', {}, _('文件 docker-compose.yml')),
			ta,
			E('div', { 'class': 'right' }, [
				E('button', { 'class': 'btn', 'click': ui.hideModal }, _('取消')),
				' ',
				E('button', {
					'class': 'btn cbi-button-action',
					'click': function() {
						var n = isNew ? nameInput.value.trim() : name;
						if (!/^[a-zA-Z0-9_-]{1,40}$/.test(n)) {
							ui.addNotification(null, E('p', {}, _('名称只能包含字母、数字、连字符和下划线')), 'error');
							return;
						}
						ui.hideModal();
						busy(_('保存中'), callWrite(n, ta.value), function() { self.refresh(); });
					}
				}, _('保存')),
				' ',
				E('button', {
					'class': 'btn cbi-button-positive',
					'click': function() {
						var n = isNew ? nameInput.value.trim() : name;
						if (!/^[a-zA-Z0-9_-]{1,40}$/.test(n)) {
							ui.addNotification(null, E('p', {}, _('请检查名称')), 'error');
							return;
						}
						ui.hideModal();
						callWrite(n, ta.value).then(function(res) {
							if (!res || !res.ok) {
								ui.addNotification(null, E('pre', { 'style': 'white-space:pre-wrap' }, (res && res.output) || _('保存失败')), 'error');
								return;
							}
							busy(_('启动中'), callUp(n), function() { self.refresh(); });
						});
					}
				}, _('保存并启动'))
			])
		]);
	},

	render: function(data) {
		var self = this;
		var st = (data[0] && data[0].data) || {};
		var stacks = (data[1] && data[1].data) || [];

		var head = [];
		if (!st.installed) {
			head.push(E('div', { 'class': 'alert-message warning' }, [
				E('p', {}, _('Docker 未安装。请使用命令 be7000-docker setup 安装，它会自动找到磁盘并将数据放在那里，而不是放在 19 MB 的内部存储中。')),
				E('pre', {}, 'be7000-docker setup --yes')
			]));
		}
		else if (!st.daemon_ok) {
			head.push(E('div', { 'class': 'alert-message warning' }, _('Docker 已安装，但服务无响应。请检查 /etc/init.d/dockerd status')));
		}
		else {
			head.push(E('p', {}, _('Docker %s 正在运行。容器、镜像、网络和卷分别位于“服务”>“Dockerman”部分。').format(st.version)));
		}

		var table = E('table', { 'class': 'table' }, [
			E('tr', { 'class': 'tr table-titles' }, [
				E('th', { 'class': 'th' }, _('堆栈')),
				E('th', { 'class': 'th' }, _('状态')),
				E('th', { 'class': 'th' }, '')
			])
		]);

		if (!stacks.length) {
			table.appendChild(E('tr', { 'class': 'tr' }, [
				E('td', { 'class': 'td', 'colspan': '3' }, _('目前没有任何堆栈。可以从下方选取现成的，或创建自己的。'))
			]));
		}

		stacks.forEach(function(s) {
			table.appendChild(E('tr', { 'class': 'tr' }, [
				E('td', { 'class': 'td' }, s.name),
				E('td', { 'class': 'td' }, E('pre', { 'style': 'margin:0;white-space:pre-wrap' }, s.state || _('已停止'))),
				E('td', { 'class': 'td', 'style': 'white-space:nowrap' }, [
					E('button', { 'class': 'btn cbi-button-positive', 'click': function() { busy(_('启动中'), callUp(s.name), function() { self.refresh(); }); } }, _('启动')),
					' ',
					E('button', { 'class': 'btn', 'click': function() { busy(_('停止中'), callDown(s.name), function() { self.refresh(); }); } }, _('停止')),
					' ',
					E('button', { 'class': 'btn', 'click': function() { busy(_('更新镜像中'), callPull(s.name)); } }, _('更新')),
					' ',
					E('button', { 'class': 'btn', 'click': function() { busy(_('读取日志中'), callLogs(s.name)); } }, _('日志')),
					' ',
					E('button', { 'class': 'btn', 'click': function() {
						callRead(s.name).then(function(res) {
							self.editor(s.name, (res && res.data && res.data.compose) || '', false);
						});
					} }, _('编辑')),
					' ',
					E('button', { 'class': 'btn cbi-button-negative', 'click': function() {
						if (confirm(_('删除堆栈 %s 及其卷？').format(s.name)))
							busy(_('删除中'), callRemove(s.name), function() { self.refresh(); });
					} }, _('删除'))
				])
			]));
		});

		var tpl = E('table', { 'class': 'table' }, [
			E('tr', { 'class': 'tr table-titles' }, [
				E('th', { 'class': 'th' }, _('现成堆栈')),
				E('th', { 'class': 'th' }, _('说明')),
				E('th', { 'class': 'th' }, '')
			])
		]);
		TEMPLATES.forEach(function(t) {
			tpl.appendChild(E('tr', { 'class': 'tr' }, [
				E('td', { 'class': 'td' }, t.title),
				E('td', { 'class': 'td' }, t.hint),
				E('td', { 'class': 'td', 'style': 'white-space:nowrap' }, [
					E('button', { 'class': 'btn', 'click': function() { self.editor(t.name, t.compose, true); } }, _('查看并修改')),
					' ',
					E('button', { 'class': 'btn cbi-button-action', 'click': function() {
						callWrite(t.name, t.compose).then(function(res) {
							if (!res || !res.ok) {
								ui.addNotification(null, E('pre', { 'style': 'white-space:pre-wrap' }, (res && res.output) || _('保存失败')), 'error');
								return;
							}
							busy(_('部署 %s 中').format(t.title), callUp(t.name), function() { self.refresh(); });
						});
					} }, _('部署'))
				])
			]));
		});

		return E('div', { 'id': 'be7000-docker' }, [
			E('h2', {}, _('Docker：堆栈')),
			E('div', {}, head),

			E('h3', {}, _('我的堆栈')),
			table,
			E('p', {}, E('button', { 'class': 'btn cbi-button-add', 'click': function() { self.editor('', 'services:\n  app:\n    image: \n    restart: always\n', true); } }, _('创建堆栈'))),

			E('h3', {}, _('现成的')),
			tpl,

			E('h3', {}, _('维护')),
			E('p', {}, [
				E('button', { 'class': 'btn', 'click': function() { busy(_('统计已用空间'), callDf()); } }, _('已用空间')),
				' ',
				E('button', { 'class': 'btn', 'click': function() { busy(_('清理中'), callPrune('system')); } }, _('清理垃圾')),
				' ',
				E('button', { 'class': 'btn', 'click': function() {
					if (confirm(_('删除所有未被运行中容器使用的镜像？')))
						busy(_('清理镜像中'), callPrune('images'));
				} }, _('删除多余镜像')),
				' ',
				E('button', { 'class': 'btn', 'click': function() {
					if (confirm(_('删除未连接到任何容器的卷？其中的数据将丢失。')))
						busy(_('清理卷中'), callPrune('volumes'));
				} }, _('删除未连接的卷'))
			])
		]);
	},

	handleSave: null,
	handleSaveApply: null,
	handleReset: null
});
