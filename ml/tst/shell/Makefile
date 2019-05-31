INC += ..
SRC += ../utils/utils.c
SRC += ../utils/mocks/mock.c
SRC += ../utils/mocks/mock_libc.c
LDFLAGS += -Wl,--wrap=ml_open
LDFLAGS += -Wl,--wrap=ml_close
LDFLAGS += -Wl,--wrap=ml_finit_module
COVERAGE_FILTERS = .*ml_shell.c

include $(ML_ROOT)/make/suite.mk
