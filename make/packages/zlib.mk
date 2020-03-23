LIBS += z
LIBZLIB = $(BUILD)/root/lib/libz.a

$(eval $(call PACKAGE_template,zlib,$(ZLIB_RSYNC),$(ZLIB_BUILD),$(LIBZLIB)))

$(ZLIB_BUILD): $(DBG_MACRO_BUILD)
$(ZLIB_BUILD): $(ZLIB_RSYNC)
	echo "Building zlib."
	cd $(PACKAGES_DIR)/zlib && \
	CHOST=$(ML_AUTOTOOLS_HOST) ./configure --prefix=$(SYSROOT) --static
	$(MAKE) -C $(PACKAGES_DIR)/zlib
	$(MAKE) -C $(PACKAGES_DIR)/zlib install
	touch $@
