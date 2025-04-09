#pragma once
#include <cstdint>
#include <array>

class ZobristHashing
{
private:

	static std::array<std::array<uint64_t, 64>, 12> ZobristPieces;// 2d array: 64 squares for the 12 piece types



	constexpr void InitializeKeys()
	{

	}


public:

};