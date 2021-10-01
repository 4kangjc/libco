#include "coctx.h"
#include <cstdio>
#include <cstring>

//----- --------
// 32 bit
// | regs[0]: ret |
// | regs[1]: ebx |
// | regs[2]: ecx |
// | regs[3]: edx |
// | regs[4]: edi |
// | regs[5]: esi |
// | regs[6]: ebp |
// | regs[7]: eax |  = esp
enum {
    kEIP = 0, 
    kEBP = 6, 
    kESP = 7,
};

//-------------
// 64 bit
//low | regs[0] : r15 |
//    | regs[1] : r14 |
//    | regs[2] : r13 |
//    | regs[3] : r12 |
//    | regs[4] : r9  |
//    | regs[5] : r8  |
//    | regs[6] : rbp |
//    | regs[7] : rdi |  // 储存函数的第一个参数地址
//    | regs[8] : rsi |  // 储存函数的第二个参数地址
//    | regs[9] : ret |  // ret func addr
//    | regs[10]: rdx |
//    | regs[11]: rcx |
//    | regs[12]: rbx |
//hig | regs[13]: rsp | // 栈顶寄存器
enum {
    kRDI = 7, 
    kRSI = 8, 
    kRETAddr = 9,
    kRSP = 13,
};

extern "C" {
    extern void coctx_swap(coctx_t*, coctx_t*) asm("coctx_swap");
};

#if defined(__i386__)

int coctx_init(coctx_t* ctx) {
    memset(ctx, 0, sizeof(*ctx));
    return 0;
}

#elif defined(__x86_64__)

int coctx_init(coctx_t* ctx) {
    memset(ctx, 0, sizeof(*ctx));
    return 0;
}

int coctx_make(coctx_t* ctx, coctx_pfn_t pfn, const void* s1, const void* s2) {
    char* sp = ctx->ss_sp + ctx->ss_size - sizeof(void*);
    sp = (char*)((unsigned long)sp & -16LL);        // 16字节对齐

    memset(ctx->regs, 0, sizeof(ctx->regs));
    void** ret_addr = (void**)(sp);
    *ret_addr = (void*)pfn;
    
    ctx->regs[kRSP] = sp;
    ctx->regs[kRETAddr] = (char*)pfn;
    ctx->regs[kRDI] = (char*)s1;
    ctx->regs[kRSI] = (char*)s2;
    return 0;
}


#endif