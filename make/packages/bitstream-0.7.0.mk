LIBS += bitstream
BITSTREAM = bitstream-0.7.0

packages: $(PACKAGES)/$(BITSTREAM)

$(PACKAGES)/$(BITSTREAM): $(ML_SOURCES)/$(BITSTREAM).tar.gz
	$(MAKE) $(BITSTREAM)-all

$(BITSTREAM)-all:
	@echo "Building $(BITSTREAM)."
	$(MAKE) $(BITSTREAM)-unpack
	$(MAKE) $(BITSTREAM)-build

$(BITSTREAM)-clean:
	cd $(PACKAGES)/$(BITSTREAM)

$(ML_SOURCES)/$(BITSTREAM).tar.gz:
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/bitstream/archive/0.7.0.tar.gz

$(BITSTREAM)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(BITSTREAM).tar.gz

$(BITSTREAM)-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch $(BITSTREAM) && \
	cd $(BITSTREAM) && \
	$(MAKE) library && \
	$(MAKE) install PREFIX=$(SYSROOT)
