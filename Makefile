include $(TOPDIR)/rules.mk

PKG_NAME:=esp_ubusd
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/esp_ubusd
	CATEGORY:=Base system
	TITLE:=esp_ubusd
	DEPENDS:= + +libserialport +libubox +libjson-c +libubus  +libblobmsg-json
endef

define Package/esp_ubusd/description
	Esp control service
endef

define Package/esp_ubusd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/esp_ubusd $(1)/usr/bin
	$(INSTALL_BIN) ./files/esp_ubusd.init $(1)/etc/init.d/esp_ubusd
endef

$(eval $(call BuildPackage,esp_ubusd))

