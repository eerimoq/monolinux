LIBS += z
LIBZLIB = $(BUILD)/root/lib/libz.a

packages-rsync: zlib-rsync

packages-build: zlib-build

zlib-all:
	$(MAKE) zlib-rsync
	$(MAKE) zlib-build

zlib-build: $(ZLIB_BUILD)

zlib-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/zlib $(PACKAGES))" ] ; then \
	    echo "zlib sources updated." && \
	    touch $(ZLIB_RSYNC) ; \
	fi

$(ZLIB_BUILD): $(ZLIB_RSYNC)
	echo "Building zlib."
	cd $(PACKAGES)/zlib && \
	CHOST=$(ML_AUTOTOOLS_HOST) ./configure --prefix=$(SYSROOT) --static
	$(MAKE) -C $(PACKAGES)/zlib
	$(MAKE) -C $(PACKAGES)/zlib install
	touch $@

zlib-clean:
	rm -rf $(PACKAGES)/zlib $(LIBZLIB)
