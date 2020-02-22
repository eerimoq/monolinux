LIBS += mbedtls mbedx509 mbedcrypto
MBEDTLS = mbedtls-2.16.5

packages: $(PACKAGES)/$(MBEDTLS)

$(PACKAGES)/$(MBEDTLS): $(ML_SOURCES)/$(MBEDTLS).tar.gz
	$(MAKE) $(MBEDTLS)-all

$(MBEDTLS)-all:
	@echo "Building $(MBEDTLS)."
	$(MAKE) $(MBEDTLS)-unpack
	$(MAKE) $(MBEDTLS)-build

$(MBEDTLS)-clean:
	cd $(PACKAGES)/$(MBEDTLS)

$(ML_SOURCES)/$(MBEDTLS).tar.gz:
	mkdir -p $(dir $@)
	wget -O $@ https://tls.mbed.org/download/mbedtls-2.16.5-apache.tgz

$(MBEDTLS)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(MBEDTLS).tar.gz

$(MBEDTLS)-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch $(MBEDTLS) && \
	cd $(MBEDTLS) && \
	$(MAKE) install DESTDIR=$(SYSROOT)
