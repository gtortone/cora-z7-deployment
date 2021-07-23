# Target
BIN_DIR = bin
BIN = $(BIN_DIR)/brc

# Match all sources in src direcotry
SRCS_DIR = src
SRCS = $(wildcard $(SRCS_DIR)/**/*.c $(SRCS_DIR)/*.c)

# Builds object list from sources, substitues .c for .o
OBJS_DIR = obj
OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS))

# Include headers files
INCLUDE_DIRS = include
INCLUDE = $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir))

# Search libraries in following directories
INCLUDE_LIB_DIRS = 
INCLUDE_LIB = $(foreach includedir,$(INCLUDE_LIB_DIRS),-L$(includedir))

# Set compiler, preprocesor and linker flags
CPPFLAGS += -O3 -Wall -Wno-unused-result
LDFLAGS += $(INCLUDE_LIB)

# use DEBUG=1 to include debugging
ifdef DEBUG
  CLAGS += -g	
endif

# Set other tools
MKDIR = mkdir -p

# Avoid filename conflicts
.PHONY: all clean

# Rules
all: $(BIN)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIRS) -c $< -o $@

$(BIN): $(OBJS)
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	@$(RM) $(BIN)
	@$(RM) $(OBJS)
