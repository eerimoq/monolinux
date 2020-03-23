$(eval $(call PACKAGE_template,dbg-macro,$(DBG_MACRO_RSYNC),$(DBG_MACRO_BUILD),))

$(DBG_MACRO_BUILD): $(DBG_MACRO_RSYNC)
	echo "Building dbg-macro."
	mkdir -p $(SYSROOT)/include
	cp $(PACKAGES_DIR)/dbg-macro/include/dbg.h $(SYSROOT)/include
	touch $@
