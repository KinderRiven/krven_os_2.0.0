C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find . -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))
B_OBJECTS = $(shell find . -name "*.bin")

CC = gcc
LD = ld
ASM = nasm

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I include -I include/sys
LD_FLAGS = -T scripts/kernel.ld -m elf_i386 -nostdlib
ASM_FLAGS = -f elf -g -F stabs -I include

Kernel_Name = kos

all: $(S_OBJECTS) $(C_OBJECTS) link update_kernel


test:
# The automatic variable `$<' is just the first prerequisite
.c.o:
	@echo 编译代码文件 $< ...
	$(CC) $(C_FLAGS) $< -o $@

.s.o:
	@echo 编译汇编文件 $< ...
	$(ASM) $(ASM_FLAGS) $<

link:
	@echo 链接内核文件...
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o $(Kernel_Name)

.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS) $(Kernel_Name) $(B_OBJECTS)

.PHONY : mount_image	
mount_image:
	sudo mount floppy.img /mnt/kos/

.PHONY : umount_image
umount_image:
	sudo umount /mnt/kos/

.PHONY : bochs
bochs:
	bochs -f bochsrc.txt
.PHONY : qemu
qemu:
	qemu -fda floppy.img -boot a

.PHONY : show_dir
show_dir:
	xxd -u -a -g 1 -c 16 -s +0x2600 -l 512 floppy.img

.PHONY : update_kernel
update_kernel:
	sudo mount floppy.img /mnt/kos/
	sudo cp $(Kernel_Name) /mnt/kos/
	sleep 1
	sudo umount /mnt/kos/

.PHONY : update_boot
update_boot:
	nasm boot/boot.asm -o boot/boot.bin -I boot/include/
	sleep 1
	dd if=boot/boot.bin of=floppy.img bs=512 count=1 conv=notrunc

.PHONY : update_loader
update_loader:
	nasm boot/loader.asm -o boot/loader.bin -I boot/include/
	sudo mount floppy.img /mnt/kos/
	sudo cp boot/loader.bin /mnt/kos/
	sleep 1
	sudo umount /mnt/kos/

.PHONY : show_mm
show_mm:
	objdump -h $(Kernel_Name)
