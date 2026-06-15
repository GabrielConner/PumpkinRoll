#include "private/fileManager.h"
#include "pumpkin/fileManager.h"
#include "pumpkin/constants.h"
#include "private/pumpkinRoll.h"
#include "pumpkin/pumpkinRoll.h"

#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <assert.h>

using namespace ::pumpkin;
using namespace ::pumpkin_private;
using namespace ::pPack;

using namespace std::string_literals;

namespace {

struct LoadedFile {
  std::string pathTo = "";
  bool pack = false;
  bool cache = false;
  bool relative = false;
  FileData data = FileData();
};


struct FileManager {
  std::unordered_map<size_t, LoadedFile> loadedFiles = std::unordered_map<size_t, LoadedFile>();
  Pumpkin* pumpkin = nullptr;
}*files = nullptr;


}; // namespace




namespace pumpkin {


int LoadFile(std::string filePath, bool pack, bool cache, bool relative, bool binary) {
  if (files == nullptr) {
    pWarn("FileManager has not been started");
    return 1;
  }

  // Can't load an already loaded file
  if (files->loadedFiles.contains(_STRING_HASHER(filePath))) {
    pWarn(("File '"s + filePath + "' already loaded").c_str());
    return 2;
  }

  #ifdef PUMPKIN_ROLL_PROD
  relative = false;
  #endif

  // Prepend exe folder location if relative
  std::string pathTo = (relative ? files->pumpkin->exePath : "") + filePath;

  // Check for existence
  if (!std::filesystem::exists(pathTo)) {
    pWarn(("File '"s + filePath + "' could not be found").c_str());
    return 1;
  }

  LoadedFile file = LoadedFile(pathTo, pack, cache, relative);

  // Read if going to cache
  if (cache) {
    file.data.shouldDelete = false;

    std::fstream fileStream(pathTo, std::ios::in | (binary ? std::ios::binary : 0));
    if (!fileStream.is_open()) {
      pWarn(("Failed to open '"s + filePath + "'").c_str());
      return 1;
    }

    // Use noexcept version of file_size
    std::error_code ec;
    std::uintmax_t sizeInBytes = std::filesystem::file_size(pathTo, ec);
    if (sizeInBytes == -1) {
      pWarn("Failed to retrieve file size");
      fileStream.close();
      return 1;
    }
    file.data.size = sizeInBytes;


    // Allocate
    file.data.data = binary ?  malloc(sizeInBytes) : calloc(sizeInBytes, 1);
    if (file.data.data == nullptr) {
      pWarn("Failed to allocate space for file data");
      fileStream.close();
      return 1;
    }

    fileStream.read((char*)file.data.data, sizeInBytes);
    fileStream.close();
  }

  // Add to list of loaded files
  files->loadedFiles.insert({_STRING_HASHER(filePath), file});

  return 0;
}



void ForgetFile(std::string path) {
  if (files == nullptr) {
    pWarn("FileManager has not been started");
    return;
  }

  auto find = files->loadedFiles.find(_STRING_HASHER(path));
  if (find == files->loadedFiles.end()) return;
  if (find->second.cache) {
    find->second.data.Delete();
  }

  files->loadedFiles.erase(_STRING_HASHER(path));
}



FileData ReadFile(std::string path, bool binary) {
  if (files == nullptr) {
    pWarn("FileManager has not been started");
    return {};
  }

  auto find = files->loadedFiles.find(_STRING_HASHER(path));
  if (find == files->loadedFiles.end()) {
    pWarn(("File '"s + path + "' not loaded").c_str());
    return {};
  }
  auto const& file = find->second;

  // Already read file
  if (file.cache) {
    return find->second.data;
  }


  FileData data = {};
  data.shouldDelete = true;

  std::string pathTo = (file.relative ? files->pumpkin->exePath : "") + path;

  std::fstream fileStream(pathTo, std::ios::in | (binary ? std::ios::binary : 0));
  if (!fileStream.is_open()) {
    pWarn(("Failed to open '"s + path + "'").c_str());
    return {};
  }

  // Use noexcept version of file_size
  std::error_code ec;
  std::uintmax_t sizeInBytes = std::filesystem::file_size(pathTo, ec);
  if (sizeInBytes == -1) {
    pWarn("Failed to retrieve file size");
    fileStream.close();
    return {};
  }
  data.size = sizeInBytes;

  // Allocate
  data.data = binary ? malloc(sizeInBytes) : calloc(sizeInBytes, 1);
  if (data.data == nullptr) {
    pWarn("Failed to allocate space for file data");
    fileStream.close();
    return {};
  }

  fileStream.read((char*)data.data, sizeInBytes);
  fileStream.close();

  return data;
}






bool OpenFileFunc(std::string const& location, bool relative, bool binary, ::pPack::FileHandle& handle) {

  if (!files->loadedFiles.contains(_STRING_HASHER(location))) {
    if (LoadFile(location, true, false, relative, binary)) {
      return false;
    }
  }


  FileData data = ReadFile(location, binary);

  handle.location = location;
  handle.data = (char*)data.data;
  handle.packetSize = data.size;
  handle.offset = 0;
  handle.totalSize = data.size;
  return true;
}




void CloseFileFunc(::pPack::FileHandle& handle) {
  auto file = files->loadedFiles.find(_STRING_HASHER(handle.location));
  if (file == files->loadedFiles.end()) return;
  free(handle.data);
  handle = FileHandle();
}


}; // namespace pumpkin





namespace pumpkin_private {


bool FileManagerStart() {
  assert(files == nullptr);

  // Start file manager
  files = new FileManager();

  // Get pumpkin
  files->pumpkin = GetPumpkin();
  assert(files->pumpkin != nullptr);

  return true;
}


void FileManagerEnd() {
  assert(files != nullptr);

  // Delete all files that were cached
  for (auto& file : files->loadedFiles) {
    if (file.second.cache) {
      file.second.data.Delete();
    }
  }

  delete(files);
  files = nullptr;
}


}; // namespace pumpkin_private