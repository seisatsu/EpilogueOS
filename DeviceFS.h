/* MalkuthOS Version 0.0.1
   Sei Satzparad and Paul Merrill - 2022

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/
#ifndef DEVICEFS_H
#define DEVICEFS_H

#include <map>
#include <utility>
#include <vector>

// This contains the function pointers for a particular driver's necessary functions.
typedef struct device_driver_t {
  int (*open)(const char *, int, int);
  int (*fstat)(int, struct stat *);
  int (*close)(int);
  ssize_t (*read)(int, void *, size_t);
  ssize_t (*write)(int, const void *, size_t);
  int (*rename)(const char *, const char *);
};

// This contains the different primary types of drivers we can have.
// Subtypes are defined by strings instead.
typedef enum device_type_t {
  device_type_pin,
  device_type_rtc,
  device_type_sdcardfs,
  device_type_eepromfs,
};

// Declare the Driver Registry, where we can look up a driver by the device type & subtype pair
//   for which it is appropriate.
typedef std::pair<device_type_t, const char *> device_type_pair_t;
std::map<device_type_pair_t, device_driver_t *> devicefs_driver_registry;

// This contains the different kinds of status codes that DeviceFS functions can return.
// See the further documentation at this function's prototype.
typedef enum devicefs_status_t {
  devicefs_status_success = 0,
  devicefs_status_no_matching_driver = -1,
  devicefs_status_file_not_found = -2,
  devicefs_status_filename_already_exists = -3,
  devicefs_status_driver_error = -4,
  devicefs_status_driver_already_registered = -5,
  devicefs_status_driver_unimplemented_function = -6,
  devicefs_status_invalid_file_handle = -7,
  // All numbers between here are reserved for devicefs_status_t.
  devicefs_status_invalid = -126,
  devicefs_status_downstream = -127,
};
#define DEVICEFS_LAST_STATUS -7

// This is a file descriptor within the File Table.
// It contains the filename, device type & subtype, and a list of its open file handles.
typedef struct devicefs_file_t {
  char filename[64];
  device_type_t type;
  char subtype[16];
  std::vector<int> handles;
};

// This is the File Table which contains all extant file descriptors, mapped by filename.
std::map<const char *, devicefs_file_t *> devicefs_file_table;

// This is the File Handle Table, which contains all open filenames mapped by their handles.
std::map<int, const char *> devicefs_handle_table;

// DeviceFS status checker.
devicefs_status_t devicefs_check_status(int status);

// Driver registration, unregistration, and retrieval functions.
devicefs_status_t devicefs_register_driver(device_driver_t *driver, device_type_t type, const char *subtype);
devicefs_status_t devicefs_unregister_driver(device_type_t type, const char *subtype);
devicefs_status_t devicefs_get_driver(device_type_t type, const char *subtype, device_driver_t *driver);

// Device file registration and unregistration functions.
devicefs_status_t devicefs_register_file(const char *filename, device_type_t type, const char *subtype);
devicefs_status_t devicefs_unregister_file(const char *filename);

// Device file manipulation functions.
int vfs_devicefs_open(const char *filename, int flags, int mode);
int vfs_devicefs_fstat(int fd, struct stat *st);
int vfs_devicefs_close(int fd);
ssize_t vfs_devicefs_read(int fd, void *dst, size_t size);
ssize_t vfs_devicefs_write(int fd, const void *data, size_t size);
int vfs_devicefs_rename(const char *src, const char *dst);

#endif
