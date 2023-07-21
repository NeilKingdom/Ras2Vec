CC = gcc
PROFILE ?= DEBUG

CCFLAGS_DEBUG = -g -O0 -fno-builtin -DDEBUG
CCFLAGS_RELEASE = -O2

SDIR := src
ODIR := obj
DDIR := include

SRCS := $(wildcard $(SDIR)/*.c)
DEPS := $(wildcard $(DDIR)/*.h)
OBJS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

CCFLAGS = $(CCFLAGS_$(PROFILE)) -I$(DDIR) -Werror -Wall -Wextra -Wformat -ansi -pedantic -std=c99
LDFLAGS = -lc

BIN = ras2vec

all: $(BIN)

clean:
	rm -rf $(ODIR)/*.o $(BIN)

rebuild: clean all

ras2vec: $(OBJS)
	$(CC) $^ -o $@ $(CCFLAGS) $(LDFLAGS)

$(ODIR)/%.o: $(SRCS) $(DEPS)
	$(CC) -c $< -o $@ $(CCFLAGS)

.PHONY: all clean rebuild
