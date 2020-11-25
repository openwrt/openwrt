# Categories

The LuCI modules are divided into several category directories, namely:
* applications (Single applications or plugins for other modules or applications)
* i18n (Translation files)
* libs (Independent libraries)
* modules (Collections of applications)
* themes (Frontend themes)

Each module goes into a subdirectory of any of this category-directories.

# Module directory
The contents of a module directory are as follows:

## Makefile
This is the module's makefile. If the module just contains Lua sourcecode or resources then the following Makefile should suffice.
	
	include ../../build/config.mk
	include ../../build/module.mk
	

If you have C(++) code in your module your Makefile should at least contain the following things.
	
	include ../../build/config.mk
	include ../../build/gccconfig.mk
	include ../../build/module.mk
	
	compile:
	    # Commands to compile and link your C-code
	    # and to install them under the dist/ hierarchy
	
	clean: luaclean
	    # Commands to clean your compiled objects
	
 

## src
The *src* directory is reserved for C sourcecode.

## luasrc
*luasrc* contains all Lua sourcecode files. These will automatically be stripped or compiled depending on the Make target and are installed in the LuCI installation directory.

## lua
*lua* is equivalent to _luasrc_ but containing Lua files will be installed in the Lua document root.

## htdocs
All files under *htdocs* will be copied to the document root of the target webserver.

## root
All directories and files under *root* will be copied to the installation target as they are.

## dist
*dist* is reserved for the builder to create a working installation tree that will represent the filesystem on the target machine.
*DO NOT* put any files there as they will get deleted.

## ipkg
*ipkg* contains IPKG package control files, like _preinst'', ''posinst'', ''prerm'', ''postrm''. ''conffiles_.
See IPKG documentation for details.


# OpenWRT feed integration
If you want to add your module to the LuCI OpenWRT feed you have to add several sections to the contrib/package/luci/Makefile.

For a Web UI applications this is:

A package description:
	
	define Package/luci-app-YOURMODULE
	  $(call Package/luci/webtemplate)
	  DEPENDS+=+some-package +some-other-package
	  TITLE:=SHORT DESCRIPTION OF YOURMODULE
	endef
	
	

A package installation target:
	
	define Package/luci-app-YOURMODULE/install
	        $(call Package/luci/install/template,$(1),applications/YOURMODULE)
	endef
	

A module build instruction:
	
	ifneq ($(CONFIG_PACKAGE_luci-app-YOURMODULE),)
	        PKG_SELECTED_MODULES+=applications/YOURMODULE
	endif
	


A build package call:
	
	$(eval $(call BuildPackage,luci-app-YOURMODULE))
	
