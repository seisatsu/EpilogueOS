/* MalkuthOS Version 0.0.1
   Sei Satzparad and Paul Merrill - 2022

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/

#include "DeviceFS.h"

typedef enum device_type_t;
int devicefs_register_file(const char *filename, device_type_t type, const char *subtype) {
  if (devicefs_driver_registry.find(std::make_pair(type, subtype)) == devicefs_driver_registry.end()) {
    return -1;
  }
  if (devicefs_file_table.find(filename) != devicefs_file_table.end()) {
    return -2;
  }
  devicefs_file_t *thisfile;
  strcpy(thisfile->filename, filename);
  thisfile->type = type;
  strcpy(thisfile->subtype, subtype);
  devicefs_file_table[filename] = thisfile;
  return 0;
}

int devicefs_unregister_file(const char *filename) {
  if (devicefs_file_table.find(filename) == devicefs_file_table.end()) {
    return -1;
  }
  devicefs_file_table.erase(filename);
  return 0;
}

int vfs_devicefs_open(const char *path, int flags, int mode) {
  if (devicefs_file_table.find(path) == devicefs_file_table.end()) {
    return -1;
  }
  vfs_last_handle++;
  devicefs_handle_table[vfs_last_handle] = path;
  devicefs_file_table[path]->handles.push_back(vfs_last_handle);
  return vfs_last_handle;
}

int vfs_devicefs_fstat(int fd, struct stat *st) {
  // TODO
  return 0;
}

int vfs_devicefs_close(int fd) {
  char filename[64];
  if (devicefs_handle_table.find(fd) == devicefs_handle_table.end()) {
    return -1;
  }
  strcpy(filename, devicefs_handle_table[fd]);
  devicefs_handle_table.erase(fd);
  devicefs_file_table[filename]->handles.erase(std::remove(devicefs_file_table[filename]->handles.begin(), 
    devicefs_file_table[filename]->handles.end(), fd), devicefs_file_table[filename]->handles.end());
  return 0;
}

ssize_t vfs_devicefs_read(int fd, void *dst, size_t size) {
  // TODO
  return 0;
}

ssize_t vfs_devicefs_write(int fd, const void *data, size_t size) {
  //TODO
  return 0;
}

int vfs_devicefs_rename(const char *src, const char *dst) {
  // Devices cannot be renamed.
  return -1;
}
