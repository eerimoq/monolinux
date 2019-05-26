BUILD = $(shell readlink -f build)
PACKAGES = $(BUILD)/packages
EXE = $(BUILD)/suite
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
CFLAGS += -coverage
CFLAGS += -Wall -Wextra -std=gnu11
CFLAGS += -g -Og
CFLAGS += -DUNIT_TEST
LDFLAGS += -lunicorn

.PHONY: all

all: $(EXE)
	$(EXE)

include $(ML_ROOT)/make/common.mk
