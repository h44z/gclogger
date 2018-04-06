EXE = gclogger

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS += -Iinclude
CFLAGS += -Wall -Wextra -Os

#LDFLAGS += -Llib
LDLIBS += -lcurl

.PHONY: all clean

all: $(EXE)

# Linking:
$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compiling:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Cleaning:
clean:
	$(RM) $(OBJ)
	$(RM) $(EXE)
