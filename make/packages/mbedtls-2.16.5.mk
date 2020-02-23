LIBS += mbedtls mbedx509 mbedcrypto
MBEDTLS = mbedtls-2.16.5

packages: $(PACKAGES)/$(MBEDTLS)

$(PACKAGES)/$(MBEDTLS): $(ML_SOURCES)/$(MBEDTLS).tar.gz
	$(MAKE) $(MBEDTLS)-all

$(MBEDTLS)-all:
	@echo "Building $(MBEDTLS)."
	$(MAKE) $(MBEDTLS)-unpack
	$(MAKE) $(MBEDTLS)-configure
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

# Programs are not wanted.
$(MBEDTLS)-configure:
	sed -i "s|install: no_test|install:|g" $(PACKAGES)/$(MBEDTLS)/Makefile

$(MBEDTLS)-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch $(MBEDTLS) && \
	cd $(MBEDTLS) && \
	$(MAKE) lib CC=$(CROSS_COMPILE)gcc && \
	$(MAKE) install DESTDIR=$(SYSROOT)
