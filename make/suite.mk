BUILD = $(shell readlink -f build)
PACKAGES = $(BUILD)/packages
EXE = $(BUILD)/suite
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
CFLAGS += -coverage
CFLAGS += -Wall -Wextra -std=gnu11
CFLAGS += -g -Og
CFLAGS += -DUNIT_TEST
LDFLAGS += -lunicorn
COVERAGE_FILTERS +=

.PHONY: all coverage

all: $(EXE)
	$(EXE)
	$(MAKE) coverage

coverage:
	gcovr --root ../.. \
	    --exclude-directories ".*tst.*" $(COVERAGE_FILTERS:%=-f %) \
	    --html-details --output index.html build
	mkdir -p $(BUILD)/coverage
	mv index.* $(BUILD)/coverage

include $(ML_ROOT)/make/common.mk
