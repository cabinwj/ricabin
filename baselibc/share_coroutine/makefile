CXX= g++
CC= gcc
AR= ar

ARFLAGS= rcs
CPPFLAGS= -g -Wall -fPIC -Wpointer-arith -Wno-invalid-offsetof -D__MULTI_THREAD__ -DCORO_ASM
CCFLAGS= -g -Wall -fPIC -Wpointer-arith -D__MULTI_THREAD__ -DCORO_ASM

TARGET= $(DEBUG_TARGET)

DEBUG_TARGET= $(DEBUG_TARGET_DIR)/libshare_coroutine.a
RELEASE_TARGET= $(RELEASE_TARGET_DIR)/libshare_coroutine.a

BASE_DIR= ${HOME}
SERVER_DIR= $(BASE_DIR)/ricabin
COMMON_DIR= $(SERVER_DIR)/baselibc/share_coroutine
INCLUDE_COMMON_DIR= $(SERVER_DIR)/baselibc/share_common/inc
INCLUDE_DIR= $(COMMON_DIR)/inc
SOURCE_DIR= $(COMMON_DIR)/src
OBJ_DIR= $(COMMON_DIR)/.objs

DEBUG_TARGET_DIR= $(SERVER_DIR)/staticlibc#/lib/debug
RELEASE_TARGET_DIR= $(SERVER_DIR)/staticlibc#/lib/release

INC= -I$(INCLUDE_DIR) -I$(INCLUDE_COMMON_DIR)
VPATH= .:$(SERVER_DIR)

CXXSRC= $(wildcard $(SOURCE_DIR)/*.cpp)
CXXOBJS= $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(CXXSRC)))

CCSRC= $(wildcard $(SOURCE_DIR)/*.c)
CCOBJS= $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(CCSRC)))

all:
	make clean && \
	make $(TARGET)

$(TARGET) : $(CCOBJS) $(CXXOBJS)
	@if [ ! -e $(DEBUG_TARGET_DIR) ]; then \
		mkdir -p $(DEBUG_TARGET_DIR); \
	fi
	@if [ ! -e $(RELEASE_TARGET_DIR) ]; then \
		mkdir -p $(RELEASE_TARGET_DIR); \
	fi
	$(AR) $(ARFLAGS) $@ $?

$(OBJ_DIR)/%.o : $(SOURCE_DIR)/%.cpp
	@if [ ! -e $(OBJ_DIR) ]; then \
		mkdir $(OBJ_DIR); \
	fi
	$(CXX) $(INC) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o : $(SOURCE_DIR)/%.c
	@if [ ! -e $(OBJ_DIR) ]; then \
		mkdir $(OBJ_DIR); \
	fi
	$(CC) $(INC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f $(CCOBJS) $(CXXOBJS) $(TARGET)

