from cmd import CommandLoader


ENV = {}


COMMAND = CommandLoader(ENV)


def mainloop():
    while True:
        line = input("> ").split(' ')
        if line[0] in COMMAND:
            if not COMMAND[line[0]]:
                print("COMMAND ERROR")
                continue
            COMMAND[line[0]](line[1:])
        else:
            print("UNKNOWN COMMAND")
            continue

