LIBS += bitstream
LIBBITSTREAM = $(BUILD)/root/lib/libbitstream.a

$(eval $(call PACKAGE_template,bitstream,$(BITSTREAM_RSYNC),$(BITSTREAM_BUILD),$(LIBBITSTREAM)))

$(BITSTREAM_BUILD): $(DBG_MACRO_BUILD)
$(BITSTREAM_BUILD): $(BITSTREAM_RSYNC)
	echo "Building bitstream."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/bitstream library
	$(MAKE) -C $(PACKAGES_DIR)/bitstream install PREFIX=$(SYSROOT)
	touch $@
