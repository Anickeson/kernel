#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#include "kprint.h"
#include "stivale2.h"
#include "handlers.h"
#include "util.h"
#include "pic.h"
#include "port.h"
#include "scan_to_ascii.h"
#include "circ_buff.c"

//INTERNAL INTERRUPTS
__attribute__((interrupt))
void divide_error_handler(interrupt_context_t* ctx){
  kprintf("Divide Error Occured\n");
  halt();
}

__attribute__((interrupt))
void debug_exception_handler(interrupt_context_t* ctx){
  kprintf("Debug Exception Occured\n");
  halt();
}

__attribute__((interrupt))
void nmi_interrupt_handler(interrupt_context_t* ctx){
  kprintf("NMI interrupt\n");
  halt();
}

__attribute__((interrupt))
void breakpoint_handler(interrupt_context_t* ctx){
  kprintf("Break point reached\n");
  halt();
}

__attribute__((interrupt))
void overflow_interupt_handler(interrupt_context_t* ctx){
  kprintf("Overflow Error Occurred\n");
  halt();
}

__attribute__((interrupt))
void bound_range_exc_handler(interrupt_context_t* ctx){
  kprintf("Bounded range exceeded\n");
  halt();
}

__attribute__((interrupt))
void invalid_opcode_handler(interrupt_context_t* ctx){
  kprintf("OPcode invalid\n");
  halt();
}

__attribute__((interrupt))
void device_not_available(interrupt_context_t* ctx){
  kprintf("Device not available\n");
  halt();
}

__attribute__((interrupt))
void double_fault_handler(interrupt_context_t* ctx, uint64_t ec){
  kprintf("Double fault occurred\n");
  halt();
}

__attribute__((interrupt))
void co_pro_seg_overrun(interrupt_context_t* ctx){
  kprintf("Coprocessor segment overrun\n");
  halt();
}

__attribute__((interrupt))
void invalid_tss_handler(interrupt_context_t* ctx, uint64_t ec){
  kprintf("Task Switch or TSS access ocurred\n");
  halt();
}

__attribute__((interrupt))
void seg_not_present(interrupt_context_t* ctx, uint64_t ec){
  kprintf("Segment not present\n");
  halt();
}

__attribute__((interrupt))
void stack_seg_fault(interrupt_context_t* ctx, uint64_t ec){
  kprintf("Stack segmentation fault\n");
  halt();
}

__attribute__((interrupt))
void gen_mem_protection(interrupt_context_t* ctx, uint64_t ec){
  kprintf("General memory fault protection\n");
  halt();
}

__attribute__((interrupt))
void page_fault_handler(interrupt_context_t* ctx, uint64_t ec){
  kprintf("Page fault occured\n");
  halt();
}

__attribute__((interrupt))
void fpu_floating_point_error(interrupt_context_t* ctx){
  kprintf("x86 FPU Floating point exception occurred\n");
  halt();
}

__attribute__((interrupt))
void alignment_check_handler(interrupt_context_t* ctx, uint64_t ec){
  kprintf("Alignment Check occurred\n");
  halt();
}

__attribute__((interrupt))
void machine_check_handler(interrupt_context_t* ctx){
  kprintf("Alignment Check occurred\n");
  halt();
}

__attribute__((interrupt))
void simd_floating_point_error(interrupt_context_t* ctx){
  kprintf("SIMD Floating Point exception occurred\n");
  halt();
}

__attribute__((interrupt))
void virtualization_exception(interrupt_context_t* ctx){
  kprintf("Virtualization Exception Occurred\n");
  halt();
}

__attribute__((interrupt))
void control_protection_exception(interrupt_context_t* ctx, uint64_t ec){
  kprintf("Virtualization Exception Occurred\n");
  halt();
}


//EXTERNAL INTERRUPTS

//global shift key flag
int shift_down = 0;

//this handler can handle all characters and can capitalize characters when the shift key is down
//cannot recognize capitals or special characters yet
__attribute__((interrupt))
void keyboard_handler(interrupt_context_t* ctx){
  //recieving the scan code
	uint64_t key_pressed = inb(0x60);

  //tracking the shift key both up and down
	if (key_pressed == 42){//up
    shift_down = 1;
  } else if (key_pressed == 170){//down
    shift_down = 0;

  //if below 128 then the scan code is in out scan code conversion table
  } else if (key_pressed < 128) {
    char ascii_conversion = kbd_US[key_pressed];

    //capitalizing if shift down and the scan code is alphabetical
    if (shift_down && ascii_conversion <= 122 && ascii_conversion >=97){
      ascii_conversion -= ('a' - 'A');
    }

    //printing to the terminal
		kprintf("%c", ascii_conversion);

    //write into the buffer //will need to finish buffer implementation before we get here
    write_to_buff(ascii_conversion);
	}

  outb(PIC1_COMMAND, PIC_EOI);
}
