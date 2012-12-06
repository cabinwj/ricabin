CXX= g++
AR= ar

ARFLAGS= rcs
CPPFLAGS= -g -Wall -fPIC -Wpointer-arith -Wno-invalid-offsetof -D__MULTI_THREAD__ -DCORO_ASM

TARGET= $(DEBUG_TARGET)

DEBUG_TARGET= $(DEBUG_TARGET_DIR)/libbaseframe.a
RELEASE_TARGET= $(RELEASE_TARGET_DIR)/libbaseframe.a

BASE_DIR= ${HOME}
SERVER_DIR= $(BASE_DIR)/ricabin
COMMON_DIR= $(SERVER_DIR)/baseframe
LIB_ENCRYPT_DIR= $(SERVER_DIR)/utils/encrypt/inc
LIB_TINYXML_DIR= $(SERVER_DIR)/utils/tinyxml/inc
LIB_COMMON_DIR= $(SERVER_DIR)/baselibc/share_common/inc
LIB_SOCKET_DIR= $(SERVER_DIR)/baselibc/share_socket/inc
LIB_COROUTINE_DIR= $(SERVER_DIR)/baselibc/share_coroutine/inc
INCLUDE_DIR= $(COMMON_DIR)/inc
SOURCE_DIR= $(COMMON_DIR)/src
OBJ_DIR= $(COMMON_DIR)/.objs

DEBUG_TARGET_DIR= $(SERVER_DIR)/staticlibc#/lib/debug
RELEASE_TARGET_DIR= $(SERVER_DIR)/staticlibc#/lib/release

INC= -I$(INCLUDE_DIR) -I$(LIB_COMMON_DIR) -I$(LIB_SOCKET_DIR) -I$(LIB_COROUTINE_DIR) -I$(LIB_ENCRYPT_DIR) -I$(LIB_TINYXML_DIR)
VPATH= .:$(SERVER_DIR)

SRC= $(wildcard $(SOURCE_DIR)/*.cpp)
OBJS= $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRC)))

all:
	make clean && \
	make $(TARGET)

$(TARGET) : $(OBJS)
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
	$(CXX) $(CPPFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
