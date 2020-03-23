LIBS += heatshrink_dynamic
LIBHEATSHRINK = $(BUILD)/root/lib/heatshrink_dynamic.a

$(eval $(call PACKAGE_template,heatshrink,$(HEATSHRINK_RSYNC),$(HEATSHRINK_BUILD),$(LIBHEATSHRINK)))

$(HEATSHRINK_BUILD): $(DBG_MACRO_BUILD)
$(HEATSHRINK_BUILD): $(HEATSHRINK_RSYNC)
	echo "Building heatshrink."
	mkdir -p $(SYSROOT)/bin $(SYSROOT)/include $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/heatshrink CC=$(CROSS_COMPILE)gcc
	$(MAKE) -C $(PACKAGES_DIR)/heatshrink install PREFIX=$(SYSROOT)
	touch $@
