#pragma once

typedef struct interrupt_context {
  uintptr_t ip;
  uint64_t cs;
  uint64_t flags;
  uintptr_t sp;
  uint64_t ss;
} __attribute__((packed)) interrupt_context_t;

////INTERNAL INTERUPTS

//INTERUPT VECTOR 0
__attribute__((interrupt))
void divide_error_handler(interrupt_context_t* ctx);

//INTERUPT VECTOR 1
__attribute__((interrupt))
void debug_exception_handler(interrupt_context_t* ctx);

//INTERUPT VECTOR 2
__attribute__((interrupt))
void nmi_interrupt_handler(interrupt_context_t* ctx);

//INTERUPT VECTOR 3
__attribute__((interrupt))
void breakpoint_handler(interrupt_context_t* ctx);

//INTERUPT VECTOR 4
__attribute__((interrupt))
void overflow_interupt_handler(interrupt_context_t* ctx);

//INTERUPT VECTOR 5
__attribute__((interrupt))
void bound_range_exc_handler(interrupt_context_t* ctx);

//INTERUPT VECTOR 6
__attribute__((interrupt))
void invalid_opcode_handler(interrupt_context_t* ctx);

//INTERUPT VECTOR 7
__attribute__((interrupt))
void device_not_available(interrupt_context_t* ctx);

//INTERUPT VECTOR 8
__attribute__((interrupt))
void double_fault_handler(interrupt_context_t* ctx, uint64_t ec);

//INTERUPT VECTOR 9
__attribute__((interrupt))
void co_pro_seg_overrun(interrupt_context_t* ctx);

//INTERUPT VECTOR 10
__attribute__((interrupt))
void invalid_tss_handler(interrupt_context_t* ctx, uint64_t ec);

//INTERUPT VECTOR 11
__attribute__((interrupt))
void seg_not_present(interrupt_context_t* ctx, uint64_t ec);

//INTERUPT VECTOR 12
__attribute__((interrupt))
void stack_seg_fault(interrupt_context_t* ctx, uint64_t ec);

//INTERUPT VECTOR 13
__attribute__((interrupt))
void gen_mem_protection(interrupt_context_t* ctx, uint64_t ec);

//INTERUPT VECTOR 14
__attribute__((interrupt))
void page_fault_handler(interrupt_context_t* ctx, uint64_t ec);

//INTERUPT VECTOR 16
__attribute__((interrupt))
void fpu_floating_point_error(interrupt_context_t* ctx);

//INTERUPT VECTOR 17
__attribute__((interrupt))
void alignment_check_handler(interrupt_context_t* ctx, uint64_t ec);

//INTERUPT VECTOR 18
__attribute__((interrupt))
void machine_check_handler(interrupt_context_t* ctx);

//INTERUPT VECTOR 19
__attribute__((interrupt))
void simd_floating_point_error(interrupt_context_t* ctx);

//INTERUPT VECTOR 20
__attribute__((interrupt))
void virtualization_exception(interrupt_context_t* ctx);

//INTERUPT VECTOR 21
__attribute__((interrupt))
void control_protection_exception(interrupt_context_t* ctx, uint64_t ec);

////EXTERNAL INTERRUPTS

//KEYBOARD INTERRUPT IRQ1_INTERRUPT (defined in pic.h)
__attribute__((interrupt))
void keyboard_handler(interrupt_context_t* ctx);

//SYSTEM CALL INTERRUPT (0X80) 
//interupt 0x80 invoked in syscall.s
int64_t syscall_handler(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5); 

