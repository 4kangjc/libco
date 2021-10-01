#include "co_routine.h"
#include "coctx.h"
#include "co_routine_inner.h"

#include <cstring>

static __thread stCoRoutineEnv_t* gCoEnvPerThread = nullptr;

stStackMem_t* co_alloc_stackmem(unsigned int stack_size) {
    stStackMem_t* stack_mem = (stStackMem_t*)malloc(sizeof(stStackMem_t));
    stack_mem->occupy_co = nullptr;
    stack_mem->stack_size = stack_size;
    stack_mem->stack_buffer = (char*)malloc(stack_size);
    stack_mem->stack_bp = stack_mem->stack_buffer + stack_size;
    return stack_mem;
}

stCoRoutine_t* co_create_env(stCoRoutineEnv_t* env, const stCoRoutineAttr_t* attr, pfn_co_routine_t pfn, void* arg) {
    stCoRoutineAttr_t at;
    if (attr) {
        memcpy(&at, attr, sizeof(at));
    }
    if (at.stack_size <= 0) {
        at.stack_size = 128 * 1024;
    } else if (at.stack_size > 1024 * 1024 * 8) {
        at.stack_size = 1024 * 1024 * 8;
    }
    
    if (at.stack_size & 0xfff) {
        at.stack_size &= ~0xfff;
        at.stack_size += 0x1000;
    }

    stCoRoutine_t* lp = (stCoRoutine_t*)malloc(sizeof(stCoRoutine_t));
    memset(lp, 0, sizeof(stCoRoutine_t));

    lp->env = env;
    lp->pfn = pfn;
    lp->arg = arg;

    stStackMem_t* stack_mem = nullptr;
    if (at.shared_stack) {
        // TODO
    } else {
        stack_mem = co_alloc_stackmem(at.stack_size);
    }
    lp->stack_mem = stack_mem;
    lp->ctx.ss_sp = stack_mem->stack_buffer;
    lp->ctx.ss_size = stack_mem->stack_size;

    lp->cStart = 0;
    lp->cEnd = 0;
    lp->cIsMain = 0;
    lp->cEnableSysHook = 0;
    lp->cIsShareStack = at.shared_stack != nullptr;

    // lp->save_size = 0;
	// lp->save_buffer = NULL;

   return lp; 
}

void co_init_curr_thread_env() {
    gCoEnvPerThread = (stCoRoutineEnv_t*)calloc(1, sizeof(stCoRoutineEnv_t));
    stCoRoutineEnv_t* env = gCoEnvPerThread;

    env->iCallStackSize = 0;
    auto self = co_create_env(env, nullptr, nullptr, nullptr);
    self->cIsMain = 1;           // main coroutine

    env->pending_co = nullptr;   // ignore
    env->occupy_co = nullptr;    // ignore

    coctx_init(&self->ctx);

    env->pCallStack[env->iCallStackSize++] = self;

    // stCoEpoll_t* ev = AllocEpoll();    ToDo
    // SetEpoll(env, ev);    
}

stCoRoutineEnv_t* co_get_curr_thread_env() {
    return gCoEnvPerThread;
}


extern "C" {
    extern void coctx_swap(coctx_t*, coctx_t*) asm("coctx_swap");
};

// stCoRoutine_t* GetCurCo(stCoRoutineEnv_t* env) {

// }

void co_swap(stCoRoutine_t* curr, stCoRoutine_t* pending_co) {
    stCoRoutineEnv_t* env = co_get_curr_thread_env();

    //get curr stack sp
    char c;
    curr->stack_sp = &c;

    if (!pending_co->cIsShareStack) {
        env->pending_co = nullptr;
        env->occupy_co = nullptr;
    } else {

    }

    // swap context
    coctx_swap(&curr->ctx, &pending_co->ctx);

    // TODO
}

int co_create(stCoRoutine_t** ppco, const stCoRoutineAttr_t* attr, pfn_co_routine_t pfn, void* arg) {
    if (!co_get_curr_thread_env()) {
        co_init_curr_thread_env();
    }
    stCoRoutine_t* co = co_create_env(co_get_curr_thread_env(), attr, pfn, arg);
    *ppco = co;
    return 0;
}

void co_free(stCoRoutine_t* co) {
    if (!co->cIsShareStack) {
        free(co->stack_mem->stack_buffer);
        free(co->stack_mem);
    } else {

    }
    free(co);
}

static int CoRoutineFunc(stCoRoutine_t* co, void*) {
    if (co->pfn) {
        co->pfn(co->arg);
    }
    co->cEnd = 1;

    stCoRoutineEnv_t* env = co->env;
    co_yield_env(env);
    return 0;
}

void co_resume(stCoRoutine_t* co) {
    stCoRoutineEnv_t* env = co->env;
    stCoRoutine_t* lpCurrRoutine = env->pCallStack[env->iCallStackSize - 1];
    if (!co->cStart) {
        coctx_make(&co->ctx, (coctx_pfn_t)CoRoutineFunc, co, 0);
        co->cStart = 1;
    }
    env->pCallStack[env->iCallStackSize++] = co;
    co_swap(lpCurrRoutine, co);
}

void co_yield_env(stCoRoutineEnv_t* env) {
    stCoRoutine_t* last = env->pCallStack[env->iCallStackSize - 2];
    stCoRoutine_t* curr = env->pCallStack[env->iCallStackSize - 1];
    env->iCallStackSize--;
    co_swap(curr, last);
} 

void co_yield_ct() {
    return co_yield_env(co_get_curr_thread_env());
}

void co_yield(stCoRoutine_t* co) {
    return co_yield_env(co->env);
}