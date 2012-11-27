CXX= g++
CC= gcc
AR= ar

ARFLAGS= rcs
CPPFLAGS= -g -Wall -fPIC -Wpointer-arith -Wno-invalid-offsetof

TARGET= $(DEBUG_TARGET)

DEBUG_TARGET= $(DEBUG_TARGET_DIR)/libshare_shmpool.a
RELEASE_TARGET= $(RELEASE_TARGET_DIR)/libshare_shmpool.a

BASE_DIR= ${HOME}
SERVER_DIR= $(BASE_DIR)/ricabin
COMMON_DIR= $(SERVER_DIR)/baselibc/share_shmpool
INCLUDE_COMMON_DIR= $(SERVER_DIR)/baselibc/share_common/inc
INCLUDE_DIR= $(COMMON_DIR)/inc
SOURCE_DIR= $(COMMON_DIR)/src
OBJ_DIR= $(COMMON_DIR)/.objs

DEBUG_TARGET_DIR= $(SERVER_DIR)/lib/debug
RELEASE_TARGET_DIR= $(SERVER_DIR)/lib/release

INC= -I$(INCLUDE_DIR) -I$(INCLUDE_COMMON_DIR) -I$(INCLUDE_SOCKET_DIR) -I$(INCLUDE_COROUTINE_DIR)
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
	$(CXX) $(INC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
