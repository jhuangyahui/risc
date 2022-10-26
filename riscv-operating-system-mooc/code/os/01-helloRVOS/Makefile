include ../../common.mk

SRCS_ASM = \
	start.S \

SRCS_C = \
	kernel.c \
	uart.c \

START_UP = startup
KERNEL = kernel
OBJ_DIR = objs

OBJS = $(addprefix $(OBJ_DIR)/,$(START_UP/SRCS_ASM:.S=.o))
# OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS_ASM:.S=.o))
OBJS += $(addprefix $(KERNEL)/,$(SRCS_C:.c=.o))


 
.DEFAULT_GOAL := all
all: os.elf

# start.o must be the first in dependency!
os.elf: ${OBJS}
	${CC} ${CFLAGS} -Ttext=0x80000000 -o os.elf $^
	${OBJCOPY} -O binary os.elf os.bin

$(OBJ_DIR)/%.o : %.c
	${CC} ${CFLAGS} -c -o $@ $<


$(OBJ_DIR)/%.o : %.S
	mkdir -p $(dir $@)
	${CC} ${CFLAGS} -c -o $@ $<

run: all
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo "------------------------------------"
	@${QEMU} ${QFLAGS} -kernel os.elf

.PHONY : debug
debug: all
	@echo "Press Ctrl-C and then input 'quit' to exit GDB and QEMU"
	@echo "-------------------------------------------------------"
	@${QEMU} ${QFLAGS} -kernel os.elf -s -S &
	@${GDB} os.elf -q -x ../gdbinit

.PHONY : code
code: all
	@${OBJDUMP} -S os.elf | less

.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf

