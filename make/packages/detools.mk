LIBS += detools
DETOOLS_TAR_GZ = $(ML_SOURCES)/detools-0.45.0.tar.gz

packages: $(PACKAGES)/detools

$(PACKAGES)/detools: $(PACKAGES)/xz
$(PACKAGES)/detools: $(PACKAGES)/heatshrink
$(PACKAGES)/detools:
	$(MAKE) detools-all

detools-all:
	@echo "Building detools."
	$(MAKE) detools-fetch
	$(MAKE) detools-unpack
	$(MAKE) detools-build

detools-clean:
	cd $(PACKAGES)/detools

detools-fetch: $(DETOOLS_TAR_GZ)

$(DETOOLS_TAR_GZ):
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/detools/archive/0.45.0.tar.gz

detools-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(DETOOLS_TAR_GZ) && \
	mv detools-0.45.0 detools

detools-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch detools && \
	cd detools && \
	$(MAKE) -C src/c library CFLAGS_EXTRA=-I$(SYSROOT)/include && \
	$(MAKE) -C src/c install PREFIX=$(SYSROOT)
