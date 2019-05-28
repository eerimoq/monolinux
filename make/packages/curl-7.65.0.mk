LIBS += curl

packages: $(PACKAGES)/curl-7.65.0

$(ML_SOURCES)/curl-7.65.0.tar.xz:
	wget -O $@ https://curl.haxx.se/download/curl-7.65.0.tar.xz

$(PACKAGES)/curl-7.65.0: $(PACKAGES)/openssl-1.1.1b
$(PACKAGES)/curl-7.65.0: $(PACKAGES)/zlib-1.2.11
$(PACKAGES)/curl-7.65.0: $(ML_SOURCES)/curl-7.65.0.tar.xz
	@echo "Building curl-7.65.0."
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/curl-7.65.0.tar.xz && \
	cd curl-7.65.0 && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections" \
	    CPPFLAGS="-I$(SYSROOT)/include" \
	    LDFLAGS="-L$(SYSROOT)/lib" \
	    --prefix=$(SYSROOT) \
	    --host=$(ML_AUTOTOOLS_HOST) --build=i586-pc-linux-gnu \
	    --with-ssl --with-zlib --disable-shared --disable-manual \
	    --disable-ftp --disable-ldap --disable-telnet --disable-dict \
	    --disable-file --disable-tftp --disable-imap --disable-pop3 \
	    --disable-smtp --disable-rtsp --disable-gopher \
	    --disable-verbose --disable-curldebug \
	    --with-random=/dev/urandom && \
	$(MAKE) && \
	$(MAKE) install
