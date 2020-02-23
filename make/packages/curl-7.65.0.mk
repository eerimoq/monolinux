LIBS += curl
CURL = curl-7.65.0

packages: $(PACKAGES)/$(CURL)

$(PACKAGES)/$(CURL): $(PACKAGES)/mbedtls-2.16.5
$(PACKAGES)/$(CURL): $(PACKAGES)/zlib-1.2.11
$(PACKAGES)/$(CURL): $(ML_SOURCES)/$(CURL).tar.xz
	$(MAKE) $(CURL)-all

$(CURL)-all:
	@echo "Building $(CURL)."
	$(MAKE) $(CURL)-unpack
	$(MAKE) $(CURL)-configure
	$(MAKE) $(CURL)-build

$(CURL)-clean:
	rm -rf $(PACKAGES)/$(CURL)

$(ML_SOURCES)/$(CURL).tar.xz:
	mkdir -p $(dir $@)
	wget -O $@ https://curl.haxx.se/download/$(CURL).tar.xz

$(CURL)-unpack:
	mkdir -p $(PACKAGES)/$(CURL)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(CURL).tar.xz

$(CURL)-configure:
	cd $(PACKAGES)/$(CURL) && \
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

$(CURL)-build:
	cd $(PACKAGES)/$(CURL) && \
	$(MAKE) && \
	$(MAKE) install
