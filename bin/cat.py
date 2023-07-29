NAME = "cat"

def ENTRY(env, args):
    if not args:
        print("Missing argument.")
        return False
    elif len(args) > 1:
        print("This cat takes one argument.")
        return False
    else:
        try:
            with open(args[0]) as f:
                print(f.read())
        except:
            print("No such file or directory.")
    
    return True
