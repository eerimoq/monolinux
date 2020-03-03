LIBS += ml
LIBML = $(BUILD)/root/lib/libml.a

packages-rsync: monolinux-c-library-rsync

packages-build: monolinux-c-library-build

monolinux-c-library-all:
	$(MAKE) monolinux-c-library-rsync
	$(MAKE) monolinux-c-library-build

monolinux-c-library-build: $(MONOLINUX_C_LIBRARY_BUILD)

monolinux-c-library-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/monolinux-c-library $(PACKAGES))" ] ; then \
	    echo "monolinux-c-library sources updated." && \
	    touch $(MONOLINUX_C_LIBRARY_RSYNC) ; \
	fi

$(MONOLINUX_C_LIBRARY_BUILD): $(MONOLINUX_C_LIBRARY_RSYNC)
	echo "Building monolinux-c-library."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES)/monolinux-c-library library \
	    CC=$(CROSS_COMPILE)gcc ML_ROOT=.
	$(MAKE) -C $(PACKAGES)/monolinux-c-library install \
	    ML_ROOT=. PREFIX=$(SYSROOT)
	touch $@

monolinux-c-library-clean:
	rm -rf $(PACKAGES)/monolinux-c-library $(LIBML)
