from lib import util

NAME = "cd"

def ENTRY(env, args):
    if not args:
        env["\x00cwd"] = "/"
    elif len(args) > 1:
        print("This command takes one argument.")
        return False
    else:
        new_dir = util.joindir(env["\x00cwd"], args[0])
        if util.dir_exists(new_dir):
            env["\x00cwd"] = new_dir
        else:
            print("No such directory.")
            return False
    
    return True

