LIBS += detools
LIBDETOOLS = $(BUILD)/root/lib/libdetools.a

packages-rsync: detools-rsync

packages-build: detools-build

detools-all:
	$(MAKE) detools-rsync
	$(MAKE) detools-build

detools-build: $(DETOOLS_BUILD)

detools-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/detools $(PACKAGES))" ] ; then \
	    echo "detools sources updated." && \
	    touch $(DETOOLS_RSYNC) ; \
	fi

$(DETOOLS_BUILD): $(XZ_BUILD)
$(DETOOLS_BUILD): $(HEATSHRINK_BUILD)
$(DETOOLS_BUILD): $(DETOOLS_RSYNC)
	echo "Building detools."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES)/detools/src/c library \
	    CFLAGS_EXTRA=-I$(SYSROOT)/include
	$(MAKE) -C $(PACKAGES)/detools/src/c install PREFIX=$(SYSROOT)
	touch $@

detools-clean:
	rm -rf $(PACKAGES)/detools $(LIBDETOOLS)
