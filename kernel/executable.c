#include "general.h"

struct stivale2_struct_tag_modules* modules_tag;

void set_mod_tag( struct stivale2_struct_tag_modules* tag) {
	modules_tag = tag;
}

void print_mod_strings(){
	kprintf("Modules:\n");

	for (int i = 0; i < modules_tag->module_count; i++){
		kprintf("\t%s %x-%x\n", modules_tag->modules[i].string, modules_tag->modules[i].begin, modules_tag->modules[i].end);
	}
}

void load_exec(){

	//find module which holds ELF in modules_tag
	int mod_index = 0;
	char * mod_name = modules_tag->modules[mod_index].string;
	uintptr_t begin = modules_tag->modules[mod_index].begin;
	uintptr_t end = modules_tag->modules[mod_index].end;

	//cast beginning of module to ELF header pointer
	elf_header_t * header = (elf_header_t *) begin;

	//find program header table at offset provided in the ELF Header
	prog_header_entry_t * prog_header_table = (prog_header_entry_t *) (begin + (uintptr_t) header->e_phoff);

	//Loop over all entries of the program header table
	for (int i = 0; i < header->e_phnum; i++){
		prog_header_entry_t curr_entry = prog_header_table[i];

		//for every entry with type LOAD and a size > 0 we will:
		if (curr_entry.p_type == PT_LOAD && curr_entry.p_filesz > 0) {

			//a. use vm_map to map the pages necessary to hold the program header at requested virtual address
			if (!vm_map(read_cr3(), curr_entry.p_vaddr, 1, 1, 1)){
				kprintf("failed to map section at requested address\n");
				//will need to get the permissions from the program header table when we find out how to do that
			}

			//b. use memcpy to copy data from ELF file (begin + offset) to the virtual address
			memcpy(curr_entry.p_vaddr, (begin + (uintptr_t) curr_entry.p_offset), curr_entry.p_filesz);	

			//c. Update the permissions with vm_protect to make it read-only and/or executable as requested

			if (curr_entry.p_flags == 1){
				//execute only
				vm_protect(read_cr3(), curr_entry.p_vaddr, false, false, true);
			} else if (curr_entry.p_flags == 2){
				//write only
				vm_protect(read_cr3(), curr_entry.p_vaddr, false, true, false);
			} else if (curr_entry.p_flags == 3){
				//execute and write
				vm_protect(read_cr3(), curr_entry.p_vaddr, false, true, true);
			} else if (curr_entry.p_flags == 4){
				//read only				
				vm_protect(read_cr3(), curr_entry.p_vaddr, true, false, false);
			} else if (curr_entry.p_flags == 5){
				//read and execute				
				vm_protect(read_cr3(), curr_entry.p_vaddr, true, false, true);
			} else if (curr_entry.p_flags == 6){
				//read and write
				vm_protect(read_cr3(), curr_entry.p_vaddr, true, true, false);
			} else if (curr_entry.p_flags == 7){
				//read, write, and execute
				vm_protect(read_cr3(), curr_entry.p_vaddr, true, true, true);
			}
		}
	}

	//cast entry address to function pointer and call it
	void (*entry_func)(void) = header->e_entry;
	entry_func();
}

