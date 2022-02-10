#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#include "stivale2.h"
#include "util.h"
#include "kprint.h"
#include "handlers.h"
#include "pic.h"

// Every interrupt handler must specify a code selector. We'll use entry 5 (5*8=0x28), which
// is where our bootloader set up a usable code selector for 64-bit mode.
#define IDT_CODE_SELECTOR 0x28

// IDT entry types
#define IDT_TYPE_INTERRUPT 0xE
#define IDT_TYPE_TRAP 0xF

// A struct the matches the layout of an IDT entry
typedef struct idt_entry {
  uint16_t offset_0;
  uint16_t selector;
  uint8_t ist : 3;
  uint8_t _unused_0 : 5;
  uint8_t type : 4;
  uint8_t _unused_1 : 1;
  uint8_t dpl : 2;
  uint8_t present : 1;
  uint16_t offset_1;
  uint32_t offset_2;
  uint32_t _unused_2;
} __attribute__((packed)) idt_entry_t;

// Make an IDT
idt_entry_t idt[256];

//Will set all bits between target and target + n to a constant bit c
void memset(void* target, uint64_t c, int n){
  uint64_t * curr = target;
  for(int i = 0; i < n; i++){
    *curr = c;
    curr++;
  }
}

/**
 * Set an interrupt handler for the given interrupt number.
 *
 * \param index The interrupt number to handle
 * \param fn    A pointer to the interrupt handler function
 * \param type  The type of interrupt handler being installed.
 *              Pass IDT_TYPE_INTERRUPT or IDT_TYPE_TRAP from above.
 */
void idt_set_handler(uint8_t index, void* fn, uint8_t type) {
  // Fill in all fields of idt[index]
  // Make sure you fill in:
  //   handler (all three parts, which requires some bit masking/shifting)
  //   type (using the parameter passed to this function)
  //   p=1 (the entry is present)
  //   dpl=0 (run the handler in kernel mode)
  //   ist=0 (we aren't using an interrupt stack table, so just pass 0)
  //   selector=IDT_CODE_SELECTOR
  //not sure how to fill in the handler yet
  idt[index].offset_0 = (uint16_t) ((uint64_t) fn & 0x000000000000FFFF); //bits 15-0
  idt[index].offset_1 = (uint16_t) (((uint64_t) fn & 0x00000000FFFF0000) >> 16); //bits 31-16
  idt[index].offset_2 = (uint32_t) (((uint64_t) fn & 0xFFFFFFFF00000000) >> 32); //bits 63-32
  //type is passed in
  idt[index].type = type;
  //dpl is given as 0
  idt[index].dpl = 0;
  //ist is given as 0
  idt[index].ist = 0;
  //present is given as 1
  idt[index].present = 1;
  //selector is given as IDT_CODE_SELECTOR
  idt[index].selector = IDT_CODE_SELECTOR;
}

// This struct is used to load an IDT once we've set it up
typedef struct idt_record {
  uint16_t size;
  void* base;
} __attribute__((packed)) idt_record_t;

/**
 * Initialize an interrupt descriptor table, set handlers for standard exceptions, and install
 * the IDT.
 */
void idt_setup() {
  // Step 1: Zero out the IDT
  memset(&idt, 0, 256); 
	

  // Step 2: Use idt_set_handler() to set handlers for the standard exceptions (1--21)
  
  //INTERNAL INTERRUPTS
  idt_set_handler(0, &divide_error_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(1, &debug_exception_handler, IDT_TYPE_TRAP);
  idt_set_handler(2, &nmi_interrupt_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(3, &breakpoint_handler, IDT_TYPE_TRAP);
  idt_set_handler(4, &overflow_interupt_handler, IDT_TYPE_TRAP);
  idt_set_handler(5, &bound_range_exc_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(6, &invalid_opcode_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(7, &device_not_available, IDT_TYPE_INTERRUPT);
  idt_set_handler(8, &double_fault_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(9, &co_pro_seg_overrun, IDT_TYPE_INTERRUPT);
  idt_set_handler(10, &invalid_tss_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(11, &seg_not_present, IDT_TYPE_INTERRUPT);
  idt_set_handler(12, &stack_seg_fault, IDT_TYPE_INTERRUPT);
  idt_set_handler(13, &gen_mem_protection, IDT_TYPE_INTERRUPT);
  idt_set_handler(14, &page_fault_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(16, &fpu_floating_point_error, IDT_TYPE_INTERRUPT);
  idt_set_handler(17, &alignment_check_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(18, &machine_check_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(19, &simd_floating_point_error, IDT_TYPE_INTERRUPT);
  idt_set_handler(20, &virtualization_exception, IDT_TYPE_INTERRUPT);
  idt_set_handler(21, &control_protection_exception, IDT_TYPE_INTERRUPT);

  //EXTERNAL INTERRUPTS
  idt_set_handler(IRQ1_INTERRUPT, &keyboard_handler, IDT_TYPE_INTERRUPT);

  // Step 3: Install the IDT
  idt_record_t record = {
  .size = sizeof(idt),
    .base = idt
    };
  __asm__("lidt %0" :: "m"(record));
}
