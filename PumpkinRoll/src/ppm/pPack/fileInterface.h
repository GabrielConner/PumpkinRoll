#ifndef PPM_INCLUDE_PPACK_FILE_INTERFACE_H
#define PPM_INCLUDE_PPACK_FILE_INTERFACE_H

#include <string>

#ifdef PUMPKIN_ROLL_LIB
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif


namespace pPack {

struct FileHandle {
  std::string location = "";
  char* data = nullptr;
  size_t packetSize = 0;
  size_t offset = 0;
  size_t totalSize = 0;
};



/*
  Open a file handle to read from

  [IN] location: location of file to be opened
  [IN] relative: file location relative to exe or PATH
  [IN] binary: open the file in binary or normal mode
  [OUT] handle: the returned FileHandle, no change if fail

  returns: true/false on if file successfully opened
*/
typedef bool (*OpenReadFileFunc)(std::string const& location, bool relative, bool binary, FileHandle& handle);

/*
  Read an open file starting from a specific offset, such as large files that couldn't be read all at once

  [INOUT] handle: a handle to an open file that will also contain new read data
  [IN] offset: offset into the file to read from

  returns: true/false on if file successfully read new data
*/
typedef bool (*ReadFileFunc)(FileHandle& handle, size_t offset);

/*
  Close a file data and cleanup any data leftover

  [IN] handle: the handle to close, if not already closed
*/
typedef void (*CloseFileFunc)(FileHandle& handle);



API bool DefaultFileRead(std::string const& location, bool relative, bool binary, FileHandle& handle);
inline void DefaultFileClose(FileHandle& handle) { free(handle.data); handle.data = nullptr; }

#undef API

}; // namespace pPack

#endif