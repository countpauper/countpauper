import gdb 

class ExpressionValuePrinter:
    def __init__(self, cls, val):
        self.val = val  # gdb.Value object representing your C++ instance
        self.cls = cls

    def to_string(self):
        # Access data members through GDB's Python API
        #return gdb.execute(f'print my_object[{self.val}]', to_string=True)
        return f"{self.cls}( {self.val} )"

class OperatorPrinter:
    def __init__(self, val):
        self.val = val['op']['sw']  

    def to_string(self):
        unicode = str(self.val['unicode']).split(' ')[-1][2:-1]
        operands = str(self.val['operands']).split(' ')[0]
        return f"{unicode}/{operands}"  
      
    def display_hint(self):
        return None # "string"

ns_prefix = 'Angel::Logic::'


class SimplePrinter:
    def __init__(self, typ, value):
        self.val = value
        self.type = str(typ)[len(ns_prefix):]  

    def to_string(self):
        return f"{self.type}({self.val})"
      
    def display_hint(self):
        return None     

def angel_printer(val):
    type_str = str(val.type)
    if type_str.startswith(ns_prefix):
        logic_type = type_str[len(ns_prefix):]
        if logic_type == "Operator":
            return OperatorPrinter(val)
        elif logic_type == "Id":
            return SimplePrinter(val.type, val["name"])
        elif logic_type == "Integer":
            return SimplePrinter(val.type, val["value"])
        elif logic_type == "Boolean":
            return SimplePrinter(val.type, val["truth"])
        elif logic_type == "Variable":
            return SimplePrinter(val.type, val["name"])    
        elif logic_type == "String":
            return SimplePrinter(val.type, val["name"])    
        elif logic_type == "Tuple":
            return SimplePrinter(val.type, val["name"])  
        #elif logic_type == "Expression":
        #    print(f"Expression: type = {val.type}")
        # elif logic_type == "Association":
        #     print(f"Association {val}")
        # elif logic_type == "Conjunction":
        #     print(f"Conjunction {val}")

#        return ExpressionValuePrinter(type_str[len(ns_prefix):], val)
    return None

gdb.pretty_printers.append(angel_printer)
