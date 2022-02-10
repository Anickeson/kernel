#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "stivale2.h"
#include "util.h"
#include "interupt_handling.h"
#include "pic.h"
#include "port.h"

// Reserve space for the stack
static uint8_t stack[8192];

// Request a terminal from the bootloader
static struct stivale2_header_tag_terminal terminal_hdr_tag = {
  .tag = {
    .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
    .next = 0
  },
  .flags = 0
};

static struct stivale2_tag unmap_null_hdr_tag = {
  .identifier = STIVALE2_HEADER_TAG_UNMAP_NULL_ID,
  //adding unmap_null_hdr_tag to terminal_hdr_tag, in order to unmap 0x0
  .next = (uintptr_t)&unmap_null_hdr_tag

};

// Declare the header for the bootloader
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
  // Use ELF file's default entry point
  .entry_point = 0,

  // Use stack (starting at the top)
  .stack = (uintptr_t)stack + sizeof(stack),

  // Bit 1: request pointers in the higher half
  // Bit 2: enable protected memory ranges (specified in PHDR)
  // Bit 3: virtual kernel mappings (no constraints on physical memory)
  // Bit 4: required
  .flags = 0x1E,
  
  // First tag struct
  .tags = (uintptr_t)&terminal_hdr_tag
};

// Find a tag with a given ID
void* find_tag(struct stivale2_struct* hdr, uint64_t id) {
  // Start at the first tag
  struct stivale2_tag* current = (struct stivale2_tag*)hdr->tags;

  // Loop as long as there are more tags to examine
  while (current != NULL) {
    // Does the current tag match?
    if (current->identifier == id) {
      return current;
    }

    // Move to the next tag
    current = (struct stivale2_tag*)current->next;
  }

  // No matching tag found
  return NULL;
}

typedef void (*term_write_t)(const char*, size_t);
term_write_t term_write = NULL;

void term_setup(struct stivale2_struct* hdr) {
  // Look for a terminal tag
  struct stivale2_struct_tag_terminal* tag = find_tag(hdr, STIVALE2_STRUCT_TAG_TERMINAL_ID);

  // Make sure we find a terminal tag
  if (tag == NULL) halt();

  // Save the term_write function pointer
  term_write = (term_write_t)tag->term_write;
}


//Kernel Implimentation Task #1, a bunch of printing

//needed strlen for kprint_s, might come in handy elsewhere
int strlen(const char * string){
  int len = 0;
  while (*string != '\0'){
  len++;
    string++;
  }
  return len;
}

//prints character to the terminal
void kprint_c(char c){
  term_write(&c, 1);
}

//prints a string to the terminal without altering string in anyway
void kprint_s (const char * str){
  term_write(str, strlen(str));
}

//takes in an unsigned 64-bit integer and prints it to the terminal
void kprint_d (uint64_t value){
  //chose to solve recursively
  //mod 10
  char printed_val = value % 10;

  //check for absence of base case
  if(value >= 10){
  kprint_d((value - printed_val) / 10);
  }

  //change printed value from number representation to ascii representation of said number
  printed_val += '0';
  

  //print value
  term_write(&printed_val, 1);
}

void kprint_x (uint64_t value){
  //similar to decimal except different mod
  char hex_digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  //mod 16
  uint64_t printed_val = value % 16;

  //check for absence of base case
  if(value >= 16){
    kprint_x((value - printed_val) / 16);
  }

  //reference and print out character for hex representation
  term_write(&hex_digits[printed_val], 1);
}

//prints the address stored in a pointer
void kprint_p(void * ptr){
  uint64_t num = (uint64_t) ptr;
  kprint_s("0x");
  kprint_x(num);
}

//imp of printf done in class
void kprintf(const char* format, ...) {
  // Start processing variadic arguments
  va_list args;
  va_start(args, format);

  // Loop until we reach the end of the format string
  size_t index = 0;
  while (format[index] != '\0') {
    // Is the current charater a '%'?
    if (format[index] == '%') {
      // Yes, print the argument
      index++;
      switch(format[index]) {
        case '%':
          kprint_c('%');
          break;
        case 'c':
          kprint_c(va_arg(args, int));
          break;
        case 's':
          kprint_s(va_arg(args, char*));
          break;
        case 'd':
          kprint_d(va_arg(args, uint64_t));
          break;
        case 'x':
          kprint_x(va_arg(args, int64_t));
          break;
        case 'p':
          kprint_p(va_arg(args, void*));
          break;
        default:
          kprint_s("<not supported>");
      }
    } else {
      // No, just a normal character. Print it.
      kprint_c(format[index]);
    }
    index++;
  }

  // Finish handling variadic arguments
  va_end(args);
}



//Implementation Task #2 printing usable memory mappings
//must have the header tag passed in
void kprint_usable_mem(struct stivale2_struct* hdr) {
  //will need to iterate over the memmap (stivale_mmap_entry) field of the memmap tag
  //the loop will loop n times, n being defined by the entries field of the tag struct
	
  //tag ids to be passed into find_tag
  uint64_t hhdm_tag_id = 12748887341935670671;
  uint64_t mem_tag_id = 2416171985333837319;
	
  //storing resulting tags from find_tag operation
  struct stivale2_struct_tag_memmap* memmap_tag = find_tag(hdr, mem_tag_id);
  struct stivale2_struct_tag_hhdm* hhdm_tag = find_tag(hdr, hhdm_tag_id);

  //storing base address of usable memory mapping and how large the mapping is
  uint64_t base;
  uint64_t length;

  for (uint64_t i = 0; i < memmap_tag->entries; i++){
	
  //at each indiviual entry we will check the type to see if it is usable (== 1)
  //if it is usable then we get the base and the length to calculate the start and finish of the mapping
    if (memmap_tag->memmap[i].type == 1 && memmap_tag->memmap[i].length > 0){
      base = memmap_tag->memmap[i].base;
      length = memmap_tag->memmap[i].length;

      //printing out the mappings after recieving all necessary info
      kprintf("%p-%p mapped at %p-%p\n", (void *) base, (void *) base + length,
        (void *) base + hhdm_tag->addr, (void *) base + length + hhdm_tag->addr);
    }
  }
}

void _start(struct stivale2_struct* hdr) {
  // We've booted! Let's start processing tags passed to use from the bootloader
  term_setup(hdr);
	
  kprintf("Hello World\n");	
  //kprint_usable_mem(hdr);
  idt_setup();
  int* p = NULL;
  *p = 123;

  pic_init();
  pic_unmask_irq(1);

  // We're done, just hang...
  halt();
}
