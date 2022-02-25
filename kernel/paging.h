#pragma once

#include <stdbool.h>

/**
 * Translate a virtual address to its mapped physical address
 *
 * \param address     The virtual address to translate
 */
void translate(void* address);

uintptr_t read_cr3(); 



uint64_t read_cr0(); 

void write_cr0(uint64_t value); 

/**
 * Sets up resources needed to properly navigate and write page table.
 *
 * Must be ran in _start before anything in paging.h is called
 *
 * \param given_hhdm	passing in the hhdm tag from the header in boot.c to properly
 * translate between physical and virtual addresses
 * \param given_memmap passing in the memmap tag from the header in boot.c so that we
 * can populate the free list with pages from all usable regions of memeory.
 */
void set_page_imp(struct stivale2_struct_tag_hhdm* given_hhdm, struct stivale2_struct_tag_memmap* given_memmap); 




/**
 * Allocate a page of physical memory.
 * \returns the physical address of the allocated physical memory or 0 on error.
 */
uintptr_t pmem_alloc();

/**
 * Free a page of physical memory.
 * \param p is the physical address of the page to free, which must be page-aligned.
 */
void pmem_free(uintptr_t p);



/**
 * Map a single page of memory into a virtual address space.
 * \param root The physical address of the top-level page table structure
 * \param address The virtual address to map into the address space, must be page-aligned
 * \param user Should the page be user-accessible?
 * \param writable Should the page be writable?
 * \param executable Should the page be executable?
 * \returns true if the mapping succeeded, or false if there was an error
 */
bool vm_map(uintptr_t root, uintptr_t address, bool user, bool writable, bool executable);



/**
 * Unmap a page from a virtual address space
 * \param root The physical address of the top-level page table structure
 * \param address The virtual address to unmap from the address space
 * \returns true if successful, or false if anything goes wrong
 */
bool vm_unmap(uintptr_t root, uintptr_t address);

/**
 * Change the protections for a page in a virtual address space
 * \param root The physical address of the top-level page table structure
 * \param address The virtual address to update
 * \param user Should the page be user-accessible or kernel only?
 * \param writable Should the page be writable?
 * \param executable Should the page be executable?
 * \returns true if successful, or false if anything goes wrong (e.g. page is not mapped)
 */
bool vm_protect(uintptr_t root, uintptr_t address, bool user, bool writable, bool executable);

