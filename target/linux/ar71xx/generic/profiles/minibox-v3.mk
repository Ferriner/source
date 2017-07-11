define Profile/MINIBOXV3
	NAME:=Gainstrong MiniBox V3
	PACKAGES:= \
		kmod-usb-core kmod-usb2
endef

define Profile/MINIBOXV3/Description
	Package set optimized for the Gainstrong MiniBox V3
endef
$(eval $(call Profile,MINIBOXV3))
