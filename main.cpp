// MessageBoxA Trampoline Hook
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include "hooking_lib/anya_hook.hpp"

std::uintptr_t original_messageboxA = 0;
anya_hook context{};

// this part was written by my girlfriend
std::int32_t __stdcall messagebox_hook(HWND hwnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    printf("[+] HOOKED: %s\n", lpText);

    const auto orig_messagebox = reinterpret_cast<std::int32_t(__stdcall*)(HWND, LPCTSTR, LPCTSTR, UINT)>(original_messageboxA)
        (hwnd, lpText, lpCaption + 1, uType);

    return orig_messagebox;
}

// this part was also written by my girlfriend but I modified some parts
void main()
{
    SetConsoleTitleA("cabeza inteligente idiota");

    // grab user32 since it has the messagebox
    const auto user32 = GetModuleHandleA("user32.dll");

    // grab messagebox from user32
    const auto messagebox_sub = reinterpret_cast<std::uintptr_t>(GetProcAddress(user32, "MessageBoxA"));

    // lets run a normal messagebox that *wont* get logged
    MessageBoxA(0, "heheheha this was a test", "Test", 0);

    // hook da function heheheha
    original_messageboxA = context.hook(messagebox_sub, reinterpret_cast<std::uintptr_t>(&messagebox_hook));

    // lets run a normal messagebox to see if its logged
    MessageBoxA(0, "heheheha this was logged", "Test", 0);

    context.yield(messagebox_sub);
    MessageBoxA(0, "heheheha this was not logged", "Test", 0);

    context.resume(messagebox_sub);
    MessageBoxA(0, "GRRRR now this was logged", "Test", 0);

    context.unhook(messagebox_sub);
    MessageBoxA(0, "GRRRR x2 the hooks gone", "Test", 0);
}
