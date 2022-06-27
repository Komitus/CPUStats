
_SRC := $(wildcard $(TESTS_DIR)/*.c)
_OBJ := $(_SRC:%.c=%.o)
_DEPEND :=  $(filter-out $(OBJDIR)/main.o,$(OBJECTS))

%.o: %.c
	$(call print_info,OBJ, $@)
	$(Q)$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@

all: $(TESTS_DIR)/$(TEST_TARGET)

$(TESTS_DIR)/$(TEST_TARGET): $(_OBJ)
	$(call print_info,LINK, $<)
	$(Q)$(LINKER) $(LFLAGS) $(_OBJ) $(_DEPEND) -o $@

clean:
	$(Q)echo CLEANING_TEST
	$(Q)$(RM) $(_OBJ)	
	$(Q)$(RM) $(TESTS_DIR)/$(TEST_TARGET)