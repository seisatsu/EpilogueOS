/* MalkuthOS Version 0.0.1
   Sei Satzparad and Paul Merrill - 2022

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/

#include "DeviceFS.h"

/* 
 * DeviceFS Return Status Checker.
 * DeviceFS functions return ints or int-like types.
 * The assumption is that 0 returns will always be success, positive numbers will always be  
 *   success but with extra information, and negative numbers will always be a failure code.
 * To check if a return code is a DeviceFS status or a downstream driver status, you can give it
 *   as an (int) to devicefs_check_status. This function will return one of the predefined status
 *   codes, and that code will be devicefs_status_downstream if the input is outside of those values,
 *   so you can know to check it with some other specialized function, or else it will be
 *   devicefs_status_invalid if the code is within the -127 to 0 code space reserved for DeviceFS
 *   statuses but isn't defined.
 * To see the list of statuses, check the devicefs_status_t enum.
 * The wisdom of this method is still being considered.
 */
devicefs_status_t devicefs_check_status(int status) {
  if (status > 0 or status <= -127) {
    return devicefs_status_downstream;
  }
  if (status < DEVICEFS_LAST_STATUS and status >= -126) {
    return devicefs_status_invalid;
  }
  return (devicefs_status_t)status;
}

/*
 * Register a driver.
 * Here we give the Driver Registry a pre-created device_driver_t, containing our necessary function pointers
 *   for open(), close(), read(), write(), etc. for a particular kind of device.
 * The kind of device this driver will work with is defined by the type and subtype.
 * Only one driver can exist for a type & subtype pair at a time.
 */
devicefs_status_t devicefs_register_driver(device_driver_t *driver, device_type_t type, const char *subtype) {
  // If a driver is already registered for this type & subtype, fail.
  if (devicefs_driver_registry.find(std::make_pair(type, subtype)) != devicefs_driver_registry.end()) {
    return devicefs_status_driver_already_registered;
  }
  
  // Add our driver to the registry.
  devicefs_driver_registry[std::make_pair(type, subtype)] = driver;

  // Return success.
  return devicefs_status_success;
}

/*
 * Unregister a driver.
 * Here we unregister a driver from the Driver Registry.
 * We select the driver to unregister by its type & subtype pair.
 */
devicefs_status_t devicefs_unregister_driver(device_type_t type, const char *subtype) {
  // If no such driver is registered for this type & subtype, fail.
  if (devicefs_driver_registry.find(std::make_pair(type, subtype)) == devicefs_driver_registry.end()) {
    return devicefs_status_no_matching_driver;
  }

  // Delete the device_driver_t for this type & subtype pair from the Driver Registry.
  devicefs_driver_registry.erase(std::make_pair(type, subtype));

  // Return success.
  return devicefs_status_success;
}

/*
 * Get a device_driver_t from the Driver Registry.
 * The device_driver_t contains the necessary function pointers for open(), close(), read(), write(), etc.
 *   for a particular kind of device.
 */
devicefs_status_t devicefs_get_driver(device_type_t type, const char *subtype, device_driver_t *driver) {
  // If no such driver is registered for this type & subtype, fail.
  if (devicefs_driver_registry.find(std::make_pair(type, subtype)) == devicefs_driver_registry.end()) {
    return devicefs_status_no_matching_driver;
  }

  // Place the matching device_driver_t from the Driver Registry into the driver pointer passed to this function.
  driver = devicefs_driver_registry[std::make_pair(type, subtype)];

  // Return success.
  return devicefs_status_success;
}

/*
 * Register a file with DeviceFS.
 * Here we register a file with DeviceFS, adding it to the File Table.
 * A filename is given for the new file, as well as a type & subtype pair which will help us select
 *   the appropriate driver for interacting with the device that the file represents.
 */
devicefs_status_t devicefs_register_file(const char *filename, device_type_t type, const char *subtype) {
  // If no such driver is registered for this type & subtype, fail.
  if (devicefs_driver_registry.find(std::make_pair(type, subtype)) == devicefs_driver_registry.end()) {
    return devicefs_status_no_matching_driver;
  }

  // If this filename already exists, fail.
  if (devicefs_file_table.find(filename) != devicefs_file_table.end()) {
    return devicefs_status_filename_already_exists;
  }

  // Declare a devicefs_file_t for to hold the device file's pertinent info, and populate it.
  devicefs_file_t *thisfile;
  strcpy(thisfile->filename, filename);
  thisfile->type = type;
  strcpy(thisfile->subtype, subtype);

  // Add the file to the File Table.
  devicefs_file_table[filename] = thisfile;

  // Return success.
  return devicefs_status_success;
}

/*
 * Unegister a file from DeviceFS.
 * Here we remove an existing file from the DeviceFS File Table.
 * This is done by filename.
 */
devicefs_status_t devicefs_unregister_file(const char *filename) {
  // If we can't find a file with this filename in the File Table, fail.
  if (devicefs_file_table.find(filename) == devicefs_file_table.end()) {
    return devicefs_status_file_not_found;
  }

  // Remove the file from the File Table.
  devicefs_file_table.erase(filename);

  // Return success.
  return devicefs_status_success;
}

/*
 * Open a file.
 * Here we find the appropriate driver for opening the file, prepare a file handle,
 *   and then pass off to the driver's own open() function for further action.
 */
int vfs_devicefs_open(const char *filename, int flags, int mode) {
  // If we can't find a file with this filename in the File Table, fail.
  if (devicefs_file_table.find(filename) == devicefs_file_table.end()) {
    return (int)devicefs_status_file_not_found;
  }

  // Retrieve the appropriate driver for this file from the Driver Registry. Otherwise, fail.
  device_driver_t *driver;
  if (devicefs_get_driver(devicefs_file_table[filename]->type, devicefs_file_table[filename]->subtype, driver) != 0) {
    return (int)devicefs_status_no_matching_driver;
  }

  // Pass off to the driver's open() function for further action. If this fails, so do we.
  if (driver->open(filename, flags, mode) != 0) {
    return (int)devicefs_status_driver_error;
  }

  // Increment the last file handle ID, and then prepare and register a file handle.
  vfs_last_handle++;
  devicefs_handle_table[vfs_last_handle] = filename;
  devicefs_file_table[filename]->handles.push_back(vfs_last_handle);

  // Return the file handle.
  // We mix return types because a handle will always be more than zero, while an error will always be less than zero.
  return vfs_last_handle;
}

/*
 * Run fstat on a file.
 * This is mostly passed off to the driver.
 * We only fail directly if the file doesn't exist, or no appropriate driver is found.
 */
int vfs_devicefs_fstat(int fd, struct stat *st) {
  // We'll need this later as a string buffer.
  char filename[64];
  
  // Make sure this file handle is valid. Otherwise, fail.
  if (devicefs_handle_table.find(fd) == devicefs_handle_table.end()) {
    return (int)devicefs_status_invalid_file_handle;
  }

  // Copy the handle's corresponding filename into our string buffer.
  strcpy(filename, devicefs_handle_table[fd]);

  // If we can't find a file with this filename in the File Table, fail.
  if (devicefs_file_table.find(filename) == devicefs_file_table.end()) {
    return (int)devicefs_status_file_not_found;
  }

  // Retrieve the appropriate driver for this file from the Driver Registry. Otherwise, fail.
  device_driver_t *driver;
  if (devicefs_get_driver(devicefs_file_table[filename]->type, devicefs_file_table[filename]->subtype, driver) != 0) {
    return (int)devicefs_status_no_matching_driver;
  }

  // Return the result from the driver, which is assumed to be positive if not an error. Errors are always negative.
  return driver->fstat(fd, st);
}

/*
 * Run stat on a file.
 * This is mostly passed off to the driver.
 * We only fail directly if the file doesn't exist, or no appropriate driver is found.
 */
int vfs_devicefs_stat(const char *filename, struct stat *st) {
  // If we can't find a file with this filename in the File Table, fail.
  if (devicefs_file_table.find(filename) == devicefs_file_table.end()) {
    return (int)devicefs_status_file_not_found;
  }

  // Retrieve the appropriate driver for this file from the Driver Registry. Otherwise, fail.
  device_driver_t *driver;
  if (devicefs_get_driver(devicefs_file_table[filename]->type, devicefs_file_table[filename]->subtype, driver) != 0) {
    return (int)devicefs_status_no_matching_driver;
  }

  // Return the result from the driver, which is assumed to be positive if not an error. Errors are always negative.
  return driver->stat(filename, st);
}

/*
 * Do an lseek on a file.
 * This is mostly passed off to the driver.
 * We only fail directly if the file doesn't exist, or no appropriate driver is found.
 */
off_t vfs_devicefs_lseek(int fd, void *dst, size_t size) {
  // We'll need this later as a string buffer.
  char filename[64];
  
  // Make sure this file handle is valid. Otherwise, fail.
  if (devicefs_handle_table.find(fd) == devicefs_handle_table.end()) {
    return (off_t)devicefs_status_invalid_file_handle;
  }

  // Copy the handle's corresponding filename into our string buffer.
  strcpy(filename, devicefs_handle_table[fd]);

  // If we can't find a file with this filename in the File Table, fail.
  if (devicefs_file_table.find(filename) == devicefs_file_table.end()) {
    return (off_t)devicefs_status_file_not_found;
  }

  // Retrieve the appropriate driver for this file from the Driver Registry. Otherwise, fail.
  device_driver_t *driver;
  if (devicefs_get_driver(devicefs_file_table[filename]->type, devicefs_file_table[filename]->subtype, driver) != 0) {
    return (off_t)devicefs_status_no_matching_driver;
  }

  // Return the result from the driver, which is assumed to be positive if not an error. Errors are always negative.
  return driver->lseek(fd, dst, size);
}

/*
 * Close a file.
 * Tell the driver to handle its own cleanup functions, and then destroy our file handle.
 */
int vfs_devicefs_close(int fd) {
  // We'll need this later as a string buffer.
  char filename[64];

  // Make sure this file handle is valid. Otherwise, fail.
  if (devicefs_handle_table.find(fd) == devicefs_handle_table.end()) {
    return (int)devicefs_status_invalid_file_handle;
  }

  // Copy the handle's corresponding filename into our string buffer.
  strcpy(filename, devicefs_handle_table[fd]);

  // If we can't find a file with this filename in the File Table, fail.
  if (devicefs_file_table.find(filename) == devicefs_file_table.end()) {
    return (int)devicefs_status_file_not_found;
  }

  // Retrieve the appropriate driver for this file from the Driver Registry. Otherwise, fail.
  device_driver_t *driver;
  if (devicefs_get_driver(devicefs_file_table[filename]->type, devicefs_file_table[filename]->subtype, driver) != 0) {
    return (int)devicefs_status_no_matching_driver;
  }

  // Pass off to the driver's close() function for further action. If this fails, so do we.
  if (driver->close(fd) != 0) {
    return (int)devicefs_status_driver_error;
  }

  // Destroy the file handle.
  devicefs_handle_table.erase(fd);
  devicefs_file_table[filename]->handles.erase(std::remove(devicefs_file_table[filename]->handles.begin(), 
    devicefs_file_table[filename]->handles.end(), fd), devicefs_file_table[filename]->handles.end());

  // Return success.
  return (int)devicefs_status_success;
}

/*
 * Read from a file.
 * This is mostly passed off to the driver.
 * We only fail directly if the file doesn't exist, or no appropriate driver is found.
 */
ssize_t vfs_devicefs_read(int fd, void *dst, size_t size) {
  // We'll need this later as a string buffer.
  char filename[64];
  
  // Make sure this file handle is valid. Otherwise, fail.
  if (devicefs_handle_table.find(fd) == devicefs_handle_table.end()) {
    return (ssize_t)devicefs_status_invalid_file_handle;
  }

  // Copy the handle's corresponding filename into our string buffer.
  strcpy(filename, devicefs_handle_table[fd]);

  // If we can't find a file with this filename in the File Table, fail.
  if (devicefs_file_table.find(filename) == devicefs_file_table.end()) {
    return (ssize_t)devicefs_status_file_not_found;
  }

  // Retrieve the appropriate driver for this file from the Driver Registry. Otherwise, fail.
  device_driver_t *driver;
  if (devicefs_get_driver(devicefs_file_table[filename]->type, devicefs_file_table[filename]->subtype, driver) != 0) {
    return (ssize_t)devicefs_status_no_matching_driver;
  }

  // Return the result from the driver, which is assumed to be positive if not an error. Errors are always negative.
  return driver->read(fd, dst, size);
}

/*
 * Write to a file.
 * This is mostly passed off to the driver.
 * We only fail directly if the file doesn't exist, or no appropriate driver is found.
 */
ssize_t vfs_devicefs_write(int fd, const void *data, size_t size) {
  // We'll need this later as a string buffer.
  char filename[64];
  
  // Make sure this file handle is valid. Otherwise, fail.
  if (devicefs_handle_table.find(fd) == devicefs_handle_table.end()) {
    return (ssize_t)devicefs_status_invalid_file_handle;
  }

  // Copy the handle's corresponding filename into our string buffer.
  strcpy(filename, devicefs_handle_table[fd]);

  // If we can't find a file with this filename in the File Table, fail.
  if (devicefs_file_table.find(filename) == devicefs_file_table.end()) {
    return (ssize_t)devicefs_status_file_not_found;
  }

  // Retrieve the appropriate driver for this file from the Driver Registry. Otherwise, fail.
  device_driver_t *driver;
  if (devicefs_get_driver(devicefs_file_table[filename]->type, devicefs_file_table[filename]->subtype, driver) != 0) {
    return (ssize_t)devicefs_status_no_matching_driver;
  }

  // Return the result from the driver, which is assumed to be positive if not an error. Errors are always negative.
  return driver->write(fd, data, size);
}

/*
 * Device files cannot be renamed, so this won't be implemented, and only exists as a stub.
 */
int vfs_devicefs_rename(const char *src, const char *dst) {
  // Devices cannot be renamed.
  return (int)devicefs_status_driver_unimplemented_function;
}
