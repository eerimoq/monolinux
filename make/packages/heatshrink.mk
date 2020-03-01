LIBS += heatshrink_dynamic
LIBHEATSHRINK = $(BUILD)/root/lib/heatshrink_dynamic.a

.PHONY: $(PACKAGES)/heatshrink

packages: $(PACKAGES)/heatshrink

$(PACKAGES)/heatshrink: $(LIBHEATSHRINK)

$(LIBHEATSHRINK): heatshrink-all

heatshrink-all:
	mkdir -p $(PACKAGES) $(SYSROOT)/bin $(SYSROOT)/include $(SYSROOT)/lib
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/heatshrink $(PACKAGES))" ] ; then \
	    echo "Building heatshrink." && \
	    $(MAKE) -C $(PACKAGES)/heatshrink CC=$(CROSS_COMPILE)gcc && \
	    $(MAKE) -C $(PACKAGES)/heatshrink install PREFIX=$(SYSROOT) ; \
	fi

heatshrink-clean:
	rm -rf $(PACKAGES)/heatshrink $(LIBHEATSHRINK)
