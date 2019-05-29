LIBS += z
ZLIB = zlib-1.2.11

packages: $(PACKAGES)/$(ZLIB)

$(PACKAGES)/$(ZLIB): $(ML_SOURCES)/$(ZLIB).tar.xz
	$(MAKE) $(ZLIB)-all

$(ZLIB)-all:
	@echo "Building $(ZLIB)."
	$(MAKE) $(ZLIB)-unpack
	$(MAKE) $(ZLIB)-configure
	$(MAKE) $(ZLIB)-build

$(ZLIB)-clean:
	rm -rf $(PACKAGES)/$(ZLIB)

$(ML_SOURCES)/$(ZLIB).tar.xz:
	mkdir -p $(dir $@)
	wget -O $@ https://zlib.net/$(ZLIB).tar.xz

$(ZLIB)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(ZLIB).tar.xz

$(ZLIB)-configure:
	cd $(PACKAGES)/$(ZLIB) && \
	CHOST=$(ML_AUTOTOOLS_HOST) ./configure \
	    --prefix=$(SYSROOT) --static

$(ZLIB)-build:
	cd $(PACKAGES)/$(ZLIB) && \
	$(MAKE) && \
	$(MAKE) install
