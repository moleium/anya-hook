#pragma once
#include <Windows.h>

#include <iostream>
#include <cstdint>
#include <cstddef>

class anya_hook
{
public:
	std::uintptr_t target_function; // detour

	std::uint8_t* function_o; // old
	std::uint8_t* function_t; // backup

	std::size_t function_size;

public:
	explicit anya_hook();

	void detour(const std::uintptr_t to_hook, const std::uintptr_t to_replace, const std::size_t length);
	std::uintptr_t hook(const std::uintptr_t to_hook, const std::uintptr_t to_replace, std::int32_t instr_nops = 0);

	void unhook(std::uintptr_t to_unhook);
	void yield(const std::uintptr_t to_yield);
	void resume(const std::uintptr_t to_resume);
};
