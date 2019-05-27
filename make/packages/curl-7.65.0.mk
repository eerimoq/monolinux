LIBS += curl

packages: $(PACKAGES)/curl-7.65.0

$(PACKAGES)/curl-7.65.0: $(PACKAGES)/openssl-1.1.1b
$(PACKAGES)/curl-7.65.0: $(ML_SOURCES)/curl-7.65.0.tar.xz
	@echo "Building curl-7.65.0."
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/curl-7.65.0.tar.xz && \
	cd curl-7.65.0 && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections -I$(BUILD)/root/include -L$(BUILD)/root/lib" \
	    --prefix=$(BUILD)/root $(ML_AUTOTOOLS_HOST) \
	    --with-ssl --without-zlib --disable-shared --disable-manual \
	    --disable-ftp --disable-ldap --disable-telnet --disable-dict \
	    --disable-file --disable-tftp --disable-imap --disable-pop3 \
	    --disable-smtp --disable-rtsp --disable-gopher && \
	$(MAKE) && \
	$(MAKE) install
