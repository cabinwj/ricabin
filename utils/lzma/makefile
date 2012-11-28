CC= gcc
AR= ar

ARFLAGS= rcs
CCFLAGS= -g -Wall -fPIC -Wpointer-arith

TARGET= $(DEBUG_TARGET)

DEBUG_TARGET= $(DEBUG_TARGET_DIR)/liblzma.a
RELEASE_TARGET= $(RELEASE_TARGET_DIR)/liblzma.a

BASE_DIR= ${HOME}
SERVER_DIR= $(BASE_DIR)/ricabin
LZMA_DIR= $(SERVER_DIR)/utils/lzma
INCLUDE_DIR= $(LZMA_DIR)/inc
SOURCE_DIR= $(LZMA_DIR)/src
OBJ_DIR= $(LZMA_DIR)/.objs

DEBUG_TARGET_DIR= $(SERVER_DIR)/lib/debug
RELEASE_TARGET_DIR= $(SERVER_DIR)/lib/release

INC= -I$(INCLUDE_DIR)

VPATH= .:$(SERVER_DIR)
CCSRC= $(wildcard $(SOURCE_DIR)/*.c)
CCOBJS= $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(CCSRC)))


all:
	make clean && \
	make $(TARGET)

$(TARGET) : $(CCOBJS)
	@if [ ! -e $(DEBUG_TARGET_DIR) ]; then \
		mkdir -p $(DEBUG_TARGET_DIR); \
	fi
	@if [ ! -e $(RELEASE_TARGET_DIR) ]; then \
		mkdir -p $(RELEASE_TARGET_DIR); \
	fi
	$(AR) $(ARFLAGS) $@ $?

$(OBJ_DIR)/%.o : $(SOURCE_DIR)/%.c
	@if [ ! -e $(OBJ_DIR) ]; then \
		mkdir $(OBJ_DIR); \
	fi
	$(CC) $(CCFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(CCOBJS) $(TARGET)

