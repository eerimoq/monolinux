LIBS += heatshrink_dynamic
LIBHEATSHRINK = $(BUILD)/root/lib/heatshrink_dynamic.a

packages-rsync: heatshrink-rsync

packages-build: heatshrink-build

heatshrink-all:
	$(MAKE) heatshrink-rsync
	$(MAKE) heatshrink-build

heatshrink-build: $(HEATSHRINK_BUILD)

heatshrink-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/heatshrink $(PACKAGES))" ] ; then \
	    echo "heatshrink sources updated." && \
	    touch $(HEATSHRINK_RSYNC) ; \
	fi

$(HEATSHRINK_BUILD): $(HEATSHRINK_RSYNC)
	echo "Building heatshrink."
	mkdir -p $(SYSROOT)/bin $(SYSROOT)/include $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES)/heatshrink CC=$(CROSS_COMPILE)gcc
	$(MAKE) -C $(PACKAGES)/heatshrink install PREFIX=$(SYSROOT)
	touch $@

heatshrink-clean:
	rm -rf $(PACKAGES)/heatshrink $(LIBHEATSHRINK)
