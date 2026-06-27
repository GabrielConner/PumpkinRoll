#include "pPack/fileInterface.h"

#include <fstream>
#include <filesystem>

namespace pPack {

bool DefaultFileRead(std::string const& location, bool relative, bool binary, FileHandle& handle) {
  if (!std::filesystem::exists(location)) return false;

  std::ifstream stream = std::ifstream(location.c_str(), (binary ? std::ios::binary : 0) | std::ios::ate);
  if (!stream.is_open()) return false;

  size_t length = stream.tellg();
  stream.seekg(0);
  char* data = binary ? (char*)malloc(length) : (char*)calloc(length, sizeof(char));
  if (!data) return false;

  stream.read(data, length);
  
  handle.location = location;
  handle.data = data;
  handle.packetSize = length;
  handle.offset = 0;
  handle.totalSize = length;

  stream.close();

  return true;
}

}; // namespace pPack