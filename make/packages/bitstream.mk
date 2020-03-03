LIBS += bitstream
LIBBITSTREAM = $(BUILD)/root/lib/libbitstream.a

packages-rsync: bitstream-rsync

packages-build: bitstream-build

bitstream-all:
	$(MAKE) bitstream-rsync
	$(MAKE) bitstream-build

bitstream-build: $(BITSTREAM_BUILD)

bitstream-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/bitstream $(PACKAGES))" ] ; then \
	    echo "bitstream sources updated." && \
	    touch $(BITSTREAM_RSYNC) ; \
	fi

$(BITSTREAM_BUILD): $(BITSTREAM_RSYNC)
	echo "Building bitstream."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES)/bitstream library
	$(MAKE) -C $(PACKAGES)/bitstream install PREFIX=$(SYSROOT)
	touch $@

bitstream-clean:
	rm -rf $(PACKAGES)/bitstream $(LIBBITSTREAM)
