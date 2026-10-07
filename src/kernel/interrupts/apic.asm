;; apic.asm
;; Configure APIC

%define APIC_SIVR 0xfee000f0h    ; Suprious Interrupt Vector Register

apic_setup_sivr:
    mov [APIC_SIVR], 0xff       ; set up SIVR
    
