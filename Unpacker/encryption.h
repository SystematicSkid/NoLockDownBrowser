#pragma once

namespace Unpacker
{

	DWORD instruction_key = 0xE2DB441E; // Used for second half of instructions
	DWORD shift_key = 0x83FE6C8; // Used for first half of instructions
	DWORD offset_key = 0x9E3779B9;
	BYTE secret_key[] =
	{
		0x19, 0x93, 0x6C, 0xEE, 0xEB, 0xAD, 0x32, 0x4D, 0xBA, 0x2E, 0xC8, 0xBF, 0xAC, 0x7F, 0x98, 0x3C
	};
	DWORD unknown_key = 0x0;


	void UpdateKeys()
	{
		unknown_key = 0;
		for (int i = 0; i < 64; i++)
		{
			shift_key += (*(DWORD*)(secret_key + 4 * (unknown_key & 3)) + unknown_key) ^ (instruction_key + ((instruction_key >> 5) ^ (16 * instruction_key)));
			unknown_key += offset_key;
			instruction_key += (*(DWORD*)(secret_key + 4 * ((unknown_key >> 0xB) & 3))
				+ unknown_key) ^ (shift_key
					+ ((shift_key >> 5) ^ (16 * shift_key)));
		}
	}
}