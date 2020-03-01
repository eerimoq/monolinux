LIBS += async
LIBASYNC = $(BUILD)/root/lib/libasync.a

.PHONY: $(PACKAGES)/async

packages: $(PACKAGES)/async

$(PACKAGES)/async: $(LIBASYNC)

$(LIBASYNC): async-all

async-all: $(PACKAGES)/bitstream
async-all: $(PACKAGES)/humanfriendly
async-all: $(PACKAGES)/monolinux-c-library
async-all: $(PACKAGES)/mbedtls
async-all:
	mkdir -p $(PACKAGES) $(SYSROOT)/lib
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/async $(PACKAGES))" ] ; then \
	    echo "Building async." && \
	    $(MAKE) -C $(PACKAGES)/async library \
		ASYNC_ROOT=. CFLAGS_EXTRA=-I$(SYSROOT)/include && \
	    $(MAKE) -C $(PACKAGES)/async install PREFIX=$(SYSROOT) ; \
	fi

async-clean:
	rm -rf $(PACKAGES)/async $(LIBASYNC)
