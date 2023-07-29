from os import listdir
from lib import util

NAME = "ls"

def ENTRY(env, args):
    if not args:
        print(listdir(env["\x00cwd"]))
    else:
        try:
            print(listdir(util.joindir(env["\00cwd"], ' '.join(args))))
        except:
            print("No such file or directory.")
            return False

    return True
