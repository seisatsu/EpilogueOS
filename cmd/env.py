NAME = "env"

def ENTRY(env, args):
    if not args:
        print(env)
    elif len(args) == 1:
        if args[0] in env:
            print(env[args[0]])
        else:
            print("")
    elif len(args) >= 2:
        env[args[0]] = ' '.join(args[1:])
        print(env[args[0]])

    return True
