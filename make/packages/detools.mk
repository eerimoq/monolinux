LIBS += detools
LIBDETOOLS = $(BUILD)/root/lib/libdetools.a

$(eval $(call PACKAGE_template,detools,$(DETOOLS_RSYNC),$(DETOOLS_BUILD),$(LIBDETOOLS)))

$(DETOOLS_BUILD): $(XZ_BUILD)
$(DETOOLS_BUILD): $(HEATSHRINK_BUILD)
$(DETOOLS_BUILD): $(DETOOLS_RSYNC)
	echo "Building detools."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/detools/src/c library \
	    CFLAGS_EXTRA=-I$(SYSROOT)/include
	$(MAKE) -C $(PACKAGES_DIR)/detools/src/c install PREFIX=$(SYSROOT)
	touch $@
