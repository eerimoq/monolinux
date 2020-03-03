LIBS += lzma
LIBXZ = $(BUILD)/root/lib/liblzma.a

packages-rsync: xz-rsync

packages-build: xz-build

xz-all:
	$(MAKE) xz-rsync
	$(MAKE) xz-build

xz-build: $(XZ_BUILD)

xz-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/xz $(PACKAGES))" ] ; then \
	    echo "xz sources updated." && \
	    touch $(XZ_RSYNC) ; \
	fi

$(XZ_BUILD): $(XZ_RSYNC)
	echo "Building xz."
	mkdir -p $(PACKAGES)
	cd $(PACKAGES)/xz && \
	./autogen.sh && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections -O2" \
	    --prefix=$(SYSROOT) \
	    --host=$(ML_AUTOTOOLS_HOST) --build=i586-pc-linux-gnu \
	    --disable-xz --disable-xzdec --disable-lzmadec \
	    --disable-lzmainfo --disable-lzma-links --disable-scripts \
	    --disable-doc --disable-shared --disable-largefile
	$(MAKE) -C $(PACKAGES)/xz
	$(MAKE) -C $(PACKAGES)/xz install
	touch $@

xz-clean:
	rm -rf $(PACKAGES)/xz $(LIBXZ)
