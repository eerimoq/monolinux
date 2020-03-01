LIBS += detools
LIBDETOOLS = $(BUILD)/root/lib/libdetools.a

.PHONY: $(PACKAGES)/detools

packages: $(PACKAGES)/detools

$(PACKAGES)/detools: $(LIBDETOOLS)

$(LIBDETOOLS): detools-all

detools-all: $(PACKAGES)/xz
detools-all: $(PACKAGES)/heatshrink
detools-all:
	mkdir -p $(PACKAGES) $(SYSROOT)/lib
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/detools $(PACKAGES))" ] ; then \
	    echo "Building detools." && \
	    $(MAKE) -C $(PACKAGES)/detools/src/c library \
		CFLAGS_EXTRA=-I$(SYSROOT)/include && \
	    $(MAKE) -C $(PACKAGES)/detools/src/c install PREFIX=$(SYSROOT) ; \
	fi

detools-clean:
	rm -rf $(PACKAGES)/detools $(LIBDETOOLS)
