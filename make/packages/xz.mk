LIBS += lzma
XZ_TAR_XZ = $(ML_SOURCES)/xz-5.2.4.tar.xz

packages: $(PACKAGES)/xz

$(PACKAGES)/xz:
	$(MAKE) xz-all

xz-all:
	@echo "Building xz."
	$(MAKE) xz-fetch
	$(MAKE) xz-unpack
	$(MAKE) xz-configure
	$(MAKE) xz-build

xz-clean:
	rm -rf $(PACKAGES)/xz

xz-fetch: $(XZ_TAR_XZ)

$(XZ_TAR_XZ):
	mkdir -p $(dir $@)
	wget -O $@ https://tukaani.org/xz/xz-5.2.4.tar.xz

xz-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(XZ_TAR_XZ) && \
	mv xz-5.2.4 xz

xz-configure:
	cd $(PACKAGES)/xz && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections -O2" \
	    --prefix=$(SYSROOT) \
	    --host=$(ML_AUTOTOOLS_HOST) --build=i586-pc-linux-gnu \
	    --disable-xz --disable-xzdec --disable-lzmadec \
	    --disable-lzmainfo --disable-lzma-links --disable-scripts \
	    --disable-doc --disable-shared --disable-largefile

xz-build:
	cd $(PACKAGES)/xz && \
	$(MAKE) && \
	$(MAKE) install
