#pragma once

#include "co_routine.h"
#include "coctx.h"

struct stStackMem_t {
    stCoRoutine_t* occupy_co;
    int stack_size;
    char* stack_bp;             // stack_buffer + stack_size
    char* stack_buffer;
};

struct stShareStack_t {

};

struct stCoRoutine_t {
    stCoRoutineEnv_t* env;
    pfn_co_routine_t pfn;
    void* arg;
    coctx_t ctx;

    char cStart;
    char cEnd;
    char cIsMain;
    char cEnableSysHook;
    char cIsShareStack;

    void* pvEnv;

    stStackMem_t* stack_mem;

    char* stack_sp; 

    //...
};

void co_init_curr_thread_env();
stCoRoutineEnv_t* co_get_curr_thread_env();

void co_free(stCoRoutine_t* co);
void co_yield_env(stCoRoutineEnv_t* env);