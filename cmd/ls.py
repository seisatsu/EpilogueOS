from os import listdir

NAME = "ls"

def ENTRY(env, args):
    if not args:
        print(listdir('.'))
    else:
        try:
            print(listdir(' '.join(args)))
        except:
            print("No such file or directory.")
            return False

    return True
