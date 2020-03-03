LIBS += bitstream
LIBBITSTREAM = $(BUILD)/root/lib/libbitstream.a

$(eval $(call PACKAGE_template,bitstream,$(BITSTREAM_RSYNC),$(BITSTREAM_BUILD),$(LIBBITSTREAM)))

$(BITSTREAM_BUILD): $(BITSTREAM_RSYNC)
	echo "Building bitstream."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES)/bitstream library
	$(MAKE) -C $(PACKAGES)/bitstream install PREFIX=$(SYSROOT)
	touch $@
