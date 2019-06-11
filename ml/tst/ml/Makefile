LDFLAGS += -Wl,--wrap=mount
LDFLAGS += -Wl,--wrap=ml_open
LDFLAGS += -Wl,--wrap=ml_close
LDFLAGS += -Wl,--wrap=ml_finit_module
INC += ..
SRC += ../utils/utils.c
SRC += ../utils/mocks/mock.c
SRC += ../utils/mocks/mock_libc.c

include $(ML_ROOT)/make/suite.mk
