# mini-os-timer-vga

A from-scratch **x86 mini operating system** written in **C + Assembly**.  
Boots via **GRUB (Multiboot v1)**, sets up **GDT/IDT**, remaps the **PIC**, installs a **PIT timer** (100 Hz), and prints to **VGA text mode**.

> This repo demonstrates core OSDev skills: bootloading, protected mode setup, interrupt handling, hardware timers, and bare-metal text output.

---

## ✨ Features

- **Multiboot-compliant kernel** (GRUB loads `_start` → `kmain`)
- **GDT / IDT** initialization
- **ISR / IRQ** dispatch with C handlers
- **8259A PIC** remapping (IRQs at 0x20–0x2F)
- **PIT (8253/8254)** timer at 100 Hz → uptime counter
- **VGA text mode** (0xB8000) writer for screen output
- Minimal, production-style **Makefile** with ISO build and QEMU run

---

## 📸 Screenshot

> Replace this with your own screenshot from QEMU.

![QEMU screenshot showing kernel output](docs/screenshot.png)

Typical output:

kmain: hello from C
soft-uptime: 1s
soft-uptime: 2s
...


If you enable real IRQs/timer in `kernel/kernel.c`, you’ll see:


kernel: timer online, printing every 1s...
uptime: 0s
uptime: 1s


---

## 🔧 Requirements

- **GCC** (32-bit capable)
- **GNU ld**
- **make**
- **qemu-system-i386**
- **grub-mkrescue**, **xorriso** (for ISO creation)
  - On Ubuntu/WSL: `sudo apt install build-essential qemu-system-x86 grub-pc-bin xorriso`

> Works great on Linux and WSL (Windows Subsystem for Linux).

---

## 🚀 Build & Run

```bash
make           # builds kernel + ISO
make run       # launches QEMU: boots GRUB -> kernel

Clean builds:

make clean     # remove build artifacts
make realclean # also wipes the ISO tree

Sanity checks:

grub-file --is-x86-multiboot build/kernel.bin && echo "OK multiboot"
xorriso -indev mini-os.iso -report_el_torito as_mkisofs | sed -n '1,80p'

🧠 How It Works (quick tour)

Boot: GRUB sees the .multiboot header and loads the ELF at 1 MiB.

Entry: _start sets up a 16 KiB stack, writes a “K” to video memory for visibility, then calls kmain.

Init: kmain sets up GDT/IDT via either *_install or *_init (whichever exists), then (optionally) remaps the PIC and enables the PIT.

Interrupts: IRQ0 (timer) fires at 100 Hz; the C handler counts ticks and prints per-second uptime.

Output: Print is direct to 0xB8000 in VGA text mode (no libc).

🧪 Enabling/Disabling IRQs

To avoid triple-faults while wiring your IDT/IRQ stubs, the kernel ships with a safe mode:

In kernel/kernel.c, toggle:

#define ENABLE_TIMER_AND_IRQS 0  // set to 1 when your IDT/IRQs are solid


When set to 0, the kernel shows a software heartbeat (“soft-uptime”) without enabling sti.
When set to 1, it enables IRQ0 and prints real uptime: once per second.

Troubleshooting

GRUB says “no multiboot header”
Ensure linker.ld places .multiboot first inside .text:

.text ALIGN(16) : {
  KEEP(*(.multiboot))
  *(.text .text.*)
}


QEMU pauses right after selecting the menu entry
That’s usually an IRQ firing with a bad IDT/handler address (triple-fault).
Set ENABLE_TIMER_AND_IRQS to 0 and re-enable once IDT is correct.

WSL users: run everything inside WSL; mixing Windows paths into the build can break GRUB ISO creation.

📝 License

MIT — do whatever helps you learn, just keep the notice.

🙌 Credits / Notes

Built following classic OSDev patterns and docs.

QEMU + GRUB tooling makes iteration fast and reproducible.

Educational project; not intended for production.


---

If you want, I can also generate a tiny **docs/screenshot.png** instruction (how to capture and add it) or tweak the README to match exactly what you’ve enabled (soft heartbeat vs. real timer).


