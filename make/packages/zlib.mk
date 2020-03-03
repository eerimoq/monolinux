LIBS += z
LIBZLIB = $(BUILD)/root/lib/libz.a

$(eval $(call PACKAGE_template,zlib,$(ZLIB_RSYNC),$(ZLIB_BUILD),$(LIBZLIB)))

$(ZLIB_BUILD): $(ZLIB_RSYNC)
	echo "Building zlib."
	cd $(PACKAGES)/zlib && \
	CHOST=$(ML_AUTOTOOLS_HOST) ./configure --prefix=$(SYSROOT) --static
	$(MAKE) -C $(PACKAGES)/zlib
	$(MAKE) -C $(PACKAGES)/zlib install
	touch $@
