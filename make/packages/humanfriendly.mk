LIBS += hf
LIBHF = $(BUILD)/root/lib/humanfriendly/libhf.a

$(eval $(call PACKAGE_template,humanfriendly,$(HUMANFRIENDLY_RSYNC),$(HUMANFRIENDLY_BUILD),$(LIBHF)))

$(HUMANFRIENDLY_BUILD): $(DBG_MACRO_BUILD)
$(HUMANFRIENDLY_BUILD): $(HUMANFRIENDLY_RSYNC)
	echo "Building humanfriendly."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/humanfriendly library
	$(MAKE) -C $(PACKAGES_DIR)/humanfriendly install PREFIX=$(SYSROOT)
	touch $@
