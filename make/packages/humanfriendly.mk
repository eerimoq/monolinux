LIBS += hf
HUMANFRIENDLY_TAR_GZ = $(ML_SOURCES)/humanfriendly-0.3.0.tar.gz

packages: $(PACKAGES)/humanfriendly

$(PACKAGES)/humanfriendly:
	$(MAKE) humanfriendly-all

humanfriendly-all:
	@echo "Building humanfriendly."
	$(MAKE) humanfriendly-fetch
	$(MAKE) humanfriendly-unpack
	$(MAKE) humanfriendly-build

humanfriendly-clean:
	cd $(PACKAGES)/humanfriendly

humanfriendly-fetch: $(HUMANFRIENDLY_TAR_GZ)

$(HUMANFRIENDLY_TAR_GZ):
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/humanfriendly/archive/0.3.0.tar.gz

humanfriendly-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(HUMANFRIENDLY_TAR_GZ) && \
	mv humanfriendly-0.3.0 humanfriendly

humanfriendly-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch humanfriendly && \
	cd humanfriendly && \
	$(MAKE) library && \
	$(MAKE) install PREFIX=$(SYSROOT)
