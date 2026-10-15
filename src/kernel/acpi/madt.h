#pragma once
#include <stdint.h>
#include <acpi/acpi_tables.h>

typedef struct {
	uint8_t entry_type;
	uint8_t len;
} apic_entry_header;

typedef struct {
	apic_entry_header head;
	uint8_t processor_id;
	uint8_t id;
	uint32_t flags;
} __attribute__((packed)) madt_entry_type0; // Processor Local APIC

typedef struct {
	apic_entry_header head;
	uint8_t id;
	uint8_t reserved;
	uint32_t addr;
	uint32_t g_sys_int; // Global System Interrupt
} __attribute__((packed)) madt_entry_type1; // I/O APIC

typedef struct {
	apic_entry_header head;
    uint8_t bus_src; 
    uint8_t irq_src; 
    uint32_t g_sys_int; // Ditto
    uint16_t flags;
} __attribute__((packed)) madt_entry_type2; // I/O APIC Interrupt Source Override

typedef struct {
	apic_entry_header head;
	uint8_t nmi_src;
	uint8_t reserved;
	uint16_t flags;
	uint32_t g_sys_int; // Ditto
} __attribute__((packed)) madt_entry_type3; // I/O APIC Non-maskable interrupt source

typedef struct {
	apic_entry_header head;
	uint8_t processor_id;
	uint16_t flags;
	uint8_t LINT;
} __attribute__((packed)) madt_entry_type4; // Local APIC Non-maskable interrupts

typedef struct {
	apic_entry_header head;
	uint8_t reserved;
	uint64_t apic_addr;
} __attribute__((packed)) madt_entry_type5; // Local APCI Address Override

typedef struct {
	apic_entry_header head;
	uint8_t reserved;
	uint8_t processor_id;
	uint32_t flags;
	uint32_t acpi_id;
} __attribute__((packed)) madt_entry_type9; // Local APCI Address Override

typedef struct {
	madt_entry_type0 type0;
	madt_entry_type1 type1;
	madt_entry_type2 type2;
	madt_entry_type3 type3;
	madt_entry_type4 type4;
	madt_entry_type5 type5;
} madt_apic_types_t;
