include ../../common.mk

SRCS_ASM = \
	start.S \

SRCS_C = \
	kernel.c \

OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)

OBJ_DIR = objs
OUT_OBJS = $(addprefix $(OUT_DIR)/, $(OBJS))

.DEFAULT_GOAL := all
all: os.elf

# start.o must be the first in dependency!
$(OBJ_DIR)/os.elf: ${OUT_OBJS}
	${CC} ${CFLAGS} -Ttext=0x80000000 -o os.elf $^
	${OBJCOPY} -O binary os.elf os.bin

$(OBJ_DIR)/%.o : %.c
	${CC} ${CFLAGS} -c -o $@ $<

$(OBJ_DIR)/%.o : %.S
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

