#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#include "stivale2.h"
#include "util.h"
#include "kprint.h"

typedef struct interrupt_context {
  uintptr_t ip;
  uint64_t cs;
  uint64_t flags;
  uintptr_t sp;
  uint64_t ss;
} __attribute__((packed)) interrupt_context_t;

__attribute__((interrupt))
void example_handler(interrupt_context_t* ctx) {
  kprintf("example interrupt handler\n");
  halt();
}



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
void memset(void* target, uint64_t c, uint64_t n){
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
  idt[index].offset_0 = ((uint64_t) fn) & 0x000000000000FFFF; //bits 15-0
  idt[index].offset_1 = ((uint64_t) fn) & 0x00000000FFFF0000; //bits 31-16
  idt[index].offset_2 = ((uint64_t) fn) & 0xFFFFFFFF00000000; //bits 63-32
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
  //gonna write some blank checks ie. call handlers before they're wrote
  idt_set_handler(14, &example_handler, IDT_TYPE_INTERRUPT);
//  idt_set_handler(0, &divide_error_handler, IDT_TYPE_INTERRUPT);


  // Step 3: Install the IDT
  idt_record_t record = {
  .size = sizeof(idt),
    .base = idt
    };
  __asm__("lidt %0" :: "m"(record));
}
