LIBS += async
ASYNC = async-0.6.0

packages: $(PACKAGES)/$(ASYNC)

$(PACKAGES)/$(ASYNC): $(PACKAGES)/bitstream-0.7.0
$(PACKAGES)/$(ASYNC): $(PACKAGES)/humanfriendly-0.3.0
$(PACKAGES)/$(ASYNC): $(PACKAGES)/mbedtls-2.16.5
$(PACKAGES)/$(ASYNC): $(ML_SOURCES)/$(ASYNC).tar.gz
	$(MAKE) $(ASYNC)-all

$(ASYNC)-all:
	@echo "Building $(ASYNC)."
	$(MAKE) $(ASYNC)-unpack
	$(MAKE) $(ASYNC)-build

$(ASYNC)-clean:
	cd $(PACKAGES)/$(ASYNC)

$(ML_SOURCES)/$(ASYNC).tar.gz:
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/async/archive/0.6.0.tar.gz

$(ASYNC)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(ASYNC).tar.gz

$(ASYNC)-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch $(ASYNC) && \
	cd $(ASYNC) && \
	$(MAKE) library ASYNC_ROOT=. CFLAGS_EXTRA=-I$(SYSROOT)/include && \
	$(MAKE) install PREFIX=$(SYSROOT)
