# luci-app-adguardhome
复杂的AdGuardHome的openwrt的luci界面

 - 可以管理网页端口
 - luci下载/更新核心版本
 - upx 压缩核心
 - dns重定向
 - 自定义执行文件路径（支持tmp，每次重启后自动下载bin）
 - 自定义配置文件路径
 - 自定义工作路径
 - 自定义运行日志路径
 - gfwlist 定义上游dns服务器
 - 修改网页登陆密码
 - 倒序/正序 查看/删除/备份 每3秒更新显示运行日志
 - 手动修改配置文件(支持yaml编辑器)
 - 使用模板快速配置(没有配置文件时)
 - 系统升级保留程序和配置（查询日志和数据库可选保留)
#### 已知问题：
 - db数据库不支持放在不支持mmap的文件系统上比如 jffs2 data-stk-oo，请修改工作目录，本软件如果检测到jffs2会自动ln(软连接)到/tmp，将会导致重启丢失dns数据库
 - AdGuardHome 不支持ipset 设置，在使用ipset的情况下，无法替代dnsmasq只能作为dnsmasq上游存在，如果你想要这个功能就去投票吧<br>
 https://github.com/AdguardTeam/AdGuardHome/issues/1191
 - 反馈出现大量127.0.0.1查询localhost的请求，问题出现原因是ddns插件，如果不用ddns插件，请删空ddns插件中的所有规则，插件中的每一个规则会在一秒钟产生一个这样的请求（只出现在mips上）
#### 项目已经基本稳定，有bug欢迎主动反馈


Complex openwrt AdGuardHome luci

 - can manage browser port
 - download/update core in luci
 - compress core with upx
 - redirect dns
 - change bin path
 - change config path
 - change work dir(support tmp,auto redownload after reboot)
 - change runtime log path
 - gfwlist query to specific dns server
 - modify browser login passord
 - Positive/reverse order see/del/backup runtime log which update every 3 second
 - modify config manually(support yaml editor)
 - use template to fast config(when no config file)
 - Keep bin file and config when system upgrade (database and querylog can be choose) 
#### known issues:
 - db database not support filesystem which not support mmap such as jffs2 and data-stk-oo,please modify work dir,if jffs2 is found,will auto ln (soft link)the dbs to /tmp ,will lost dns database after reboot
 - AdGuardhome not support ipset,when we use ipset ,it can`t be the repacement of dnsmasq but the upstream of dnsmasq ,if you want it,vote for it.<br>
 https://github.com/AdguardTeam/AdGuardHome/issues/1191<br>
#### pic
example in zh-cn:<br>
![Screenshot_2019-12-06 newifi-d1-home - 基础设置 - LuCI](https://user-images.githubusercontent.com/22387141/70305470-74372a00-183f-11ea-80fd-6fd96262c789.png)
![Screenshot_2019-12-06 newifi-d1-home - 日志 - LuCI](https://user-images.githubusercontent.com/22387141/70305550-a8aae600-183f-11ea-92c7-eb1e085143f5.png)
![Screenshot_2019-12-06 newifi-d1-home - 手动设置 - LuCI](https://user-images.githubusercontent.com/22387141/70305555-ab0d4000-183f-11ea-87c2-22315891b4c4.png)
