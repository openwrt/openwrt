m = Map("filebrowser", translate("FileBrowser"), translate(
            "文件浏览器是一种创建你自己的云的软件，你可以在服务器上安装它，将它指向一个路径，然后通过一个漂亮的web界面访问你的文件。您有许多可用的特性!"))
m:append(Template("filebrowser/status"))

s = m:section(TypedSection, "global", translate("Global Setting"))
s.anonymous = true
s.addremove = false

o = s:option(Flag, "enable", translate("Enable"))
o.rmempty = false

o = s:option(Value, "port", translate("监听端口"))
o.datatype = "port"
o.default = 8088
o.rmempty = false

o = s:option(Value, "root_path", translate("指向路径"), translate(
                 "指向一个路径，可在web界面访问你的文件，默认为 /"))
o.default = "/"
o.rmempty = false

o = s:option(Value, "project_directory", translate("项目存放目录"),
             translate(
                 "文件较大，至少需要32M空间。建议插入U盘或硬盘，或放入tmp目录里使用<br>例如：/mnt/sda1<br>例如：/tmp"))
o.default = "/tmp"
o.rmempty = false

o = s:option(Button, "_download", translate("手动下载"), translate(
                 "请确保具有足够的空间。<br /><font style='color:red'>第一次运行务必填好项目存放目录，然后保存应用。再手动下载，否则无法使用！</font>"))
o.template = "filebrowser/download"
o.inputstyle = "apply"
o.btnclick = "downloadClick(this);"
o.id = "download_btn"

return m
