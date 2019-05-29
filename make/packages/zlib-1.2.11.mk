LIBS += z

packages: $(PACKAGES)/zlib-1.2.11

$(PACKAGES)/zlib-1.2.11: $(ML_SOURCES)/zlib-1.2.11.tar.xz
	$(MAKE) zlib-1.2.11-all

zlib-1.2.11-all:
	@echo "Building zlib-1.2.11."
	$(MAKE) zlib-1.2.11-unpack
	$(MAKE) zlib-1.2.11-configure
	$(MAKE) zlib-1.2.11-build

zlib-1.2.11-clean:
	rm -rf $(PACKAGES)/zlib-1.2.11

$(ML_SOURCES)/zlib-1.2.11.tar.xz:
	wget -O $@ https://zlib.net/zlib-1.2.11.tar.xz

zlib-1.2.11-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/zlib-1.2.11.tar.xz

zlib-1.2.11-configure:
	cd $(PACKAGES)/zlib-1.2.11 && \
	CHOST=$(ML_AUTOTOOLS_HOST) ./configure \
	    --prefix=$(SYSROOT) --static

zlib-1.2.11-build:
	cd $(PACKAGES)/zlib-1.2.11 && \
	$(MAKE) && \
	$(MAKE) install
