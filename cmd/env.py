from copy import copy

NAME = "env"

def ENTRY(env, args):
    if not args:
        env_temp = copy(env)
        for e in env:
            if e.startswith("\x00"):
                del env_temp[e]
        print(env_temp)
    elif len(args) == 1:
        if args[0] in env:
            print(env[args[0]])
        else:
            print("")
    elif len(args) >= 2:
        env[args[0]] = ' '.join(args[1:])
        print(env[args[0]])

    return True
