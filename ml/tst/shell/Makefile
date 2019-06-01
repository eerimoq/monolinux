INC += ..
SRC += ../utils/utils.c
SRC += ../utils/mocks/mock.c
SRC += ../utils/mocks/mock_libc.c
LDFLAGS += -Wl,--wrap=ml_open
LDFLAGS += -Wl,--wrap=ml_close
LDFLAGS += -Wl,--wrap=ml_finit_module
LDFLAGS += -Wl,--wrap=statvfs
LDFLAGS += -Wl,--wrap=setmntent
LDFLAGS += -Wl,--wrap=getmntent
LDFLAGS += -Wl,--wrap=endmntent
COVERAGE_FILTERS = .*ml_shell.c

include $(ML_ROOT)/make/suite.mk
