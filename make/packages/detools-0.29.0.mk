LIBS += detools

packages: $(PACKAGES)/detools-0.29.0

$(PACKAGES)/detools-0.29.0: $(PACKAGES)/xz-5.2.4
$(PACKAGES)/detools-0.29.0: $(PACKAGES)/heatshrink-0.4.1
$(PACKAGES)/detools-0.29.0: $(ML_SOURCES)/detools-0.29.0.tar.gz
	$(MAKE) detools-0.29.0-all

detools-0.29.0-all:
	@echo "Building detools-0.29.0."
	$(MAKE) detools-0.29.0-unpack
	$(MAKE) detools-0.29.0-build

detools-0.29.0-clean:
	cd $(PACKAGES)/detools-0.29.0

$(ML_SOURCES)/detools-0.29.0.tar.gz:
	wget -O $@ https://github.com/eerimoq/detools/archive/0.29.0.tar.gz

detools-0.29.0-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/detools-0.29.0.tar.gz

detools-0.29.0-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch detools-0.29.0 && \
	cd detools-0.29.0 && \
	$(MAKE) -C src/c library CFLAGS_EXTRA=-I$(SYSROOT)/include && \
	$(MAKE) -C src/c install PREFIX=$(SYSROOT)
