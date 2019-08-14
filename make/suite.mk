BUILD = $(shell readlink -f build)
PACKAGES = $(BUILD)/packages
EXE = $(BUILD)/suite
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -coverage
CFLAGS += -Wall -Wextra -std=gnu11
CFLAGS += -g -Og
CFLAGS += -DUNIT_TEST
LDFLAGS += -lnarwhal
COVERAGE_FILTERS +=

.PHONY: all run build coverage

all: run
	$(MAKE) coverage

build: $(EXE)

run: $(EXE)
	$(EXE)

coverage:
	gcovr --root ../.. \
	    --exclude-directories ".*tst.*" $(COVERAGE_FILTERS:%=-f %) \
	    --html-details --output index.html build
	mkdir -p $(BUILD)/coverage
	mv index.* $(BUILD)/coverage
	@echo
	@echo "Code coverage report: $$(readlink -f build/coverage/index.html)"
	@echo

include $(ML_ROOT)/make/common.mk
