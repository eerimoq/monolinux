LIBS += async
LIBASYNC = $(BUILD)/root/lib/libasync.a

packages-rsync: async-rsync

packages-build: async-build

async-all:
	$(MAKE) async-rsync
	$(MAKE) async-build

async-build: $(ASYNC_BUILD)

async-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/async $(PACKAGES))" ] ; then \
	    echo "async sources updated" && \
	    touch $(ASYNC_RSYNC) ; \
	fi

$(ASYNC_BUILD): $(BITSTREAM_BUILD)
$(ASYNC_BUILD): $(HUMANFRIENDLY_BUILD)
$(ASYNC_BUILD): $(MONOLINUX_C_LIBRARY_BUILD)
$(ASYNC_BUILD): $(MBEDTLS_BUILD)
$(ASYNC_BUILD): $(ASYNC_RSYNC)
	echo "Building async."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES)/async library \
	    ASYNC_ROOT=. CFLAGS_EXTRA=-I$(SYSROOT)/include
	$(MAKE) -C $(PACKAGES)/async install PREFIX=$(SYSROOT)
	touch $@

async-clean:
	rm -rf $(PACKAGES)/async $(LIBASYNC)
