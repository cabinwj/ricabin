CXX= g++
AR= ar
CC= gcc

ARFLAGS= cr
CPPFLAGS= -g -D__MULTI_THREAD__ -Wall -fPIC -Wpointer-arith -Wno-invalid-offsetof -DCORO_ASM

TARGET= $(DEBUG_TARGET)

DEBUG_TARGET= $(DEBUG_TARGET_DIR)/libcoro_frame_mt.a
RELEASE_TARGET= $(RELEASE_TARGET_DIR)/libcoro_frame_mt.a

BASE_DIR= ${HOME}
SERVER_DIR= $(BASE_DIR)/frame_research-trunk
PUBLIC_DIR= $(SERVER_DIR)/cpp_actor/lib_coro_frame
LIB_PUBLIC_DIR= $(SERVER_DIR)/cpp_actor/lib_public/inc
LIB_SOCKET_DIR= $(SERVER_DIR)/cpp_actor/lib_socket/inc
LIB_COROUTINE_DIR= $(SERVER_DIR)/cpp_actor/lib_coroutine/inc
INCLUDE_DIR= $(PUBLIC_DIR)/inc
SOURCE_DIR= $(PUBLIC_DIR)/src
OBJ_DIR= $(PUBLIC_DIR)/.objs

DEBUG_TARGET_DIR= $(SERVER_DIR)/lib/debug
RELEASE_TARGET_DIR= $(SERVER_DIR)/lib/release

INC= -I$(INCLUDE_DIR) -I$(LIB_PUBLIC_DIR) -I$(LIB_SOCKET_DIR) -I$(LIB_COROUTINE_DIR)
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
