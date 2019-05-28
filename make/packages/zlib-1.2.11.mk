LIBS += z

packages: $(PACKAGES)/zlib-1.2.11

$(PACKAGES)/zlib-1.2.11: $(ML_SOURCES)/zlib-1.2.11.tar.gz
	@echo "Building zlib-1.2.11."
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/zlib-1.2.11.tar.gz && \
	cd zlib-1.2.11 && \
	CHOST=$(ML_AUTOTOOLS_HOST) ./configure \
	    --prefix=$(SYSROOT) --static && \
	$(MAKE) && \
	$(MAKE) install
