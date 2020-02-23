LIBS += async
ASYNC_TAR_GZ = $(ML_SOURCES)/async-0.6.0.tar.gz

packages: $(PACKAGES)/async

$(PACKAGES)/async: $(PACKAGES)/bitstream
$(PACKAGES)/async: $(PACKAGES)/humanfriendly
$(PACKAGES)/async: $(PACKAGES)/ml
$(PACKAGES)/async: $(PACKAGES)/mbedtls
$(PACKAGES)/async:
	$(MAKE) async-all

async-all:
	@echo "Building async."
	$(MAKE) async-fetch
	$(MAKE) async-unpack
	$(MAKE) async-build

async-clean:
	cd $(PACKAGES)/async

async-fetch: $(ASYNC_TAR_GZ)

$(ASYNC_TAR_GZ):
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/async/archive/0.6.0.tar.gz

async-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ASYNC_TAR_GZ) && \
	mv async-0.6.0 async

async-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch async && \
	cd async && \
	$(MAKE) library ASYNC_ROOT=. CFLAGS_EXTRA=-I$(SYSROOT)/include && \
	$(MAKE) install PREFIX=$(SYSROOT)
