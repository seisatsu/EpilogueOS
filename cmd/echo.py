NAME = "echo"

def ENTRY(env, args):
    if not args:
        print("")
    else:
        print(' '.join(args))

    return True

