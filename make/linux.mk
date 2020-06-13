LINUX_SRC = $(BUILD)/linux

linux-all:
	$(MAKE) linux-rsync
	$(MAKE) linux-build

linux-build: $(LINUX_BUILD)

linux-rsync:
	mkdir -p $(BUILD)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/linux $(BUILD))" ] ; then \
	    echo "linux sources updated." && \
	    touch $(LINUX_RSYNC) ; \
	fi


$(LINUX_BUILD): $(ML_LINUX_CONFIG)
$(LINUX_BUILD): $(LINUX_RSYNC)
	echo "Building the Linux kernel."
	cp $(ML_LINUX_CONFIG) $(LINUX_SRC)/.config
	$(MAKE) -C $(LINUX_SRC)
ifeq ($(STRIP), yes)
	for ko in $(LINUX_MODULE_FILES) ; do \
	    $(CROSS_COMPILE)strip --strip-debug $$ko ; \
	done
endif
	touch $@

linux-clean:
	rm -rf $(LINUX_SRC)

linux-menuconfig:
	$(MAKE) -C $(LINUX_SRC) menuconfig
	cp $(LINUX_SRC)/.config $(ML_LINUX_CONFIG)

linux-nconfig:
	$(MAKE) -C $(LINUX_SRC) nconfig
	cp $(LINUX_SRC)/.config $(ML_LINUX_CONFIG)
