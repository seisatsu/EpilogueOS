NAME = "echo"

def ENTRY(env, args):
    print(' '.join(args))

    return True
