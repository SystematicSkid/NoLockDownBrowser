#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <cstdint>
#include <fstream>

namespace Unpacker
{
	uint8_t* file;
	size_t file_length;
}

/* Custom Classes */
#include "./chunk.h"
#include "./encryption.h"

namespace Unpacker
{
	std::vector<Chunk*> decrypted_chunks;

	void write_to_file(std::string name, uint8_t* data, size_t  len)
	{
		std::ofstream output_file(name.c_str(), std::ios::binary);
		for (int i = 0; i < len; i++)
			output_file.put(data[i]);
	}

	void decrypt()
	{
		if (!file)
		{
			printf("File is null!\n");
			return;
		}

		Unpacker::populate_chunk_data();

		int current_bytes_written = 0;
		BYTE current_bytes[8];
		std::vector<BYTE> decrypted_bytes;
		for (int i = 0; i < 10000; i++)
		{
			auto* chunk = get_next_chunk();
			for (int k = 0; k < chunk->size; k++)
			{
				if (current_bytes_written == 8)
				{
					UpdateKeys();
					*(DWORD*)(current_bytes) ^= shift_key;
					*(DWORD*)(current_bytes + 4) ^= instruction_key;
					for (auto dec_byte : current_bytes)
						decrypted_bytes.push_back(dec_byte);
					ZeroMemory(current_bytes, 8);
					current_bytes_written = 0;
				}
				current_bytes[current_bytes_written++] = chunk->data[k];

			}
		}
		printf("Decrypted %i bytes!\n", decrypted_bytes.size());
		Unpacker::reset_chunks();
		int byte_idx = 0;
		std::uintptr_t last_write_address = 0x0;
		for (int i = 0; i < 10000; i++)
		{
			auto* chunk = get_next_chunk();
			for (int k = 0; k < chunk->size; k++)
			{
				auto decrypted_byte = decrypted_bytes[byte_idx++];
				chunk->decrypted_data[k] = decrypted_byte;
			}
			PVOID current_write_address = PVOID((std::uintptr_t)file + chunk->offset);
			memcpy(current_write_address, chunk->decrypted_data, chunk->size);
			if (last_write_address)
				FillMemory((PVOID)(last_write_address + 1), (((std::uintptr_t)current_write_address - 1) - (std::uintptr_t)last_write_address), 0x00);
			last_write_address = (std::uintptr_t)current_write_address + chunk->size;
			//printf("Writing chunk 0x%p\n", (std::uintptr_t)current_write_address - (std::uintptr_t)file);
		}
		
		write_to_file("LockDownBrowser_unpacked.exe", file, file_length);
	}
}