/* MalkuthOS Version 0.0.1
   Sei Satzparad and Paul Merrill - 2022

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/
#ifndef DEVICEFS_H
#define DEVICEFS_H

#include <map>
#include <vector>

typedef struct devicefs_file_t {
  char filename[64];
  std::vector<int> handles;
  ssize_t (*read_function)(int, void *, size_t);
  ssize_t (*write_function)(int, const void *, size_t);
};

std::map<const char *, devicefs_file_t *> devicefs_file_table;
std::map<int, const char *> devicefs_handle_table;

int devicefs_register(const char *filename/*, void (*read_function)(int, void, size_t), (*write_function)(int, const void *, size_t)*/);
int devicefs_unregister(const char *filename);

int vfs_devicefs_open(const char *path, int flags, int mode);
int vfs_devicefs_fstat(int fd, struct stat *st);
int vfs_devicefs_close(int fd);
ssize_t vfs_devicefs_read(int fd, void *dst, size_t size);
ssize_t vfs_devicefs_write(int fd, const void *data, size_t size);
int vfs_devicefs_rename(const char *src, const char *dst);

#endif
