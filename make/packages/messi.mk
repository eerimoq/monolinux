LIBS += messi
LIBMESSI = $(BUILD)/root/lib/libmessi.a

$(eval $(call PACKAGE_template,messi,$(MESSI_RSYNC),$(MESSI_BUILD),$(LIBMESSI)))

$(MESSI_BUILD): $(MESSI_RSYNC)
	echo "Building messi."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/messi/lib library
	$(MAKE) -C $(PACKAGES_DIR)/messi/lib install PREFIX=$(SYSROOT)
	touch $@
