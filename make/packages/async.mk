LIBS += async
LIBASYNC = $(BUILD)/root/lib/libasync.a

ifeq ($(DEBUG),yes)
ASYNC_CFLAGS_EXTRA += -g
endif

$(eval $(call PACKAGE_template,async,$(ASYNC_RSYNC),$(ASYNC_BUILD),$(LIBASYNC)))

$(ASYNC_BUILD): $(DBG_MACRO_BUILD)
$(ASYNC_BUILD): $(BITSTREAM_BUILD)
$(ASYNC_BUILD): $(HUMANFRIENDLY_BUILD)
$(ASYNC_BUILD): $(MONOLINUX_C_LIBRARY_BUILD)
$(ASYNC_BUILD): $(MBEDTLS_BUILD)
$(ASYNC_BUILD): $(ASYNC_RSYNC)
	echo "Building async."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/async library \
	    ASYNC_ROOT=. CFLAGS_EXTRA="-I$(SYSROOT)/include $(ASYNC_CFLAGS_EXTRA)"
	$(MAKE) -C $(PACKAGES_DIR)/async install PREFIX=$(SYSROOT)
	touch $@
