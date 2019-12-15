# luci-app-adguardhome
复杂的AdGuardHome的openwrt的luci界面

 - 可以管理网页端口
 - luci下载/更新核心版本
 - upx 压缩核心（xz依赖，脚本自动下载）
 - dns重定向
   - 作为dnsmasq的上游服务器(在AGH中无法统计客户端信息，也无法针对客户端调整设置，ssr-plus正常)
   - 重定向53端口到 AdGuardHome（ipv6需要开启ipv6 nat redirect 否则如果客户端使用ipv6过滤无效，不以dnsmasq为上游ssr-plus失效）
   - 使用53端口替换 dnsmasq(需要设置AGH,dnsip为0.0.0.0, AGH和dnsmasq的端口将被交换，不以dnsmasq为上游ssr-plus失效)
 - 自定义执行文件路径（支持tmp，每次重启后自动下载bin）
 - 自定义配置文件路径
 - 自定义工作路径
 - 自定义运行日志路径
 - gfwlist 定义上游dns服务器
 - 修改网页登陆密码
 - 倒序/正序 查看/删除/备份 每3秒更新显示运行日志 + 本地浏览器时区转换
 - 手动修改配置文件(支持yaml编辑器)
 - 使用模板快速配置(没有配置文件时)
 - 系统升级保留程序和配置（查询日志和数据库可选保留)
 - 开机启动时等待网络准备延迟启动(3分钟超时)
 - 关机时备份工作目录
#### 已知问题：
 - db数据库不支持放在不支持mmap的文件系统上比如 jffs2 data-stk-oo，请修改工作目录，本软件如果检测到jffs2会自动ln(软连接)到/tmp，将会导致重启丢失dns数据库
 - AdGuardHome 不支持ipset 设置，在使用ipset的情况下，无法替代dnsmasq只能作为dnsmasq上游存在，如果你想要这个功能就去投票吧<br>
 https://github.com/AdguardTeam/AdGuardHome/issues/1191
 - 反馈出现大量127.0.0.1查询localhost的请求，问题出现原因是ddns插件，如果不用ddns插件，请删除或者注释掉\etc\hotplug.d\iface\95-ddns的内容，如果还有其他来自本机的异常查询情况，高级玩家可以使用kmod来查找原因https://github.com/rufengsuixing/kmod-plog-port
#### 使用方法
 - 下载release，使用opkg安装即可
 - 或者编译op时clone本项目加入软件包并勾选
#### 关于压缩
本着较真的想法，我测试了在jffs2的压缩文件系统上进行upx压缩结果的内存占用与空间占用(单位kb，使用最好压缩)<br>
文件大小<br>
源文件   14112 使用upx 压缩后 5309 <br>
实际占用 6260  使用upx 压缩后 5324 差值为 936<br>
VmRSS运存占用值<br>
不压缩   14380 使用upx 压缩后 18496 差值 -4116 <br>
对于压缩文件系统来说开启收益有，但不大<br>
如果是非压缩文件系统，性价比还是比较高的<br>
所以压缩是用运存空间来换rom空间，觉得值得就可以开启
#### 项目已经基本稳定，有bug欢迎主动反馈

Complex openwrt AdGuardHome luci

 - can manage browser port
 - download/update core in luci
 - compress core with upx
 - redirect dns
   - as the upstream of dnsmasq
   - redirect port 53 to AdGuardHome（ipv6 need to install ipv6 nat redirect or  if client use ipv6 redirect is invalid）
   - replace dnsmasq with port 53 (need to set AGH,dnsip=0.0.0.0,the port of dnsmasq and AGH will be exchange)
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
 - when boot wait for network access （3min timeout）
 - backup workdir when shutdown
#### known issues:
 - db database not support filesystem which not support mmap such as jffs2 and data-stk-oo,please modify work dir,if jffs2 is found,will auto ln (soft link)the dbs to /tmp ,will lost dns database after reboot
 - AdGuardhome not support ipset,when we use ipset ,it can\`t be the repacement of dnsmasq but the upstream of dnsmasq ,if you want it,vote for it.<br>
 https://github.com/AdguardTeam/AdGuardHome/issues/1191<br>
 - find so many localhost query from 127.0.0.1,the ddns plugin is the reason,if you don\`t use ddns, please remove or comment \etc\hotplug.d\iface\95-ddns
#### usage
 - download release，install it with opkg
 - or when make op,clone the code to the package path and set it as y or m

#### pic
example in zh-cn:<br>
![Screenshot_2019-12-06 newifi-d1-home - 基础设置 - LuCI](https://user-images.githubusercontent.com/22387141/70305470-74372a00-183f-11ea-80fd-6fd96262c789.png)
![Screenshot_2019-12-06 newifi-d1-home - 日志 - LuCI](https://user-images.githubusercontent.com/22387141/70305550-a8aae600-183f-11ea-92c7-eb1e085143f5.png)
![Screenshot_2019-12-06 newifi-d1-home - 手动设置 - LuCI](https://user-images.githubusercontent.com/22387141/70305555-ab0d4000-183f-11ea-87c2-22315891b4c4.png)
