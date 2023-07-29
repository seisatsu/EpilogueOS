NAME = "help"

def ENTRY(env, args):
    print("List of commands:")
    print(env["\x00cmdlist"])
    
    return True
