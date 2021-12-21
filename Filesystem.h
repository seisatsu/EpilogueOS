/* MalkuthOS Version 0.0.1
   Sei Satzparad and Paul Merrill - 2022

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <esp_vfs.h>

// ID of the last file handle produced.
int vfs_last_handle;

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

typedef enum vfs_mode_t {
  vfs_mode_r,
  vfs_mode_w,
  vfs_mode_rw,
  vfs_mode_a,
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

// VFS Functions

vfs_type_t vfs_check_type(char *filename);
char *vfs_check_type_string(char *filename);
char *vfs_change_directory(char *path);

#endif
