LIBS += curl

packages: $(PACKAGES)/curl-7.65.0

$(PACKAGES)/curl-7.65.0: $(PACKAGES)/openssl-1.1.1b
$(PACKAGES)/curl-7.65.0: $(PACKAGES)/zlib-1.2.11
$(PACKAGES)/curl-7.65.0: $(ML_SOURCES)/curl-7.65.0.tar.xz
	$(MAKE) curl-7.65.0-all

curl-7.65.0-all:
	@echo "Building curl-7.65.0."
	$(MAKE) curl-7.65.0-unpack
	$(MAKE) curl-7.65.0-configure
	$(MAKE) curl-7.65.0-build

curl-7.65.0-clean:
	rm -rf $(PACKAGES)/curl-7.65.0

$(ML_SOURCES)/curl-7.65.0.tar.xz:
	wget -O $@ https://curl.haxx.se/download/curl-7.65.0.tar.xz

curl-7.65.0-unpack:
	mkdir -p $(PACKAGES)/curl-7.65.0
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/curl-7.65.0.tar.xz

curl-7.65.0-configure:
	cd $(PACKAGES)/curl-7.65.0 && \
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
	    --with-random=/dev/urandom --enable-curldebug

curl-7.65.0-build:
	cd $(PACKAGES)/curl-7.65.0 && \
	$(MAKE) && \
	$(MAKE) install
