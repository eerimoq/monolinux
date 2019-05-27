LIBS += lzma

packages: $(PACKAGES)/xz-5.2.4

$(PACKAGES)/xz-5.2.4: $(ML_SOURCES)/xz-5.2.4.tar.gz
	@echo "Building xz-5.2.4."
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/xz-5.2.4.tar.gz && \
	cd xz-5.2.4 && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections -O2" \
	    --prefix=$(BUILD)/root $(ML_AUTOTOOLS_HOST) \
	    --disable-xz --disable-xzdec --disable-lzmadec \
	    --disable-lzmainfo --disable-lzma-links --disable-scripts \
	    --disable-doc --disable-shared --disable-largefile && \
	$(MAKE) && \
	$(MAKE) install
