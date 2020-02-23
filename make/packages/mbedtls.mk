LIBS += mbedtls mbedx509 mbedcrypto
MBEDTLS_TAR_GZ = $(ML_SOURCES)/mbedtls-2.16.5.tar.gz

packages: $(PACKAGES)/mbedtls

$(PACKAGES)/mbedtls:
	$(MAKE) mbedtls-all

mbedtls-all:
	@echo "Building mbedtls."
	$(MAKE) mbedtls-fetch
	$(MAKE) mbedtls-unpack
	$(MAKE) mbedtls-configure
	$(MAKE) mbedtls-build

mbedtls-clean:
	cd $(PACKAGES)/mbedtls

mbedtls-fetch: $(MBEDTLS_TAR_GZ)

$(MBEDTLS_TAR_GZ):
	mkdir -p $(dir $@)
	wget -O $@ https://tls.mbed.org/download/mbedtls-2.16.5-apache.tgz

mbedtls-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(MBEDTLS_TAR_GZ) && \
	mv mbedtls-2.16.5 mbedtls

# Programs are not wanted.
mbedtls-configure:
	sed -i "s|install: no_test|install:|g" $(PACKAGES)/mbedtls/Makefile

mbedtls-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch mbedtls && \
	cd mbedtls && \
	$(MAKE) lib CC=$(CROSS_COMPILE)gcc && \
	$(MAKE) install DESTDIR=$(SYSROOT)
