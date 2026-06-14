#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_FILE_MANAGER_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_FILE_MANAGER_H

#include "pPack/fileInterface.h"
#include <string>

namespace pumpkin {

struct FileData {
  void* data = 0;
  size_t size = 0;
  bool shouldDelete = false;

  void Delete() { free(data); data = nullptr; size = 0;}
};

int LoadFile(std::string filePath, bool pack, bool cache, bool relative, bool binary=true);
void ForgetFile(std::string path);
FileData ReadFile(std::string path, bool binary=true);


bool OpenFileFunc(std::string const& location, bool relative, bool binary, ::pPack::FileHandle& handle);
void CloseFileFunc(::pPack::FileHandle& handle);

}; // namespace pumpkin

#endif