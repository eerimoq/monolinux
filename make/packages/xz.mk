LIBS += lzma
LIBXZ = $(BUILD)/root/lib/liblzma.a

$(eval $(call PACKAGE_template,xz,$(XZ_RSYNC),$(XZ_BUILD),$(LIBXZ)))

$(XZ_BUILD): $(DBG_MACRO_BUILD)
$(XZ_BUILD): $(XZ_RSYNC)
	echo "Building xz."
	mkdir -p $(PACKAGES_DIR)
	cd $(PACKAGES_DIR)/xz && \
	./autogen.sh && \
	./configure \
	    CFLAGS="-ffunction-sections -fdata-sections -O2" \
	    --prefix=$(SYSROOT) \
	    --host=$(ML_AUTOTOOLS_HOST) --build=i586-pc-linux-gnu \
	    --disable-xz --disable-xzdec --disable-lzmadec \
	    --disable-lzmainfo --disable-lzma-links --disable-scripts \
	    --disable-doc --disable-shared --disable-largefile
	$(MAKE) -C $(PACKAGES_DIR)/xz
	$(MAKE) -C $(PACKAGES_DIR)/xz install
	touch $@
