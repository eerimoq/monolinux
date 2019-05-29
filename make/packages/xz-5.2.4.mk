LIBS += lzma

packages: $(PACKAGES)/xz-5.2.4

$(PACKAGES)/xz-5.2.4: $(ML_SOURCES)/xz-5.2.4.tar.xz
	$(MAKE) xz-5.2.4-all

xz-5.2.4-all:
	@echo "Building xz-5.2.4."
	$(MAKE) xz-5.2.4-unpack
	$(MAKE) xz-5.2.4-configure
	$(MAKE) xz-5.2.4-build

xz-5.2.4-clean:
	rm -rf $(PACKAGES)/xz-5.2.4

$(ML_SOURCES)/xz-5.2.4.tar.xz:
	wget -O $@ https://tukaani.org/xz/xz-5.2.4.tar.xz

xz-5.2.4-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/xz-5.2.4.tar.xz

xz-5.2.4-configure:
	cd $(PACKAGES)/xz-5.2.4 && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections -O2" \
	    --prefix=$(SYSROOT) \
	    --host=$(ML_AUTOTOOLS_HOST) --build=i586-pc-linux-gnu \
	    --disable-xz --disable-xzdec --disable-lzmadec \
	    --disable-lzmainfo --disable-lzma-links --disable-scripts \
	    --disable-doc --disable-shared --disable-largefile

xz-5.2.4-build:
	cd $(PACKAGES)/xz-5.2.4 && \
	$(MAKE) && \
	$(MAKE) install
