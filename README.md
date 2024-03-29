# Trampoline Hook Library
Lightweight and fast minimal x86 hooking library with useful features.

## What is a trampoline hook?
Trampoline hook is a technique to hook a function by replacing the first few bytes with a jump to your hook function, which gets called before jumping back to the original function.

<img src="http://jbremer.org/wp-posts/ah-trampoline.png">

## Features
- Hooking Functions 
- Unhook Functions
- Puase Hooking
- Resume Hooking
- Quick, Simple and Easy to use


## Usage
Create hook context
```
Hook context{};
```

Define original function and hook function
```cpp
std::uintptr_t original_function = 0;

std::int32_t __stdcall hook_function()
{
    // Your hook code here
    
    const auto original = reinterpret_cast<std::int32_t(__stdcall*)()>(original_function)();
    
    // Optionally, you can add code to run after the original function
    
    return original;
}
```
**NOTE:** Function parameters and return type must be the same as the original function.

Grab the original function
```cpp
const auto module = GetModuleHandleA("module.dll");
const auto function = reinterpret_cast<std::uintptr_t>(GetProcAddress(module, "function_name"));
```

Hooking libray options
```cpp
// Hook
const auto original = context.hook(function, reinterpret_cast<std::uintptr_t>(&hook_function));

// Unhook function
context.unhook(function);

// Pause hook
context.pause(function);

// Resume hook
context.resume(function);
```

# License
MIT. See [LICENSE](LICENSE)
