CXX= g++
AR= ar

ARFLAGS= rcs
CPPFLAGS= -g -Wall -fPIC -Wpointer-arith

TARGET= $(DEBUG_TARGET)

DEBUG_TARGET= $(DEBUG_TARGET_DIR)/libencrypt.a
RELEASE_TARGET= $(RELEASE_TARGET_DIR)/libencrypt.a

BASE_DIR= ${HOME}
SERVER_DIR= $(BASE_DIR)/utils
ENCRYPT_DIR= $(SERVER_DIR)/encrypt
INCLUDE_DIR= $(ENCRYPT_DIR)/inc
SOURCE_DIR= $(ENCRYPT_DIR)/src
OBJ_DIR= $(ENCRYPT_DIR)/.objs

DEBUG_TARGET_DIR= $(SERVER_DIR)/lib/debug
RELEASE_TARGET_DIR= $(SERVER_DIR)/lib/release

INC= -I$(INCLUDE_DIR)
VPATH= .:$(SERVER_DIR)
SRC= $(wildcard $(SOURCE_DIR)/*.cpp)
OBJS= $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRC)))


all :
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

