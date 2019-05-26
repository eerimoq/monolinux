LIBS += curl

packages: $(PACKAGES)/curl-7.65.0

$(PACKAGES)/curl-7.65.0: $(ML_SOURCES)/curl-7.65.0.tar.xz
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/curl-7.65.0.tar.xz && \
	cd curl-7.65.0 && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections" \
	    --prefix=$(BUILD)/root $(ML_AUTOTOOLS_HOST) \
	    --without-ssl --without-zlib --disable-shared --disable-manual \
	    --disable-ftp --disable-ldap --disable-telnet --disable-dict \
	    --disable-file --disable-tftp --disable-imap --disable-pop3 \
	    --disable-smtp --disable-rtsp --disable-gopher && \
	$(MAKE) && \
	$(MAKE) install
