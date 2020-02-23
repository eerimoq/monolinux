LIBS += heatshrink_dynamic
HEATSHRINK_TAR_GZ = $(ML_SOURCES)/heatshrink-0.4.1.tar.gz

packages: $(PACKAGES)/heatshrink

$(PACKAGES)/heatshrink:
	$(MAKE) heatshrink-all

heatshrink-all:
	@echo "Building heatshrink."
	$(MAKE) heatshrink-fetch
	$(MAKE) heatshrink-unpack
	$(MAKE) heatshrink-build

heatshrink-clean:
	rm -rf $(PACKAGES)/heatshrink

heatshrink-fetch: $(HEATSHRINK_TAR_GZ)

$(HEATSHRINK_TAR_GZ):
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/atomicobject/heatshrink/archive/v0.4.1.tar.gz

heatshrink-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(HEATSHRINK_TAR_GZ) && \
	mv heatshrink-0.4.1 heatshrink

heatshrink-build:
	mkdir -p $(SYSROOT)/bin
	mkdir -p $(SYSROOT)/include
	mkdir -p $(SYSROOT)/lib
	cd $(PACKAGES)/heatshrink && \
	$(MAKE) CC=$(CROSS_COMPILE)gcc && \
	$(MAKE) install PREFIX=$(SYSROOT)
