[bits 64]

global _enable_apic
global _disable_apic
global _get_apic_base
global _set_apic_base
global _lapic_enable

APIC_MSR_BASE equ 0x1b
APIC_ENABLE_BIT equ 11
APIC_BASE_SH equ 12         ; Shift
APIC_BASE_WIDTH equ 24
APIC_BASE_MASK equ ((1 << APIC_BASE_WIDTH) - 1) << APIC_BASE_SH
APIC_BASE_PAGE_MASK equ ~0xfff
LAPIC_SVR_OFFSET equ 0xf0
LAPIC_SVR_VAL equ 0x1ff

_enable_apic:
    mov ecx, APIC_MSR_BASE
    rdmsr
    mov edx, APIC_ENABLE_BIT
    bts eax, edx
    wrmsr
    ret

_disable_apic:
    mov ecx, APIC_MSR_BASE
    rdmsr
    mov edx, APIC_ENABLE_BIT
    btr eax, edx
    wrmsr
    ret

_get_apic_base:
    mov ecx, APIC_MSR_BASE
    rdmsr
    shl rdx, 32
    or rax, rdx
    ret

_set_apic_base:
    mov rax, rdi
    mov rdx, rdi
    shr rdx, 32
    mov ecx, APIC_MSR_BASE
    wrmsr
    ret

_lapic_enable:
    mov dword [rdi + LAPIC_SVR_OFFSET], LAPIC_SVR_VAL
    ret
