LIBS += hf
HUMANFRIENDLY = humanfriendly-0.3.0

packages: $(PACKAGES)/$(HUMANFRIENDLY)

$(PACKAGES)/$(HUMANFRIENDLY): $(ML_SOURCES)/$(HUMANFRIENDLY).tar.gz
	$(MAKE) $(HUMANFRIENDLY)-all

$(HUMANFRIENDLY)-all:
	@echo "Building $(HUMANFRIENDLY)."
	$(MAKE) $(HUMANFRIENDLY)-unpack
	$(MAKE) $(HUMANFRIENDLY)-build

$(HUMANFRIENDLY)-clean:
	cd $(PACKAGES)/$(HUMANFRIENDLY)

$(ML_SOURCES)/$(HUMANFRIENDLY).tar.gz:
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/humanfriendly/archive/0.3.0.tar.gz

$(HUMANFRIENDLY)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(HUMANFRIENDLY).tar.gz

$(HUMANFRIENDLY)-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch $(HUMANFRIENDLY) && \
	cd $(HUMANFRIENDLY) && \
	$(MAKE) library && \
	$(MAKE) install PREFIX=$(SYSROOT)
