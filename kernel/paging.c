#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#include "stivale2.h"
#include "kprint.h"
#include "paging.h"
#include "interupt_handling.h"

#define PAGE_SIZE 4096

struct stivale2_struct_tag_hhdm* hhdm_tag; 
struct stivale2_struct_tag_memmap* memmap_tag;

typedef struct page_table_entry {
  bool present : 1;
  bool writable : 1;
  bool user : 1;
  uint16_t unused : 9;
  uint64_t address : 51;
  bool no_execute : 1;
} __attribute__((packed)) pt_entry_t;

//implement freelist 
//addresses stored in the list will be PHYSICAL
//meaning that we will NEED to ensure that address translation happens before anything happens to the list
typedef struct free_list_entry {
	uintptr_t phys_address;
	struct free_list_entry * next_page;
} freelist_t;

//without malloc we have to make this point to the usable space at the virtual address and we may have to cast that space
freelist_t * first_page = NULL;

//void translate(uintptr_t page_table, void* address) {
void translate(void* address) {

	// get top level page ptr, mask off last 12 bits and then convert to virtual address
	uintptr_t hhdm_addr = hhdm_tag->addr; //used for conversions
	uintptr_t phys_addr = read_cr3() & 0xFFFFFFFFFFFFF000; //top level table address
	pt_entry_t * virt_addr = (pt_entry_t *) (phys_addr + hhdm_addr); //virtual address of top level table

	kprintf("Translating %p\n", address);

	// get page table indicies from given address
	uintptr_t addr = (uintptr_t) address; //converting to uintptr from void * so that we can bit shift and mask

	uint16_t indicies[4] = {
		(addr >> 12) & 0x1FF,
		(addr >> 21) & 0x1FF,
		(addr >> 30) & 0x1FF,
		(addr >> 39) & 0x1FF };

	for (int i = 4; i > 0; i--) {
		kprintf("  Level %d (index %d of %p)\n", i, indicies[i-1], phys_addr);

		if (virt_addr[indicies[i-1]].present) {

			if (virt_addr[indicies[i-1]].user) {
				kprintf("user");

				if (virt_addr[indicies[i-1]].writable){
					kprintf(" writable");
				}

			} else {
				kprintf("kernel");
			}			
			if (!virt_addr[indicies[i-1]].no_execute){
				kprintf(" executable");
			}

			kprintf(" -> %p", (indicies[i-1] + phys_addr));

		} else {
			kprintf("not present");
		}
		kprintf("\n");

		phys_addr = (uintptr_t) virt_addr[indicies[i-1]].address << 12;
		virt_addr = (pt_entry_t *) (phys_addr + hhdm_addr);
	}
	kprintf("%p maps to %p\n", address, phys_addr);
}

uintptr_t read_cr3() {
  uintptr_t value;
  __asm__("mov %%cr3, %0" : "=r" (value));
  return value;
}

uint64_t read_cr0() {
  uintptr_t value;
  __asm__("mov %%cr0, %0" : "=r" (value));
  return value;
}

void write_cr0(uint64_t value) {
  __asm__("mov %0, %%cr0" : : "r" (value));
}

void set_page_imp(struct stivale2_struct_tag_hhdm* given_hhdm, struct stivale2_struct_tag_memmap* given_memmap) {
	hhdm_tag = given_hhdm;
	memmap_tag = given_memmap;
	
	//storing base address of usable memory mapping and how large the mapping is
  uint64_t base;
  uint64_t length;
	uint64_t curr_ind = 0;

  for (int i = 0; i < memmap_tag->entries; i++){
	
  //at each indiviual entry we will check the type to see if it is usable (== 1)
  //if it is usable then we get the base and the length to calculate the start and finish of the mapping
    if (memmap_tag->memmap[i].type == 1 && memmap_tag->memmap[i].length > 0){
      base = memmap_tag->memmap[i].base;
      length = memmap_tag->memmap[i].length;
			
			//will need to divide up the usable memory regions into pages corresponding to PAGE_SIZE
			//usuable memory regions will all be divisable by PAGE_SIZE by definition
			while (length != curr_ind) {
				freelist_t * new_page = (freelist_t *) (hhdm_tag->addr + ((uintptr_t) base + curr_ind)); 
				new_page->phys_address = (uintptr_t) (base + curr_ind);
				if (first_page == NULL) {
					new_page->next_page = NULL;
				} else {
					new_page->next_page = first_page;
				}
				first_page = new_page;
				curr_ind += PAGE_SIZE;
			}
    }
  }
}

//before writing these functions we will need to get the memmap tag and find all of the usable spaced and then we'll
//create the free list with pages of size 4096 which is specified in the #define PAGE_SIZE macro

uintptr_t pmem_alloc(){
	uintptr_t phys_address = first_page->phys_address;
	first_page = first_page->next_page;
	return phys_address;
}

//this function is just a pop function to the free list (linked list) (you need to implement a linked list)

void pmem_free(uintptr_t p){
	freelist_t * new_page = (freelist_t *) (hhdm_tag->addr + p);
	new_page->phys_address = p;
	new_page->next_page = first_page;
	first_page = new_page;
}
//this function adds to the linked list, but should zero out the memory before adding it.



bool vm_map(uintptr_t root, uintptr_t address, bool user, bool writable, bool executable){
//this function will navigate through a 4 level page table using the indicies specified in the given address
//if a page table is not already present at the specified index then we will use pmem_alloc to write a new page table
//we will then traverse all the way down to the first level and on the bottom level we will write a pt_entry_t with
//the given user, writeable, and !no-execute bits (I put the ! infront of no execute to remind myself that we are given
//an executable flag and the bit is the opposite of that)

//store the virtual and physical addresses
	uintptr_t phys_addr = root;
	pt_entry_t * virt_addr = (pt_entry_t *) (phys_addr + hhdm_tag->addr); //virtual address of top level table
//	kprintf("phys: %p -- virt: %p\n", phys_addr, (uintptr_t) virt_addr);

	// get page table indicies from given address
	uint16_t indicies[4] = {
		(address >> 12) & 0x1FF,
		(address >> 21) & 0x1FF,
		(address >> 30) & 0x1FF,
		(address >> 39) & 0x1FF };
//	kprintf("indicies: %d, %d, %d, %d\n\n", indicies[0], 	indicies[1], indicies[2], indicies[3]);
	
	for (int i = 4; i > 0; i--) {
		//kprintf("%c - continuing\n", kgetc());
		
		pt_entry_t * new_pt_entry;

		//do not write bits until we are at level 1 page table
//		kprintf("%d\n", i);
//		kprintf("next phys addr: %p\n", virt_addr[indicies[i-1]]);

		if (!virt_addr[indicies[i-1]].present){
//			kprintf("allocating a new page\n");
			uintptr_t phys_pt_space = pmem_alloc();
//			kprintf("the newly allocated space: %p\n", phys_pt_space);
			new_pt_entry = (pt_entry_t *) (phys_pt_space + hhdm_tag->addr);
//			kprintf("virt address for new entry: %p\n", new_pt_entry);
			new_pt_entry->present = 1;
			new_pt_entry->user = 1;
			new_pt_entry->writable = 1;
			new_pt_entry->no_execute = 1; //page table entry shouldn't be executable??
			new_pt_entry->address = phys_pt_space;
			virt_addr[indicies[i-1]] = *new_pt_entry;
//			kprintf("page allocated and new table created\n");
/*		} else {
			kprintf("page already present\n");
*/		}
			
		phys_addr = (uintptr_t) (virt_addr[indicies[i-1]].address);//<< 12)
		virt_addr = (pt_entry_t *) (phys_addr + hhdm_tag->addr);	
//		kprintf("phys: %p -- virt: %p\n\n", phys_addr, (uintptr_t) virt_addr);

		if (i == 1) {
//			kprintf("allocating the final entry's physical page space\n");

			uintptr_t phys_mem_space = pmem_alloc();
//			kprintf("allocating phys: %p\n", phys_mem_space);
			new_pt_entry->address = phys_mem_space;
		}
	}
	return true;
}

bool vm_unmap(uintptr_t root, uintptr_t address){
	//will navigate to the bottom and wipe the bottom page entry while setting no exectute to one
	//call pmem_free() on the spacea
	
	uintptr_t phys_addr = root;
	pt_entry_t * virt_addr = (pt_entry_t *) (phys_addr + hhdm_tag->addr); //virtual address of top level table
	
	// get page table indicies from given address
	uint16_t indicies[4] = {
		(address >> 12) & 0x1FF,
		(address >> 21) & 0x1FF,
		(address >> 30) & 0x1FF,
		(address >> 39) & 0x1FF };
	
	for (int i = 4; i > 0; i--) {
		//do not change bits until we are at level 1 page table
		if (i > 1) {
			if (!virt_addr[indicies[i-1]].present){
				//if the entry is not present then the address is invalid
				return false;
			}
			phys_addr = (uintptr_t) (virt_addr[indicies[i-1]].address << 12);
			virt_addr = (pt_entry_t *) (phys_addr + hhdm_tag->addr);
		} else {
			if (!virt_addr[indicies[i-1]].present){
				return false;
			}

			pmem_free((uintptr_t) (virt_addr[indicies[i-1]].address << 12));
			virt_addr[indicies[i-1]].user = 0;
			virt_addr[indicies[i-1]].writable = 0;
			virt_addr[indicies[i-1]].no_execute = 1;
		 	virt_addr[indicies[i-1]].address = 0x0;
			virt_addr[indicies[i-1]].present = 0;
		}
	}
	return true;
}


bool vm_protect(uintptr_t root, uintptr_t address, bool user, bool writable, bool executable){
	uintptr_t phys_addr = root;
	pt_entry_t * virt_addr = (pt_entry_t *) (phys_addr + hhdm_tag->addr); //virtual address of top level table
	
	// get page table indicies from given address
	uint16_t indicies[4] = {
		(address >> 12) & 0x1FF,
		(address >> 21) & 0x1FF,
		(address >> 30) & 0x1FF,
		(address >> 39) & 0x1FF };
	
	for (int i = 4; i > 0; i--) {
		//do not change bits until we are at level 1 page table
		if (i > 1) {
			if (!virt_addr[indicies[i-1]].present){
				//if the entry is not present then the address is invalid
				return false;
			}
			phys_addr = (uintptr_t) virt_addr[indicies[i-1]].address << 12;
			virt_addr = (pt_entry_t *) (phys_addr + hhdm_tag->addr);
		} else {
			if (!virt_addr[indicies[i-1]].present){
				return false;
			}
			virt_addr[indicies[i-1]].user = user;
			virt_addr[indicies[i-1]].writable = writable;
			virt_addr[indicies[i-1]].no_execute = (executable ? 0 : 1 ); 
		}
	}
	return true;
}

