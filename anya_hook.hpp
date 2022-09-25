#pragma once
#include <Windows.h>

#include <iostream>
#include <cstdint>
#include <cstddef>

class anya_hook
{
public:
	std::uint8_t* function_o; // old
	std::uint8_t* function_t; // backup

	std::size_t function_size;

public:
	explicit anya_hook();

	void detour(const std::uintptr_t to_hook, const std::uintptr_t to_replace);
	std::uintptr_t hook(const std::uintptr_t to_hook, const std::uintptr_t to_replace);

	void unhook(std::uintptr_t to_unhook);
	void yield(const std::uintptr_t to_yield);
	void resume(const std::uintptr_t to_resume);
};
