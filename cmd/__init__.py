from os import listdir


class CommandLoader:
    def __init__(self, env):
        self.env = env
        self.cmdlist = []
        self.loaded_modules = {}
        
        all_files_list = listdir("./cmd")
        for item in all_files_list:
            if item == "__init__.py":
                continue
            split_filename = item.split(".")
            if len(split_filename) > 1 and split_filename[1] == "py":
                self.cmdlist.append(split_filename[0])
    
    def __contains__(self, cmd_name):
        return cmd_name in self.cmdlist
    
    def __getitem__(self, cmd_name):
        try:
            return lambda args: self.__load(cmd_name).ENTRY(self.env, args)
        except AttributeError:
            return None
    
    def __load(self, cmd_name):
        if cmd_name in self.loaded_modules:
            return self.loaded_modules[cmd_name]
        elif cmd_name in self.cmdlist:
            self.loaded_modules[cmd_name] = getattr(__import__("cmd."+cmd_name), cmd_name)
            return self.loaded_modules[cmd_name]
        else:
            return None

