LIBS += curl
LIBCURL = $(BUILD)/root/lib/libcurl.a

.PHONY: $(PACKAGES)/curl

packages: $(PACKAGES)/curl

$(PACKAGES)/curl: $(LIBCURL)

$(LIBCURL): curl-all

curl-all: $(PACKAGES)/mbedtls
curl-all: $(PACKAGES)/zlib
curl-all:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_ROOT)/3pp/curl $(PACKAGES))" ] ; then \
	    echo "Building curl." ; \
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
		--with-random=/dev/urandom && \
	    $(MAKE) ; \
	    $(MAKE) install ; \
	fi

curl-clean:
	rm -rf $(PACKAGES)/curl $(LIBCURL)

curl-build:
	echo "Building curl."
	mkdir -p $(PACKAGES)
	rsync -ariOu $(ML_ROOT)/3pp/curl $(PACKAGES)
	cd $(PACKAGES)/curl && \
	    $(MAKE) ; \
	    $(MAKE) install ; \
