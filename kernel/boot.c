#include "general.h"
#include "util.h"

/*
 * Where we are at:
 * 	need to implement memcpy and vm_protect in order to finish loading executables
 * 	need to implement library
 * 	need to be able to swap out of kernel mode
 */


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

void term_setup(struct stivale2_struct* hdr) {
  // Look for a terminal tag
  struct stivale2_struct_tag_terminal* tag = find_tag(hdr, STIVALE2_STRUCT_TAG_TERMINAL_ID);

  // Make sure we find a terminal tag
  if (tag == NULL) halt();

  // Save the term_write function pointer
  // term_write = (term_write_t)tag->term_write;
  set_term_write((term_write_t)tag->term_write);
}



void _start(struct stivale2_struct* hdr) {
  // We've booted! Let's start processing tags passed to use from the bootloader
  
  //setting stuff up
  term_setup(hdr);
	
	  //interupt handler being set up
  idt_setup();

  //allows us to handle keyboard interupt inputs
  pic_init();
  pic_unmask_irq(1);

	//setting tags for page table operations
	uint64_t hhdm_id = 12748887341935670671;
	uint64_t mem_tag_id = 2416171985333837319;

	set_page_imp(find_tag(hdr, hhdm_id), find_tag(hdr, mem_tag_id));
	
	//setting tag for executables
	uint64_t modules_tag_id = 0x4b6fe466aade04ce;
	set_mod_tag(find_tag(hdr, modules_tag_id));

	//Greetings
  kprintf("Hello World\n");


	/*TESTS*/

	//Kprintf// success - 3/31/2022
	/*
	char buff[5] = "work";
	kprintf("D: %d, C: %c, S: %s, P: %p, X: %x, %%\n", 'a', 'a', buff, &buff, 'a');
	kprintf("\n");
	*/

	//Print Usable Memory// success - 3/31/2022
	/*
	kprint_usable_mem(hdr);
	kprintf("\n");
	*/

	//Catch Interupts// success - 3/31/2022
	//int* r = NULL;
	//*r = 123;

	//Handling Keyboard Interrups// success - 3/31/2022
	//This one is just checked by Typing in the terminal
	
	//Keyboard Input Buffer
	/*
	char gotten = kgetc();
	kprintf("recieved from buffer %c\n", gotten);
	kprintf("\n");
	*/

	//Paging 
		//translation// success - 3/31/2022
	/*	
	translate(stack);
	kprintf("\n");
	
	kprintf("%c - continuing\n", kgetc());
	*/

		//mapping// success - 3/31/2022
	int * p = (int*) 0x50003000;
	uintptr_t root = read_cr3() & 0xFFFFFFFFFFFFF000;
	/*
	bool result = vm_map(root, (uintptr_t)p, false, true, false);
	if(result) {
		*p = 123;
		kprintf("stored %d at %p\n", *p, p);
	} else { 
		kprintf("vm_map failed with an error\n");
	}
	kprintf("\n");
	*/

		//unmapping// failed - 3/31/2022
	
	bool result_unmap = vm_unmap(root, (uintptr_t)p);
	if(result_unmap) {
		kprintf("vm_unmap sucessfully unmapped p\n");
	} else {
		kprintf("failed to unmap %d at %p\n", *p, p);
	}
	

	//System Calls// success - 3/31/2022
		//syntax: syscall(SYS_read/SYS_write, fd, buffer, bytes written or read)

	/*	
	kprintf("writing \"write\" to buffer\n");
	char write_buff[6] = "write";
	long wc = syscall(SYS_write, 1, write_buff, 5);
	if (wc <= 0) {
		kprintf("write failed\n");
	}

	kprintf("reading\n");
	char read_buff[6];
	long rc = syscall(SYS_read, 0, read_buff, 6);
	if (rc <= 0) {
		kprintf("read failed\n");
	} else {
		read_buff[rc] = '\0';
		kprintf("read '%s'\n", read_buff);
	}
	*/

	//Loading Executable
	print_mod_strings();
	load_exec();

	//Std Library
	
	//Switching to User Mode
	


  // We're done, just hang...
  halt();
}
