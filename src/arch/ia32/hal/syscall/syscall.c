#include "common/include/data.h"
#include "common/include/memlayout.h"
#include "common/include/context.h"
#include "common/include/kdisp.h"
#include "common/include/syscall.h"
#include "hal/include/print.h"
#include "hal/include/mem.h"
#include "hal/include/lib.h"
#include "hal/include/cpu.h"
#include "hal/include/task.h"
#include "hal/include/kernel.h"
#include "hal/include/int.h"
#include "hal/include/syscall.h"


u32 syscall_proxy_entry = 0;


static void init_syscall_msr()
{
    u64 sysenter_cs = (u64)GDT_SELECTOR_CODE_K;
    u64 sysenter_esp = (u64)get_my_cpu_area_start_vaddr() + PER_CPU_STACK_TOP_OFFSET;
    u64 sysenter_eip = (u64)(ulong)&sysenter_handler;
    
    // Initialize Sysenter parameters
    msr_write(SYSENTER_MSR_CS, &sysenter_cs);
    msr_write(SYSENTER_MSR_ESP, &sysenter_esp);
    msr_write(SYSENTER_MSR_EIP, &sysenter_eip);
}

static int int_syscall_handler(struct int_context *context, struct kernel_dispatch_info *kdi)
{
    ulong num = context->context->esi;
    ulong param0 = context->context->edi;
    ulong param1 = context->context->eax;
    ulong param2 = context->context->edx;
    
    kdi->dispatch_type = kdisp_syscall;
    kdi->syscall.num = num;
    kdi->syscall.param0 = param0;
    kdi->syscall.param1 = param1;
    kdi->syscall.param2 = param2;
    
    return 1;
}


void init_syscall_mp()
{
    kprintf("Initializing syscall\n");
    init_syscall_msr();
}

void init_syscall()
{
    kprintf("Initializing syscall\n");
    
    syscall_proxy_entry = SYSCALL_PROXY_VADDR;
    
    memcpy(
        (void *)SYSCALL_PROXY_VADDR, sysenter_proxy_start_origin,
        (ulong)sysenter_proxy_end_origin - (ulong)sysenter_proxy_start_origin
    );
    
    init_syscall_msr();
    set_int_vector(INT_VECTOR_SYS_CALL, int_syscall_handler);
}

void asmlinkage save_context_sysenter(struct context *context)
{
    // Setup context
    context->esp = context->ecx;
    context->eip = context->edx;
    
    context->ss = GDT_SELECTOR_DATA_U;
    context->cs = GDT_SELECTOR_CODE_U;
    
    context->eflags = 0x200202;
    
    // Make sure we are in user mode
    int user_mode = *get_per_cpu(int, cur_in_user_mode);
    assert(user_mode);
    
    // Call the regular context save function
    save_context(context);
}

void asmlinkage sysenter_handler_entry(struct context *context)
{
    ulong num = context->esi;
    ulong param0 = context->edi;
    ulong param1 = context->eax;
    ulong param2 = context->edx;
    
    ulong ret_addr = 0;
    ulong ret_size = 0;
    int succeed = 0;
    
    int call_kernel = 1;
    
    if (num == SYSCALL_PING) {
        ret_addr = param0 + 1;
        succeed = 1;
        call_kernel = 0;
    }
    
    if (call_kernel) {
        struct kernel_dispatch_info kdispatch;
        kdispatch.context = context;
        kdispatch.dispatch_type = kdisp_syscall;
        kdispatch.syscall.num = num;
        kdispatch.syscall.param0 = param0;
        kdispatch.syscall.param1 = param1;
        kdispatch.syscall.param2 = param2;
        kernel_dispatch(&kdispatch);
        
        //kprintf("Syscall from user!\n");
    }
    
    else {
        // Prepare return value
        context->eax = succeed;
        context->esi = ret_addr;
        context->edi = ret_size;
    }
}

void set_syscall_return(struct context *context, int succeed, ulong return0, ulong return1)
{
    context->eax = succeed;
    context->esi = return0;
    context->edi = return1;
}
