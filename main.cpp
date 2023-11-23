#include <iostream>
#include <Windows.h>
#include "lib/anya_hook.hpp"

typedef void(__stdcall* MsgBoxA_t)(HWND, LPCSTR, LPCSTR, UINT);
MsgBoxA_t OrigMsgBoxA;

void __stdcall HookedMsgBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    printf("> (%s) %s\n", lpCaption, lpText);
    OrigMsgBoxA(hWnd, lpText, lpCaption, uType);
}

int main() {
    Hook hook;
    HMODULE u32 = GetModuleHandleA("user32.dll");
    if (!u32) return 1;

    uintptr_t addr = (uintptr_t)GetProcAddress(u32, "MessageBoxA");
    if (!addr) return 1;

    uintptr_t HookedMsgBoxAAddr = reinterpret_cast<uintptr_t>(&HookedMsgBoxA);
    OrigMsgBoxA = reinterpret_cast<MsgBoxA_t>(hook.hook(addr, HookedMsgBoxAAddr, 0));

    MessageBoxA(NULL, "This will be intercepted", "Message", MB_OK);

    hook.unhook(addr);
    MessageBoxA(NULL, "This will not be intercepted", "Message", MB_OK);

    return 0;
}