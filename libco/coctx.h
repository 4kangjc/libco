#pragma once

#include <stdlib.h>

using coctx_pfn_t = void*(*)(void* s1, void* s2);

struct coctx_param_t {
    const void* s1;
    const void* s2;
};

struct coctx_t {
#if defined(__i386__)
    void* regs[8];
#else 
    void* regs[14];
#endif
    size_t ss_size;
    char* ss_sp;
};

int coctx_init(coctx_t* ctx);
int coctx_make(coctx_t* ctx, coctx_pfn_t pfn, const void* s1, const void* s2);
