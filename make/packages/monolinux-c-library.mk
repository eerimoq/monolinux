LIBS += ml
LIBML = $(BUILD)/root/lib/libml.a

$(eval $(call PACKAGE_template,monolinux-c-library,$(MONOLINUX_C_LIBRARY_RSYNC),$(MONOLINUX_C_LIBRARY_BUILD),$(LIBML)))

$(MONOLINUX_C_LIBRARY_BUILD): $(DBG_MACRO_BUILD)
$(MONOLINUX_C_LIBRARY_BUILD): $(MONOLINUX_C_LIBRARY_RSYNC)
	echo "Building monolinux-c-library."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/monolinux-c-library library \
	    CC=$(CROSS_COMPILE)gcc ML_ROOT=.
	$(MAKE) -C $(PACKAGES_DIR)/monolinux-c-library install \
	    ML_ROOT=. PREFIX=$(SYSROOT)
	touch $@
