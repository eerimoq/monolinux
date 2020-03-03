LIBS += mbedtls mbedx509 mbedcrypto
LIBMBEDTLS = $(BUILD)/root/lib/libmbedtls.a
LIBMBEDTLS += $(BUILD)/root/lib/libmbedx509.a
LIBMBEDTLS += $(BUILD)/root/lib/libmbedcrypto.a

$(eval $(call PACKAGE_template,mbedtls,$(MBEDTLS_RSYNC),$(MBEDTLS_BUILD),$(LIBMBEDTLS)))

$(MBEDTLS_BUILD): $(MBEDTLS_RSYNC)
	echo "Building mbedtls."
	mkdir -p $(SYSROOT)/lib
	sed -i "s|install: no_test|install:|g" $(PACKAGES)/mbedtls/Makefile
	$(MAKE) -C $(PACKAGES)/mbedtls lib CC=$(CROSS_COMPILE)gcc
	$(MAKE) -C $(PACKAGES)/mbedtls install DESTDIR=$(SYSROOT)
	touch $@
