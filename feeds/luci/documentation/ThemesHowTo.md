# HowTo: Create Themes
*Note:* You should read the [Module Reference](Modules.md) and the [Template Reference](Templates.md) before.

We assume you want to call your new theme _mytheme_. Make sure you replace this by your module name every time this is mentionend in this Howto.



# Creating the structure
At first create a new theme directory *themes/_mytheme_*.

Create a _Makefile_ inside your theme directory with the following content:
	
	include ../../build/config.mk
	include ../../build/module.mk
	

Create the following directory structure inside your theme directory.
* ipkg
* htdocs
  * luci-static
   * _mytheme_
* luasrc
  * view
   * themes
    * _mytheme_
* root
  * etc
   * uci-defaults



# Designing
Create two LuCI HTML-Templates named _header.htm'' and ''footer.htm'' under *luasrc/view/themes/''mytheme_*.
The _header.htm'' will be included at the beginning of each rendered page and the ''footer.htm_ at the end.
So your _header.htm'' will probably contain a DOCTYPE description, headers, the menu and layout of the page and the ''footer.htm_ will close all remaining open tags and may add a footer bar but hey that's your choice you are the designer ;-).

Just make sure your _header.htm_ *begins* with the following lines:
	
	<%
	require("luci.http").prepare_content("text/html")
	-%>
	

This makes sure your content will be sent to the client with the right content type. Of course you can adapt _text/html_ to your needs.


Put any stylesheets, Javascripts, images, ... into *htdocs/luci-static/_mytheme_*.
You should refer to this directory in your header and footer templates as: _<%=media%>''. That means for a stylesheet *htdocs/luci-static/''mytheme_/cascade.css* you would write:
	
	<link rel="stylesheet" type="text/css" href="<%=media%>/cascade.css" />
	



# Making the theme selectable
If you are done with your work there are two last steps to do.
To make your theme OpenWRT-capable and selectable on the settings page you should now create a file *root/etc/uci-defaults/luci-theme-_mytheme_* with the following contents:
	
	#!/bin/sh
	uci batch <<-EOF
	        set luci.themes.MyTheme=/luci-static/mytheme
	        commit luci
	EOF
	

and another file *ipkg/postinst* with the following content:
	
	#!/bin/sh
	[ -n "${IPKG_INSTROOT}" ] || {
	        ( . /etc/uci-defaults/luci-theme-mytheme ) &&        rm -f /etc/uci-defaults/luci-theme-mytheme
	}
	

This is some OpenWRT magic to correctly register the template with LuCI when it gets installed.

That's all. Now send your theme to the LuCI developers to get it into the development repository - if you like.
