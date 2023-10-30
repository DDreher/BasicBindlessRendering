#include "Core/FileIO.h"

#include <fstream>

std::vector<uint8> FileIO::ReadFile(const std::string& filename)
{
    // ate: Start reading at the end of the file -> we can use the read position to determine the file size and allocate a buffer
    // binary : Read the file as binary file (avoid text transformations)
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t file_size = (size_t)file.tellg();
    std::vector<uint8> buffer(file_size);

    file.seekg(0);
    file.read((char*) buffer.data(), file_size);
    file.close();

    return buffer;
}
