LIBS += heatshrink_dynamic

packages: $(PACKAGES)/heatshrink-0.4.1

$(ML_SOURCES)/heatshrink-0.4.1.tar.gz:
	wget -O $@ https://github.com/atomicobject/heatshrink/archive/v0.4.1.tar.gz

$(PACKAGES)/heatshrink-0.4.1: $(ML_SOURCES)/heatshrink-0.4.1.tar.gz
	@echo "Building heatshrink-0.4.1."
	mkdir -p $(SYSROOT)/bin
	mkdir -p $(SYSROOT)/include
	mkdir -p $(SYSROOT)/lib
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/heatshrink-0.4.1.tar.gz && \
	cd heatshrink-0.4.1 && \
	$(MAKE) CC=$(CROSS_COMPILE)gcc && \
	$(MAKE) install PREFIX=$(SYSROOT)
