TARGET   := main.out

CC       := clang-9
VALGRIND = 1

#for clang some flags bcs of POSIX
ifeq ($(CC), gcc)
	CFLAGS   := -Wall -Wextra -pthread
else 
	CFLAGS   := -Weverything -Wno-padded \
				-Wno-reserved-id-macro -Wno-disabled-macro-expansion
endif

CFLAGS += -std=c99

ifeq ($(VALGRIND), 1)
	CFLAGS   += -g
endif

LINKER   := $(CC)
LFLAGS   := -Wall -lm -pthread 

PROJECT_DIR := $(shell pwd)

BINDIR   := $(PROJECT_DIR)/bin
INCDIR   := $(PROJECT_DIR)/inc
SRCDIR   := $(PROJECT_DIR)/src
OBJDIR   := $(PROJECT_DIR)/obj
SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
HEADERS  := $(foreach d, $(INCDIR), -I$d)

VERBOSE = 0

ifeq ($(VERBOSE),1)
  Q =
else
  Q = @
endif

define print_info
	@echo "[$(1)]        $(2)"
endef

# Was trying to do multiple folders but i ended up having many Makefiles

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(call print_info,LINK, $<)
	$(Q)$(LINKER) $(LFLAGS) $(OBJECTS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(Q)$(CC) $(HEADERS) $(CFLAGS) -c $< -o $@
	$(call print_info,OBJ, $@)

clean:
	$(Q)echo CLEANING
	$(Q)$(RM) $(wildcard $(OBJDIR)/*.o)

remove: clean
	$(Q)$(RM) $(BINDIR)/$(TARGET)


run_test:
	$(MAKE) -f $(PROJECT_DIR)/Makefile --no-print-directory
