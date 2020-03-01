LIBS += bitstream
LIBBITSTREAM = $(BUILD)/root/lib/libbitstream.a

.PHONY: $(PACKAGES)/bitstream

packages: $(PACKAGES)/bitstream

$(PACKAGES)/bitstream: $(LIBBITSTREAM)

$(LIBBITSTREAM): bitstream-all

bitstream-all:
	mkdir -p $(PACKAGES) $(SYSROOT)/lib
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/bitstream $(PACKAGES))" ] ; then \
	    echo "Building bitstream." && \
	    $(MAKE) -C $(PACKAGES)/bitstream library && \
	    $(MAKE) -C $(PACKAGES)/bitstream install PREFIX=$(SYSROOT) ; \
	fi

bitstream-clean:
	rm -rf $(PACKAGES)/bitstream $(LIBBITSTREAM)
