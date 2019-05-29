# ToDo: getrandom() currently blocks forever in QEMU. Fix, and remove
# --with-rand-seed=devrandom.

LIBS += ssl crypto
OPENSSL = openssl-1.1.1b

packages: $(PACKAGES)/$(OPENSSL)

$(PACKAGES)/$(OPENSSL): $(ML_SOURCES)/$(OPENSSL).tar.gz
	$(MAKE) $(OPENSSL)-all

$(OPENSSL)-all:
	@echo "Building $(OPENSSL)."
	$(MAKE) $(OPENSSL)-unpack
	$(MAKE) $(OPENSSL)-configure
	$(MAKE) $(OPENSSL)-build

$(OPENSSL)-clean:
	rm -rf $(PACKAGES)/$(OPENSSL)

$(ML_SOURCES)/$(OPENSSL).tar.gz:
	mkdir -p $(dir $@)
	wget -O $@ https://www.openssl.org/source/$(OPENSSL).tar.gz

$(OPENSSL)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(OPENSSL).tar.gz

$(OPENSSL)-configure:
	cd $(PACKAGES)/$(OPENSSL) && \
	./Configure \
	    linux-generic32 \
	    CFLAGS="-ffunction-sections -fdata-sections -O2" \
	    CPPFLAGS="-I$(SYSROOT)/include" \
	    LDFLAGS="-L$(SYSROOT)/lib" \
	    --prefix=$(SYSROOT) \
	    --with-rand-seed=devrandom \
	    no-tests no-fuzz-libfuzzer no-fuzz-afl no-shared no-pic && \
	sed -i "s| install_docs| # install_docs|g" Makefile

$(OPENSSL)-build:
	cd $(PACKAGES)/$(OPENSSL) && \
	$(MAKE) && \
	$(MAKE) install
