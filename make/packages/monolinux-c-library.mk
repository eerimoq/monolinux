LIBS += ml
LIBML = $(BUILD)/root/lib/libml.a
MONOLINUX_C_LIBRARY_LOG_OBJECT_TXT ?= "\"/tmp/log_object.txt\""

ifeq ($(DEBUG),yes)
MONOLINUX_C_LIBRARY_CFLAGS_EXTRA += -g
endif

$(eval $(call PACKAGE_template,monolinux-c-library,$(MONOLINUX_C_LIBRARY_RSYNC),$(MONOLINUX_C_LIBRARY_BUILD),$(LIBML)))

$(MONOLINUX_C_LIBRARY_BUILD): $(DBG_MACRO_BUILD)
$(MONOLINUX_C_LIBRARY_BUILD): $(MONOLINUX_C_LIBRARY_RSYNC)
	echo "Building monolinux-c-library."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES_DIR)/monolinux-c-library library \
	    CC=$(CROSS_COMPILE)gcc ML_ROOT=. \
	    CFLAGS_EXTRA="$(MONOLINUX_C_LIBRARY_CFLAGS_EXTRA) -DML_LOG_OBJECT_TXT='$(MONOLINUX_C_LIBRARY_LOG_OBJECT_TXT)'"

	$(MAKE) -C $(PACKAGES_DIR)/monolinux-c-library install \
	    ML_ROOT=. PREFIX=$(SYSROOT)
	touch $@
