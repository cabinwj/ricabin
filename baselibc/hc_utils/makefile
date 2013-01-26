include ../../makefile_inc

CXX_FLAGS= $(CXXFLAGS) -D__MULTI_THREAD__

TARGET= $(TARGET_DIR)/libhc_utils.a

INC_DIR= $(INC_UTILS_DIR)
SRC_DIR= $(SRC_UTILS_DIR)
OBJS_DIR= $(OBJS_UTILS_DIR)

INC= -I$(INC_UTILS_DIR) -I$(INC_MYSQL_DIR)

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

