LIBS += lzma

packages: $(PACKAGES)/xz-5.2.4

$(ML_SOURCES)/xz-5.2.4.tar.xz:
	wget -O $@ https://tukaani.org/xz/xz-5.2.4.tar.xz

$(PACKAGES)/xz-5.2.4: $(ML_SOURCES)/xz-5.2.4.tar.xz
	@echo "Building xz-5.2.4."
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/xz-5.2.4.tar.xz && \
	cd xz-5.2.4 && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections -O2" \
	    --prefix=$(SYSROOT) \
	    --host=$(ML_AUTOTOOLS_HOST) --build=i586-pc-linux-gnu \
	    --disable-xz --disable-xzdec --disable-lzmadec \
	    --disable-lzmainfo --disable-lzma-links --disable-scripts \
	    --disable-doc --disable-shared --disable-largefile && \
	$(MAKE) && \
	$(MAKE) install
