LIBS += detools

packages: $(PACKAGES)/detools-0.29.0

$(PACKAGES)/detools-0.29.0: $(PACKAGES)/xz-5.2.4
$(PACKAGES)/detools-0.29.0: $(PACKAGES)/heatshrink-0.4.1
$(PACKAGES)/detools-0.29.0: $(ML_SOURCES)/detools-0.29.0.tar.gz
	@echo "Building detools-0.29.0."
	mkdir -p $(SYSROOT)
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/detools-0.29.0.tar.gz && \
	touch detools-0.29.0 && \
	cd detools-0.29.0 && \
	$(MAKE) -C src/c library CFLAGS_EXTRA=-I$(SYSROOT)/include && \
	$(MAKE) -C src/c install PREFIX=$(SYSROOT)
