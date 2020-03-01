LIBS += hf
LIBHF = $(BUILD)/root/lib/humanfriendly/libhf.a

packages: $(PACKAGES)/humanfriendly

$(PACKAGES)/humanfriendly: $(LIBHF)

$(LIBHF): humanfriendly-all

humanfriendly-all:
	mkdir -p $(PACKAGES) $(SYSROOT)/lib
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/humanfriendly $(PACKAGES))" ] ; then \
	    echo "Building humanfriendly." && \
	    $(MAKE) -C $(PACKAGES)/humanfriendly library && \
	    $(MAKE) -C $(PACKAGES)/humanfriendly install PREFIX=$(SYSROOT) ; \
	fi

humanfriendly-clean:
	rm -rf $(PACKAGES)/humanfriendly $(LIBHF)
