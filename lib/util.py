import os

def joindir(cwd, cd):
    if cd.startswith("/"):
        return cd
    elif cd.startswith("./"):
        return cwd+cd[2:]
    elif ".." in cd:
        cwd_temp = cwd.split("/")
        trv_num = 0
        if cd == "..":
            trv_num = 1
        elif "../" in cd:
            trv_num = cd.count("../")
        if cd.endswith("/.."):
            trv_num += 1
        return '/'.join(cwd[:trv_num])
    else:
        return cwd+cd

def path_exists(pathname):
    try:
        os.stat(pathname)
        return True
    except OSError:
        return False

def dir_exists(pathname):
    try:
        return (os.stat(pathname)[0] & 0x4000) != 0
    except OSError:
        return False
        
def file_exists(pathname):
    try:
        return (os.stat(pathname)[0] & 0x4000) == 0
    except OSError:
        return False

