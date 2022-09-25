# Trampoline Hook Library
Minimal hooking library with few useful features.

## What is trampoline hook?
Trampoline hook is a hooking technique that allows you to hook a function by overwriting the first few bytes of the function with a jump to your hook function. so it will call your hook function and then jump back to the original function.
<img src="http://jbremer.org/wp-posts/ah-trampoline.png">

## Features
- Hooking Functions 
- Unhooking Functions
- Suspend Hooking
- Resume Hooking
- Quick, Simple and Easy to use


## Usage
Include required headers
```cpp
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include "hooking_lib/anya_hook.hpp"
```

Define original function and hook function
```cpp
std::uintptr_t original_messageboxA = 0;

std::int32_t __stdcall hook_messageboxA(HWND hwnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    printf("[+] HOOKED: %s\n", lpText);

    const auto orig_messagebox = reinterpret_cast<std::int32_t(__stdcall*)(HWND, LPCTSTR, LPCTSTR, UINT)>(original_messageboxA)
        (hwnd, lpText, lpCaption + 1, uType);

    return orig_messagebox;
}
```
**NOTE:** Function parameters and return type must be the same as the original function.

Hook the function
```cpp
original_messageboxA = anya_hook::hook_function(MessageBoxA, hook_messageboxA);
```

Unhook the function
```cpp
anya_hook::unhook_function(MessageBoxA, original_messageboxA);
```

Suspend the hook
```cpp
anya_hook::suspend_hook(MessageBoxA);
```

Resume the hook
```cpp
anya_hook::resume_hook(MessageBoxA);
```

## Full Example
```cpp
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include "hooking_lib/anya_hook.hpp"

std::uintptr_t original_messageboxA = 0;

std::int32_t __stdcall hook_messageboxA(HWND hwnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    printf("[+] HOOKED: %s\n", lpText);

    const auto orig_messagebox = reinterpret_cast<std::int32_t(__stdcall*)(HWND, LPCTSTR, LPCTSTR, UINT)>(original_messageboxA)
        (hwnd, lpText, lpCaption + 1, uType);

    return orig_messagebox;
}

int main()
{
	original_messageboxA = anya_hook::hook_function(MessageBoxA, hook_messageboxA);
	MessageBoxA(NULL, "This was logged by the hook", "Hooked", MB_OK);
	anya_hook::unhook_function(MessageBoxA, original_messageboxA);
	MessageBoxA(NULL, "This was not logged by the hook", "Unhooked", MB_OK);
	return 0;
}
```
So, when we call MessageBoxA function, it will call our hook function (in this case it will print the inputed message) and then jump back to the original function and continue the execution.<br>

**Like This:**
<img src="https://media.discordapp.net/attachments/990376903129038869/1023236458619863111/unknown.png">
It printed the message box content to the console when MessageBoxA was called.
