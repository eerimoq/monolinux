LIBS += curl
CURL_TAR_XZ = $(ML_SOURCES)/curl-7.65.0.tar.xz

packages: $(PACKAGES)/curl

$(PACKAGES)/curl: $(PACKAGES)/mbedtls
$(PACKAGES)/curl: $(PACKAGES)/zlib
$(PACKAGES)/curl:
	$(MAKE) curl-all

curl-all:
	@echo "Building curl."
	$(MAKE) curl-fetch
	$(MAKE) curl-unpack
	$(MAKE) curl-configure
	$(MAKE) curl-build

curl-clean:
	rm -rf $(PACKAGES)/curl

curl-fetch: $(CURL_TAR_XZ)

$(CURL_TAR_XZ):
	mkdir -p $(dir $@)
	wget -O $@ https://curl.haxx.se/download/curl-7.65.0.tar.xz

curl-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(CURL_TAR_XZ) && \
	mv curl-7.65.0 curl

curl-configure:
	cd $(PACKAGES)/curl && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections" \
	    CPPFLAGS="-I$(SYSROOT)/include" \
	    LDFLAGS="-L$(SYSROOT)/lib" \
	    --prefix=$(SYSROOT) \
	    --host=$(ML_AUTOTOOLS_HOST) --build=i586-pc-linux-gnu \
	    --without-ssl --with-mbedtls \
	    --with-zlib --disable-shared --disable-manual \
	    --disable-ftp --disable-ldap --disable-telnet --disable-dict \
	    --disable-file --disable-tftp --disable-imap --disable-pop3 \
	    --disable-smtp --disable-rtsp --disable-gopher \
	    --with-random=/dev/urandom

curl-build:
	cd $(PACKAGES)/curl && \
	$(MAKE) && \
	$(MAKE) install
