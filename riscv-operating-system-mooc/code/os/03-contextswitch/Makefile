include ../../common.mk

SRCS_ASM = \
	start.S \
	mem.S \
	entry.S \

SRCS_C = \
	kernel.c \
	uart.c \
	printf.c \
	page.c \
	sched.c \

START_UP = startup
KERNEL = kernel
LIB = lib
ASM = asm
OBJ_DIR = objs

OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS_ASM:.S=.o))
OBJS += $(addprefix $(OBJ_DIR)/,$(SRCS_C:.c=.o))

.DEFAULT_GOAL := all
all: os.elf

# start.o must be the first in dependency!
os.elf: ${OBJS}
	${CC} ${CFLAGS} -T os.ld -o os.elf $^
	${OBJCOPY} -O binary os.elf os.bin

$(OBJ_DIR)/%.o : $(KERNEL)/%.c
	${CC} ${CFLAGS} -c -o $@ $<

$(OBJ_DIR)/%.o : $(START_UP)/%.S $(ASM)/%.S
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
	rm -rf $(OBJ_DIR)/*.o *.bin *.elf

