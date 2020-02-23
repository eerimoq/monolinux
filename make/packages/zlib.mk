LIBS += z
ZLIB_TAR_XZ = $(ML_SOURCES)/zlib-1.2.11.tar.xz

packages: $(PACKAGES)/zlib

$(PACKAGES)/zlib:
	$(MAKE) zlib-all

zlib-all:
	@echo "Building zlib."
	$(MAKE) zlib-fetch
	$(MAKE) zlib-unpack
	$(MAKE) zlib-configure
	$(MAKE) zlib-build

zlib-fetch: $(ZLIB_TAR_XZ)

zlib-clean:
	rm -rf $(PACKAGES)/zlib

$(ZLIB_TAR_XZ):
	mkdir -p $(dir $@)
	wget -O $@ https://zlib.net/zlib-1.2.11.tar.xz

zlib-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ZLIB_TAR_XZ) && \
	mv zlib-1.2.11 zlib

zlib-configure:
	cd $(PACKAGES)/zlib && \
	CHOST=$(ML_AUTOTOOLS_HOST) ./configure \
	    --prefix=$(SYSROOT) --static

zlib-build:
	cd $(PACKAGES)/zlib && \
	$(MAKE) && \
	$(MAKE) install
