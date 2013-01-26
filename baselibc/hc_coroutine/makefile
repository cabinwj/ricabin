include ../../makefile_inc

CXX_FLAGS= $(CXXFLAGS) -D__MULTI_THREAD__ -DCORO_ASM
CC_FLAGS= $(CCFLAGS) -D__MULTI_THREAD__ -DCORO_ASM

TARGET= $(TARGET_DIR)/libhc_coroutine.a

INC_DIR= $(INC_COROUTINE_DIR)
SRC_DIR= $(SRC_COROUTINE_DIR)
OBJS_DIR= $(OBJS_COROUTINE_DIR)

INC= -I$(INC_COROUTINE_DIR) -I$(INC_UTILS_DIR)


CXXSRC= $(wildcard $(SRC_DIR)/*.cpp)
CXXOBJS= $(patsubst %.cpp, $(OBJS_DIR)/%.o, $(notdir $(CXXSRC)))

CCSRC= $(wildcard $(SRC_DIR)/*.c)
CCOBJS= $(patsubst %.c, $(OBJS_DIR)/%.o, $(notdir $(CCSRC)))

all:
	make clean && \
	make $(TARGET)

$(TARGET) : $(CCOBJS) $(CXXOBJS)
	@if [ ! -e $(TARGET_DIR) ]; then \
		mkdir -p $(TARGET_DIR); \
	fi
	$(AR) $(ARFLAGS) $@ $?

$(OBJS_DIR)/%.o : $(SRC_DIR)/%.cpp
	@if [ ! -e $(OBJS_DIR) ]; then \
		mkdir $(OBJS_DIR); \
	fi
	$(CXX) $(INC) $(CXX_FLAGS) -c $< -o $@

$(OBJS_DIR)/%.o : $(SRC_DIR)/%.c
	@if [ ! -e $(OBJS_DIR) ]; then \
		mkdir $(OBJS_DIR); \
	fi
	$(CC) $(CC_FLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(CCOBJS) $(CXXOBJS) $(TARGET)

