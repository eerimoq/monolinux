LIBS += lzma
XZ = xz-5.2.4

packages: $(PACKAGES)/$(XZ)

$(PACKAGES)/$(XZ): $(ML_SOURCES)/$(XZ).tar.xz
	$(MAKE) $(XZ)-all

$(XZ)-all:
	@echo "Building $(XZ)."
	$(MAKE) $(XZ)-unpack
	$(MAKE) $(XZ)-configure
	$(MAKE) $(XZ)-build

$(XZ)-clean:
	rm -rf $(PACKAGES)/$(XZ)

$(ML_SOURCES)/$(XZ).tar.xz:
	mkdir -p $(dir $@)
	wget -O $@ https://tukaani.org/xz/$(XZ).tar.xz

$(XZ)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(XZ).tar.xz

$(XZ)-configure:
	cd $(PACKAGES)/$(XZ) && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections -O2" \
	    --prefix=$(SYSROOT) \
	    --host=$(ML_AUTOTOOLS_HOST) --build=i586-pc-linux-gnu \
	    --disable-xz --disable-xzdec --disable-lzmadec \
	    --disable-lzmainfo --disable-lzma-links --disable-scripts \
	    --disable-doc --disable-shared --disable-largefile

$(XZ)-build:
	cd $(PACKAGES)/$(XZ) && \
	$(MAKE) && \
	$(MAKE) install
