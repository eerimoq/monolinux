LIBS += lzma
LIBXZ = $(BUILD)/root/lib/liblzma.a

.PHONY: $(PACKAGES)/xz

packages: $(PACKAGES)/xz

$(PACKAGES)/xz: $(LIBXZ)

$(LIBXZ): xz-all

xz-all:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_ROOT)/3pp/xz $(PACKAGES))" ] ; then \
	    echo "Building xz." && \
	    cd $(PACKAGES)/xz && \
	    ./autogen.sh && \
	    ./configure \
	        CFLAGS="-ffunction-sections -fdata-sections -O2" \
	        --prefix=$(SYSROOT) \
	        --host=$(ML_AUTOTOOLS_HOST) --build=i586-pc-linux-gnu \
	        --disable-xz --disable-xzdec --disable-lzmadec \
	        --disable-lzmainfo --disable-lzma-links --disable-scripts \
	        --disable-doc --disable-shared --disable-largefile && \
	    $(MAKE) && \
	    $(MAKE) install ; \
	fi

xz-clean:
	rm -rf $(PACKAGES)/xz $(LIBXZ)

xz-build:
	echo "Building xz."
	mkdir -p $(PACKAGES)
	rsync -ariOu $(ML_ROOT)/3pp/xz $(PACKAGES)
	cd $(PACKAGES)/xz && \
	$(MAKE) && \
	$(MAKE) install
