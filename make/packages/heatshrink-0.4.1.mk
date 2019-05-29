LIBS += heatshrink_dynamic
HEATSHRINK = heatshrink-0.4.1

packages: $(PACKAGES)/$(HEATSHRINK)

$(PACKAGES)/$(HEATSHRINK): $(ML_SOURCES)/$(HEATSHRINK).tar.gz
	$(MAKE) $(HEATSHRINK)-all

$(HEATSHRINK)-all:
	@echo "Building $(HEATSHRINK)."
	$(MAKE) $(HEATSHRINK)-unpack
	$(MAKE) $(HEATSHRINK)-build

$(HEATSHRINK)-clean:
	rm -rf $(PACKAGES)/$(HEATSHRINK)

$(ML_SOURCES)/$(HEATSHRINK).tar.gz:
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/atomicobject/heatshrink/archive/v0.4.1.tar.gz

$(HEATSHRINK)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(HEATSHRINK).tar.gz

$(HEATSHRINK)-build:
	mkdir -p $(SYSROOT)/bin
	mkdir -p $(SYSROOT)/include
	mkdir -p $(SYSROOT)/lib
	cd $(PACKAGES)/$(HEATSHRINK) && \
	$(MAKE) CC=$(CROSS_COMPILE)gcc && \
	$(MAKE) install PREFIX=$(SYSROOT)
