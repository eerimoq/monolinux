INC += $(ML_ROOT)/ml/include
SRC ?= \
	main.c \
	$(ML_ROOT)/ml/src/ml.c \
	$(ML_ROOT)/ml/src/ml_bus.c \
	$(ML_ROOT)/ml/src/ml_log_object.c \
	$(ML_ROOT)/ml/src/ml_message.c \
	$(ML_ROOT)/ml/src/ml_network.c \
	$(ML_ROOT)/ml/src/ml_queue.c \
	$(ML_ROOT)/ml/src/ml_shell.c
OBJ = $(patsubst %,$(BUILD)%,$(abspath $(SRC:%.c=%.o)))
CFLAGS += $(INC:%=-I%)

.PHONY: clean

$(EXE): $(OBJ)
	@echo "LD $@"
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -lpthread -o $@

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
