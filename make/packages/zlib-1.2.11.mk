LIBS += z

packages: $(PACKAGES)/zlib-1.2.11

$(ML_SOURCES)/zlib-1.2.11.tar.xz:
	wget -O $@ https://zlib.net/zlib-1.2.11.tar.xz

$(PACKAGES)/zlib-1.2.11: $(ML_SOURCES)/zlib-1.2.11.tar.xz
	@echo "Building zlib-1.2.11."
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/zlib-1.2.11.tar.xz && \
	cd zlib-1.2.11 && \
	CHOST=$(ML_AUTOTOOLS_HOST) ./configure \
	    --prefix=$(SYSROOT) --static && \
	$(MAKE) && \
	$(MAKE) install
