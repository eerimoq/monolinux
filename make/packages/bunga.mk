LIBS += bunga
LIBBUNGA = $(BUILD)/root/lib/libbunga.a

$(eval $(call PACKAGE_template,bunga,$(BUNGA_RSYNC),$(BUNGA_BUILD),$(LIBBUNGA)))

$(BUNGA_BUILD): $(MESSI_BUILD)
$(BUNGA_BUILD): $(BUNGA_RSYNC)
	echo "Building bunga."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/bunga/lib CFLAGS_EXTRA=-I$(SYSROOT)/include \
	    library
	$(MAKE) -C $(PACKAGES_DIR)/bunga/lib install PREFIX=$(SYSROOT)
	touch $@
