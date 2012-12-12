include ../../makefile_inc

CXX_FLAGS= $(CXXFLAGS) -D__MULTI_THREAD__  -DCORO_ASM

TARGET= $(TARGET_DIR)/libstatus.a

INC_DIR= $(INC_STATUS_DIR)
SRC_DIR= $(SRC_STATUS_DIR)
OBJS_DIR= $(OBJS_STATUS_DIR)

INC= -I$(INC_STATUS_DIR) \
     -I$(INC_COROACTOR_DIR) \
     -I$(INC_COMMON_DIR) \
     -I$(INC_SOCKET_DIR) \
     -I$(INC_COROUTINE_DIR) \
     -I$(INC_TINYXML_DIR) \
     -I$(INC_ENCRYPT_DIR)

CXXSRC= $(wildcard $(SRC_DIR)/*.cpp)
CXXOBJS= $(patsubst %.cpp, $(OBJS_DIR)/%.o, $(notdir $(CXXSRC)))

BIN_CPP= $(wildcard *.cpp)
BIN_FILE= $(BIN_CPP:%.cpp=%)

ALL_LIB= -L$(TARGET_DIR) -lstatus \
         -L$(TARGET_DIR) -lcoro_actor \
         -L$(TARGET_DIR) -lshare_coroutine \
         -L$(TARGET_DIR) -lshare_socket \
         -L$(TARGET_DIR) -lshare_common -lpthread \
         -L$(TARGET_DIR) -ltinyxml \
         -L$(TARGET_DIR) -lencrypt

all:
	make clean && \
	make $(BIN_FILE)

$(BIN_FILE) : $(TARGET)
	$(CXX) $(CXX_FLAGS) $(INC) -o $(BIN_FILE) $(BIN_CPP) $(ALL_LIB)

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
	rm -f $(CXXOBJS) $(TARGET) $(BIN_FILE)

