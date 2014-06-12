include ../../makefile_inc

#CCFLAGS= $(CCFLAGS) -D__MULTI_THREAD__

TARGET= $(TARGET_DIR)/liblzma.a

INC_DIR= $(INC_LZMA_DIR)
SRC_DIR= $(SRC_LZMA_DIR)
OBJS_DIR= $(OBJS_LZMA_DIR)

INC= -I$(INC_LZMA_DIR)

CCSRC= $(wildcard $(SRC_DIR)/*.c)
CCOBJS= $(patsubst %.c, $(OBJS_DIR)/%.o, $(notdir $(CCSRC)))

all:
	make clean && \
	make $(TARGET)

$(TARGET) : $(CCOBJS)
	@if [ ! -e $(TARGET_DIR) ]; then \
		mkdir -p $(TARGET_DIR); \
	fi
	$(AR) $(ARFLAGS) $@ $?

$(OBJS_DIR)/%.o : $(SRC_DIR)/%.c
	@if [ ! -e $(OBJS_DIR) ]; then \
		mkdir $(OBJS_DIR); \
	fi
	$(CC) $(CCFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(CCOBJS) $(TARGET)


