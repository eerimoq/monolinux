LIBS += curl

packages: $(PACKAGES)/curl-7.65.0

ifeq ($(ARCH), arm)
CURL_CPPFLAGS = CPPFLAGS="-I$(BUILD)/root/include"
CURL_LDFLAGS = LDFLAGS="-L$(BUILD)/root/lib"
endif

$(PACKAGES)/curl-7.65.0: $(PACKAGES)/openssl-1.1.1b
$(PACKAGES)/curl-7.65.0: $(ML_SOURCES)/curl-7.65.0.tar.xz
	@echo "Building curl-7.65.0."
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/curl-7.65.0.tar.xz && \
	cd curl-7.65.0 && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections" \
	    $(CURL_CPPFLAGS) \
	    $(CURL_LDFLAGS) \
	    --prefix=$(BUILD)/root $(ML_AUTOTOOLS_HOST) \
	    --with-ssl --without-zlib --disable-shared --disable-manual \
	    --disable-ftp --disable-ldap --disable-telnet --disable-dict \
	    --disable-file --disable-tftp --disable-imap --disable-pop3 \
	    --disable-smtp --disable-rtsp --disable-gopher && \
	$(MAKE) && \
	$(MAKE) install
