LIBS += heatshrink_dynamic

packages: $(PACKAGES)/heatshrink-0.4.1

$(PACKAGES)/heatshrink-0.4.1: $(ML_SOURCES)/heatshrink-0.4.1.tar.gz
	$(MAKE) heatshrink-0.4.1-all

heatshrink-0.4.1-all:
	@echo "Building heatshrink-0.4.1."
	$(MAKE) heatshrink-0.4.1-unpack
	$(MAKE) heatshrink-0.4.1-build

heatshrink-0.4.1-clean:
	rm -rf $(PACKAGES)/heatshrink-0.4.1

$(ML_SOURCES)/heatshrink-0.4.1.tar.gz:
	wget -O $@ https://github.com/atomicobject/heatshrink/archive/v0.4.1.tar.gz

heatshrink-0.4.1-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/heatshrink-0.4.1.tar.gz

heatshrink-0.4.1-build:
	mkdir -p $(SYSROOT)/bin
	mkdir -p $(SYSROOT)/include
	mkdir -p $(SYSROOT)/lib
	cd $(PACKAGES)/heatshrink-0.4.1 && \
	$(MAKE) CC=$(CROSS_COMPILE)gcc && \
	$(MAKE) install PREFIX=$(SYSROOT)
