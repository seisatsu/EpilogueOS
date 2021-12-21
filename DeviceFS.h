/* MalkuthOS Version 0.0.1
   Sei Satzparad and Paul Merrill - 2022

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/
#ifndef DEVICEFS_H
#define DEVICEFS_H

#include <map>
#include <utility>
#include <vector>

typedef struct device_driver_t {
  int (*open)(const char, int, int);
  int (*fstat)(int, struct stat *);
  int (*close)(int);
  ssize_t (*read)(int, void *, size_t);
  ssize_t (*write)(int, const void *, size_t);
  int (*rename)(const char *, const char *);
};

typedef enum device_type_t {
  device_type_pin,
  device_type_rtc,
  device_type_sdcardfs,
  device_type_eepromfs,
};

typedef std::pair<device_type_t, const char *> device_type_pair_t;
std::map<device_type_pair_t, device_driver_t> devicefs_driver_registry;

typedef struct devicefs_file_t {
  char filename[64];
  device_type_t type;
  char subtype[16];
  std::vector<int> handles;
};

std::map<const char *, devicefs_file_t *> devicefs_file_table;
std::map<int, const char *> devicefs_handle_table;

int devicefs_register_driver(device_driver_t driver, device_type_t type, const char *subtype);
int devicefs_unregister_driver(device_type_t type, const char *subtype);

int devicefs_register_file(const char *filename, device_type_t type, const char *subtype);
int devicefs_unregister_file(const char *filename);

int vfs_devicefs_open(const char *path, int flags, int mode);
int vfs_devicefs_fstat(int fd, struct stat *st);
int vfs_devicefs_close(int fd);
ssize_t vfs_devicefs_read(int fd, void *dst, size_t size);
ssize_t vfs_devicefs_write(int fd, const void *data, size_t size);
int vfs_devicefs_rename(const char *src, const char *dst);

#endif
