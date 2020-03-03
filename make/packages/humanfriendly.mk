LIBS += hf
LIBHF = $(BUILD)/root/lib/humanfriendly/libhf.a

packages-rsync: humanfriendly-rsync

packages-build: humanfriendly-build

humanfriendly-all:
	$(MAKE) humanfriendly-rsync
	$(MAKE) humanfriendly-build

humanfriendly-build: $(HUMANFRIENDLY_BUILD)

humanfriendly-rsync:
	mkdir -p $(PACKAGES)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/humanfriendly $(PACKAGES))" ] ; then \
	    echo "humanfriendly sources updated." && \
	    touch $(HUMANFRIENDLY_RSYNC) ; \
	fi

$(HUMANFRIENDLY_BUILD): $(HUMANFRIENDLY_RSYNC)
	echo "Building humanfriendly."
	mkdir -p $(SYSROOT)/lib
	$(MAKE) -C $(PACKAGES)/humanfriendly library
	$(MAKE) -C $(PACKAGES)/humanfriendly install PREFIX=$(SYSROOT)
	touch $@

humanfriendly-clean:
	rm -rf $(PACKAGES)/humanfriendly $(LIBHF)
