#pragma once

void set_mod_tag( struct stivale2_struct_tag_modules* tag);
void print_mod_strings();
void load_exec();


//ELF64 data types
typedef uint16_t Elf64_Half;
typedef uintptr_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

typedef struct elf_header {
	unsigned char e_ident[16]; /* ELF identification */
	Elf64_Half e_type; /* Object file type */
	Elf64_Half e_machine; /* Machine type */
	Elf64_Word e_version; /* Object file version */
	Elf64_Addr e_entry; /* Entry point address */
	Elf64_Off e_phoff; /* Program header offset */
	Elf64_Off e_shoff; /* Section header offset */
	Elf64_Word e_flags; /* Processor-specific flags */
	Elf64_Half e_ehsize; /* ELF header size */
	Elf64_Half e_phentsize; /* Size of program header entry */
	Elf64_Half e_phnum; /* Number of program header entries */
	Elf64_Half e_shentsize; /* Size of section header entry */
	Elf64_Half e_shnum; /* Number of section header entries */
	Elf64_Half e_shstrndx; /* Section name string table index */
} elf_header_t;

//The segment types stored in p_type in program header table
#define PT_NULL 0 
#define PT_LOAD 1 
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOOS 0x60000000
#define PT_HIOS 0x6FFFFFFF
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7FFFFFFF

//Segment Attributes for p_flags
#define PF_X 0x1 //Execute permission
#define PF_W 0x2 //Write Permission
#define PF_R 0x4 //Read Permission
#define PF_MASKDS 0x00FF0000 //flag bits are reserved for environment specific use
#define PF_MASKPROC 0xFF000000 //flag bits are reserved for process-specific use

typedef struct progam_header_table_entry {
	Elf64_Word p_type; /* Type of segment */
	Elf64_Word p_flags; /* Segment attributes */
	Elf64_Off p_offset; /* Offset in file */
	Elf64_Addr p_vaddr; /* Virtual address in memory */
	Elf64_Addr p_paddr; /* Reserved */
	Elf64_Xword p_filesz; /* Size of segment in file */
	Elf64_Xword p_memsz; /* Size of segment in memory */
	Elf64_Xword p_align; /* Alignment of segment */
} prog_header_entry_t;
