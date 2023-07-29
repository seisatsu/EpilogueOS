NAME = "unenv"

def ENTRY(env, args):
    if not args:
        print("Missing argument.")
        return False
    elif args[0] not in env:
        print("No such variable set.")
        return False
    else:
        del env[args[0]]
    
    return True

