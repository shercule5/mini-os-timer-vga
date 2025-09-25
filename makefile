# ===========================
#  Mini-OS Makefile (32-bit)
# ===========================
CC      := gcc
LD      := ld
AS      := gcc

BUILD    := build
ISO_DIR  := iso
ISO_NAME := mini-os.iso

CFLAGS  += -m32 -ffreestanding -O2 -fno-stack-protector -fno-pic -fno-builtin \
           -Wall -Wextra -Ikernel
ASFLAGS += -m32
LDFLAGS += -m elf_i386 -T linker.ld -nostdlib

# ---- sources ----
KERNEL_SSRCS := \
  multiboot_header.s \
  gdt_flush.s \
  idt_load.s \
  isr_stubs.s

KERNEL_CSRCS := \
  gdt.c idt.c isr.c irq.c kernel.c keyboard.c paging.c pic.c timer.c vga.c

# ---- objects ----
KSOBJS := $(addprefix $(BUILD)/,$(KERNEL_SSRCS:.s=.o))
KCOBJS := $(addprefix $(BUILD)/,$(KERNEL_CSRCS:.c=.o))
OBJS   := $(KSOBJS) $(KCOBJS)
KERNEL_BIN := $(BUILD)/kernel.bin

.PHONY: all clean run iso dirs symbols checkiso realclean

all: dirs $(KERNEL_BIN) iso

# Wipe ISO tree completely each time to avoid stale files at /
dirs:
	@rm -rf $(ISO_DIR)
	@mkdir -p $(BUILD)
	@mkdir -p $(ISO_DIR)/boot/grub

# asm -> o
$(BUILD)/%.o: kernel/%.s
	$(AS) $(ASFLAGS) -c $< -o $@

# c -> o
$(BUILD)/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# link
$(KERNEL_BIN): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "[OK] Linked $@"
	@grub-file --is-x86-multiboot $@ && echo "[OK] Multiboot header detected." || \
	  (echo "[ERR] Not a multiboot kernel." && false)

# Write grub.cfg into /boot/grub/grub.cfg (GRUB only looks here)
$(ISO_DIR)/boot/grub/grub.cfg: | dirs
	@printf '%s\n' \
'set timeout=-1' \
'set default=0' \
'' \
'menuentry "mini-os" {' \
'    multiboot /boot/kernel.bin' \
'    boot' \
'}' > $@

# Build ISO with grub-mkrescue (puts El Torito boot image in place)
iso: $(ISO_DIR)/boot/grub/grub.cfg $(KERNEL_BIN)
	@cp -f $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	@grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)
	@echo "[OK] ISO: $(ISO_NAME)"

# Quick inspection helpers
checkiso: iso
	@echo "---- El Torito ----"; xorriso -indev $(ISO_NAME) -report_el_torito as_mkisofs | sed -n '1,80p'
	@echo "---- / ----";        xorriso -indev $(ISO_NAME) -lsl /
	@echo "---- /boot ----";    xorriso -indev $(ISO_NAME) -lsl /boot/
	@echo "---- /boot/grub ----"; xorriso -indev $(ISO_NAME) -lsl /boot/grub/

# Run: force CD boot and keep VM open on crash; print serial to terminal
run: all
	qemu-system-i386 -boot d -cdrom $(ISO_NAME) -m 64 -no-reboot -no-shutdown -serial stdio

symbols: $(KERNEL_BIN)
	nm -n $(KERNEL_BIN) | less

# Thorough clean (also wipes ISO tree)
realclean:
	@rm -rf $(BUILD) $(ISO_NAME) $(ISO_DIR)
	@echo "[REALCLEAN] done"

clean:
	@rm -rf $(BUILD) $(ISO_NAME) $(ISO_DIR)/boot/kernel.bin
	@echo "[CLEAN] done"
