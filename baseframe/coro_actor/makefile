include ../../makefile_inc

CXX_FLAGS= $(CXXFLAGS) -D__MULTI_THREAD__  -DCORO_ASM

TARGET= $(TARGET_DIR)/libcoro_actor.a

INC_DIR= $(INC_COROACTOR_DIR)
SRC_DIR= $(SRC_COROACTOR_DIR)
OBJS_DIR= $(OBJS_COROACTOR_DIR)

INC= -I$(INC_COROACTOR_DIR) -I$(INC_COMMON_DIR) \
         -I$(INC_SOCKET_DIR) -I$(INC_COROUTINE_DIR) \
         -I$(INC_ENCRYPT_DIR) -I$(INC_TINYXML_DIR)

CXXSRC= $(wildcard $(SRC_DIR)/*.cpp)
CXXOBJS= $(patsubst %.cpp, $(OBJS_DIR)/%.o, $(notdir $(CXXSRC)))

all:
	make clean && \
	make $(TARGET)

$(TARGET) : $(CXXOBJS)
	@if [ ! -e $(TARGET_DIR) ]; then \
		mkdir -p $(TARGET_DIR); \
	fi
	$(AR) $(ARFLAGS) $@ $?

$(OBJS_DIR)/%.o : $(SRC_DIR)/%.cpp
	@if [ ! -e $(OBJS_DIR) ]; then \
		mkdir $(OBJS_DIR); \
	fi
	$(CXX) $(CXX_FLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(CXXOBJS) $(TARGET)

