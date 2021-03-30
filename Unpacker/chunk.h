#pragma once

namespace Unpacker
{
	struct Chunk
	{
		DWORD offset; // File relative offset
		DWORD size; // Size of chunk
		BYTE* data;
		BYTE* decrypted_data;

		Chunk(DWORD offset, DWORD size) : offset(offset), size(size)
		{
			data = (BYTE*)malloc(size);
			decrypted_data = (BYTE*)malloc(size);
			memcpy(data, PVOID((std::uintptr_t)file + offset), size);
			ZeroMemory(decrypted_data, size);
		}

		~Chunk()
		{
			delete data;
		}
	};

	size_t number_of_chunks = 0;

	std::vector<BYTE> chunk_data;
	void populate_chunk_data()
	{
		printf("Populating chunk data...\n");
		BYTE last_byte = 0x0;
		std::uintptr_t table = (std::uintptr_t)(Unpacker::file)+0x6768A5;
		int idx = 0;
		while (last_byte != 0xFF)
		{
			last_byte = *(BYTE*)(table + idx++);
			chunk_data.push_back(last_byte);
		}
		printf("Chunk Data: %i\n", chunk_data.size());
		number_of_chunks = (chunk_data.size() - 2) / 2;
	}

	DWORD chunk_idx = 2; // skip 128 & 0x20 -> Turns into 0x1000
	DWORD current_address = 0x400; // start of .text

	Chunk* get_next_chunk()
	{
		DWORD offset = chunk_data[chunk_idx + 1];
		DWORD size = chunk_data[chunk_idx];
		//printf("Chunk %i: Offset: 0x%p -> Size: 0x%p\n", chunk_idx / 2, current_address, size);
		auto file_offset = current_address;
		current_address += (offset + size);
		chunk_idx += 2;

		return new Chunk(file_offset, size);
	}

	void reset_chunks()
	{
		chunk_idx = 2;
		current_address = 0x400;
	}
}