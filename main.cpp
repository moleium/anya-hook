#include <iostream>
#include <Windows.h>
#include "anya_hook.hpp"

// Function prototypes
typedef void(__stdcall* MessageBoxA_t)(HWND, LPCSTR, LPCSTR, UINT);
MessageBoxA_t OriginalMessageBoxA;

void __stdcall HookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    std::cout << "[uwu] Logged MessageBoxA: " << lpText << std::endl;
    OriginalMessageBoxA(hWnd, lpText, lpCaption, uType);
}

int main() {
    Hook hook;
    HMODULE user32Module = GetModuleHandle("user32.dll");

    if (!user32Module) {
        std::cerr << "Failed to get user32.dll module handle" << std::endl;
        return 1;
    }

    uintptr_t MessageBoxAAddr = reinterpret_cast<uintptr_t>(GetProcAddress(user32Module, "MessageBoxA"));

    if (!MessageBoxAAddr) {
        std::cerr << "Failed to get MessageBoxA address" << std::endl;
        return 1;
    }

    uintptr_t HookedMessageBoxAAddr = reinterpret_cast<uintptr_t>(&HookedMessageBoxA);
    OriginalMessageBoxA = reinterpret_cast<MessageBoxA_t>(hook.hook(MessageBoxAAddr, HookedMessageBoxAAddr));

    MessageBoxA(NULL, "Original MessageBoxA", "Test", MB_OK);

    hook.yield(MessageBoxAAddr);
    MessageBoxA(NULL, "This wont be logged", "Test", MB_OK);

    hook.resume(MessageBoxAAddr);
    MessageBoxA(NULL, "This was logged", "Test", MB_OK);

    hook.unhook(MessageBoxAAddr);
    MessageBoxA(NULL, "Original MessageBoxA after unhook", "Test", MB_OK);

    return 0;
}
