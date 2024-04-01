PROJECT:=pso


# Compiler settings
CC:=gcc
WARNINGS:=-Wall -Wextra -Wpedantic
STANDARD:=-std=c2x
OPTIM:=-O3
DEBUG:=-g3
CFLAGS:=$(WARNINGS) $(STANDARD) $(OPTIM) $(DEBUG)
LIBS:=-lm


# Directories
EXAMPLES_DIR:=Examples
BUILD_DIR:=Build
INSTALL_DIR?=/usr/local


# List files
EXAMPLES:=$(wildcard $(EXAMPLES_DIR)/*.c)
OBJ_FILES:=$(EXAMPLES:$(EXAMPLES_DIR)/%.c=$(BUILD_DIR)/%.o)


# Rules
all: $(OBJ_FILES) 

$(BUILD_DIR)/%.o: $(EXAMPLES_DIR)/%.c $(PROJECT).c $(PROJECT).h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PROJECT).c $< -o $@ $(LIBS)

$(BUILD_DIR)/lib$(PROJECT).so:
	$(CC) $(CFLAGS) $(PROJECT).c -fPIC -shared -o $@

install: $(BUILD_DIR)/lib$(PROJECT).so
	mv $(BUILD_DIR)/lib$(PROJECT).so $(INSTALL_DIR)/lib/
	cp $(PROJECT).h $(INSTAL_DIR)/include/
	ldconfig

clean:
	rm -f $(BUILD_DIR)/*

.PHONY: all clean install
	
