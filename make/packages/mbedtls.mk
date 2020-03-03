LIBS += mbedtls mbedx509 mbedcrypto
LIBMBEDTLS = $(BUILD)/root/lib/libmbedtls.a
LIBMBEDTLS += $(BUILD)/root/lib/libmbedx509.a
LIBMBEDTLS += $(BUILD)/root/lib/libmbedcrypto.a

packages-rsync: mbedtls-rsync

packages-build: mbedtls-build

mbedtls-all:
	$(MAKE) mbedtls-rsync
	$(MAKE) mbedtls-build

mbedtls-build: $(MBEDTLS_BUILD)

mbedtls-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/mbedtls $(PACKAGES))" ] ; then \
	    echo "mbedtls sources updated." && \
	    touch $(MBEDTLS_RSYNC) ; \
	fi

$(MBEDTLS_BUILD): $(MBEDTLS_RSYNC)
	echo "Building mbedtls."
	mkdir -p $(SYSROOT)/lib
	sed -i "s|install: no_test|install:|g" $(PACKAGES)/mbedtls/Makefile
	$(MAKE) -C $(PACKAGES)/mbedtls lib CC=$(CROSS_COMPILE)gcc
	$(MAKE) -C $(PACKAGES)/mbedtls install DESTDIR=$(SYSROOT)
	touch $@

mbedtls-clean:
	rm -rf $(PACKAGES)/mbedtls $(LIBMBEDTLS)
