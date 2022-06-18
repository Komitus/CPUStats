.PHONY: all clean remove run

TARGET   = main.out
CC       = gcc
CFLAGS   = -std=c99 -Wall -Wextra -pthread

LINKER   = gcc 
LFLAGS   = -Wall -lm -pthread

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(LFLAGS) $(OBJECTS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo $<" compiled successfully!"

clean:
	@$(rm) $(wildcard $(OBJDIR)/*.o)
	@echo "Cleanup complete!"

remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"


