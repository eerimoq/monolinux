LIBS += ml
LIBML = $(BUILD)/root/lib/libml.a

packages: $(PACKAGES)/monolinux-c-library

$(PACKAGES)/monolinux-c-library: $(LIBML)

$(LIBML): monolinux-c-library-all

monolinux-c-library-all:
	mkdir -p $(PACKAGES) $(SYSROOT)/lib
	if [ -n "$$(rsync -ariOu $(ML_ROOT)/3pp/monolinux-c-library $(PACKAGES))" ] ; then \
	    echo "Building monolinux-c-library." && \
	    $(MAKE) -C $(PACKAGES)/monolinux-c-library library \
		CC=$(CROSS_COMPILE)gcc ML_ROOT=. && \
	    $(MAKE) -C $(PACKAGES)/monolinux-c-library install \
		ML_ROOT=. PREFIX=$(SYSROOT) ; \
	fi

monolinux-c-library-clean:
	rm -rf $(PACKAGES)/monolinux-c-library $(LIBML)
