LIBS += curl
LIBCURL = $(BUILD)/root/lib/libcurl.a

$(eval $(call PACKAGE_template,curl,$(CURL_RSYNC),$(CURL_BUILD),$(LIBCURL)))

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
