LIBS += z
LIBZLIB = $(BUILD)/root/lib/libz.a

.PHONY: $(PACKAGES)/zlib

packages: $(PACKAGES)/zlib

$(PACKAGES)/zlib: $(LIBZLIB)

$(LIBZLIB): zlib-all

zlib-all:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_ROOT)/3pp/zlib $(PACKAGES))" ] ; then \
	    echo "Building zlib." ; \
	    cd $(PACKAGES)/zlib && \
	    CHOST=$(ML_AUTOTOOLS_HOST) ./configure \
		--prefix=$(SYSROOT) --static && \
	    $(MAKE) && \
	    $(MAKE) install ; \
	fi

zlib-clean:
	rm -rf $(PACKAGES)/zlib $(LIBZLIB)

zlib-build:
	echo "Building zlib."
	mkdir -p $(PACKAGES)
	rsync -ariOu $(ML_ROOT)/3pp/zlib $(PACKAGES)
	cd $(PACKAGES)/zlib && \
	$(MAKE) && \
	$(MAKE) install
