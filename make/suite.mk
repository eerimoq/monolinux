BUILD = build

INC += $(ML_ROOT)/src
SRC += \
	main.c \
	$(ML_ROOT)/src/ml.c \
	$(ML_ROOT)/src/ml_bus.c \
	$(ML_ROOT)/src/ml_log_object.c \
	$(ML_ROOT)/src/ml_message.c \
	$(ML_ROOT)/src/ml_network.c \
	$(ML_ROOT)/src/ml_queue.c \
	$(ML_ROOT)/src/ml_shell.c
OBJ = $(patsubst %,$(BUILD)%,$(abspath $(SRC:%.c=%.o)))
EXE = $(BUILD)/suite

CFLAGS += -fsanitize=address -fno-omit-frame-pointer
CFLAGS += -coverage
CFLAGS += -Wall -Wextra -std=gnu11
CFLAGS += -g -Og
CFLAGS += $(INC:%=-I%)
LIBS = -lunicorn -lpthread

.PHONY: all clean

all: $(EXE)
	./$(EXE)

$(EXE): $(OBJ)
	@echo "LD $@"
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

define COMPILE_template
$(patsubst %.c,$(BUILD)%.o,$(abspath $1)): $1
	@echo "CC $1"
	mkdir -p $(BUILD)$(abspath $(dir $1))
	$(CC) $(CFLAGS) -c -o $$@ $$<
endef
$(foreach file,$(SRC),$(eval $(call COMPILE_template,$(file))))

clean:
	rm -rf $(BUILD)

print-%:
	@echo $($*)
