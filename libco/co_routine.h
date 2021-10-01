#pragma once

#include <stdint.h>

struct stCoRoutine_t;
struct stShareStack_t;
struct stCoEpoll_t;

struct stCoRoutineAttr_t {
    int stack_size = 128 * 1024;
    stShareStack_t* shared_stack = nullptr;
}__attribute__ ((packed));

struct stCoRoutineEnv_t {
    stCoRoutine_t* pCallStack[128];
    int iCallStackSize;
    stCoEpoll_t* pEoll;

    // shared stack
    stCoRoutine_t* pending_co;
	stCoRoutine_t* occupy_co;
};


using pfn_co_routine_t = void*(*)(void*);


int co_create(stCoRoutine_t** co, const stCoRoutineAttr_t* attr, pfn_co_routine_t pfn, void* arg);
void co_resume(stCoRoutine_t* co);
void co_yield(stCoRoutine_t* co);
void co_yield_ct();            
void co_reset(stCoRoutine_t* co);