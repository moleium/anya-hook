#ifndef ANYA_HOOK_HPP
#define ANYA_HOOK_HPP

#include <Windows.h>
#include <cstddef>
#include <cstdint>

#include "hde/hde32_disasm.hpp"

class Hook {
public:
    Hook();
    void set_memory_protect(uintptr_t address, size_t size, DWORD protect);
    void detour(const uintptr_t to_hook, const uintptr_t to_replace, const size_t length);
    uintptr_t hook(const uintptr_t to_hook, const uintptr_t to_replace, int32_t num_nops);
    void unhook(uintptr_t to_unhook);
    void pause(uintptr_t to_pause);
    void resume(uintptr_t to_resume);

private:
    uint8_t* function_original;
    uint8_t* function_temp;
    size_t function_size;
};

#endif // ANYA_HOOK_HPP