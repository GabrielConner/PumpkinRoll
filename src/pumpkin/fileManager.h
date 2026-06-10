#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_FILE_MANAGER_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_FILE_MANAGER_H

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

}; // namespace pumpkin

#endif