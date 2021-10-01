#include "libco/co"
#include <iostream>

void* test(void* argc) {
    std::cout << "hello libco\n";
    co_yield_ct();
    std::cout << "from main\n";
    return nullptr;
}

int main() {
    std::cout << "begin...\n";
    stCoRoutine_t* co = (stCoRoutine_t*)malloc(sizeof(stCoRoutine_t));
    co_create(&co, nullptr, test, nullptr);
    co_resume(co);
    std::cout << "yield to test\n";
    co_resume(co);
    co_free(co);
    std::cout << "end!\n";

    return 0;
}