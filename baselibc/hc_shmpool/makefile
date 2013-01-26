include ../../makefile_inc

TARGET= $(TARGET_DIR)/libshare_shmpool.a

INC_DIR= $(INC_SHMPOOL_DIR)
SRC_DIR= $(SRC_SHMPOOL_DIR)
OBJS_DIR= $(OBJS_SHMPOOL_DIR)

INC= -I$(INC_SHMPOOL_DIR) -I$(INC_COMMON_DIR)

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
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(CXXOBJS) $(TARGET)

