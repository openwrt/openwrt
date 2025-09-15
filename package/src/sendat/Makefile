include $(TOPDIR)/rules.mk#将openwrt顶层目录下的rules.mk文件中的内容导入进来
 
PKG_NAME:=sendat#软件包名
PKG_VERSION:=6.8.2#软件包版本
PKG_BUILD_DIR:= $(BUILD_DIR)/$(PKG_NAME)#真正编译当前软件包的目录
 
 
include $(INCLUDE_DIR)/package.mk #将$(TOPDIR)/include目录下的package.mk文件中的内容导入进来
 
define Package/sendat
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Sendat AT#软件包标题
	DEPENDS:=+libpthread
endef
 
define Package/sendat/description	#软件包描述
	A sample for sendat test
endef
 
define Build/Prepare	#编译之前的准备动作
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef
 
 
define Package/sendat/install	#软件包的安装方法，主要就是将一系列编译好的文件、启动脚本、UCI配置文件等拷贝到指定位置
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sendat $(1)/bin/
endef
 
$(eval $(call BuildPackage,sendat))