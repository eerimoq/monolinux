LIBS += pbtools
LIBPBTOOLS = $(BUILD)/root/lib/libpbtools.a

$(eval $(call PACKAGE_template,pbtools,$(PBTOOLS_RSYNC),$(PBTOOLS_BUILD),$(LIBPBTOOLS)))

$(PBTOOLS_BUILD): $(PBTOOLS_RSYNC)
	echo "Building pbtools."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/pbtools/lib library
	$(MAKE) -C $(PACKAGES_DIR)/pbtools/lib install PREFIX=$(SYSROOT)
	touch $@
