LIBS += mbedtls mbedx509 mbedcrypto
LIBMBEDTLS = $(BUILD)/root/lib/libmbedtls.a
LIBMBEDTLS += $(BUILD)/root/lib/libmbedx509.a
LIBMBEDTLS += $(BUILD)/root/lib/libmbedcrypto.a

.PHONY: $(PACKAGES)/mbedtls

packages: $(PACKAGES)/mbedtls

$(PACKAGES)/mbedtls: $(LIBMBEDTLS)

$(LIBMBEDTLS): mbedtls-all

mbedtls-all:
	mkdir -p $(PACKAGES) $(SYSROOT)/lib
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/mbedtls $(PACKAGES))" ] ; then \
	    echo "Building mbedtls." && \
	    sed -i "s|install: no_test|install:|g" $(PACKAGES)/mbedtls/Makefile && \
	    $(MAKE) -C $(PACKAGES)/mbedtls lib CC=$(CROSS_COMPILE)gcc && \
	    $(MAKE) -C $(PACKAGES)/mbedtls install DESTDIR=$(SYSROOT) ; \
	fi

mbedtls-clean:
	rm -rf $(PACKAGES)/mbedtls $(LIBMBEDTLS)
