import gdb 
import traceback

ns_prefix = 'Angel::Logic::'

class ExpressionPrinter:
    def __init__(self, val):
        self.val = val 
    
    def to_string(self):
        obj_addr = int(self.val.address)
        result = gdb.parse_and_eval(f"{ns_prefix}to_string(*({ns_prefix}Expression*){obj_addr})")
        return str(result).strip("\"")

class OperatorPrinter:
    def __init__(self, val):
        self.val = val['op']['sw']  
        # print(f"OperatorPrinter {self.val}")

    def to_string(self):
        unicode = str(self.val['unicode']).split(' ')[-1][2:-1]
        operands = str(self.val['operands']).split(' ')[0]
        return f"{unicode}/{operands}"  
      
    def display_hint(self):
        return None # "string"


class SimplePrinter:
    def __init__(self, typ, value):
        # print(f"SimplePrinter {typ} for {value}")
        self.val = value
        self.type = str(typ)[len(ns_prefix):]  

    def to_string(self):
        return f"{self.type}({self.val})"
      
    def display_hint(self):
        return None     

def angel_printer(val):
    try: 
        type_str = str(val.type)
        if type_str.startswith(ns_prefix):
            logic_type = type_str[len(ns_prefix):]
            if logic_type == "Expression": 
                return ExpressionPrinter(val)
            elif logic_type == "Operator":
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
            # TODO but gdb seems to have these with value in storage?
            #elif logic_type == "Expression":
            #    print(f"Expression: type = {val.type}")
            # elif logic_type == "Association":
            #     print(f"Association {val}")
            # elif logic_type == "Conjunction":
            #     print(f"Conjunction {val}")

    #        return ExpressionValuePrinter(type_str[len(ns_prefix):], val)
        return None
    except Exception as e:
        stack = traceback.extract_stack()# [:-1]
        print(f"{e} at\n{traceback.format_exc()}") # \n{traceback.format_exc()}")



gdb.pretty_printers.append(angel_printer)
