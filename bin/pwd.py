NAME = "pwd"

def ENTRY(env, args):
    if args:
        print("This command does not take arguments.")
    else:
        print(env["\x00cwd"])

    return True
