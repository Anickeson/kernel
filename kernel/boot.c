#include <stdint.h>
#include <stddef.h>

#include "stivale2.h"
#include "util.h"

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
	uint64_t printed_val = value % 10;

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

void kprint_p(void * ptr){
	//this will be very similar to printing in hexadecimal except it could be a bit bigger on average
	uint64_t num = ptr;
	kprint_s("0x");
	kprint_x(num);
}

void kprint_usable_mem(struct strivale2_struct_tag_memmap* memmap_entries) {
	//will need to iterate over the memmap (stivale_mmap_entry) field of the memmap tag
	//the loop will loop n times, n being defined by the entries field of the tag struct
	
	uint64_t base;
	uint64_t length; //these will be used for mem addresses
	for (uint64_t i = 0; i < memmap_entries->entries; i++){
	
	//at each indiviual entry we will check the type to see if it is usable (== 1)
	//if it is usable then we get the base and the length to calculate the start and finish of the mapping
		if (memmap_entries->memmap[i]->type == 1){
			base = memmap_entries->memmap.base;
			length = memmap_entries->memmap.length;
		}
	
	//after getting physical addresses we will need to figure out how to convert to virtual addresses
	
	//after we get all of this we will need a kprintf statement
	//otherwise we'll have to combine a bunch of kprint statements :(
		kprint_p((void *) base);
		kprint_c('-');
		kprint_p((void *) base + length);
		kprint_s(" mapped at ");
		//kprint_p(); these are for the virtual addresses, I just need to figure out what to add
		//kprint_p(); the information I'm looking for is probably in a tag
	}
}

void _start(struct stivale2_struct* hdr) {
	// We've booted! Let's start processing tags passed to use from the bootloader
	term_setup(hdr);

	// Print a greeting
	term_write("Hello Kernel!\n", 14);
	
	uint64_t mem_tag_id = 2416171985333837319;
	struct stivale2_struct_tag_memmap* memmap_tag = (struct stivale2_struct_tag_memmap *) find_tag(hdr, mem_tag_id);
	kprint_usable_mem(memmap_tag);



	// We're done, just hang...
	halt();
}
