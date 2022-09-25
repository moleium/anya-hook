#include "anya_hook.hpp"
#include "hde32_disasm.hpp"

anya_hook::anya_hook()
{
    this->function_o = nullptr; // old
    this->function_t = nullptr; // backup

    this->function_size = 0;
}

// detour is basically used in making a certain instruction(s) jmp to a different location
// if its done wrong then it can cause so many things, so we make sure we "jmp back" to the original position after whatever we do
void anya_hook::detour(const std::uintptr_t to_hook, const std::uintptr_t to_replace, const std::size_t length)
{
    // let us read/write to memory as much as we want
    DWORD old_protect{0};

    VirtualProtect(reinterpret_cast<void*>(to_hook), length, PAGE_EXECUTE_READWRITE, &old_protect);

    // clone the original memory's contents, we will be using them for other functions (e.g yield)
    this->function_o = reinterpret_cast<std::uint8_t*>(std::malloc(length));

    for (auto i = 0u; i < length; i++)
       this->function_o[i] = *reinterpret_cast<std::uint8_t*>(to_hook + i);

    std::memset(reinterpret_cast<void*>(to_hook), 0x90, length);

    *reinterpret_cast<std::uint8_t*>(to_hook) = 0xE9;
    *reinterpret_cast<std::uintptr_t*>(to_hook + 1) = (to_replace - to_hook - 5);

    VirtualProtect(reinterpret_cast<void*>(to_hook), length, old_protect, &old_protect);
}

std::uintptr_t anya_hook::hook(const std::uintptr_t to_hook, const std::uintptr_t to_replace, std::int32_t instr_nops)
{
    // calculate the size of the function you're trying to hook
    auto at = to_hook; // we have to do it like this, otherwise HDE32 Disassembler fucks up
    auto nops = 0u;

    while (true)
    {
        hde32s disasm{0};
        hde32_disasm(reinterpret_cast<void*>(at), &disasm);

        at += disasm.len;
        nops += disasm.len;

        if (nops > 5)
        {
            if (instr_nops)
            {
                instr_nops--;
                continue;
            }

            break;
        }
    }

    nops -= 5;
    this->function_size = (nops + 5);
    
    // create our detour
    const auto detour = reinterpret_cast<std::uintptr_t>(VirtualAlloc(nullptr, this->function_size + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

    // copy the contents
    std::memmove(reinterpret_cast<void*>(detour), reinterpret_cast<void*>(to_hook), this->function_size);

    // jmp
    *reinterpret_cast<std::uint8_t*>(detour + this->function_size) = 0xE9;

    // add to jmp
    const auto rel_addr = (to_hook - detour - 5);
    *reinterpret_cast<std::uint32_t*>(detour + this->function_size + 1) = rel_addr;

    this->detour(to_hook, to_replace, this->function_size);
    return detour;
}

// unhook will completely erase whatever you've hooked the function with
// making the function return to its original form
void anya_hook::unhook(std::uintptr_t to_unhook)
{
    DWORD old_protect{0};

    VirtualProtect(reinterpret_cast<void*>(to_unhook), this->function_size, PAGE_EXECUTE_READWRITE, &old_protect);
    std::memcpy(reinterpret_cast<void*>(to_unhook), this->function_o, this->function_size);
    VirtualProtect(reinterpret_cast<void*>(to_unhook), this->function_size, old_protect, &old_protect);
    VirtualFree(reinterpret_cast<void*>(to_unhook), 0, MEM_FREE);

    std::free(this->function_o);
    to_unhook = 0;
}

// yield will suspend given *hooked* function
// meaning the hooked function will go back to its original form
// until you resume it
void anya_hook::yield(const std::uintptr_t to_yield)
{
    DWORD old_protect{0};

    VirtualProtect(reinterpret_cast<void*>(to_yield), this->function_size, PAGE_EXECUTE_READWRITE, &old_protect);
    this->function_t = reinterpret_cast<std::uint8_t*>(VirtualAlloc(nullptr, this->function_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

    std::memcpy(this->function_t, reinterpret_cast<void*>(to_yield), this->function_size);
    std::memcpy(reinterpret_cast<void*>(to_yield), this->function_o, this->function_size);

    VirtualProtect(reinterpret_cast<void*>(to_yield), this->function_size, old_protect, &old_protect);
}

// resume will resume given *hooked* function
// it will make your hook active again, so it can be useful
// only use this if you have a suspended hook
void anya_hook::resume(const std::uintptr_t to_resume)
{
    DWORD old_protect{0};

    VirtualProtect(reinterpret_cast<void*>(to_resume), this->function_size, PAGE_EXECUTE_READWRITE, &old_protect);
    std::memcpy(reinterpret_cast<void*>(to_resume), this->function_t, this->function_size);
    VirtualProtect(reinterpret_cast<void*>(to_resume), this->function_size, old_protect, &old_protect);

    VirtualFree(this->function_t, 0, MEM_RELEASE);
}
