LIBS += bitstream
BITSTREAM_TAR_GZ = $(ML_SOURCES)/bitstream-0.7.0.tar.gz

packages: $(PACKAGES)/bitstream

$(PACKAGES)/bitstream:
	$(MAKE) bitstream-all

bitstream-all:
	@echo "Building bitstream."
	$(MAKE) bitstream-fetch
	$(MAKE) bitstream-unpack
	$(MAKE) bitstream-build

bitstream-clean:
	cd $(PACKAGES)/bitstream

bitstream-fetch: $(BITSTREAM_TAR_GZ)

$(BITSTREAM_TAR_GZ):
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/bitstream/archive/0.7.0.tar.gz

bitstream-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(BITSTREAM_TAR_GZ) && \
	mv bitstream-0.7.0 bitstream

bitstream-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch bitstream && \
	cd bitstream && \
	$(MAKE) library && \
	$(MAKE) install PREFIX=$(SYSROOT)
