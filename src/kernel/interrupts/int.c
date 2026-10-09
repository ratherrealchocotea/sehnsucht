// int.c
// interrupt-related stuff
#include <drivers/serial/serial.h>
#include <drivers/tty/tty.h>
#include <interrupts/int.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define IDT_GATES_MAX 256

typedef void (*interrupt_routine_t)(const cpu_state_t*);
extern void
_load_idt(const idt_ptr_t* idt_ptr);
extern void
_enable_ints(void);
extern void
_hang(void);
__attribute__((aligned(0x1000))) static idt_gate_t idt[IDT_GATES_MAX];

uint64_t routine_handlers[IDT_GATES_MAX];
static const char* exception_messages[] = {
  "Division by Zero (0x00)",
  "Debug (0x01)",
  "Non-maskable interrupt (0x02)",
  "Breakpoint (0x03)",
  "Into detected overflow (0x04)",
  "Out of bounds (0x05)",
  "Invalid opcode (0x06)",
  "No coprocessor (0x07)",
  "Double fault (0x08)",
  "Coprocessor segment overrun (0x09)",
  "Bad TSS 0x0a",
  "Segment not present (0x0b)",
  "Stack Fault (0x0c)	",
  "General Protection Fault (0x0d)",
  "Page Fault (0x0e)",
  "Unknown Interrupt (0x0f)",
  "Coprocessor Fault (0x10)",
  "Alignment check (0x11)",
  "Machine check (0x12)",
  "SIMD Floating Exception (0x13)",
  "Virtual Exception (0x14)",
  "Control Protection Exception (0x15)",
  "Reserved (0x16)",
  "Reserved (0x17)",
  "Reserved (0x18)",
  "Reserved (0x19)",
  "Reserved (0x1a)",
  "Reserved (0x1b)",
  "Hypervisor Intrusion Exception (0x1c)",
  "VMM Communications Exception (0x1d)",
  "Security Excpetion (0x1e)",
  "Reserved (0x1f)"
};

void
idt_init(void)
{
  __attribute__((aligned(0x10))) idt_ptr_t idt_ptr =
    (idt_ptr_t){ .limit = (uint16_t)(sizeof(idt) - 1), .base = (uint64_t)&idt };
  idt_set_gate(0, isr_stub_0);
  idt_set_gate(1, isr_stub_1);
  idt_set_gate(2, isr_stub_2);
  idt_set_gate(3, isr_stub_3);
  idt_set_gate(4, isr_stub_4);
  idt_set_gate(5, isr_stub_5);
  idt_set_gate(6, isr_stub_6);
  idt_set_gate(7, isr_stub_7);
  idt_set_gate(8, isr_stub_8);
  idt_set_gate(9, isr_stub_9);
  idt_set_gate(10, isr_stub_10);
  idt_set_gate(11, isr_stub_11);
  idt_set_gate(12, isr_stub_12);
  idt_set_gate(13, isr_stub_13);
  idt_set_gate(14, isr_stub_14);
  idt_set_gate(15, isr_stub_15);
  idt_set_gate(16, isr_stub_16);
  idt_set_gate(17, isr_stub_17);
  idt_set_gate(18, isr_stub_18);
  idt_set_gate(19, isr_stub_19);
  idt_set_gate(20, isr_stub_20);
  idt_set_gate(21, isr_stub_21);
  idt_set_gate(22, isr_stub_22);
  idt_set_gate(23, isr_stub_23);
  idt_set_gate(24, isr_stub_24);
  idt_set_gate(25, isr_stub_25);
  idt_set_gate(26, isr_stub_26);
  idt_set_gate(27, isr_stub_27);
  idt_set_gate(28, isr_stub_28);
  idt_set_gate(29, isr_stub_29);
  idt_set_gate(30, isr_stub_30);
  idt_set_gate(31, isr_stub_31);
  idt_set_gate(32, irq_stub_32);
  idt_set_gate(33, irq_stub_33);
  idt_set_gate(34, irq_stub_34);
  idt_set_gate(35, irq_stub_35);
  idt_set_gate(36, irq_stub_36);
  idt_set_gate(37, irq_stub_37);
  idt_set_gate(38, irq_stub_38);
  idt_set_gate(39, irq_stub_39);
  idt_set_gate(40, irq_stub_40);
  idt_set_gate(41, irq_stub_41);
  idt_set_gate(42, irq_stub_42);
  idt_set_gate(43, irq_stub_43);
  idt_set_gate(44, irq_stub_44);
  idt_set_gate(45, irq_stub_45);
  idt_set_gate(46, irq_stub_46);
  idt_set_gate(47, irq_stub_47);
  /* idt_setup_gates(); */
  _load_idt(&idt_ptr); // lidt; ret (might change this to idt_ptr->base)
  _enable_ints();      // sti; ret
}

void
idt_set_gate(uint8_t vector, void* handler)
{
  uint64_t offset = (uint64_t)handler;

  idt[vector] = (idt_gate_t){ .offset_low = offset & 0xffff,
                              // VERY IMPORTANT!
                              // Limine idiosyncracy in the GDT, it seems
                              // Seemingly the Limine GDT is different and
                              // thus the segments are different too.
                              // This value has to be 0x28.
                              .segment = 0x28,
                              .ist = 0,
                              .attributes = 0x8e,
                              .offset_mid = (offset >> 16) & 0xffff,
                              .offset_high = (offset >> 32) & 0xffffffff,
                              .reserved = 0 };
}

void
isr_handler(const cpu_state_t* cpu_state)
{
  //	serial_write("INTERRUPT: ");
  if (cpu_state->inter < 32)
    exception_handler(cpu_state);

  interrupt_routine_t handler =
    (interrupt_routine_t)routine_handlers[cpu_state->inter];

  if (handler)
    handler(cpu_state);

  if (cpu_state->inter >= 40)
    _outb(0xa0, 0x20);
  _outb(0x20, 0x20);

  // serial_write("Interrupt invoked");
}

__attribute__((noreturn)) void
exception_handler(const cpu_state_t* cpu_state)
{
  kwrite("An exception occurred. You screwed up.\n");
  if (cpu_state->inter < 32)
    kwrite(exception_messages[cpu_state->inter]);
  else
    kwrite("Unknown exception");
  for (;;)
    _hang(); // clt; ret
}
