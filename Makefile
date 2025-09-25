# ===== MiniOS Makefile (full, copy-paste) =====

CC=gcc
LD=ld
CFLAGS=-m32 -ffreestanding -nostdlib -fno-pic -fno-pie -Wall -Wextra
# ===== MiniOS Makefile (full, copy-paste) =====

CC=gcc
LD=ld
CFLAGS=-m32 -ffreestanding -nostdlib -fno-pic -fno-pie -Wall -Wextra
LDFLAGS=-m elf_i386 -T linker.ld

BUILD=build
ISO_DIR=iso
GRUB_DIR=$(ISO_DIR)/boot/grub
GRUB_CFG=$(GRUB_DIR)/grub.cfg
KERNEL=$(BUILD)/kernel.bin
ISO=mini-os.iso

# -------- Objects (ALL components) --------
OBJ=$(BUILD)/boot.o \
    $(BUILD)/kernel.o \
    $(BUILD)/idt.o \
    $(BUILD)/isr.o \
    $(BUILD)/isr_stubs.o \
    $(BUILD)/idt_load.o \
    $(BUILD)/pic.o \
    $(BUILD)/irq.o \
    $(BUILD)/timer.o \
    $(BUILD)/vga.o \
    $(BUILD)/keyboard.o \
    $(BUILD)/gdt.o \
    $(BUILD)/gdt_flush.o \
    $(BUILD)/paging.o

# -------- Top-level targets --------
all: $(ISO)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf $(BUILD) $(ISO) $(ISO_DIR)/boot/kernel.bin

.PHONY: all run clean

# -------- Build rules --------
$(BUILD):
	mkdir -p $(BUILD)

# Boot (multiboot header) asm
$(BUILD)/boot.o: boot/boot.s | $(BUILD)
	$(CC) -m32 -c -o $@ $<

# C sources
$(BUILD)/kernel.o: kernel/kernel.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/idt.o: kernel/idt.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/isr.o: kernel/isr.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/pic.o: kernel/pic.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/irq.o: kernel/irq.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/timer.o: kernel/timer.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/vga.o: kernel/vga.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/keyboard.o: kernel/keyboard.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/gdt.o: kernel/gdt.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/paging.o: kernel/paging.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

# Assembly stubs (force 32-bit via gcc driver)
$(BUILD)/isr_stubs.o: kernel/isr_stubs.s | $(BUILD)
	$(CC) -m32 -c -o $@ $<

$(BUILD)/idt_load.o: kernel/idt_load.s | $(BUILD)
	$(CC) -m32 -c -o $@ $<

$(BUILD)/gdt_flush.o: kernel/gdt_flush.s | $(BUILD)
	$(CC) -m32 -c -o $@ $<

# -------- Link kernel --------
$(KERNEL): linker.ld $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

# -------- ISO + GRUB (auto-create grub.cfg) --------
$(ISO): $(KERNEL) $(GRUB_CFG)
	mkdir -p $(ISO_DIR)/boot
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	grub-mkrescue -o $(ISO) $(ISO_DIR)

$(GRUB_CFG):
	mkdir -p $(GRUB_DIR)
	echo 'set timeout=0' > $(GRUB_CFG)
	echo 'set default=0' >> $(GRUB_CFG)
	echo 'menuentry "MiniOS" { multiboot /boot/kernel.bin; boot }' >> $(GRUB_CFG)
