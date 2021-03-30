#include "./unpacker.h"

int main(int argv, char** argc)
{
	if (argv < 2)
	{
		printf("Please drag & drop the file onto this application!");
		return 0;
	}

	char* file_path = argc[1];
	std::ifstream fl(file_path, std::ios::binary);
	/* Read file into our global var */
	fl.seekg(0, std::ios::end);
	size_t length = fl.tellg();
	Unpacker::file = new uint8_t[length];
	Unpacker::file_length = length;
	fl.seekg(0, std::ios::beg);
	fl.read((char*)Unpacker::file, length);
	fl.close(); /* Close file */

	/* Run the decrypter */
	Unpacker::decrypt();

	/* Pause */
	std::system("PAUSE");
}