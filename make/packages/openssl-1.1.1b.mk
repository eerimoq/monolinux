# ToDo: getrandom() currently blocks forever in QEMU. Fix, and remove
# --with-rand-seed=devrandom.

LIBS += ssl crypto

packages: $(PACKAGES)/openssl-1.1.1b

$(ML_SOURCES)/openssl-1.1.1b.tar.gz:
	wget -O $@ https://www.openssl.org/source/openssl-1.1.1b.tar.gz

$(PACKAGES)/openssl-1.1.1b: $(ML_SOURCES)/openssl-1.1.1b.tar.gz
	@echo "Building openssl-1.1.1b."
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/openssl-1.1.1b.tar.gz && \
	cd openssl-1.1.1b && \
	./Configure \
	    linux-generic32 \
	    CFLAGS="-ffunction-sections -fdata-sections -O2" \
	    CPPFLAGS="-I$(SYSROOT)/include" \
	    LDFLAGS="-L$(SYSROOT)/lib" \
	    --prefix=$(SYSROOT) \
	    --with-rand-seed=devrandom \
	    no-tests no-fuzz-libfuzzer no-fuzz-afl no-shared no-pic && \
	sed -i "s| install_docs| # install_docs|g" Makefile
	$(MAKE) openssl-1.1.1b-build

openssl-1.1.1b-build:
	cd $(PACKAGES)/openssl-1.1.1b && \
	$(MAKE) && \
	$(MAKE) install
