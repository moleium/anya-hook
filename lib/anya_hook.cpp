#include "anya_hook.hpp"

Hook::Hook()
    : function_original(nullptr), function_temp(nullptr), function_size(0) {}

void Hook::set_memory_protect(uintptr_t address, size_t size, DWORD protect) {
    DWORD old_protect;
    if (!VirtualProtect(reinterpret_cast<void*>(address), size, protect, &old_protect)) {
        throw std::runtime_error("Failed to set memory protection.");
    }
}

void Hook::detour(const uintptr_t to_hook, const uintptr_t to_replace, const size_t length) {
    set_memory_protect(to_hook, length, PAGE_EXECUTE_READWRITE);

    function_original = reinterpret_cast<uint8_t*>(std::malloc(length));

    if (function_original == nullptr) {
        throw std::runtime_error("function_original is a NULL pointer.");
    }

    std::memcpy(function_original, reinterpret_cast<const void*>(to_hook), length);
    std::memset(reinterpret_cast<void*>(to_hook), 0x90, length);

    *reinterpret_cast<uint8_t*>(to_hook) = 0xE9;
    *reinterpret_cast<uintptr_t*>(to_hook + 1) = (to_replace - to_hook - 5);

    set_memory_protect(to_hook, length, PAGE_EXECUTE_READ);
}

uintptr_t Hook::hook(const uintptr_t to_hook, const uintptr_t to_replace, int32_t num_nops = 0) {
    function_size = 0;

    uintptr_t at = to_hook;
    while (true) {
        hde32s disasm{ 0 };
        hde32_disasm(reinterpret_cast<void*>(at), &disasm);

        if (function_size + disasm.len > 5) {
            if (num_nops) {
                num_nops--;
            }
            else {
                break;
            }
        }

        at += disasm.len;
        function_size += disasm.len;
    }

    const uintptr_t detour_address = reinterpret_cast<uintptr_t>
        (VirtualAlloc(nullptr, function_size + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

    if (!detour_address) {
        throw std::runtime_error("Failed to allocate memory for detour.");
    }

    std::memcpy(reinterpret_cast<void*>(detour_address), reinterpret_cast<const void*>(to_hook), function_size);
    *reinterpret_cast<uint8_t*>(detour_address + function_size) = 0xE9;

    const uintptr_t rel_addr = (to_hook - detour_address - 5);
    *reinterpret_cast<uint32_t*>(detour_address + function_size + 1) = rel_addr;

    detour(to_hook, to_replace, function_size);

    set_memory_protect(detour_address, function_size + 5, PAGE_EXECUTE_READ);

    return detour_address;
}

void Hook::unhook(uintptr_t to_unhook) {
    if (to_unhook == 0) {
        throw std::runtime_error("Invalid memory address.");
    }

    set_memory_protect(to_unhook, function_size + 5, PAGE_EXECUTE_READWRITE);
    std::memcpy(reinterpret_cast<void*>(to_unhook), function_original, function_size);
    set_memory_protect(to_unhook, function_size + 5, PAGE_EXECUTE_READ);

    VirtualFree(reinterpret_cast<void*>(to_unhook), 0, MEM_RELEASE);

    std::free(function_original);
    to_unhook = 0;
}

void Hook::pause(uintptr_t to_pause) {
    set_memory_protect(to_pause, function_size, PAGE_EXECUTE_READWRITE);
    function_temp = reinterpret_cast<uint8_t*>
        (VirtualAlloc(nullptr, function_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

    if (!function_temp) {
        throw std::runtime_error("Failed to allocate memory for pause.");
    }

    std::memcpy(function_temp, reinterpret_cast<const void*>(to_pause), function_size);
    std::memcpy(reinterpret_cast<void*>(to_pause), function_original, function_size);

    set_memory_protect(to_pause, function_size, PAGE_EXECUTE_READ);
}

void Hook::resume(uintptr_t to_resume) {
    set_memory_protect(to_resume, function_size, PAGE_EXECUTE_READWRITE);
    std::memcpy(reinterpret_cast<void*>(to_resume), function_temp, function_size);
    set_memory_protect(to_resume, function_size, PAGE_EXECUTE_READ);

    if (!VirtualFree(function_temp, 0, MEM_RELEASE)) {
        throw std::runtime_error("Failed to free memory.");
    }
}