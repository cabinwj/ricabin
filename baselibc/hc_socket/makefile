include ../../makefile_inc

CXX_FLAGS= $(CXXFLAGS) -D__MULTI_THREAD__

TARGET= $(TARGET_DIR)/libshare_socket.a

INC_DIR= $(INC_SOCKET_DIR)
SRC_DIR= $(SRC_SOCKET_DIR)
OBJS_DIR= $(OBJS_SOCKET_DIR)

INC= -I$(INC_SOCKET_DIR) -I$(INC_COMMON_DIR)

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



