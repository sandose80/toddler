#include "common/include/data.h"
#include "common/include/proc.h"
#include "klibc/include/stdio.h"


no_opt struct thread_control_block *get_tcb()
{
    unsigned long tcb = 0;
    
//     __asm__ __volatile__
//     (
//         "mr %[reg], 13;"
//         : [reg]"=r"(tcb)
//         :
//     );
    
    //kprintf("TCB @ %p\n", tcb);
    
    return (struct thread_control_block *)tcb;
}

no_opt int do_syscall(unsigned long num, unsigned long param1, unsigned long param2, unsigned long *out1, unsigned long *out2)
{
//     register ulong __ppc32_reg_r3 __asm__("3") = num;
//     register ulong __ppc32_reg_r4 __asm__("4") = param1;
//     register ulong __ppc32_reg_r5 __asm__("5") = param2;
// 
//     __asm__ __volatile__
//     (
//         "sc;"
//         : "=r" (__ppc32_reg_r3), "=r" (__ppc32_reg_r4), "=r" (__ppc32_reg_r5)
//         : "r" (__ppc32_reg_r3), "r" (__ppc32_reg_r4), "r" (__ppc32_reg_r5)
//     );
//     
//     if (out1) {
//         *out1 = __ppc32_reg_r4;
//     }
//     
//     if (out2) {
//         *out2 = __ppc32_reg_r5;
//     }
    
    return 1;   //(int)__ppc32_reg_r3;
}
