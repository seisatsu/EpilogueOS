/* MalkuthOS Version 0.0.1
   Sei Satzparad and Paul Merrill - 2022

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/

#include "Filesystem.h"

/*  ESP32 VFS Initialization Procedures
 *  This registers the EEPROM, SDCARD, and External Device Filesystem Drivers
 *  with the ESP32 VFS Component.
 */
void init_vfs() {
  /*
  // EEPROM FS Initialization
  esp_vfs_t vfs_eepromfs {};

  vfs_eepromfs.flags = ESP_VFS_FLAG_DEFAULT;
  vfs_eepromfs.open  = &vfs_eepromfs_open;
  vfs_eepromfs.fstat = &vfs_eepromfs_fstat;
  vfs_eepromfs.close = &vfs_eepromfs_close;
  vfs_eepromfs.read  = &vfs_eepromfs_read;
  vfs_eepromfs.write = &vfs_eepromfs_write;
  vfs_eepromfs.rename = &vfs_eepromfs_rename;

  ESP_ERROR_CHECK(esp_vfs_register("/eeprom", &vfs_eepromfs, NULL));

  // SDCARD FS Initialization

  #if defined(sdcardsupport)
    esp_vfs_t vfs_sdcardfs {};

    vfs_sdcardfs.flags = ESP_VFS_FLAG_DEFAULT;
    vfs_sdcardfs.open  = &vfs_sdcardfs_open;
    vfs_sdcardfs.fstat = &vfs_sdcardfs_fstat;
    vfs_sdcardfs.close = &vfs_sdcardfs_close;
    vfs_sdcardfs.read  = &vfs_sdcardfs_read;
    vfs_sdcardfs.write = &vfs_sdcardfs_write;
    vfs_sdcardfs.rename = &vfs_sdcardfs_rename;

    ESP_ERROR_CHECK(esp_vfs_register("/sdcard", &vfs_sdcardfs, NULL));
  #endif

  */
  // DEVICE FS Initialization

  esp_vfs_t vfs_devicefs {};

  vfs_devicefs.flags = ESP_VFS_FLAG_DEFAULT;
  vfs_devicefs.open  = &vfs_devicefs_open;
  vfs_devicefs.fstat = &vfs_devicefs_fstat;
  vfs_devicefs.close = &vfs_devicefs_close;
  vfs_devicefs.read  = &vfs_devicefs_read;
  vfs_devicefs.write = &vfs_devicefs_write;
  vfs_devicefs.rename = &vfs_devicefs_rename;

  ESP_ERROR_CHECK(esp_vfs_register("/device", &vfs_devicefs, NULL));
}

void vfs_init() {
  vfs_last_handle = 0;
}

/*  Check the type of Filesystem based on pathname.
 *  Returns a typedef enum vfs_type_t.
 *  We have to redefine our prototype to get around an Arduino compiler quirk.
 */
vfs_type_t vfs_check_type(char *filename);
vfs_type_t vfs_check_type(char *filename) {
  // Check if this path is the VFS root.
  if (strcmp(filename, "/") == 0) {
    return vfs_type_root;
  }
  // Check if this path is the EEPROM filesystem root.
  if (strcmp(filename, "/eeprom/") == 0 or strcmp(filename, "/eeprom") == 0) {
    return vfs_type_eeprom_root;
  }
  // Check if this path is the SDCARD filesystem root.
  if (strcmp(filename, "/sdcard/") == 0 or strcmp(filename, "/sdcard") == 0) {
    return vfs_type_sdcard_root;
  }
  // Check if this path is the External DEVICE filesystem root.
  if (strcmp(filename, "/device/") == 0 or strcmp(filename, "/device") == 0) {
    return vfs_type_device_root;
  }
  // Check if this path is a file in the EEPROM filesystem.
  if (strncmp(filename, "/eeprom/", 8) == 0) {
    return vfs_type_eeprom;
  }
  // Check if this path is a file in the SDCARD filesystem.
  if (strncmp(filename, "/sdcard/", 8) == 0) {
    return vfs_type_sdcard;
  }
  // Check if this path is a virtual file in the DEVICE filesystem.
  if (strncmp(filename, "/device/", 8) == 0) {
    return vfs_type_device;
  }
  // This is an illegal path; files must exist in the EEPROM, SDCARD, or DEVICE filesystem.
  // filesystems.
  return vfs_type_error;
}

/*  Check the type of Filesystem based on pathname.
 *  Returns a string corresponding to the filesystem type.
 *  We have to redefine our prototype to get around an Arduino compiler quirk.
 */
char *vfs_check_type_string(char *filename) {
  // Check if this path is the VFS root.
  if (strcmp(filename, "/") == 0) {
    return "root";
  }
  // Check if this path is the EEPROM filesystem root.
  if (strcmp(filename, "/eeprom/") == 0 or strcmp(filename, "/eeprom") == 0) {
    return "eeprom_root";
  }
  // Check if this path is the SDCARD filesystem root.
  if (strcmp(filename, "/sdcard/") == 0 or strcmp(filename, "/sdcard") == 0) {
    return "sdcard_root";
  }
  // Check if this path is the DEVICE filesystem root.
  if (strcmp(filename, "/device/") == 0 or strcmp(filename, "/device") == 0) {
    return "device_root";
  }
  // Check if this path is a file in the EEPROM filesystem.
  if (strncmp(filename, "/eeprom/", 8) == 0) {
    return "eeprom";
  }
  // Check if this path is a file in the SDCARD filesystem.
  if (strncmp(filename, "/sdcard/", 8) == 0) {
    return "sdcard";
  }
  // Check if this path is a virtual file in the DEVICE filesystem.
  if (strncmp(filename, "/device/", 8) == 0) {
    return "device";
  }
  // This is an illegal path; files must exist in the EEPROM, SDCARD, or DEVICE filesystem.
  // filesystems.
  return "error";
}

/*
 * Change the current working directory for this task.
 */
char *vfs_change_directory(char *path) {
  // TODO: Handle irritating edge cases, like a user putting two slashes in the middle of a path.
  ps_tbl_entry_t *ps = getps();

  // No arguments, so just return to the root directory.
  if (strcmp(path, "/") == 0 or strlen(path) == 0) {
    strcpy(ps->currdir, "/");
    return ps->currdir;
  }

  // The path string starts with "/", so it is an absolute path.
  if (strncmp(path, "/", 1) == 0) {
    strcpy(ps->currdir, path);
  }
  
  // The path string does not start with "/", so it is a relative path.
  else {
    // The only time the cwd will end with a slash is when it's root.
    // So if cwd is anything other than a single slash,
    // Assume we should add one before continuing.
    if (strcmp(ps->currdir, "/") != 0) {
      strcat(ps->currdir, "/");
    }
    strcat(ps->currdir, path);
  }

  // Make sure our path does not end with a "/".
  // If it does, replace the "/" with a null character.
  // Also handle multiple slashes if that happens for some reason.
  while (strncmp(&ps->currdir[strlen(ps->currdir) - 1], "/", 1) == 0) {
    ps->currdir[strlen(ps->currdir) - 1] = '\0';
  }

  // Return the new current directory.
  return(ps->currdir);
}

//char *join_paths(char *left, char *right) {
//}

/* 
 * Save-image and load-image code from uLisp.
 * This will need to be mostly rewritten.
 */

#if defined(sdcardsupport)
void SDWriteInt(File file, int data) {
  file.write(data & 0xFF);
  file.write(data >> 8 & 0xFF);
  file.write(data >> 16 & 0xFF);
  file.write(data >> 24 & 0xFF);
}

int SDReadInt(File file) {
  uintptr_t b0 = file.read();
  uintptr_t b1 = file.read();
  uintptr_t b2 = file.read();
  uintptr_t b3 = file.read();
  return b0 | b1 << 8 | b2 << 16 | b3 << 24;
}
#endif

void EpromWriteInt(int *addr, uintptr_t data) {
  EEPROM.write((*addr)++, data & 0xFF);
  EEPROM.write((*addr)++, data >> 8 & 0xFF);
  EEPROM.write((*addr)++, data >> 16 & 0xFF);
  EEPROM.write((*addr)++, data >> 24 & 0xFF);
}

int EpromReadInt(int *addr) {
  uint8_t b0 = EEPROM.read((*addr)++);
  uint8_t b1 = EEPROM.read((*addr)++);
  uint8_t b2 = EEPROM.read((*addr)++);
  uint8_t b3 = EEPROM.read((*addr)++);
  return b0 | b1 << 8 | b2 << 16 | b3 << 24;
}

unsigned int saveimage(object *arg) {
  unsigned int imagesize = compactimage(&arg);
#if defined(sdcardsupport)
  SD.begin(SDCARD_SS_PIN);
  File file;
  if (stringp(arg)) {
    file = SD.open(MakeFilename(arg), FILE_WRITE);
    arg = NULL;
  } else if (arg == NULL || listp(arg))
    file = SD.open("/ULISP.IMG", FILE_WRITE);
  else
    error(SAVEIMAGE, PSTR("illegal argument"), arg);
  if (!file)
    error2(SAVEIMAGE, PSTR("problem saving to SD card"));
  SDWriteInt(file, (uintptr_t)arg);
  SDWriteInt(file, imagesize);
  SDWriteInt(file, (uintptr_t)GlobalEnv);
  SDWriteInt(file, (uintptr_t)GCStack);
  for (unsigned int i = 0; i < imagesize; i++) {
    object *obj = &Workspace[i];
    SDWriteInt(file, (uintptr_t)car(obj));
    SDWriteInt(file, (uintptr_t)cdr(obj));
  }
  file.close();
  return imagesize;
#else
  if (!(arg == NULL || listp(arg)))
    error(SAVEIMAGE, PSTR("illegal argument"), arg);
  int bytesneeded = imagesize * 8 + 36;
  if (bytesneeded > EEPROMSIZE)
    error(SAVEIMAGE, PSTR("image too large"), number(imagesize));
  EEPROM.begin(EEPROMSIZE);
  int addr = 0;
  EpromWriteInt(&addr, (uintptr_t)arg);
  EpromWriteInt(&addr, imagesize);
  EpromWriteInt(&addr, (uintptr_t)GlobalEnv);
  EpromWriteInt(&addr, (uintptr_t)GCStack);
  for (unsigned int i = 0; i < imagesize; i++) {
    object *obj = &Workspace[i];
    EpromWriteInt(&addr, (uintptr_t)car(obj));
    EpromWriteInt(&addr, (uintptr_t)cdr(obj));
  }
  EEPROM.commit();
  return imagesize;
#endif
}

unsigned int loadimage(object *arg) {
#if defined(sdcardsupport)
  SD.begin(SDCARD_SS_PIN);
  File file;
  if (stringp(arg))
    file = SD.open(MakeFilename(arg));
  else if (arg == NULL)
    file = SD.open("/ULISP.IMG");
  else
    error(LOADIMAGE, PSTR("illegal argument"), arg);
  if (!file)
    error2(LOADIMAGE, PSTR("problem loading from SD card"));
  SDReadInt(file);
  unsigned int imagesize = SDReadInt(file);
  GlobalEnv = (object *)SDReadInt(file);
  GCStack = (object *)SDReadInt(file);
  for (unsigned int i = 0; i < imagesize; i++) {
    object *obj = &Workspace[i];
    car(obj) = (object *)SDReadInt(file);
    cdr(obj) = (object *)SDReadInt(file);
  }
  file.close();
  gc(NULL, NULL);
  return imagesize;
#else
  EEPROM.begin(EEPROMSIZE);
  int addr = 0;
  EpromReadInt(&addr); // Skip eval address
  unsigned int imagesize = EpromReadInt(&addr);
  if (imagesize == 0 || imagesize == 0xFFFFFFFF)
    error2(LOADIMAGE, PSTR("no saved image"));
  GlobalEnv = (object *)EpromReadInt(&addr);
  GCStack = (object *)EpromReadInt(&addr);
  for (unsigned int i = 0; i < imagesize; i++) {
    object *obj = &Workspace[i];
    car(obj) = (object *)EpromReadInt(&addr);
    cdr(obj) = (object *)EpromReadInt(&addr);
  }
  gc(NULL, NULL);
  return imagesize;
#endif
}

void autorunimage() {
#if defined(sdcardsupport)
  SD.begin(SDCARD_SS_PIN);
  File file = SD.open("/ULISP.IMG");
  if (!file)
    error2(NIL, PSTR("problem autorunning from SD card"));
  object *autorun = (object *)SDReadInt(file);
  file.close();
  if (autorun != NULL) {
    loadimage(NULL);
    apply(NIL, autorun, NULL, NULL);
  }
#else
  EEPROM.begin(EEPROMSIZE);
  int addr = 0;
  object *autorun = (object *)EpromReadInt(&addr);
  if (autorun != NULL && (unsigned int)autorun != 0xFFFF) {
    loadimage(NULL);
    apply(NIL, autorun, NULL, NULL);
  }
#endif
}

// Note: Remember __getreent()
