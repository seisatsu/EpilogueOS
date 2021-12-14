/* MalkuthOS Version 0.0.1
   Sei Satzparad and Paul Merrill

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <esp_vfs.h>

// Filesystem Type Enumeration

typedef enum vfs_type_t {
  vfs_type_root,
  vfs_type_eeprom,
  vfs_type_sdcard,
  vfs_type_device,
  vfs_type_eeprom_root,
  vfs_type_sdcard_root,
  vfs_type_device_root,
  vfs_type_error,
};

// Virtual Filesystem State
typedef struct vfs_state_t {
  char cwd[128];
};

// EEPROM FS Definitions

int vfs_eepromfs_open(const char *path, int flags, int mode);
int vfs_eepromfs_fstat(int fd, struct stat *st);
int vfs_eepromfs_close(int fd);
ssize_t vfs_eepromfs_read(int fd, void *dst, size_t size);
ssize_t vfs_eepromfs_write(int fd, const void *data, size_t size);
int vfs_eepromfs_rename(const char *src, const char *dst);

// SDCARD FS Definitions

#if defined(sdcardsupport)
int vfs_sdcardfs_open(const char *path, int flags, int mode);
int vfs_sdcardfs_fstat(int fd, struct stat *st);
int vfs_sdcardfs_close(int fd);
ssize_t vfs_sdcardfs_read(int fd, void *dst, size_t size);
ssize_t vfs_sdcardfs_write(int fd, const void *data, size_t size);
int vfs_sdcardfs_rename(const char *src, const char *dst);
#endif

// External Device FS Definitions

int vfs_devicefs_open(const char *path, int flags, int mode);
int vfs_devicefs_fstat(int fd, struct stat *st);
int vfs_devicefs_close(int fd);
ssize_t vfs_devicefs_read(int fd, void *dst, size_t size);
ssize_t vfs_devicefs_write(int fd, const void *data, size_t size);
int vfs_devicefs_rename(const char *src, const char *dst);

// VFS Functions

vfs_type_t check_vfs_type(char *filename);
char *check_vfs_type_string(char *filename);

#endif
