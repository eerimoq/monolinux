LIBS += curl
LIBCURL = $(BUILD)/root/lib/libcurl.a

packages-rsync: curl-rsync

packages-build: curl-build

curl-all:
	$(MAKE) curl-rsync
	$(MAKE) curl-build

curl-build: $(CURL_BUILD)

curl-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/curl $(PACKAGES))" ] ; then \
	    echo "curl sources updated." && \
	    touch $(CURL_RSYNC) ; \
	fi

$(CURL_BUILD): $(MBEDTLS_BUILD)
$(CURL_BUILD): $(ZLIB_BUILD)
$(CURL_BUILD): $(CURL_RSYNC)
	echo "Building curl."
	cd $(PACKAGES)/curl && \
	./buildconf && \
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
	$(MAKE) -C $(PACKAGES)/curl/lib
	$(MAKE) -C $(PACKAGES)/curl/lib install
	$(MAKE) -C $(PACKAGES)/curl/include install
	touch $@

curl-clean:
	rm -rf $(PACKAGES)/curl $(LIBCURL)
